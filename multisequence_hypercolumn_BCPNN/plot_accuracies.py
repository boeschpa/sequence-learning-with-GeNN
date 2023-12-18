import pandas as pd
import matplotlib.pyplot as plt

# Load accuracies into a DataFrame
df = pd.read_csv('accuracies.csv')
print(df)

# Grouping by "sequence_length" and calculating mean and standard deviation
grouped_df = df.groupby('sequence_length').agg({'accuracy': ['mean', 'std']}).reset_index()

# Plotting the mean and standard deviation
plt.errorbar(grouped_df['sequence_length'], grouped_df['accuracy']['mean'],
             yerr=grouped_df['accuracy']['std'], fmt='o-', label='Mean with Std Dev')

plt.xlabel('Sequence Length')
plt.ylabel('Accuracy')
plt.title('Mean Accuracy with Standard Deviation')
plt.legend()
plt.show()




