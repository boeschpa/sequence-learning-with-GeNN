import numpy as np
import matplotlib.pyplot as plt
import sys
import os
import re
import pandas as pd
import scipy
import matplotlib.pylab as pylab


# arguments: 0: file
#            1: -noshow
#            2: time window start
#            3: time window stop


class Parameters:
    def __init__(self, **kwargs):
        self.__dict__.update(kwargs)

def parse_cpp_header(header_text):
    # Define a regular expression to match any variable name and its value
    regex = r"const\s+(\w+::\w+|\w+)\s+(\w+)\s*=\s*(.+);"

    # Use re.findall to find all matches in the header_text
    matches = re.findall(regex, header_text)

    # Create a dictionary to store the variable names and values
    param_dict = {}
    for match in matches:
        var_type, var_name, var_value = match
        if var_type == "std::string":
            # Remove quotes for string values
            var_value = var_value.strip('\"')
        elif var_type == "float":
            var_value = float(var_value)
        elif var_type == "int":
            var_value = int(var_value)

        # Add the variable name and value to the dictionary
        param_dict[var_name] = var_value

    # Create and return a Parameters object
    return Parameters(**param_dict)

def read_file(file_path):
    with open(file_path, 'r') as file:
        return file.read()
    
def idToPattern(spikes,sequence,length):
    spikes_per_minicolumn = spikes // param.N_pyramidal  #global minicolumn index (which mc does neuron/spike belong to)
    spikes_local_minicoliumn = spikes_per_minicolumn % param.N_minicolumns
    spikes_per_pattern = np.zeros((length,len(spikes)),dtype=bool)
    for sp_id, sp in enumerate(spikes_per_minicolumn): # iterate through spikes (indexed by MC)
        hc = sp//param.N_minicolumns # HC of spike
        for pt_id, pt in enumerate(sequence): # iterate through patterns in sequence
            if (spikes_local_minicoliumn[sp_id]==pt[hc]):  # if spike sp with hypercolumn hc is part of the current pattern pt
                spikes_per_pattern[pt_id][sp_id] = True # set index to true for this spike in this pattern
    return spikes_per_pattern  
    
def calculate_firing_rate(spike_times, time_window_start, time_window_end, N_neurons):
    spikes_in_window = [spike for spike in spike_times if time_window_start <= spike <= time_window_end] # inefficient
    firing_rate = 1000.0 * len(spikes_in_window) / ((time_window_end - time_window_start) * N_neurons)
    return firing_rate

def compute_rolling_average_spike_rate(spikes, time_window, bin_size, rolling_window, N_neurons):
    # spikes: array of spike timestamps
    # time_window: total duration of the signal
    # bin_size: size of each bin
    # rolling_window: number of bins for the rolling average
    # Calculate the number of bins
    num_bins = int(time_window / bin_size)+1

    # Create an array to store spike counts in each bin
    spike_counts = np.zeros(num_bins)

    # Count spikes in each bin
    for spike in spikes:
        bin_index = int(spike / bin_size)
        spike_counts[bin_index] += 1

    # Calculate spike rates for each bin
    spike_rates =  1000.0 * spike_counts / bin_size / N_neurons # average neuron firing rate in Hz

    # Calculate rolling average spike rate
    rolling_average_spike_rate = np.convolve(spike_rates, np.ones(rolling_window)/rolling_window, mode='same')

    return rolling_average_spike_rate

def pattern_list(firing_rates, t_window): # add pattern index to list if it stays winner for t_window 
    # firing_rate[i,t_id]
    patterns = []
    winners = np.argmax(firing_rates,axis=0)
    count = 0
    current_num = -1
    num_repeat = np.round(t_window)
    for win in winners:
        if win == current_num:
            count+=1
        else:
            current_num = win
            count = 0
        if count == num_repeat:
            patterns.append(current_num)
    return patterns

def find_output_files(folder="."):
    # Get the list of files in the specified folder
    files = os.listdir(folder)

    # Filter files that start with "output."
    output_files = [file for file in files if file.startswith("output.spikes")]

    return output_files

