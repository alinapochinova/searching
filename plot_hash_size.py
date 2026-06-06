"""
Графики зависимости коллизий и времени поиска от размера хэш-таблицы.
Запуск: python plot_hash_size.py
"""

import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('hash_size_results.csv')

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))

# График 1: Коллизии от размера таблицы
ax1.plot(df['table_size'], df['collisions'], 'o-', color='red', linewidth=2, markersize=6)
ax1.set_xlabel('Размер хэш-таблицы', fontsize=12)
ax1.set_ylabel('Количество коллизий', fontsize=12)
ax1.set_title('Зависимость коллизий от размера таблицы\n(500 000 элементов, 2000 уникальных ключей)', fontsize=10)
ax1.grid(True, alpha=0.3)
ax1.axvline(x=4000, color='green', linestyle='--', alpha=0.7, label='Оптимальный размер (~4000)')
ax1.legend()

# График 2: Время поиска от размера таблицы (логарифмическая шкала по X)
ax2.semilogx(df['table_size'], df['search_time_us'], 'o-', color='blue', linewidth=2, markersize=6)
ax2.set_xlabel('Размер хэш-таблицы (логарифмическая шкала)', fontsize=12)
ax2.set_ylabel('Время поиска (микросекунды)', fontsize=12)
ax2.set_title('Зависимость времени поиска от размера таблицы', fontsize=10)
ax2.grid(True, alpha=0.3)

# Дополнительная ось для load factor
ax3 = ax2.twiny()
ax3.set_xlabel('Коэффициент загрузки (уникальные_ключи / размер)', fontsize=10, color='green')
ax3.set_xscale('log')
ax3.set_xticks([0.01, 0.05, 0.1, 0.2, 0.5, 1.0])
ax3.set_xticklabels(['0.01', '0.05', '0.1', '0.2', '0.5', '1.0'])
ax3.set_xlim(ax2.get_xlim())

plt.tight_layout()
plt.savefig('hash_size_plot.png', dpi=150, bbox_inches='tight')
plt.show()

print("Графики сохранены как hash_size_plot.png")
print("\nАнализ результатов:")
print("- При малом размере таблицы (101-1000) коллизий много, время поиска большое")
print("- При размере около 4000 (2×уникальные_ключи) коллизии резко падают")
print("- При дальнейшем увеличении размера коллизии стремятся к 0, время стабилизируется")
print("- Слишком большой размер (500000+) не даёт выигрыша в скорости, но тратит память")