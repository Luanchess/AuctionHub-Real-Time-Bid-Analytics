#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;

struct Bid {
    string bidderId;
    double amount{};
    long long timestamp{};
    int auctionId{};
};

// Task A and Task E use this ordering: amount ascending, then timestamp ascending.
bool bidLessAscending(const Bid& lhs, const Bid& rhs) {
    if (lhs.amount != rhs.amount) return lhs.amount < rhs.amount;
    return lhs.timestamp < rhs.timestamp;
}

void printBids(const vector<Bid>& bids, size_t limit = 10) {
    const size_t count = min(bids.size(), limit);
    for (size_t i = 0; i < count; ++i) {
        cout << "[" << bids[i].bidderId << "] $"
             << fixed << setprecision(2) << bids[i].amount
             << " @ " << bids[i].timestamp
             << " (auction " << bids[i].auctionId << ")\n";
    }
    if (bids.size() > limit) {
        cout << "... (" << bids.size() << " total records)\n";
    }
}

bool loadBids(const string& path, vector<Bid>& out) {
    ifstream input(path);
    if (!input) return false;

    Bid bid;
    while (input >> bid.bidderId >> bid.amount >> bid.timestamp >> bid.auctionId) {
        out.push_back(bid);
    }
    return input.eof();
}

bool saveBids(const string& path, const vector<Bid>& bids) {
    ofstream output(path);
    if (!output) return false;
    output << fixed << setprecision(2);
    for (const Bid& bid : bids) {
        output << bid.bidderId << ' ' << bid.amount << ' '
               << bid.timestamp << ' ' << bid.auctionId << '\n';
    }
    return static_cast<bool>(output);
}

vector<Bid> generateBids(size_t count, uint32_t seed = 20261123) {
    mt19937 generator(seed);
    uniform_real_distribution<double> amountDistribution(10.0, 100000.0);
    uniform_int_distribution<long long> timestampDistribution(1700000000000LL,
                                                               1800000000000LL);
    uniform_int_distribution<int> auctionDistribution(1, 500);

    vector<Bid> bids;
    bids.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        ostringstream id;
        id << "bidder" << setw(6) << setfill('0') << (i + 1);
        bids.push_back({id.str(), amountDistribution(generator),
                        timestampDistribution(generator),
                        auctionDistribution(generator)});
    }
    return bids;
}

// ---------- Task A: Insertion Sort ----------
// Stable ordering by amount ascending; timestamp breaks amount ties.
void insertionSort(vector<Bid>& bids) {
    for (size_t i = 1; i < bids.size(); ++i) {
        Bid key = bids[i];
        size_t j = i;
        while (j > 0 && bidLessAscending(key, bids[j - 1])) {
            bids[j] = bids[j - 1];
            --j;
        }
        bids[j] = key;
    }
}

// ---------- Task B: Selection Sort ----------
// In-place descending order by bid amount.
void selectionSort(vector<Bid>& bids) {
    for (size_t i = 0; i < bids.size(); ++i) {
        size_t maxIndex = i;
        for (size_t j = i + 1; j < bids.size(); ++j) {
            if (bids[j].amount > bids[maxIndex].amount) {
                maxIndex = j;
            }
        }
        if (maxIndex != i) {
            swap(bids[i], bids[maxIndex]);
        }
    }
}

// ---------- Task C: Interchange Sort ----------
// Ascending by amount; returns the total number of swaps performed.
size_t interchangeSort(vector<Bid>& bids) {
    size_t swaps = 0;
    for (size_t i = 0; i < bids.size(); ++i) {
        for (size_t j = i + 1; j < bids.size(); ++j) {
            if (bids[j].amount < bids[i].amount) {
                swap(bids[i], bids[j]);
                ++swaps;
            }
        }
    }
    return swaps;
}

// ---------- Task D: Bubble Sort with Early Stop ----------
// Returns true only when the FIRST complete pass performs no swaps.
// The function still sorts the entire vector before returning.
bool bubbleSortEarlyStop(vector<Bid>& bids) {
    bool firstPassHadNoSwaps = true;
    if (bids.size() < 2) return true;

    for (size_t pass = 0; pass + 1 < bids.size(); ++pass) {
        bool swappedThisPass = false;
        for (size_t j = 0; j + 1 < bids.size() - pass; ++j) {
            if (bids[j].amount > bids[j + 1].amount) {
                swap(bids[j], bids[j + 1]);
                swappedThisPass = true;
            }
        }

        if (pass == 0) {
            firstPassHadNoSwaps = !swappedThisPass;
        }
        if (!swappedThisPass) break;
    }
    return firstPassHadNoSwaps;
}

