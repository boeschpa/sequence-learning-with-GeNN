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
post_pj = post_data[:,5]
post_zj = post_data[:,6]
post_ib = post_data[:,7]

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
figure, axes = plt.subplots(7,1,sharex = True)
figure.set_figwidth(10)
figure.set_figheight(10)

axes[0].plot(pre_times, pre_z, label="Zi", color="red")
axes[0].plot(post_times, post_z, label="Zj", color="blue")
axes[1].plot(pre_times, pre_p, label="Pi", color="red")
axes[1].plot(post_times, post_p, label="Pj", color="blue")
axes[1].plot(times, pij, label="Pij", color="black")
axes[2].plot(times, w, label="g", color="black")
axes[3].plot(post_times, post_pj, label="post Zj", color="blue")
axes[4].plot(post_times, post_zj, label="post Pj", color="blue")
axes[5].plot(post_times, post_ib, label="post Ib", color="blue")

#axes[0].set_ylabel("Neuron number")
#axes[1].set_ylabel("Mean firing rate [Hz]")

axes[6].vlines(pre_spike_times, -5.0, -4.0, color="red", label="Pre spikes")
axes[6].vlines(post_spike_times, -4.75, -3.75, color="blue", label="Post spikes")

axes[6].set_xlabel("Time [ms]")
axes[0].legend()
axes[1].legend()
axes[2].legend()
axes[3].legend()
axes[4].legend()
axes[5].legend()
axes[6].legend()

for ax in axes:
    ax.grid()

# Show plot
if len(sys.argv)<=1 or sys.argv[1] != "-noshow":
    plt.show()

plt.savefig("plot_example.pdf")
