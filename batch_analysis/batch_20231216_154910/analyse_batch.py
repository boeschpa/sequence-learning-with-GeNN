import numpy as np
import matplotlib.pyplot as plt
import sys
import os
import re

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
    
def idToPattern(spikes,sequence):
    spikes_per_minicolumn = spikes // param.N_pyramidal  #global minicolumn index (which mc does neuron/spike belong to)
    spikes_local_minicoliumn = spikes_per_minicolumn % param.N_minicolumns
    spikes_per_pattern = np.zeros((param.N_patterns,len(spikes)),dtype=bool)
    for sp_id, sp in enumerate(spikes_per_minicolumn): # iterate through spikes (indexed by MC)
        hc = sp//param.N_minicolumns # HC of spike
        for pt_id, pt in enumerate(sequence): # iterate through patterns in sequence
            if (spikes_local_minicoliumn[sp_id]==pt[hc]):  # if spike sp with hypercolumn hc is part of the current pattern pt
                spikes_per_pattern[pt_id][sp_id] = True # set index to true for this spike in this pattern
    return spikes_per_pattern  
    
def calculate_firing_rate(spike_times, time_window_start, time_window_end, N_neurons):
    spikes_in_window = [spike for spike in spike_times if time_window_start <= spike <= time_window_end]
    firing_rate = 1000.0 * len(spikes_in_window) / ((time_window_end - time_window_start) * N_neurons)
    return firing_rate

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



cpp_param = read_file("model_param.h")

# get params from model_param
param = parse_cpp_header(cpp_param)

# get all pyramidal output files in the folder
output_file_list = find_output_files()

# iterate over files
for file in output_file_list:
    seed, sequence_length = extract_seed_and_length(file)

    # Load data from the .dat file
    data = np.loadtxt(file)
    dataBasket = np.loadtxt("output.basketSpikes_seed"+str(seed)+"_len"+str(sequence_length)+".csv")

    # Load sequence
    sequence = np.loadtxt("sequence"+str(seed)+"_len"+str(sequence_length)+".csv",delimiter=",",dtype=int,ndmin=2)

    # Split the data into time (first column) and spikes 
    time = data[:, 0]
    timeBasket = dataBasket[:,0]
    N= np.shape(data)[1]-1
    spikes = data[:, 1:N+1]
    spikesBasket = dataBasket[:, 1:N+1]
    spikes = spikes.astype(int)
    spikesBasket = spikesBasket.astype(int)


    # get firing rates
    i=0
    sim_time = time[-1]
    time_start = 0
    t_window = 50.0 #ms
    stride = 20 #ms
    dense_time = range(time_start,int(sim_time),stride)

    # firing rate of all pyramidal neurons
    firing_rate_pyramidal = np.zeros((len(dense_time)))
    for (t_id, t) in enumerate(dense_time):
            firing_rate_pyramidal[t_id] = calculate_firing_rate(time, t, t+t_window, param.N_pyramidal*param.N_minicolumns*param.hyper_height*param.hyper_width)

    # firing rate of all basket neurons
    firing_rate_basket = np.zeros((len(dense_time)))
    for (t_id, t) in enumerate(dense_time):
            firing_rate_basket[t_id] = calculate_firing_rate(timeBasket, t, t+t_window, param.N_basket*param.hyper_height*param.hyper_width)


    # firing rate per pattern in pyramidal neurons
    spikes_per_pattern = idToPattern(spikes,sequence)
    firing_rate = np.zeros((param.N_patterns,len(dense_time)))

    for i in range(param.N_patterns):
        for (t_id, t) in enumerate(dense_time):
            indices = spikes_per_pattern[i]
            spike_times = time[indices]
            firing_rate[i,t_id] = calculate_firing_rate(spike_times, t, t+t_window, param.N_pyramidal*param.hyper_height*param.hyper_width)

    min_pattern_length = 10.0 / stride
    patterns = pattern_list(firing_rate,min_pattern_length)
    print("patterns: "+ str(patterns))

    # get longterm firing rates
    basket_rate = 1000.0*len(timeBasket[np.where(timeBasket < param.settle_time)])/(param.N_basket*param.hyper_height*param.hyper_width*param.settle_time)
    pyramidal_rate = 1000.0*len(time[np.where(time < param.settle_time)])/(param.N_pyramidal*param.N_minicolumns*param.hyper_height*param.hyper_width*param.settle_time)
    print("basket rate: " + str(basket_rate) + " Hz")
    print("pyramidal rate: " + str(pyramidal_rate) + " Hz")

    indices = spikes_per_pattern[0]
    spike_times = time[indices]
    pattern_rate = 1000.0*len(spike_times[np.where(np.logical_and(spike_times<param.settle_time+param.pattern_time, spike_times > param.settle_time))])/(param.N_basket*param.hyper_height*param.hyper_width*param.pattern_time)
    print("pattern rate: " + str(pattern_rate) + " Hz")

    # plot
    fig, ax = plt.subplots(3,1,sharex=True)
    fig.set_size_inches(12,8)

    # plot spikes
    if (sim_time != time[-1] and time_start != 0):
        spike_id = np.where(np.logical_and(time<sim_time, time > time_start))[0]
        ax[0].plot(time[spike_id], spikes[spike_id],".", markersize=1)
        spike_id_basket = np.where(np.logical_and(timeBasket<sim_time, timeBasket > time_start))[0]
        ax[1].plot(timeBasket[spike_id_basket], spikesBasket[spike_id],".", markersize=1, color = 'r')
    else:
        ax[0].plot(time, spikes,".", markersize=1)
        ax[1].plot(timeBasket, spikesBasket,".", markersize=1,color = 'r')

    for i in range(param.hyper_height*param.hyper_width-1):
        ax[0].axhline(y = (i+1)*param.N_pyramidal*param.N_minicolumns, color = 'k', linestyle = '-')
        ax[1].axhline(y = (i+1)*param.N_basket, color = 'k', linestyle = '-') 

    # plot firing rates
    ax[2].plot(np.tile(dense_time,(param.N_patterns,1)).T,firing_rate.T,label="pattern")
    ax[2].plot(dense_time,firing_rate_basket,label="basket",color='r')
    ax[2].plot(dense_time,firing_rate_pyramidal,label="pyramidal",color='b')



    # Add labels and a legend
    ax[2].set_xlabel('Time (ms)')
    ax[0].set_ylabel('Pyramidal Neuron id')
    ax[1].set_ylabel('Basket Neuron id')
    ax[0].title.set_text('Spikes of Neuron N vs. Time')
    ax[2].grid()
    ax[2].legend()

    plt.tight_layout()
    plt.savefig("plot_spikes.png")



