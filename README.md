# searching

# Лабораторная работа №2: Поиск данных

## Вариант 21
Авиарейсы. Ключ поиска: номер рейса.

## Отчёт

**PDF-отчёт:** `docs/doxygen/latex/refman.pdf`

В отчёте представлены: описание всех классов, результаты замеров времени, графики сравнения алгоритмов, анализ коллизий хэш-таблицы и тепловая карта распределения.

HTML-версия отчёта (альтернатива без графиков): `docs/doxygen/html/index.html`

## Структура проекта
searching/
│
├── Основная программа (бенчмарк)
│ ├── main.cpp # Замер времени поиска для всех структур
│ ├── generate_data.cpp # Генератор CSV-файлов с данными
│ ├── Flight.hpp / .cpp # Класс авиарейса (ключ - номер рейса)
│ │
│ ├── BSTree.hpp / .cpp # Бинарное дерево поиска
│ ├── RBTree.hpp / .cpp # Красно-чёрное дерево
│ └── HashTable.hpp / .cpp # Хэш-таблица (метод цепочек)
│
├── Дополнительные исследования
│ ├── benchmark_unique_keys.cpp # Пункт 1: зависимость от числа уникальных ключей
│ ├── benchmark_hash_size.cpp # Пункт 2: влияние размера таблицы
│ ├── benchmark_hash_functions.cpp # Пункт 3: сравнение хэш-функций (POLY, RS, JS, DJB)
│ └── hash_heatmap.cpp # Пункт 4: тепловая карта загрузки
│
├── Python-скрипты для графиков
│ ├── plot_main.py # Сравнение всех методов (основной график)
│ ├── plot_unique_keys.py # График для пункта 1
│ ├── plot_hash_size.py # График для пункта 2
│ ├── plot_hash_functions.py # График для пункта 3
│ └── plot_heatmap.py # Графики для пункта 4
│
├── Результаты замеров (CSV)
│ ├── search_times.csv # Время поиска от размера массива
│ ├── unique_keys_results.csv # Данные для пункта 1
│ ├── hash_size_results.csv # Данные для пункта 2
│ ├── hash_functions_results.csv # Данные для пункта 3
│ └── bucket_load.csv # Данные для пункта 4
│
├── Графики (PNG)
│ ├── search_comparison.png # Сравнение всех методов
│ ├── unique_keys_plot.png
│ ├── hash_size_plot.png
│ ├── hash_functions_plot.png
│ ├── load_distribution.png
│ ├── hash_heatmap.png
│ └── cumulative_distribution.png
│
├── Документация Doxygen
│ └── docs/doxygen/
│ ├── html/index.html # HTML-отчёт
│ └── latex/refman.pdf # PDF-отчёт
│
└── data/ # CSV-файлы с тестовыми данными
└── flights_*.csv # (генерируются отдельно)

text

---

## Краткое описание реализованных структур

| Структура | Сложность поиска | Особенности |
|-----------|------------------|-------------|
| Линейный поиск | O(n) | Последовательный перебор, не требует доп. памяти |
| BST (бинарное дерево) | O(log n) средн., O(n) худш. | Дубликаты группируются в векторе узла |
| RBT (красно-чёрное дерево) | O(log n) гарант. | Самобалансирующееся, цвета узлов |
| Хэш-таблица | O(1) средн., O(n) худш. | Метод цепочек, полиномиальная хэш-функция |
| std::multimap | O(log n) | Стандартная реализация (для сравнения) |

---

## Инструкция по запуску

### 1. Генерация тестовых данных

```bash
g++ -std=c++17 -O2 generate_data.cpp -o generate_data
./generate_data
2. Основной бенчмарк
bash
g++ -std=c++17 -O2 main.cpp Flight.cpp BSTree.cpp RBTree.cpp HashTable.cpp -o benchmark
./benchmark
3. Дополнительные исследования (каждый отдельно)
bash
# Пункт 1: зависимость от числа уникальных ключей
g++ -std=c++17 -O2 benchmark_unique_keys.cpp Flight.cpp BSTree.cpp RBTree.cpp HashTable.cpp -o bench_unique
./bench_unique
python3 plot_unique_keys.py

# Пункт 2: влияние размера хэш-таблицы
g++ -std=c++17 -O2 benchmark_hash_size.cpp Flight.cpp HashTable.cpp -o bench_size
./bench_size
python3 plot_hash_size.py

# Пункт 3: сравнение хэш-функций
g++ -std=c++17 -O2 benchmark_hash_functions.cpp Flight.cpp -o bench_hash
./bench_hash
python3 plot_hash_functions.py

# Пункт 4: тепловая карта
g++ -std=c++17 -O2 hash_heatmap.cpp Flight.cpp HashTable.cpp -o heatmap
./heatmap
python3 plot_heatmap.py
4. Генерация документации Doxygen
bash
doxygen Doxyfile
cd docs/doxygen/latex
pdflatex refman.tex
makeindex refman.idx
pdflatex refman.tex
pdflatex refman.tex
open refman.pdf