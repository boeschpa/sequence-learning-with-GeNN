import numpy as np
import matplotlib.pyplot as plt
import sys
import math
import matplotlib.pylab as pylab

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
    
def read_weights(file_path):
    # Read the lines from the text file
    with open(file_path, 'r') as file:
        lines = file.readlines()
    # Convert each line (containing numbers separated by commas) to a NumPy array
    weights = [list(map(float, line.strip().split(','))) for line in lines]
    
    return weights


cpp_param = read_file("model_param.h")

# get params from model_param
param = parse_cpp_header(cpp_param)

# Load data from the .dat file; each row includes conductivities of one 
data_ampa = read_weights("weights_ampa.csv")
data_nmda = read_weights("weights_nmda.csv")

#find max len
lengths = []
for lst in data_ampa:
    lengths.append(len(lst))
for lst in data_nmda:
    lengths.append(len(lst))
max_length = max(lengths)

square_side = math.ceil(math.sqrt(max_length))

grid_ampa = np.zeros((param.N_minicolumns,param.N_minicolumns,square_side,square_side)) 
grid_nmda = np.zeros((param.N_minicolumns,param.N_minicolumns,square_side,square_side)) 

for row in range(param.N_minicolumns):
    for col in range(param.N_minicolumns):
        lst = data_ampa[col + row * param.N_minicolumns]
        # Calculate the number of zeros needed for padding
        num_zeros = max(0, square_side**2 - len(lst))
        # Pad the list with zeros
        lst = lst + [0] * num_zeros

        grid_ampa[row,col,:,:] = np.reshape(lst,(square_side,square_side))

for row in range(param.N_minicolumns):
    for col in range(param.N_minicolumns):
        lst = data_nmda[col + row * param.N_minicolumns]
        # Calculate the number of zeros needed for padding
        num_zeros = max(0, square_side**2 - len(lst))
        # Pad the list with zeros
        lst = lst + [0] * num_zeros

        grid_nmda[row,col,:,:] = np.reshape(lst,(square_side,square_side))

        

#plot
params = {'legend.fontsize': 'large',
         'axes.labelsize': 'large',
         'axes.titlesize':'large',
         'xtick.labelsize':'large',
         'ytick.labelsize':'large'}
pylab.rcParams.update(params)

grid1 = np.hstack(np.hstack(grid_ampa))
grid2 = np.hstack(np.hstack(grid_nmda))
range1 = np.max([ np.max(grid1), -np.min(grid1) ])
range2 = np.max([ np.max(grid2), -np.min(grid2) ])

figure, ax = plt.subplots(1,2)
figure.set_size_inches(10,6)

im0 = ax[0].imshow(-grid1,cmap='RdBu',vmin = -range1, vmax=range1,interpolation='none')
im1 = ax[1].imshow(-grid2,cmap='RdBu',vmin = -range2, vmax=range2,interpolation='none')

# add colorbars
cbar0 = plt.colorbar(im0, orientation='vertical', label="Synapse conductivity (nS)",fraction=0.046, pad=0.04)
cbar0.minorticks_on()
cbar1 = plt.colorbar(im1, orientation='vertical', label="Synapse conductivity (nS)",fraction=0.046, pad=0.04)
cbar1.minorticks_on()

# add grid lines
for i in range(param.N_minicolumns-1):
    ax[0].axhline(y = (i+1)*square_side-0.5, color = 'k', linestyle = '-', linewidth = 1) # todo thinner
    ax[1].axhline(y = (i+1)*square_side-0.5, color = 'k', linestyle = '-', linewidth = 1) 
    ax[0].axvline(x = (i+1)*square_side-0.5, color = 'k', linestyle = '-', linewidth = 1)
    ax[1].axvline(x = (i+1)*square_side-0.5, color = 'k', linestyle = '-', linewidth = 1) 

#ticks every square_side in range(param.N_minicolumns)
ax[0].set_xticks(range(square_side//2, square_side*param.N_minicolumns+square_side//2, square_side),range(param.N_minicolumns))
ax[1].set_xticks(range(square_side//2, square_side*param.N_minicolumns+square_side//2, square_side),range(param.N_minicolumns))
ax[0].set_yticks(range(square_side//2, square_side*param.N_minicolumns+square_side//2, square_side),range(param.N_minicolumns))
ax[1].set_yticks(range(square_side//2, square_side*param.N_minicolumns+square_side//2, square_side),range(param.N_minicolumns))

ax[0].set_xlabel("Target minicolumn index")
ax[1].set_xlabel("Target minicolumn index")
ax[0].set_ylabel("Projecting minicolumn index")
ax[1].set_ylabel("Projecting minicolumn index")
ax[0].set_ylim((param.N_minicolumns*square_side-0.5,0-0.5))
ax[1].set_ylim((param.N_minicolumns*square_side-0.5,0-0.5))
ax[0].set_xlim((0-0.5,param.N_minicolumns*square_side-0.5))
ax[1].set_xlim((0-0.5,param.N_minicolumns*square_side-0.5))

ax[0].set_title("AMPA") # title??
ax[1].set_title("NMDA") # title??

ax[0].annotate("A", xy=(0, 0), xytext=(-0.07, 1.07), xycoords='axes fraction', ha='right', va='top', fontsize=14, fontweight='bold')
ax[1].annotate("B", xy=(0, 0), xytext=(-0.07, 1.07), xycoords='axes fraction', ha='right', va='top', fontsize=14, fontweight='bold')

plt.tight_layout(pad=3.0)

# Add labels and a legend
plt.savefig("weights_slide.png",dpi=600)

# Show plot
if len(sys.argv)<=2 or sys.argv[2] != "-noshow":
    plt.show()

