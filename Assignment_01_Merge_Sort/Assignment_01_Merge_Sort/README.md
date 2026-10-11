# Assignment 1 — Merge Sort

## Aim
Implement Merge Sort in C++ to arrange customer orders by `order_purchase_timestamp` in ascending chronological order.

## Dataset
This assignment uses the real **Brazilian E-Commerce Public Dataset by Olist** from Kaggle:
https://www.kaggle.com/datasets/olistbr/brazilian-ecommerce

Input file: `olist_orders_dataset.csv`

The dataset has eight columns. The program detects the `order_purchase_timestamp` column from the header and keeps each complete original CSV row together while sorting. The dataset's Kaggle page identifies the license as CC BY-NC-SA 4.0; retain attribution to Olist and follow that license's conditions when redistributing the data.

## How it works
1. Read the CSV header and locate `order_purchase_timestamp`.
2. Read each row and keep its timestamp together with the original row.
3. Split the records into two halves recursively.
4. Merge the halves by timestamp. When timestamps are equal, take the left record first so the sort is stable.
5. Write the original header and sorted rows to `sorted_olist_orders_dataset.csv`.
6. Print the record count and a preview of the first 10 sorted orders.

The timestamp uses the fixed-width `YYYY-MM-DD HH:MM:SS` format, so string comparison gives chronological order for valid values in this dataset.

## Complexity
- Best, average, and worst-case time: **O(n log n)**.
- Auxiliary space: **O(n)** for the temporary merge buffer.

## Compile and run on Ubuntu
Run these commands from this folder:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic customer_order_merge_sort.cpp -o merge_sort
./merge_sort
```

Optional input/output filenames:

```bash
./merge_sort olist_orders_dataset.csv my_sorted_orders.csv
```

To save the program's terminal output:

```bash
./merge_sort 2>&1 | tee Output.txt
```

On Windows with MinGW-w64, compile with the same compiler flags and run `./merge_sort.exe` from PowerShell using `./merge_sort.exe`.

## Files
- `customer_order_merge_sort.cpp` — C++ source implementing Merge Sort.
- `olist_orders_dataset.csv` — original Kaggle input dataset.
- `sorted_olist_orders_dataset.csv` — sorted output produced by running the program.
- `Output.txt` — genuine console output from a run on the input dataset.
- `.gitignore` — excludes locally compiled executables and object files.

## Verification
The provided `Output.txt` records the number of input records processed and shows the earliest sorted orders. Run the program again to regenerate the sorted CSV and output log on your machine.