// ---------- Task E: Quick Sort with median-of-three ----------
// Three-way Lomuto-style partitioning handles repeated keys efficiently.
// The complete ordering is amount ascending, then timestamp ascending.
void swapBid(Bid& a, Bid& b) {
    if (&a != &b) swap(a, b);
}

Bid medianOfThree(const Bid& first, const Bid& middle, const Bid& last) {
    if (bidLessAscending(first, middle)) {
        if (bidLessAscending(middle, last)) return middle;
        if (bidLessAscending(first, last)) return last;
        return first;
    }
    if (bidLessAscending(first, last)) return first;
    if (bidLessAscending(middle, last)) return last;
    return middle;
}

// Dutch-national-flag partition: [lo, lt) < pivot, [lt, gt] == pivot,
// (gt, hi] > pivot. Returns the inclusive equal range.
pair<int, int> partitionMedianOfThree(vector<Bid>& bids, int lo, int hi) {
    const int mid = lo + (hi - lo) / 2;
    const Bid pivot = medianOfThree(bids[lo], bids[mid], bids[hi]);

    int lt = lo;
    int current = lo;
    int gt = hi;

    while (current <= gt) {
        if (bidLessAscending(bids[current], pivot)) {
            swapBid(bids[lt], bids[current]);
            ++lt;
            ++current;
        } else if (bidLessAscending(pivot, bids[current])) {
            swapBid(bids[current], bids[gt]);
            --gt;
        } else {
            ++current;
        }
    }
    return {lt, gt};
}

void quickSort(vector<Bid>& bids, int lo, int hi) {
    // Recurse on the smaller side and iterate over the larger side to limit
    // call-stack growth even when a partition is highly unbalanced.
    while (lo < hi) {
        const pair<int, int> equalRange = partitionMedianOfThree(bids, lo, hi);
        const int leftSize = equalRange.first - lo;
        const int rightSize = hi - equalRange.second;

        if (leftSize < rightSize) {
            if (lo < equalRange.first - 1) {
                quickSort(bids, lo, equalRange.first - 1);
            }
            lo = equalRange.second + 1;
        } else {
            if (equalRange.second + 1 < hi) {
                quickSort(bids, equalRange.second + 1, hi);
            }
            hi = equalRange.first - 1;
        }
    }
}

void quickSortWrapper(vector<Bid>& bids) {
    if (bids.size() > 1) {
        quickSort(bids, 0, static_cast<int>(bids.size()) - 1);
    }
}

bool isSortedAscending(const vector<Bid>& bids) {
    for (size_t i = 1; i < bids.size(); ++i) {
        if (bidLessAscending(bids[i], bids[i - 1])) return false;
    }
    return true;
}

bool isSortedByAmountDescending(const vector<Bid>& bids) {
    for (size_t i = 1; i < bids.size(); ++i) {
        if (bids[i - 1].amount < bids[i].amount) return false;
    }
    return true;
}

void printStatus(const string& label, bool passed) {
    cout << left << setw(43) << label << (passed ? "PASS" : "FAIL") << '\n';
}

vector<Bid> sampleBids() {
    return {
        {"alice", 100.0, 1000, 1},
        {"bob", 102.5, 1005, 1},
        {"carol", 101.0, 1003, 1},
        {"dave", 103.0, 1010, 1}
    };
}

