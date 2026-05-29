import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('search_times.csv')
plt.figure(figsize=(12, 8))
plt.xscale('log')
plt.yscale('log')
plt.xlabel('Размер массива (N)')
plt.ylabel('Среднее время поиска (мкс)')
for col in ['linear_us', 'bst_us', 'rbt_us', 'hash_us', 'multimap_us']:
    plt.plot(df['size'], df[col], marker='o', label=col.replace('_us', ''))
plt.legend()
plt.grid(True)
plt.title('Сравнение времени поиска')
plt.savefig('search_comparison.png')
plt.show()

# График коллизий
plt.figure()
plt.plot(df['size'], df['collisions'], 'r-o')
plt.xlabel('Размер массива')
plt.ylabel('Количество коллизий')
plt.title('Зависимость коллизий от размера массива')
plt.grid(True)
plt.savefig('collisions.png')
plt.show()