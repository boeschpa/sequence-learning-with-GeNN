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




cpp_param = read_file("model_param.h")

# get params from model_param
param = parse_cpp_header(cpp_param)

# Load data from the .dat file
data = np.loadtxt("output.spikes_0.csv")
dataBasket = np.loadtxt("output.basketSpikes_0.csv")

# Load sequence
sequence = np.loadtxt("sequence.csv",delimiter=",",dtype=int,ndmin=2)

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
sim_time = np.max(time)
sim_time_basket = np.max(timeBasket)
time_start = 0
t_window = 50.0 #ms
bin_size = 10 #ms
dense_time = range(time_start,int(sim_time),bin_size)
dense_time_basket = range(time_start,int(sim_time_basket),bin_size)

# firing rate per pattern in pyramidal neurons
sequence_length= param.N_patterns
spikes_per_pattern = idToPattern(spikes,sequence)

firing_rate_pyramidal = compute_rolling_average_spike_rate(time,sim_time,bin_size,int(t_window/bin_size),param.N_pyramidal*param.N_minicolumns*param.hyper_height*param.hyper_width)
firing_rate_basket = compute_rolling_average_spike_rate(timeBasket,sim_time_basket,bin_size,int(t_window/bin_size),param.N_basket*param.hyper_height*param.hyper_width)

# firing rate per pattern in pyramidal neurons
spikes_per_pattern = idToPattern(spikes,sequence)
firing_rate = np.zeros((param.N_patterns,len(dense_time)))

for i in range(sequence_length):
    indices = spikes_per_pattern[i]
    spike_times = time[indices]
    firing_rate[i,:] = compute_rolling_average_spike_rate(spike_times,sim_time,bin_size,int(t_window/bin_size),param.N_pyramidal*param.hyper_height*param.hyper_width)

min_pattern_length = 50.0 / bin_size
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
ax[2].plot(dense_time_basket,firing_rate_basket,label="basket",color='r')
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

# Show plot
if len(sys.argv)<=1 or sys.argv[1] != "-noshow":
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

