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
data = np.loadtxt(sys.argv[1],delimiter=",")

# Split the data into time (first column) and voltage (subsequent N columns)
time = data[:, 0]
N= np.shape(data)[1]-1
traces = data[:, 1:N+1]  # Assuming the voltage columns are from 1 to N+1

figure, axes = plt.subplots(4,1,sharex = True)
labels=["g 0-0", "g 0-0", "g 0-1", "g 0-1", "g 1-0", "g 1-0"]
labels_p = ["pi", "pj", "pij"]
labels_z = ["zi", "zj"]
assert(len(labels)+len(labels_p)+len(labels_z)==N)

for i in range(len(labels)):
    axes[0].plot(time, traces[:,i],label=labels[i])
for i in range(len(labels_p)):
    axes[1].plot(time, traces[:,i+len(labels)],label=labels_p[i])
axes[2].plot(time, traces[:,0+len(labels)+len(labels_p)],label=labels_z[0])
axes[3].plot(time, traces[:,1+len(labels)+len(labels_p)],label=labels_z[1])

axes[0].legend()
axes[1].legend()
axes[2].legend()
axes[3].legend()


# Add labels and a legend
plt.xlabel('Time (ms)')
#plt.ylabel('Conductance (nS)')
#plt.title('Synapse Strenght vs. Time')


# Show plot
if len(sys.argv)<=3 or sys.argv[3] != "-noshow":
    plt.show()

plt.savefig(sys.argv[2])