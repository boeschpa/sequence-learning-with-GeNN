import numpy as np

spikes = [1,1,1,1,1,2,2,2,3,6,1,2,3,4,6,1,5]
max_spikes= np.max(np.unique(spikes, return_counts=True)[1])
print(max_spikes)
print(np.unique(spikes,return_counts=True))