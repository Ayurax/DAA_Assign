# Assignment 1 — Merge Sort

## 1. Aim and Problem Statement

**Aim:** Implement **Merge Sort** in C++ to arrange e-commerce order records in
ascending chronological order of their purchase timestamp, using the **Divide
and Conquer** paradigm. The sort is performed on the real **Olist Brazilian
E-Commerce** dataset (`olist_orders_dataset.csv`), sorting complete order records
without separating any order ID, customer ID, or other attribute from its
associated row.

**Problem:** The company needs all order records ordered by
`order_purchase_timestamp` so that the oldest orders are processed first. The
implementation must:

- Use a manually implemented Merge Sort (no `std::sort`).
- Sort **complete records** — every original column stays with its row.
- Be **stable** — orders that share a purchase timestamp keep their original
  relative input order.
- Scale efficiently to **1,000,000 records** (O(n log n) time, O(n) space).
- Compile and run on **Ubuntu Linux with GCC/G++**.

## 2. Dataset (real data, not synthetic)

The program sorts the real **Brazilian E-Commerce Public Dataset by Olist**,
downloaded from Kaggle:

- **Source:** https://www.kaggle.com/datasets/olistbr/brazilian-ecommerce
- **Attribution:** "Brazilian E-Commerce Public Dataset by Olist", generously
  provided by Olist — the largest department store in the Brazilian marketplaces.
- **Description:** 100,000 orders placed at Olist Store between 2016 and 2018,
  across multiple marketplaces in Brazil. The data is anonymised commercial
  data (company/partner references in review text were replaced with Game of
  Thrones great-house names).
- **License:** **CC BY-NC-SA 4.0** (Creative Commons
  Attribution-NonCommercial-ShareAlike 4.0 International) — see the Kaggle
  dataset page.
- **File used:** `olist_orders_dataset.csv` (the program reads this exact file).
- **Columns:**

  | # | Column | Description |
  |---|--------|-------------|
  | 1 | `order_id` | Unique order identifier |
  | 2 | `customer_id` | Customer identifier |
  | 3 | `order_status` | e.g. `delivered`, `canceled`, `shipped`, `processing`, `invoiced` |
  | 4 | `order_purchase_timestamp` | **Sort key** — `YYYY-MM-DD HH:MM:SS` |
  | 5 | `order_approved_at` | When the order was approved |
  | 6 | `order_delivered_carrier_date` | Carrier hand-over date |
  | 7 | `order_delivered_customer_date` | Customer delivery date |
  | 8 | `order_estimated_delivery_date` | Estimated delivery date |

Columns 5–8 can be **empty** (for example, an order that was `canceled` or not
yet shipped). These empty fields are preserved exactly in the output — only the
timestamp sort column is validated, so no valid row is ever discarded.

## 3. Brief Explanation of Merge Sort

Merge Sort is a **stable** Divide-and-Conquer algorithm:

1. **Divide** — Recursively split the array into two halves.
2. **Conquer** — Recursively sort each half.
3. **Merge** — Merge the two sorted halves into one sorted array by repeatedly
   taking the smaller (earlier) element from the front of either half. When the
   two front elements are **equal**, the element from the *left* half is taken
   first — this is what makes the sort **stable**.

Because the purchase-timestamp format `YYYY-MM-DD HH:MM:SS` is zero-padded and
fixed-width, a component-wise comparison (year → month → day → hour → minute →
second) yields correct chronological order. The program parses each timestamp
once during reading, then compares the already-parsed components during sorting.

## 4. Algorithm Steps

```
mergeSort(arr, left, right):
    if left >= right:                 # base case: 0 or 1 element
        return
    mid = left + (right - left) / 2   # avoids integer overflow
    mergeSort(arr, left, mid)         # sort left half
    mergeSort(arr, mid+1, right)      # sort right half
    merge(arr, left, mid, right)      # merge the two halves

merge(arr, left, mid, right):
    copy arr[left..right] into temp[left..right]      # shared buffer
    i = left ; j = mid+1 ; k = left
    while i <= mid and j <= right:
        if NOT (temp[j] < temp[i]):                   # left <= right  => stable
            arr[k] = temp[i]; i++
        else:
            arr[k] = temp[j]; j++
        k++
    copy any remaining left elements
    copy any remaining right elements
```

