"""
Сравнение хэш-функций: коллизии и время поиска.
Запуск: python plot_hash_functions.py
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv('hash_functions_results.csv')

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

# Цветовая схема
colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728']

# График 1: Коллизии по функциям (столбчатая диаграмма)
x = np.arange(len(df['hash_function']))
bars1 = ax1.bar(x, df['collisions'], color=colors, edgecolor='black')
ax1.set_xticks(x)
ax1.set_xticklabels(df['hash_function'], fontsize=12)
ax1.set_ylabel('Количество коллизий', fontsize=12)
ax1.set_title('Сравнение хэш-функций: коллизии', fontsize=11)
ax1.grid(True, alpha=0.3, axis='y')

# Добавляем значения над столбцами
for bar, v in zip(bars1, df['collisions']):
    ax1.text(bar.get_x() + bar.get_width()/2, v + 5, str(v), 
             ha='center', va='bottom', fontsize=9)

# График 2: Время поиска (столбчатая диаграмма)
bars2 = ax2.bar(x, df['search_time_us'], color=colors, edgecolor='black')
ax2.set_xticks(x)
ax2.set_xticklabels(df['hash_function'], fontsize=12)
ax2.set_ylabel('Время поиска (микросекунды)', fontsize=12)
ax2.set_title('Сравнение хэш-функций: время поиска', fontsize=11)
ax2.grid(True, alpha=0.3, axis='y')

# Добавляем значения над столбцами
for bar, v in zip(bars2, df['search_time_us']):
    ax2.text(bar.get_x() + bar.get_width()/2, v + 0.05, f"{v:.2f}", 
             ha='center', va='bottom', fontsize=9)

plt.tight_layout()
plt.savefig('hash_functions_plot.png', dpi=150, bbox_inches='tight')
plt.show()

print("\nСводная таблица результатов")
print(df[['hash_function', 'collisions', 'search_time_us']].to_string(index=False))