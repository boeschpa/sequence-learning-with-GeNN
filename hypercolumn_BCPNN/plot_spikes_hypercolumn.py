import numpy as np
import matplotlib.pyplot as plt
import sys

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
    
cpp_param = read_file("model_param.h")

# get params from model_param
param = parse_cpp_header(cpp_param)

# Load data from the .dat file
data = np.loadtxt(sys.argv[1])

# Split the data into time (first column) and voltage (subsequent N columns)
time = data[:, 0]
N= np.shape(data)[1]-1
spikes = data[:, 1:N+1]  # Assuming the voltage columns are from 1 to N+1

# neuron id is counted through (inner to outer loop) minicolumn, hyper_width, hyper_height
# neuron_id = minicolumn_id * N_pyarmidal + local_neuron_id
# minicolumn_id = hypercolumn_id * n_minicolumns + local_minicolumn_id
# todo check if time is unique to see if it is saved correctly

# split data, only keep local_neuron_id
local_spikes = spikes % param.N_pyramidal
local_mini = (spikes // param.N_pyramidal) % param.N_minicolumns
hyper_n = (spikes // param.N_pyramidal // param.N_minicolumns) % param.hyper_width
hyper_m = spikes // param.N_pyramidal // param.N_minicolumns // param.hyper_width

max_spikes= np.max(np.unique((spikes // param.N_pyramidal), return_counts=True)[1])
spikes_new = np.empty((param.hyper_height,param.hyper_width,param.N_minicolumns,max_spikes))
for m in range(param.hyper_height):
    for n in range(param.hyper_width):
        for i in range(param.N_minicolumns):
            indices = np.where((local_mini == i) & (hyper_n == n) & (hyper_m == m))
            spikes_new[m,n,i,:]=spikes[indices].resize(8)

plt.figure().set_figwidth(15)
ax = plt.gca()


plt.plot(time, spikes,".")

for i in range(param.hyper_height*param.hyper_width-1):
    plt.axhline(y = (i+1)*param.N_pyramidal*param.N_minicolumns, color = 'k', linestyle = '-') 
  

# Add labels and a legend
plt.xlabel('Time (ms)')
plt.ylabel('Neuron')
plt.title('Spikes of Neuron N vs. Time')

# Show plot
if len(sys.argv)<=2 or sys.argv[2] != "-noshow":
    plt.show()

plt.savefig("plot_spikes.png")