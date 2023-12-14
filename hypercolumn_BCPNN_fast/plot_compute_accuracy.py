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
    spikes_per_pattern = np.zeros((param.N_patterns,len(spikes)),dtype=bool)
    for sp_id, sp in enumerate(spikes_per_minicolumn): # iterate through spikes (indexed by MC)
        hc = sp//param.N_minicolumns # HC of spike
        for pt_id, pt in enumerate(sequence): # iterate through patterns in sequence
            if (spikes_local_minicoliumn[sp_id]==pt[hc]):  # if spike sp with hypercolumn hc is part of the current pattern pt
                spikes_per_pattern[pt_id][sp_id] = True # set index to true for this spike in this pattern
    return spikes_per_pattern  
    
def idToMC(spikes):
    spikes_per_minicolumn = spikes // param.N_pyramidal  #global minicolumn index (which mc does neuron/spike belong to)
    spikes_per_minicolumn = spikes_per_minicolumn.flatten() #remove extra dimension
    N_global_mc=param.hyper_height*param.hyper_width*param.N_minicolumns
    return np.eye(N_global_mc,dtype=bool)[spikes_per_minicolumn].T # from index vector to onehot encoded matrix
    

def calculate_firing_rate(spike_times, time_window_start, time_window_end):
    spikes_in_window = [spike for spike in spike_times if time_window_start <= spike <= time_window_end]
    firing_rate = len(spikes_in_window) / (time_window_end - time_window_start)
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

def sequence_list(firing_rate_mc,firing_rates,t_window):
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
    
    #discard learning epochs and 5 "transition" patterns
    patterns=patterns[param.N_epochs*param.N_patterns+4:]
    patterns_midpoint=patterns_midpoint[param.N_epochs*param.N_patterns+4:]

    # find one cycle (pattern 0 to pattern 0)
    cycle_start = -1
    cycle_end = -1
    for pat_id, pat in enumerate(patterns):
        if(cycle_start!=-1 and pat==0):
            cycle_start = pat_id
        elif(cycle_end!=-1 and pat==0):
            cycle_end = pat_id

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


# get firing rates per pattern
sim_time = time[-1]
time_start = 0
t_window = 20.0 #ms
stride = 10 #ms

if len(sys.argv)>=4:
    time_start = int(sys.argv[3])
if len(sys.argv)>=5:
    sim_time = int(sys.argv[4])

spikes_per_pattern = idToPattern(spikes,sequence)

dense_time = range(time_start,int(sim_time),stride)
firing_rate = np.zeros((param.N_patterns,len(dense_time)))

for i in range(param.N_patterns):
    for (t_id, t) in enumerate(dense_time):
        indices = spikes_per_pattern[i]
        spike_times = time[indices]
        firing_rate[i,t_id] = calculate_firing_rate(spike_times, t, t+t_window)

pattern_window = 10.0 / stride
patterns = pattern_list(firing_rate,pattern_window)
print(patterns)


# get firing rates per minicolumn
spikes_per_mc = idToMC(spikes)
N_hypercolumns = param.hyper_height*param.hyper_width

dense_time = range(time_start,int(sim_time),stride)
firing_rate_mc = np.zeros((N_hypercolumns*param.N_minicolumns,len(dense_time)))

for i in range(param.hyper_height*param.hyper_width*param.N_minicolumns):
    for (t_id, t) in enumerate(dense_time):
        indices = spikes_per_mc[i]
        spike_times = time[indices]
        firing_rate_mc[i,t_id] = calculate_firing_rate(spike_times, t, t+t_window)

sequence_recall = sequence_list(firing_rate_mc,firing_rate,pattern_window)

#plot
fig, ax = plt.subplots(1+param.hyper_height*param.hyper_width,1,sharex=True)
fig.set_size_inches(16,10)

# plot firing rates per pattern
ax[0].plot(np.tile(dense_time,(param.N_patterns,1)).T,firing_rate.T)
# Add labels and a legend
ax[-1].set_xlabel('Time (ms)')
ax[0].set_ylabel('Firing rate per Pattern')
ax[0].title.set_text('Firing rates')

# plot firing rates per mc
for i in range(N_hypercolumns):
    ax[i+1].plot(np.tile(dense_time,(param.N_minicolumns,1)).T,firing_rate_mc[param.N_minicolumns*i:param.N_minicolumns*(i+1)].T)
    ax[i+1].set_ylabel("Firing rate per minicolum in hypercolum "+str(i+1))

plt.savefig("plot_rates.png")

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