**Stability:** `NOT (temp[j] < temp[i])` selects the left element when the two
timestamps are equal, so original input order is preserved among ties.

**Reusable buffer:** a single temporary buffer of size *n* is allocated once and
shared across every merge call (passed by reference), avoiding per-merge memory
allocations even for 1,000,000 records.

## 5. Time and Space Complexity

| Case      | Time      | Reason                                   |
|-----------|-----------|------------------------------------------|
| Best      | O(n log n)| Always splits into halves and merges.    |
| Average   | O(n log n)| Same divide/merge work on random data.   |
| Worst     | O(n log n)| Input distribution never changes the work.|

**Auxiliary space:** O(n) — one temporary buffer of size *n* (no other
significant allocations; the buffer is reused for every merge).

## 6. Comparison with Other Sorting Algorithms

| Algorithm      | Best       | Average     | Worst        | Space   | Stable? | Notes                                  |
|----------------|------------|-------------|--------------|---------|---------|----------------------------------------|
| **Merge Sort** | O(n log n) | O(n log n)  | O(n log n)   | O(n)    | **Yes** | Guaranteed O(n log n); stable. Ideal for records. |
| **Bubble Sort**| O(n)       | O(n^2)      | O(n^2)       | O(1)    | Yes     | Simple but O(n^2) — impractical at scale. |
| **Insertion Sort**| O(n)    | O(n^2)      | O(n^2)       | O(1)    | Yes     | Only fast on small/nearly-sorted data. |
| **Quick Sort** | O(n log n) | O(n log n)  | O(n^2)       | O(log n)| No*     | In-place but worst-case O(n^2); standard version not stable. |

\* A standard in-place Quicksort is **not stable**. Merge Sort's guaranteed
O(n log n) worst-case time **and** its stability make it the correct choice for
sorting records where ties must keep their original order — exactly the
requirement here.

## 7. Instructions to Compile and Run on Ubuntu Linux

```bash
# From inside Assignment_01_Merge_Sort/
g++ -std=c++17 -O2 -Wall -Wextra -pedantic customer_order_merge_sort.cpp -o merge_sort
./merge_sort
```

This reads `olist_orders_dataset.csv`, sorts every record by
`order_purchase_timestamp`, and writes `sorted_olist_orders_dataset.csv`.

### Custom input/output files

```bash
./merge_sort olist_orders_dataset.csv my_output.csv
```

If no arguments are given, the program defaults to `olist_orders_dataset.csv`
(input) and `sorted_olist_orders_dataset.csv` (output).

### Saving terminal output to Output.txt

Use shell redirection **or** `tee`:

```bash
# Option A — redirection (stdout + stderr)
./merge_sort > Output.txt 2>&1

# Option B — tee (shows output on screen AND saves it)
./merge_sort | tee Output.txt

# Option B variant — include stderr as well
./merge_sort 2>&1 | tee Output.txt
```

## 8. Files in This Folder

| File | Description |
|------|-------------|
| `customer_order_merge_sort.cpp` | C++ source — manual, stable Merge Sort on Olist orders. |
| `olist_orders_dataset.csv` | The real Kaggle input dataset (Brazilian E-Commerce Public Dataset by Olist). |
| `sorted_olist_orders_dataset.csv` | Sorted output generated by running the program. |
| `Output.txt` | Captured terminal output from a real run on the dataset. |
| `README.md` | This documentation. |
| `.gitignore` | Ignores the compiled `merge_sort` executable. |
| `merge_sort` / `merge_sort.exe` | Compiled executable — **gitignored, not committed to GitHub**. |

## 9. Sample Input and Expected Output

### Sample input (excerpt from `olist_orders_dataset.csv`)

```
order_id,customer_id,order_status,order_purchase_timestamp,order_approved_at,order_delivered_carrier_date,order_delivered_customer_date,order_estimated_delivery_date
e481f51cbdc54678b7cc49136f2d6af7,"9ef432eb6251297304e76186b10a928d",delivered,2017-10-02 10:56:33,2017-10-02 11:07:15,2017-10-04 19:55:00,2017-10-10 21:25:13,2017-10-18 00:00:00
"53cdb2fc8bc7dce0b6741e2150273451",b0830fb4747a6c6d20dea0b8c802d7ef,delivered,2018-07-24 20:41:37,2018-07-26 03:24:27,2018-07-26 14:31:00,2018-08-07 15:27:45,2018-08-13 00:00:00
...
```

