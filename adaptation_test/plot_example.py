import matplotlib.pyplot as plt
import numpy as np
import sys

# Read data and zip into columns
pre_data = np.loadtxt("pre_trace.csv", delimiter=",")
pre_spikes_data = np.loadtxt("pre_spikes.csv")
pre_stim_data = np.loadtxt("pre_stim.csv")

# Convert CSV columns to numpy
pre_times = pre_data[:,0]
pre_v = pre_data[:,1]
pre_iw = pre_data[:,2]

pre_spike_times = pre_spikes_data[:,0]
pre_stim_times = pre_stim_data[:,0]

# Create plot
figure, axes = plt.subplots(2,1,sharex = True)
figure.set_figwidth(10)
figure.set_figheight(6)

axes[0].plot(pre_times, pre_v, label="Vmem Pre")
axes[1].plot(pre_times, pre_iw, label="Ga Pre")
axes[0].vlines(pre_spike_times, -50.0, -40.0, color="red", label="Pre spikes")
axes[0].vlines(pre_stim_times, -40.0, -30.0, color="blue", label="Pre stimulus spikes")

axes[1].set_xlabel("Time [ms]")
axes[0].legend()
axes[1].legend()


for ax in axes:
    ax.grid(which='major', color='#DDDDDD', linewidth=0.8)
    ax.grid(which='minor', color='#EEEEEE', linestyle=':', linewidth=0.5)
    ax.minorticks_on()

# Show plot
if len(sys.argv)<=1 or sys.argv[1] != "-noshow":
    plt.show()

plt.savefig("plot_example.pdf")
