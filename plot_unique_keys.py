"""
График зависимости времени поиска от числа уникальных ключей.
Запуск: python plot_unique_keys.py
"""

import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('unique_keys_results.csv')

plt.figure(figsize=(10, 6))
plt.xscale('log')  # логарифмическая шкала по X (число уникальных ключей)
plt.yscale('log')  # логарифмическая шкала по Y (время)

plt.plot(df['unique_keys'], df['bst_time_us'], 'o-', label='BST', linewidth=2, markersize=8)
plt.plot(df['unique_keys'], df['rbt_time_us'], 's-', label='RBT', linewidth=2, markersize=8)
plt.plot(df['unique_keys'], df['hash_time_us'], '^-', label='Hash Table', linewidth=2, markersize=8)

plt.xlabel('Количество уникальных ключей', fontsize=12)
plt.ylabel('Время поиска (микросекунды)', fontsize=12)
plt.title('Зависимость времени поиска от числа уникальных ключей\n(размер массива = 1 000 000 элементов)', fontsize=14)
plt.legend()
plt.grid(True, alpha=0.3)

plt.savefig('unique_keys_plot.png', dpi=150, bbox_inches='tight')
plt.show()

print("График сохранён как unique_keys_plot.png")