import numpy as np
import matplotlib.pyplot as plt
import sys

# Load data from the .dat file
data = np.loadtxt(sys.argv[1])

# Split the data into time (first column) and voltage (subsequent N columns)
time = data[:, 0]
N= np.shape(data)[1]-1
spikes = data[:, 1:N+1]  # Assuming the voltage columns are from 1 to N+1

ax = plt.gca()
# ax.set_prop_cycle('color',plt.cm.Spectral(np.linspace(0,1,N)))
# Create a plot for each voltage column

# for i in range(N):
#     plt.plot(time, [i+1 if spike!=0 else 0.0 for spike in spikes[:, i]], ".")

for i in range(N):
    #plt.plot(time[np.where(spikes[:,i] != 0)[0]], [i]*len(np.where(spikes[:,i] != 0)[0]),".")
    plt.plot(time, spikes,".")

# Add labels and a legend
plt.xlabel('Time (ms)')
plt.ylabel('Neuron')
plt.title('Spikes of Neuron N vs. Time')

# Show the plot or save it to a file
plt.show()