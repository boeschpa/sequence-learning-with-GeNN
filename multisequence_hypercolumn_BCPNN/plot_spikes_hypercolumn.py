import numpy as np
import matplotlib.pyplot as plt
import sys

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
    spikes_per_pattern = np.zeros((param.N_patterns*param.N_sequences,len(spikes)),dtype=bool)
    for sp_id, sp in enumerate(spikes_per_minicolumn): # iterate through spikes (indexed by MC)
        hc = sp//param.N_minicolumns # HC of spike
        for pt_id, pt in enumerate(sequence): # iterate through patterns in sequence
            if (spikes_local_minicoliumn[sp_id]==pt[hc]):  # if spike sp with hypercolumn hc is part of the current pattern pt
                spikes_per_pattern[pt_id][sp_id] = True # set index to true for this spike in this pattern
    return spikes_per_pattern  
    
def calculate_firing_rate(spike_times, time_window_start, time_window_end):
    spikes_in_window = [spike for spike in spike_times if time_window_start <= spike <= time_window_end]
    firing_rate = len(spikes_in_window) / (time_window_end - time_window_start)
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




cpp_param = read_file("model_param.h")

# get params from model_param
param = parse_cpp_header(cpp_param)

# Load data from the .dat file
data = np.loadtxt(sys.argv[1])

# Load sequence
sequence = np.loadtxt("sequence.csv",delimiter=",",dtype=int)

# Split the data into time (first column) and voltage (subsequent N columns)
time = data[:, 0]
N= np.shape(data)[1]-1
spikes = data[:, 1:N+1]  # Assuming the voltage columns are from 1 to N+1
spikes = spikes.astype(int)


# get firing rates
i=0
sim_time = time[-1]
time_start = 0
t_window = 50.0 #ms
bin_size = 10 #ms
dense_time = range(time_start,int(sim_time),bin_size)

if len(sys.argv)>=4:
    time_start = int(sys.argv[3])
if len(sys.argv)>=5:
    sim_time = int(sys.argv[4])

spikes_per_pattern = idToPattern(spikes,sequence)

dense_time = range(time_start,int(sim_time),bin_size)

# firing rate per pattern in pyramidal neurons
spikes_per_pattern = idToPattern(spikes,sequence)
firing_rate = np.zeros((param.N_patterns,len(dense_time)))

for i in range(param.N_patterns):
    indices = spikes_per_pattern[i]
    spike_times = time[indices]
    firing_rate[i,:] = compute_rolling_average_spike_rate(spike_times,sim_time,bin_size,int(t_window/bin_size),param.N_pyramidal*param.hyper_height*param.hyper_width)

min_pattern_length = 50.0 / bin_size
patterns = pattern_list(firing_rate,min_pattern_length)
print("patterns: "+ str(patterns))

pattern_window = 10.0 / bin_size
patterns = pattern_list(firing_rate,pattern_window)
print(patterns)

#plot
fig, ax = plt.subplots(2,1,sharex=True)
fig.set_size_inches(16,16)

# plot spikes
if (sim_time != time[-1] and time_start != 0):
    spike_id = np.where(np.logical_and(time<sim_time, time > time_start))[0]
    ax[0].plot(time[spike_id], spikes[spike_id],".", markersize=1)
else:
    ax[0].plot(time, spikes,".", markersize=1)

for i in range(param.hyper_height*param.hyper_width-1):
    ax[0].axhline(y = (i+1)*param.N_pyramidal*param.N_minicolumns, color = 'k', linestyle = '-') 

# plot firing rates
ax[1].plot(np.tile(dense_time,(param.N_patterns*param.N_sequences,1)).T,firing_rate.T)


# Add labels and a legend
ax[1].set_xlabel('Time (ms)')
ax[0].set_ylabel('Neuron')
ax[0].title.set_text('Spikes of Neuron N vs. Time')

plt.savefig("plot_spikes.png")

# Show plot
if len(sys.argv)<=2 or sys.argv[2] != "-noshow":
    plt.show()



# slow! maybe increase stride
# ^CTraceback (most recent call last):
#   File "/home/boeshpa/sequence-learning-with-GeNN/hypercolumn_BCPNN_fast/plot_spikes_hypercolumn.py", line 84, in <module>
#     firing_rate[i,t_id] = calculate_firing_rate(spike_times, t, t+t_window)
#   File "/home/boeshpa/sequence-learning-with-GeNN/hypercolumn_BCPNN_fast/plot_spikes_hypercolumn.py", line 47, in calculate_firing_rate
#     spikes_in_window = [spike for spike in spike_times if time_window_start <= spike <= time_window_end]
#   File "/home/boeshpa/sequence-learning-with-GeNN/hypercolumn_BCPNN_fast/plot_spikes_hypercolumn.py", line 47, in <listcomp>
#     spikes_in_window = [spike for spike in spike_times if time_window_start <= spike <= time_window_end]
# KeyboardInterrupt