def extract_seed_and_length(input_string):
    # Define the pattern using regular expression
    pattern = r'seed(\d+)_len(\d+)'

    # Use re.search to find the pattern in the input string
    match = re.search(pattern, input_string)

    # Check if a match is found
    if match:
        # Extract seed and length from the matched groups
        seed = int(match.group(1))
        length = int(match.group(2))

        return seed, length
    else:
        # Return None if no match is found
        return None

def idToMC(spikes):
    spikes_per_minicolumn = spikes // param.N_pyramidal  #global minicolumn index (which mc does neuron/spike belong to)
    spikes_per_minicolumn = spikes_per_minicolumn.flatten() #remove extra dimension
    N_global_mc=param.hyper_height*param.hyper_width*param.N_minicolumns
    return np.eye(N_global_mc,dtype=bool)[spikes_per_minicolumn].T # from index vector to onehot encoded matrix
    
def sequence_list(firing_rate_mc,firing_rates,t_window,length):
    # first find pattern transition times
    patterns = []
    patterns_start_index = []
    winners = np.argmax(firing_rates,axis=0)
    count = 0
    current_num = -1
    current_start_index = 0
    num_repeat = np.round(t_window)
    for win_id, win in enumerate(winners):
        if win == current_num:
            count+=1
        else:
            current_num = win
            current_start_index = win_id
            count = 0
        if count == num_repeat:
            patterns.append(current_num)
            patterns_start_index.append(current_start_index)
    
    # compute pattern mid point
    patterns_start_index.append(len(winners)-1)
    patterns_start_index = np.array(patterns_start_index)
    patterns_midpoint = patterns_start_index[0:-1]+np.divide(patterns_start_index[1:]-patterns_start_index[0:-1],2)
    
    #discard settle random patterns by finding the first correct sequence (training epoch 1)
    cycle_start = -1
    cycle_end = -1
    for pat_id, pat in enumerate(patterns):
        if(cycle_start==-1 and pat==0):
            cycle_start = pat_id
        elif(cycle_end==-1 and pat==0):
            cycle_end = pat_id
            # check if pattern was found
            if (patterns[cycle_start:cycle_end]==list(range(sequence_length))):
                patterns=patterns[cycle_start:]
                patterns_midpoint=patterns_midpoint[cycle_start:]
                break
            else:
                cycle_start = pat_id
                cycle_end = -1
    #discard learning epochs
    patterns=patterns[param.epochs*sequence_length:]
    patterns_midpoint=patterns_midpoint[param.epochs*sequence_length:]

    # # find one cycle (pattern 0 to pattern 0)
    # cycle_start = -1
    # cycle_end = -1
    # for pat_id, pat in enumerate(patterns):
    #     if(cycle_start==-1 and pat==0):
    #         cycle_start = pat_id
    #     elif(cycle_end==-1 and pat==0):
    #         cycle_end = pat_id
    # patterns=patterns[cycle_start:cycle_end]
    # patterns_midpoint=patterns_midpoint[cycle_start:cycle_end]

    #build recall sequence
    offset = 0
    mini_patterns = np.zeros((len(patterns),N_hypercolumns),dtype=int)-1
    for pat_id in range(len(patterns)):
        for hc in range(N_hypercolumns):
            mini_patterns[pat_id,hc] = np.argmax(firing_rate_mc[hc*param.N_minicolumns:(hc+1)*param.N_minicolumns,round(patterns_midpoint[pat_id])])
    
    return patterns, mini_patterns

cpp_param = read_file("model_param.h")

# get params from model_param
param = parse_cpp_header(cpp_param)

# get all pyramidal output files in the folder
output_file_list = find_output_files()

# dataframe to save all results
df = pd.DataFrame(columns=['seed', 'sequence_length','mini_sequence', 'mini_sequence_recall' , 'sequence'])

