import matplotlib.pyplot as plt
import numpy as np
import sys

# Read data and zip into columns
pre_data = np.loadtxt("pre_trace.csv", delimiter=",")
post_data = np.loadtxt("post_trace.csv", delimiter=",")
pre_spikes_data = np.loadtxt("pre_spikes.csv")
post_spikes_data = np.loadtxt("post_spikes.csv")

# Convert CSV columns to numpy
pre_times = pre_data[:,0]
pre_w = pre_data[:,3]
pre_z = pre_data[:,1]
pre_p = pre_data[:,2]
pre_pij = pre_data[:,4]

post_times = post_data[:,0]
post_w = post_data[:,3]
post_z = post_data[:,1]
post_p = post_data[:,2]
post_pij = post_data[:,4]

pre_spike_times = pre_spikes_data[:,0]
post_spike_times = post_spikes_data[:,0]

# Combine weights and times from pre and postsynaptic traces
times = np.hstack((pre_times, post_times))
w = np.hstack((pre_w, post_w))
order = np.argsort(times)
times = times[order]
w = w[order]

pij = np.hstack((pre_pij, post_pij))
pij = pij[order]

# Create plot
figure, axis = plt.subplots(3,1)

axis[0].plot(pre_times, pre_z, label="Zi*", color="red")
axis[0].plot(post_times, post_z, label="Zj*", color="blue")
axis[1].plot(pre_times, pre_p, label="Pi*", color="red")
axis[1].plot(post_times, post_p, label="Pj*", color="blue")
axis[1].plot(times, pij, label="Pij*", color="black")
axis[2].plot(times, w, label="g", color="black")

#axes[0].set_ylabel("Neuron number")
#axes[1].set_ylabel("Mean firing rate [Hz]")

axis[2].vlines(pre_spike_times, -5.0, -4.0, color="red", label="Pre spikes")
axis[2].vlines(post_spike_times, -4.75, -3.75, color="blue", label="Post spikes")

axis[2].set_xlabel("Time [ms]")
axis[0].legend()
axis[1].legend()
axis[2].legend()

# Show plot
if len(sys.argv)<=1 or sys.argv[1] != "-noshow":
    plt.show()

plt.savefig("plot_example.pdf")