void runAll(const string& smallPath, const string& largePath) {
    cout << "============================================================\n";
    cout << "AuctionHub - Real-Time Bid Analytics Engine\n";
    cout << "============================================================\n\n";

    vector<Bid> small;
    if (!loadBids(smallPath, small) || small.empty()) {
        cerr << "Could not load small dataset: " << smallPath << '\n';
        return;
    }
    cout << "SMALL DATASET: " << small.size() << " bids loaded from "
         << smallPath << "\n\n";

    // Task A
    cout << "[Task A] Live Auction Monitor - Insertion Sort\n";
    vector<Bid> insertionData = small;
    insertionSort(insertionData);
    printBids(insertionData);
    printStatus("Ascending amount/timestamp verification: ",
                isSortedAscending(insertionData));
    cout << '\n';

    // Task B
    cout << "[Task B] Top-K Bid Finder - Selection Sort\n";
    vector<Bid> selectionData = small;
    selectionSort(selectionData);
    cout << "Top 3 bidders (descending amount):\n";
    printBids(selectionData, 3);
    printStatus("Descending amount verification: ",
                isSortedByAmountDescending(selectionData));
    cout << '\n';

    // Task C
    cout << "[Task C] Price Anomaly Validator - Interchange Sort\n";
    vector<Bid> interchangeData = small;
    const size_t interchangeSwaps = interchangeSort(interchangeData);
    printBids(interchangeData);
    cout << "Total swaps: " << interchangeSwaps << '\n';
    printStatus("Ascending amount verification: ",
                is_sorted(interchangeData.begin(), interchangeData.end(),
                          [](const Bid& a, const Bid& b) {
                              return a.amount < b.amount;
                          }));
    cout << '\n';

    // Task D
    cout << "[Task D] Stabilization Detector - Early-Exit Bubble Sort\n";
    vector<Bid> stableInput = {
        {"s1", 10.0, 1, 1}, {"s2", 20.0, 2, 1}, {"s3", 30.0, 3, 1}
    };
    vector<Bid> unstableInput = {
        {"u1", 30.0, 1, 1}, {"u2", 20.0, 2, 1}, {"u3", 10.0, 3, 1}
    };
    const bool stableResult = bubbleSortEarlyStop(stableInput);
    const bool unstableResult = bubbleSortEarlyStop(unstableInput);
    cout << "Already-sorted input: " << (stableResult ? "STABLE" : "UNSTABLE") << '\n';
    cout << "Reverse-sorted input:  " << (unstableResult ? "STABLE" : "UNSTABLE") << '\n';
    printStatus("Already-sorted case returns true: ", stableResult);
    printStatus("Reverse-sorted case returns false: ", !unstableResult);
    printStatus("Both outputs are sorted: ",
                isSortedAscending(stableInput) && isSortedAscending(unstableInput));
    cout << '\n';

    // Task E
    cout << "[Task E] Full Historical Sorter - Median-of-Three Quick Sort\n";
    vector<Bid> large;
    if (!loadBids(largePath, large) || large.empty()) {
        cerr << "Could not load large dataset: " << largePath << '\n';
        return;
    }
    cout << "Large dataset: " << large.size() << " bids loaded from "
         << largePath << '\n';
    const auto start = high_resolution_clock::now();
    quickSortWrapper(large);
    const auto finish = high_resolution_clock::now();
    const double elapsedMs = duration<double, milli>(finish - start).count();
    cout << fixed << setprecision(3)
         << "Quick Sort runtime: " << elapsedMs << " ms\n";
    printStatus("Ascending amount/timestamp verification: ",
                isSortedAscending(large));
    cout << "First 5 sorted bids:\n";
    printBids(large, 5);

    vector<Bid> tieCase = {
        {"later", 50.0, 200, 1},
        {"earlier", 50.0, 100, 1},
        {"lower", 40.0, 300, 1}
    };
    quickSortWrapper(tieCase);
    const bool tieBreakPassed =
        tieCase.size() == 3 && tieCase[0].bidderId == "lower" &&
        tieCase[1].bidderId == "earlier" && tieCase[2].bidderId == "later";
    printStatus("Timestamp tie-break verification: ", tieBreakPassed);

    vector<Bid> allEqualCase = {
        {"eq1", 25.0, 1, 1}, {"eq2", 25.0, 1, 1},
        {"eq3", 25.0, 1, 1}, {"eq4", 25.0, 1, 1}
    };
    quickSortWrapper(allEqualCase);
    printStatus("All-equal-key Quick Sort verification: ",
                isSortedAscending(allEqualCase));
    cout << "\nAll tasks finished.\n";
}

int main(int argc, char* argv[]) {
    const string command = (argc >= 2) ? argv[1] : "--all";

    if (command == "--generate") {
        const string outputPath = (argc >= 3) ? argv[2] : "test_data/large.txt";
        const size_t count = (argc >= 4) ? stoull(argv[3]) : 100000;
        const uint32_t seed = (argc >= 5) ? static_cast<uint32_t>(stoul(argv[4]))
                                           : 20261123;
        const vector<Bid> generated = generateBids(count, seed);
        if (!saveBids(outputPath, generated)) {
            cerr << "Could not write dataset: " << outputPath << '\n';
            return 1;
        }
        cout << "Generated " << generated.size() << " bids in " << outputPath << '\n';
        return 0;
    }

    if (command == "--all") {
        const string smallPath = (argc >= 3) ? argv[2] : "test_data/small.txt";
        const string largePath = (argc >= 4) ? argv[3] : "test_data/large.txt";
        runAll(smallPath, largePath);
        return 0;
    }

    cout << "Usage:\n"
         << "  auctionhub --all [small.txt] [large.txt]\n"
         << "  auctionhub --generate [output.txt] [count] [seed]\n";
    return 0;
}
