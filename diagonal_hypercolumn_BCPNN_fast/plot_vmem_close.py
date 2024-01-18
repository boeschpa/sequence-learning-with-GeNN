import numpy as np
import matplotlib.pyplot as plt
import sys
from matplotlib import cm
import matplotlib.ticker as ticker
import matplotlib.pylab as pylab

import re

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

## TRACE
# Load data from the .dat file
traceData = np.loadtxt('trace_vmem.csv',delimiter=",")

# Split the data into time (first column) and voltage (subsequent N columns)
traceTime = traceData[:, 0]
N= np.shape(traceData)[1]-1
traces = traceData[:, 5]  # Select number of signals
mean_trace = np.mean(traceData[:,1:31], axis = 1)

## RASTER + FIRNG RATE
# Load data from the .dat file
data = np.loadtxt("output.spikes.csv")
dataBasket = np.loadtxt("output.basketSpikes.csv")

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

# get hypercolumn 0
time, spikes = zip(*[(spike_time, spike) for spike_time, spike in zip(time, spikes) if spike < 300])
spikes = np.squeeze(np.asarray(spikes))
timeBasket, spikesBasket = zip(*[(spike_time, spike) for spike_time, spike in zip(timeBasket, spikesBasket) if spike < 30])

# get firing rates
i=0
sim_time = np.max(timeBasket)
time_start = 0
t_window = 10.0 #ms
bin_size = 2 #ms
dense_time = range(time_start,int(sim_time),bin_size)
sequence_length= param.N_patterns

firing_rate_basket = compute_rolling_average_spike_rate(timeBasket,sim_time,bin_size,int(t_window/bin_size),param.N_basket)

## PLOT
params = {'legend.fontsize': 'large',
         'axes.labelsize': 'large',
         'axes.titlesize':'large',
         'xtick.labelsize':'large',
         'ytick.labelsize':'large'}
pylab.rcParams.update(params)

figure, ax = plt.subplots(4,1,sharex = True)
figure.set_size_inches(10,10)
ax[0].plot(traceTime, traces)
ax[0].set_ylabel("Membrane\npotential (mV)")
plt.xlabel('Time (ms)')
ax[1].plot(traceTime, mean_trace)
ax[1].set_ylabel("Average membrane\npotential (mV)")
plt.xlabel('Time (ms)')

# plot spikes
colors = np.asarray([int(spike // 30) for spike in spikes])
ax[2].scatter(time, spikes, s=4, c=colors, cmap="tab10",edgecolor='none')
ax[2].set_ylabel('Pyramidal\nneuron index')

# plot basket raster and firing rate
ax[3].scatter(timeBasket, spikesBasket, s=4 ,c = 'k',edgecolor='none')
ax[3].set_ylabel('Basket\nneuron index')
ax2 = ax[3].twinx()  # instantiate a second axes that shares the same x-axis
ax2.set_ylabel('Average firing\nrate (Hz)', color='r')  # we already handled the x-label with ax1
ax2.tick_params(axis='y', labelcolor='r')
ax2.plot(np.asarray(dense_time),firing_rate_basket,color='r')

# Add labels and a legend
ax[3].set_xlabel('Time (ms)')
ax[3].set_ylabel('Basket\nneuron index')

#set range and ticks
range = (8280,8620)
ax[3].set_xlim(range)
ax[3].xaxis.set_major_locator(ticker.MultipleLocator(base=50)) 
ax[3].xaxis.set_minor_locator(ticker.AutoMinorLocator())

for axis in ax:
    axis.tick_params(which='major', axis='x', length=7)
    axis.tick_params(which='minor', length=3)

# add letters
ax[0].annotate("A", xy=(0, 0), xytext=(-0.1, 1.05), xycoords='axes fraction', ha='right', va='top', fontsize=14, fontweight='bold')
ax[1].annotate("B", xy=(0, 0), xytext=(-0.1, 1.05), xycoords='axes fraction', ha='right', va='top', fontsize=14, fontweight='bold')
ax[2].annotate("C", xy=(0, 0), xytext=(-0.1, 1.05), xycoords='axes fraction', ha='right', va='top', fontsize=14, fontweight='bold')
ax[3].annotate("D", xy=(0, 0), xytext=(-0.1, 1.05), xycoords='axes fraction', ha='right', va='top', fontsize=14, fontweight='bold')



plt.tight_layout()
plt.savefig("vmem_close.png",dpi=600)

plt.show()