# iterate over files
for file in output_file_list:
    seed, sequence_length = extract_seed_and_length(file)

    if(seed==0 and sequence_length==10):

        # Load data from the .dat file
        data = np.loadtxt(file)
        # dataBasket = np.loadtxt("output.basketSpikes_seed"+str(seed)+"_len"+str(sequence_length)+".csv")

        # Load sequence
        sequence = np.loadtxt("sequence_seed"+str(seed)+"_len"+str(sequence_length)+".csv",delimiter=",",dtype=int,ndmin=2)

        # Split the data into time (first column) and spikes 
        time = data[:, 0]
        # timeBasket = dataBasket[:,0]
        N= np.shape(data)[1]-1
        spikes = data[:, 1:N+1]
        # spikesBasket = dataBasket[:, 1:N+1]
        spikes = spikes.astype(int)
        # spikesBasket = spikesBasket.astype(int)


        # get firing rates
        i=0
        sim_time = time[-1]
        time_start = 0
        t_window = 50.0 #ms
        bin_size = 10 #ms
        dense_time = range(time_start,int(sim_time),bin_size)

        # firing rate per pattern in pyramidal neurons
        spikes_per_pattern = idToPattern(spikes,sequence,sequence_length)
        firing_rate = np.zeros((sequence_length,len(dense_time)))

        for i in range(sequence_length):
            indices = spikes_per_pattern[i]
            spike_times = time[indices]
            firing_rate[i,:] = compute_rolling_average_spike_rate(spike_times,sim_time,bin_size,int(t_window/bin_size),param.N_pyramidal*param.hyper_height*param.hyper_width)

        min_pattern_length = 50.0 / bin_size
        patterns = pattern_list(firing_rate,min_pattern_length)
        print("patterns: "+ str(patterns))

        # get firing rates per minicolumn
        spikes_per_mc = idToMC(spikes)
        N_hypercolumns = param.hyper_height*param.hyper_width

        dense_time = range(time_start,int(sim_time),bin_size)
        firing_rate_mc = np.zeros((N_hypercolumns*param.N_minicolumns,len(dense_time)))

        for i in range(param.hyper_height*param.hyper_width*param.N_minicolumns):
            indices = spikes_per_mc[i]
            spike_times = time[indices]
            firing_rate_mc[i,:] = compute_rolling_average_spike_rate(spike_times,sim_time,bin_size,int(t_window/bin_size),param.N_pyramidal)

        patterns, mini_patterns = sequence_list(firing_rate_mc,firing_rate,min_pattern_length,sequence_length)
        print("shortened patterns: "+ str(patterns))
        # Adding a new row to the DataFrame
        new_row_data = {'seed': seed, 'sequence_length': sequence_length, 'mini_sequence': sequence, 'mini_sequence_recall': mini_patterns, 'sequence': patterns}
        df = pd.concat([df, pd.DataFrame([new_row_data])], ignore_index=True)

        #############################
        # PLOT
        #############################
        params = {'legend.fontsize': 'large',
         'axes.labelsize': 'large',
         'axes.titlesize':'large',
         'xtick.labelsize':'large',
         'ytick.labelsize':'large'}
        pylab.rcParams.update(params)

        figure, ax = plt.subplots(2,1,sharex=True)
        figure.set_size_inches(10,6)

        # plot spikes
        ax[0].scatter(time, spikes, s=4 ,edgecolor='none')
        for i in range(param.hyper_height*param.hyper_width-1):
            ax[0].axhline(y = (i+1)*param.N_pyramidal*param.N_minicolumns, color = 'k', linestyle = '-')

        # plot firing rates
        ax[1].plot(np.tile(dense_time,(sequence_length,1)).T,firing_rate.T)



        # Add labels and a legend
        ax[1].set_xlabel('Time (ms)')
        ax[0].set_ylabel('Pyramidal neuron index')
        ax[1].set_ylabel('Average firing rate (Hz)')
        ax[1].grid()

        # add letters
        ax[0].annotate("A", xy=(0, 0), xytext=(-0.07, 1.05), xycoords='axes fraction', ha='right', va='top', fontsize=14, fontweight='bold')
        ax[1].annotate("B", xy=(0, 0), xytext=(-0.07, 1.05), xycoords='axes fraction', ha='right', va='top', fontsize=14, fontweight='bold')

        plt.tight_layout()
        plt.savefig("plot_3x3_sequence.png",dpi=600)
        plt.show()



