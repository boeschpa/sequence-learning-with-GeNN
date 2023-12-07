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
    
def calculate_firing_rate(spike_times, time_window_start, time_window_end):
    spikes_in_window = [spike for spike in spike_times if time_window_start <= spike <= time_window_end]
    firing_rate = len(spikes_in_window) / (time_window_end - time_window_start)
    return firing_rate

cpp_param = read_file("model_param.h")

# get params from model_param
param = parse_cpp_header(cpp_param)

# Load data from the .dat file
data = np.loadtxt(sys.argv[1])

# Split the data into time (first column) and voltage (subsequent N columns)
time = data[:, 0]
N= np.shape(data)[1]-1
spikes = data[:, 1:N+1]  # Assuming the voltage columns are from 1 to N+1

i=0
sim_time = time[-1]
time_start = 0
t_window = 10.0 #ms

if len(sys.argv)>=4:
    time_start = int(sys.argv[3])
if len(sys.argv)>=5:
    sim_time = int(sys.argv[4])

spikes_per_minicolumn = spikes // param.N_pyramidal
spikes_per_minicolumn_index = spikes_per_minicolumn % param.N_minicolumns

dense_time = range(time_start,int(sim_time),1)
firing_rate = np.zeros((param.N_minicolumns,len(dense_time)))

for i in range(param.N_minicolumns):
    for (t_id, t) in enumerate(dense_time):
        indices = np.where(spikes_per_minicolumn_index == i)[0]
        spike_times = time[indices]
        firing_rate[i,t_id] = calculate_firing_rate(spike_times, t, t+t_window)
#plot

fig, ax = plt.subplots(2,1,sharex=True)
fig.set_size_inches(16,16)

# plot spikes
if (sim_time != time[-1] and time_start != 0):
    spike_id = np.where(np.logical_and(time<sim_time, time > time_start))[0]
    ax[0].plot(time[spike_id], spikes[spike_id],".")
else:
    ax[0].plot(time, spikes,".")

for i in range(param.hyper_height*param.hyper_width-1):
    ax[0].axhline(y = (i+1)*param.N_pyramidal*param.N_minicolumns, color = 'k', linestyle = '-') 

# plot firing rates
ax[1].plot(np.tile(dense_time,(param.N_minicolumns,1)).T,firing_rate.T)


# Add labels and a legend
ax[1].set_xlabel('Time (ms)')
ax[0].set_ylabel('Neuron')
ax[0].title.set_text('Spikes of Neuron N vs. Time')

plt.savefig("plot_spikes.png")

# Show plot
if len(sys.argv)<=2 or sys.argv[2] != "-noshow":
    plt.show()

