import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import ast
import matplotlib.pylab as pylab

def edit_distance(seq1, seq2):
    len_seq1 = len(seq1)
    len_seq2 = len(seq2)

    # Initialize a matrix to store the edit distances
    dp = [[0] * (len_seq2 + 1) for _ in range(len_seq1 + 1)]

    # Initialize the first row and column of the matrix
    for i in range(len_seq1 + 1):
        dp[i][0] = i
    for j in range(len_seq2 + 1):
        dp[0][j] = j

    # Fill in the matrix using dynamic programming
    for i in range(1, len_seq1 + 1):
        for j in range(1, len_seq2 + 1):
            if seq1[i - 1] == seq2[j - 1]:
                dp[i][j] = dp[i - 1][j - 1]
            else:
                dp[i][j] = 1 + min(dp[i - 1][j],      # Deletion
                                  dp[i][j - 1],      # Insertion
                                  dp[i - 1][j - 1])  # Substitution

    return dp[len_seq1][len_seq2]

dataframes = ["data_5.pkl","data_pmod.pkl"]
dist_grouped_df = []
acc_grouped_df = []

for id, dfFile in enumerate(dataframes):
        
    # Load accuracies into a DataFrame
    df = pd.read_pickle(dfFile)

    if(id==1): #duplicate df to plot n=1 data
        df = pd.concat([df, df], ignore_index=True)

    df["sequence_short"]=""
    df["mini_sequence_recall_short"]=""
    df["sequence_edit_distance"]=""
    df["pattern_accuracy"]=""

    # iterate through rows
    for index, row in df.iterrows():
        # find last sequence
        cycle_start = 0
        cycle_end = -1
        # backwards go from 0 to 0 in sequence list (if no 0 is present, try 1,2,3...)
        if(len(row['sequence'])==0):
            row['sequence']= [-1]
        start_pattern = row['sequence'][-1]
        for pat_id, pat in reversed(list(enumerate(row['sequence']))):
            if(cycle_end==-1 and pat==start_pattern):
                cycle_end = pat_id
            elif(cycle_start==0 and pat==start_pattern):
                cycle_start = pat_id

        df.at[index,'sequence_short']=row['sequence'][cycle_start:cycle_end]
        df.at[index,'mini_sequence_recall_short']=row['mini_sequence_recall'][cycle_start:cycle_end]

        # find sequence indices edit distance
        optimal_sequence = np.roll(list(range(df.at[index,'sequence_length'])),-start_pattern)
        df.at[index,'sequence_edit_distance']=edit_distance(df.at[index,'sequence_short'],optimal_sequence)

        # dominant pattern accuracy over all hypercolumns
        patterns = df.at[index,'mini_sequence_recall_short']
        optimal_patterns = df.at[index,'mini_sequence'][df.at[index,'sequence_short']]
        if(len(patterns)==0):
            accuracy = np.NaN
        else : accuracy = (optimal_patterns == patterns).mean()
        df.at[index,'pattern_accuracy']=accuracy

    print(df)

    # Grouping by "sequence_length" and calculating mean and standard deviation
    dist_grouped_df.append(df.groupby('sequence_length').agg({'sequence_edit_distance': ['mean', 'std', 'count']}).reset_index())
    acc_grouped_df.append(df.groupby('sequence_length').agg({'pattern_accuracy': ['mean', 'std', 'count']}).reset_index())

## PLOT
params = {'legend.fontsize': 'large',
         'axes.labelsize': 'large',
         'axes.titlesize':'large',
         'xtick.labelsize':'large',
         'ytick.labelsize':'large'}
pylab.rcParams.update(params)

figure, ax = plt.subplots(1,1,sharex = True, sharey = True)
ax=[ax]
figure.set_size_inches(6,4)

# Plotting the mean and standard deviation

ax[0].errorbar(dist_grouped_df[0]['sequence_length'], dist_grouped_df[0]['sequence_edit_distance']['mean'],
             yerr=dist_grouped_df[0]['sequence_edit_distance']['std'], fmt='o-', label=r'$\tau_p=5 s$ (N=5)')
ax[0].errorbar(dist_grouped_df[1]['sequence_length'], dist_grouped_df[1]['sequence_edit_distance']['mean'],
             yerr=dist_grouped_df[1]['sequence_edit_distance']['std'], fmt='o-', label=r'$\tau_p=20 s$ (N=1)')

for axis in ax:
    axis.set_xlabel('Sequence length')
    axis.grid(axis='y', linestyle='--')
    axis.xaxis.set_tick_params(labelbottom=True)
ax[0].set_ylabel('Edit distance')

ax[0].legend()

# add letters
# ax[0].annotate("A", xy=(0, 0), xytext=(-0.08, 1.1), xycoords='axes fraction', ha='right', va='top', fontsize=14, fontweight='bold')
# ax[1].annotate("B", xy=(0, 0), xytext=(-0.08, 1.1), xycoords='axes fraction', ha='right', va='top', fontsize=14, fontweight='bold')
# ax[2].annotate("C", xy=(0, 0), xytext=(-0.08, 1.1), xycoords='axes fraction', ha='right', va='top', fontsize=14, fontweight='bold')

# ax[0].set_title("\tau_p=5 s (N=5)", fontweight = 'bold') 
# ax[1].set_title("\tau_p=20 s (N=1)", fontweight = 'bold')

#figure.set_facecolor("none")

plt.tight_layout()
plt.savefig("taup_edit_distance.png")

plt.show()





