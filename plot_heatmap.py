"""
Heatmap and histogram of hash table bucket distribution.
Usage: python3 plot_heatmap.py
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv('bucket_load.csv')

load_counts = df['load'].value_counts().sort_index()

loads = load_counts.index.tolist()
counts = load_counts.values.tolist()

max_show = 20
filtered_loads = []
filtered_counts = []
other = 0

for l, c in zip(loads, counts):
    if l <= max_show:
        filtered_loads.append(l)
        filtered_counts.append(c)
    else:
        other += c

if other > 0:
    filtered_loads.append(max_show + 1)
    filtered_counts.append(other)

plt.figure(figsize=(12, 6))
plt.bar(filtered_loads, filtered_counts, edgecolor='black', color='steelblue')
plt.xlabel('Number of elements per bucket')
plt.ylabel('Number of buckets')
plt.title('Bucket Load Distribution')
plt.xticks(filtered_loads)
plt.grid(axis='y', alpha=0.3)
plt.tight_layout()
plt.savefig('load_distribution.png', dpi=150)
print("Saved: load_distribution.png")

sample_size = 2000
sample = df.head(sample_size)
rows = 40
cols = 50
heatmap_data = sample['load'].values[:rows*cols].reshape(rows, cols)

plt.figure(figsize=(14, 8))
plt.imshow(heatmap_data, aspect='auto', cmap='YlOrRd', interpolation='nearest')
plt.colorbar(label='Number of elements')
plt.xlabel('Bucket index (mod 50)')
plt.ylabel('Bucket group (50 per row)')
plt.title(f'Hash Table Heatmap (first {sample_size} buckets)')
plt.tight_layout()
plt.savefig('hash_heatmap.png', dpi=150)
print("Saved: hash_heatmap.png")

sorted_loads = np.sort(df['load'].values)
cumulative = np.arange(1, len(sorted_loads) + 1) / len(sorted_loads) * 100

plt.figure(figsize=(10, 6))
plt.plot(sorted_loads, cumulative, 'b-', linewidth=2)
plt.xlabel('Number of elements per bucket')
plt.ylabel('Cumulative percentage of buckets')
plt.title('Cumulative Distribution')
plt.grid(True, alpha=0.3)
plt.xlim(0, 10)
plt.tight_layout()
plt.savefig('cumulative_distribution.png', dpi=150)
print("Saved: cumulative_distribution.png")

print("\nStatistics:")
print(f"  Total buckets: {len(df):,}")
print(f"  Empty buckets: {(df['load'] == 0).sum():,}")
print(f"  Non-empty buckets: {(df['load'] > 0).sum():,}")
print(f"  Max load: {df['load'].max()}")
print(f"  Mean load (non-empty): {df[df['load'] > 0]['load'].mean():.2f}")