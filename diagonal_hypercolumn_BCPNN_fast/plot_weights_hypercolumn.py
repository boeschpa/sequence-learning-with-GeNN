import numpy as np
import matplotlib.pyplot as plt
import sys

import re

# arguments: 0: file
#            1: -noshow


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
N2_hyper = param.hyper_width*param.hyper_height*param.hyper_width*param.hyper_height
data = np.reshape(data.T,(param.N_pyramidal*param.N_pyramidal,N2_hyper,param.N_minicolumns,param.N_minicolumns),order='F')
data = np.transpose(data, (2, 3, 1, 0))

# make squares square
onesquare=data[0,0,:,:]
side = int(np.ceil(np.sqrt(param.N_pyramidal*param.N_pyramidal*N2_hyper)))
data = np.reshape(data,(param.N_minicolumns,param.N_minicolumns,param.N_pyramidal*param.N_pyramidal*N2_hyper))
data = np.pad(data,((0,0),(0,0),(0,side*side-(param.N_pyramidal*param.N_pyramidal*N2_hyper))))
data = np.reshape(data,(param.N_minicolumns,param.N_minicolumns,side,side))

#plot
grid = np.hstack(np.hstack(data))
range = np.max([np.max(grid),-np.min(grid)])
#range = np.max([np.max(onesquare),-np.min(onesquare)])
im = plt.imshow(grid,cmap='RdBu',vmin = -range, vmax=range,interpolation='none')
cbar = plt.colorbar(im, extend='both')#,orientation='horizontal')
cbar.minorticks_on()
# Add labels and a legend
plt.savefig("weights.png")

# Show plot
if len(sys.argv)<=2 or sys.argv[2] != "-noshow":
    plt.show()

