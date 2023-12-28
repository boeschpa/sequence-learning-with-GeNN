import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import ast

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

# Example usage:
sequence1 = [1, 2, 3, 4, 5]
sequence2 = [2, 4, 6, 8, 10]
distance = edit_distance(sequence1, sequence2)
print(f"Edit distance between {sequence1} and {sequence2}: {distance}")



# Load accuracies into a DataFrame
df = pd.read_pickle('data.csv')

df["sequence_short"]=""
df["mini_sequence_recall_short"]=""
df["sequence_edit_distance"]=""
df["pattern_accuracy"]=""

# iterate through rows
for index, row in df.iterrows():
    # find last sequence
    cycle_start = -1
    cycle_end = -1
    # backwards go from 0 to 0 in sequence list (if no 0 is present, try 1,2,3...)
    start_pattern = row['sequence'][-2]
    for pat_id, pat in reversed(list(enumerate(row['sequence']))):
        if(cycle_end==-1 and pat==start_pattern):
            cycle_end = pat_id
        elif(cycle_start==-1 and pat==start_pattern):
            cycle_start = pat_id

    df.at[index,'sequence_short']=row['sequence'][cycle_start:cycle_end]
    df.at[index,'mini_sequence_recall_short']=row['mini_sequence_recall'][cycle_start:cycle_end]

    # find sequence indices edit distance
    optimal_sequence = np.roll(list(range(df.at[index,'sequence_length'])),-start_pattern)
    df.at[index,'sequence_edit_distance']=edit_distance(df.at[index,'sequence_short'],optimal_sequence)

    # dominant pattern accuracy over all hypercolumns
    patterns = df.at[index,'mini_sequence_recall_short']
    optimal_patterns = df.at[index,'mini_sequence'][df.at[index,'sequence_short']]
    accuracy = (optimal_patterns == patterns).mean()
    df.at[index,'pattern_accuracy']=accuracy

print(df)

print(df.iloc[9]['sequence_short'])
print(df.iloc[9]['sequence'])

# Grouping by "sequence_length" and calculating mean and standard deviation
dist_grouped_df = df.groupby('sequence_length').agg({'sequence_edit_distance': ['mean', 'std', 'count']}).reset_index()
acc_grouped_df = df.groupby('sequence_length').agg({'pattern_accuracy': ['mean', 'std', 'count']}).reset_index()



# Plotting the mean and standard deviation
plt.figure(0)
plt.errorbar(dist_grouped_df['sequence_length'], dist_grouped_df['sequence_edit_distance']['mean'],
             yerr=dist_grouped_df['sequence_edit_distance']['std'], fmt='o-', label='Edit Distance Mean with Std Dev')

plt.xlabel('Sequence Length')
plt.ylabel('Edit Distance')
plt.title(f"Mean Edit Distance with Standard Deviation (N={dist_grouped_df.iloc[0]['sequence_edit_distance']['count']})")
plt.savefig("edit_distance.png")

plt.figure(1)
plt.errorbar(acc_grouped_df['sequence_length'], acc_grouped_df['pattern_accuracy']['mean'],
             yerr=acc_grouped_df['pattern_accuracy']['std'], fmt='o-', label='Pattern Accuracy Mean with Std Dev')

plt.xlabel('Sequence Length')
plt.ylabel('Pattern Accuracy')
plt.title(f"Pattern Accuracy Distance with Standard Deviation (N={acc_grouped_df.iloc[0]['pattern_accuracy']['count']})")
plt.savefig("accuracy.png")
plt.show()