The source CSV wraps some identifiers in double quotes (e.g. `"53cdb2fc..."`,
`"9ef432eb..."`) and leaves others unquoted — the reader handles both styles and
preserves the underlying values.

### Expected output (`sorted_olist_orders_dataset.csv`)

Every input row appears **exactly once**, in the **same 8-column order**, sorted
by `order_purchase_timestamp` ascending. Rows that share a timestamp keep their
original relative order (stable). Columns that were empty in the input remain
empty in the output — no valid row is dropped. The first (oldest) sorted record
is the earliest purchase in the dataset; note its empty
`order_delivered_customer_date` field is preserved as `,,`:

```
order_id,customer_id,order_status,order_purchase_timestamp,order_approved_at,order_delivered_carrier_date,order_delivered_customer_date,order_estimated_delivery_date
2e7a8482f6fb09756ca50c10d7bfc047,08c5351a6aca1c1589a38f244edeee9d,shipped,2016-09-04 21:15:19,2016-10-07 13:18:03,2016-10-18 13:14:51,,2016-10-20 00:00:00
...
```

Note the `,,` before `2016-10-20 00:00:00` — the originally-empty
`order_delivered_customer_date` is retained exactly, demonstrating that empty
fields are not silently discarded.

### Captured terminal output (`Output.txt`)

```
=== Merge Sort on Olist Orders ===
Input file  : olist_orders_dataset.csv
Output file : sorted_olist_orders_dataset.csv
Sort column : order_purchase_timestamp
Total records processed: 99441
Malformed records skipped: 0

First 10 sorted records (ascending by order_purchase_timestamp):
  1. 2e7a8482f6fb09756ca50c10d7bfc047 | shipped | 2016-09-04 21:15:19
  2. e5fa5a7210941f7d56d0208e4e071d35 | canceled | 2016-09-05 00:15:34
  3. 809a282dbd5dbcabb6f2f724fca862ec | canceled | 2016-09-13 15:24:19
  4. bfbd0f9bdef84302105ad712db648a6c | delivered | 2016-09-15 12:16:38
  5. 71303d7e93b399f5bcd537d124c0bcfa | canceled | 2016-10-02 22:07:52
  6. 3b697a20d9e427646d92567910af6d57 | delivered | 2016-10-03 09:44:50
  7. be5bc2f0da14d8071e2d45451ad119d9 | delivered | 2016-10-03 16:56:50
  8. 65d1e226dfaeb8cdc42f665422522d14 | canceled | 2016-10-03 21:01:41
  9. a41c8759fbe7aab36ea07e038b2d4465 | delivered | 2016-10-03 21:13:36
  10. d207cc2726756378fed0062edffd0818 | delivered | 2016-10-03 22:06:03

--- Timing ---
File read time      : 142.7250 ms
Sorting time        : 394.3050 ms
File write time     : 136.6780 ms
Total processing time: 673.7080 ms
```

## 10. Verification

The sorted output was verified against the input dataset:

- **Record count preserved:** 99,441 input → 99,441 output rows.
- **Ascending order:** every `order_purchase_timestamp` is ≤ the next (verified
  across all 99,441 records).
- **All columns preserved:** header and all 8 columns identical; the full row
  multiset matches the input exactly (empty fields included).
- **Stability:** 556 distinct timestamps are shared by multiple rows, and every
  one of those groups retains its original input order in the output.
- **Input untouched:** `olist_orders_dataset.csv` was only read, never written.

### Edge-case tests

| Scenario | Behaviour |
|----------|-----------|
| Single record (with empty fields) | Sorted correctly; empty fields preserved. |
| Empty input file (0 bytes) | Reports 0 records safely; writes a header-only (empty) output. |
| Malformed/empty timestamps | Invalid rows skipped with a clear stderr warning; valid rows are **not** discarded. |
| Missing input file | Prints an error and exits with a non-zero status. |
| Quoted fields / embedded commas / escaped quotes | Handled by the RFC-4180-style CSV reader/writer. |

### Large-scale test

A synthetic dataset of **1,000,000 records** was generated in a temporary folder
(outside the repository) solely to confirm the scalability target:
99,441-style records → 1,000,000 sorted successfully with **0 out-of-order
records**, sorting time ~1.5 s, confirming O(n log n) performance and O(n)
auxiliary space. The large test file was **not** committed.
