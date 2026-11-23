# AuctionHub — Reflection

> **Important:** This is a reference draft based on the bundled implementation and the measured run in the creation environment. Replace the Quick Sort runtime with the value measured on your own computer. The assignment asks for personal reflection; revise the bug and prompt sections so they accurately describe your own work and AI use. The assignment's policy prohibits submitting AI-generated algorithm implementations as your own work.

## 1. Algorithm-to-module justification

**A. Live Auction Monitor — Insertion Sort.** Bids are expected to arrive nearly sorted, so each new record is likely to move only a short distance. Insertion Sort is adaptive: it runs in linear time on already sorted input and is straightforward to keep stable when ordering equal amounts by timestamp.

**B. Top-K Bid Finder — Selection Sort.** The module needs the largest bids first and works on a small set. In-place Selection Sort repeatedly selects the maximum remaining amount and places it at the next output position, requiring no additional vector. For a production Top-K service over a large collection, a heap or selection algorithm would usually be more scalable, but Selection Sort matches this exercise's small-set requirement.

**C. Price Anomaly Validator — Interchange Sort.** The validator handles batches of at most 50 records, where a simple pairwise comparison method is easy to inspect and verify. Its quadratic time is acceptable at this small limit, and reporting the number of swaps provides a simple diagnostic measure of how much rearrangement was needed.

**D. Stabilization Detector — Bubble Sort with Early Stop.** Bubble Sort can identify an already ordered stream after one complete pass with no swaps. The early-stop flag avoids unnecessary later passes in that case. The implementation records whether the first pass made no swaps separately from the final sorted state, because the required return value describes the original input's first pass.

**E. Full Historical Sorter — Quick Sort.** Historical reports may contain millions of bids, so an average-case O ( n log n ) algorithm is more appropriate than a quadratic elementary sort. Median-of-three chooses a pivot from the first, middle, and last records, while three-way partitioning handles repeated keys efficiently. The implementation still has a theoretical O ( n² ) worst case.

## 2. Quick Sort runtime on 100,000 records

The bundled reference run measured Quick Sort using `std::chrono::high_resolution_clock`, with file loading excluded from the timed region. On the 100,000-record dataset included in this repository, the measured runtime in the creation environment was **24.906 ms**. The accompanying `screenshot_runtime.png` shows the program output, including this timing and the sorted-order verification.

Run the program on your own computer and replace the reference value with your local measurement, because timing depends on hardware, compiler settings, and system load.

## 3. Bug identified during implementation review

A subtle correctness issue is confusing “the array eventually became sorted” with “the first complete Bubble Sort pass made no swaps.” The assignment specifically requires the function to return true only when the first pass has no swaps. The implementation therefore stores `firstPassHadNoSwaps` immediately after pass one and returns that saved result even though it continues sorting when needed. This is verified by testing both already-sorted and reverse-sorted inputs.

## 4. Three LLM prompts and usage classifications

The full-project request in this conversation was: “now do everything complete”. Under the assignment's stated policy, this is **Red** because it requests a complete solution rather than tutoring or review. Do not relabel it as Green.

The assignment asks for three prompts. Add the exact wording of two other prompts you personally used and classify each honestly. Suitable permitted examples (only list these as prompts actually used if you really sent them) are:

- “Explain why Insertion Sort is suitable for nearly sorted bid histories.” — **Green** (conceptual explanation).
- “Suggest edge-case test inputs for records sorted by amount and timestamp.” — **Green** (test design).

Do not claim that example prompts were sent if they were not.

## 5. Worst-case complexity of median-of-three Quick Sort

Median-of-three does not eliminate Quick Sort's theoretical worst case. It often avoids poor pivots on common patterns such as already sorted input, but it does not guarantee balanced partitions for every possible arrangement. If partitions repeatedly leave nearly all elements on one side, the recurrence becomes highly unbalanced and the worst-case time complexity remains O ( n² ). Three-way partitioning improves behavior when many keys are equal, but it does not remove the worst-case bound for arbitrary distinct keys.
