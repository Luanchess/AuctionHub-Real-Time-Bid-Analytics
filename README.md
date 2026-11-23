# AuctionHub — Real-Time Bid Analytics Engine

A C++17 reference implementation of the five sorting modules in the AuctionHub exercise.

## Files

```text
AuctionHub/
├── auctionhub.cpp
├── README.md
├── REFLECTION.md
├── IMPLEMENTATION_CHECKLIST.md
├── screenshot_runtime.png
└── test_data/
    ├── small.txt
    └── large.txt
```

## Build

Linux / macOS with GCC or Clang:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic auctionhub.cpp -o auctionhub
```

Windows with MinGW-w64:

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -pedantic auctionhub.cpp -o auctionhub.exe
```

## Run all modules

Linux / macOS:

```bash
./auctionhub --all test_data/small.txt test_data/large.txt
```

Windows PowerShell:

```powershell
.\auctionhub.exe --all test_data/small.txt test_data/large.txt
```

## Regenerate the large dataset

```bash
./auctionhub --generate test_data/large.txt 100000 20261123
```

The program was built with `g++ -std=c++17 -O2 -Wall -Wextra -pedantic` and run against the bundled four-record and 100,000-record datasets. The reference runtime captured in `screenshot_runtime.png` was 24.906 ms in the creation environment; measure again on your own machine for your reflection.

The input file format is one bid per line:

```text
bidderId amount timestamp auctionId
```

## Algorithm notes

- Task A uses stable insertion sort, ordered by amount and then timestamp.
- Task B uses in-place selection sort in descending amount order and prints the first three bids.
- Task C uses interchange sort and reports its swap count.
- Task D uses an early-stop bubble sort. Its return value reports whether the **first full pass** made no swaps; the function still completes sorting.
- Task E uses median-of-three pivot selection and a three-way partition to handle duplicate keys. The quicksort comparator orders by amount and then timestamp.

## Timing

Task E times only the sort operation, excluding file loading and output. Timing depends on hardware, compiler, and system load; record the value from your own machine in the reflection document.

## Academic-use note

The assignment's LLM policy says AI may be used as a tutor/reviewer, not as a ghostwriter. This package is an AI-generated reference implementation. Do not submit generated code as your own work contrary to the course policy. Read, understand, independently implement or adapt only in ways your instructor permits, and disclose AI assistance as required.
