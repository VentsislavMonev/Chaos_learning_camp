#include "Tiered_vector.hpp"
#include "AVL.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>


// I made the circular dequeue, the tiered vector and the indexing avl, but his benchmarking code is made with ai
//
// Im sorry for that. It was fun building the structures and I really wanted to try and do it myself and not use AI, 
// but my university is at my neck as this week i had 4 exams and tomorrow i have another exam. I hope I atleast
// made the data structures ok.

// Timing
using Clock = std::chrono::high_resolution_clock;
using us    = std::chrono::microseconds;

static long long elapsed_us(Clock::time_point t0, Clock::time_point t1)
{
    return std::chrono::duration_cast<us>(t1 - t0).count();
}

//  RNG
static std::mt19937 rng(42);

static int rand_int(int lo, int hi)
{
    return std::uniform_int_distribution<int>(lo, hi)(rng);
}

//  Result store
struct Row {
    std::string workload;
    int         N;
    long long   tv_us;
    long long   avl_us;
    size_t      tv_mem;
    size_t      avl_mem;
};

static std::vector<Row> results;

static void record(const std::string& w, int N,
                   long long tv, long long avl,
                   size_t tvm, size_t avlm)
{
    results.push_back({w, N, tv, avl, tvm, avlm});
}

//  Workload 1 — Sequential Append
void bench_seq_append(int N)
{
    Tiered_vector tv(2);
    auto t0 = Clock::now();
    for (int i = 0; i < N; ++i) tv.push_back(i);
    auto t1 = Clock::now();
    long long tv_t = elapsed_us(t0, t1);
    size_t tvm = tv.memory_bytes();

    AVLTree avl;
    t0 = Clock::now();
    for (int i = 0; i < N; ++i) avl.add(i);
    t1 = Clock::now();
    long long avl_t = elapsed_us(t0, t1);
    size_t avlm = avl.memory_bytes();

    record("SeqAppend", N, tv_t, avl_t, tvm, avlm);
}

//  Workload 2 — Random Append
void bench_rand_append(int N)
{
    std::vector<int> vals(N);
    for (int& v : vals) v = rand_int(0, N * 10);

    Tiered_vector tv(2);
    auto t0 = Clock::now();
    for (int v : vals) tv.push_back(v);
    auto t1 = Clock::now();
    long long tv_t = elapsed_us(t0, t1);
    size_t tvm = tv.memory_bytes();

    AVLTree avl;
    t0 = Clock::now();
    for (int v : vals) avl.add(v);
    t1 = Clock::now();
    long long avl_t = elapsed_us(t0, t1);
    size_t avlm = avl.memory_bytes();

    record("RandAppend", N, tv_t, avl_t, tvm, avlm);
}

//  Workload 3 — Random Access
void bench_rand_access(int N, int M = 0)
{
    if (M == 0) M = N;

    Tiered_vector tv(2);
    for (int i = 0; i < N; ++i) tv.push_back(i);

    AVLTree avl;
    for (int i = 0; i < N; ++i) avl.add(i);

    std::vector<int> indices(M);
    for (int& idx : indices) idx = rand_int(0, N - 1);

    volatile long long sink = 0;

    auto t0 = Clock::now();
    for (int idx : indices) sink += tv[idx];
    auto t1 = Clock::now();
    long long tv_t = elapsed_us(t0, t1);

    t0 = Clock::now();
    for (int idx : indices) sink += avl.get_at(idx);
    t1 = Clock::now();
    long long avl_t = elapsed_us(t0, t1);

    (void)sink;
    record("RandAccess", N, tv_t, avl_t, tv.memory_bytes(), avl.memory_bytes());
}

//  Workload 4a — Front Mutation
//  TV : insert/remove at rank 0.
//  AVL: insert strictly decreasing values so each new one lands
//       at rank 0 (becomes the minimum), then remove in order.
void bench_front_mutation(int N)
{
    // Tiered Vector
    Tiered_vector tv(2);
    auto t0 = Clock::now();
    for (int i = 0; i < N; ++i) tv.insert(0, i);
    for (int i = 0; i < N; ++i) tv.remove(0);
    auto t1 = Clock::now();
    long long tv_t = elapsed_us(t0, t1);

    // Peak memory: measure at full fill
    Tiered_vector tv_peak(2);
    for (int i = 0; i < N; ++i) tv_peak.insert(0, i);
    size_t tvm = tv_peak.memory_bytes();

    // AVL 
    AVLTree avl;
    int min_val = 0;
    t0 = Clock::now();
    for (int i = 0; i < N; ++i) avl.add(--min_val);
    for (int i = 0; i < N; ++i) avl.remove(min_val++);
    t1 = Clock::now();
    long long avl_t = elapsed_us(t0, t1);

    // Peak memory: measure at full fill
    AVLTree avl_peak;
    min_val = 0;
    for (int i = 0; i < N; ++i) avl_peak.add(--min_val);
    size_t avlm = avl_peak.memory_bytes();

    record("FrontMut", N, tv_t, avl_t, tvm, avlm);
}

//  Workload 4b — Random Mutation
//  Pre-fill N/2, then N alternating insert/remove at random ranks.
void bench_rand_mutation(int N)
{
    int prefill = N / 2;
    int ops     = N;

    // Tiered Vector
    Tiered_vector tv(2);
    for (int i = 0; i < prefill; ++i) tv.push_back(i);

    auto t0 = Clock::now();
    for (int i = 0; i < ops; ++i) {
        int sz = tv.size();
        if (i % 2 == 0 || sz == 0) {
            int rank = (sz == 0) ? 0 : rand_int(0, sz);
            tv.insert(rank, i + prefill);
        } else {
            int rank = rand_int(0, sz - 1);
            tv.remove(rank);
        }
    }
    auto t1 = Clock::now();
    long long tv_t = elapsed_us(t0, t1);
    size_t tvm = tv.memory_bytes();

    // AVL 
    // Shadow vector tracks present values so we can pick one to remove randomly.
    AVLTree avl;
    std::vector<int> present;
    present.reserve(prefill + ops);
    for (int i = 0; i < prefill; ++i) { avl.add(i); present.push_back(i); }

    int next_val = prefill + ops;

    t0 = Clock::now();
    for (int i = 0; i < ops; ++i) {
        if (i % 2 == 0 || present.empty()) {
            avl.add(next_val);
            present.push_back(next_val++);
        } else {
            int pick = rand_int(0, (int)present.size() - 1);
            avl.remove(present[pick]);
            present.erase(present.begin() + pick);
        }
    }
    t1 = Clock::now();
    long long avl_t = elapsed_us(t0, t1);
    size_t avlm = avl.memory_bytes();

    record("RandMut", N, tv_t, avl_t, tvm, avlm);
}

//  Output
static std::string fmt_us(long long v)
{
    std::ostringstream s;
    if (v >= 1000) s << std::fixed << std::setprecision(1) << v / 1000.0 << " ms";
    else s << v << " us";
    return s.str();
}

static void print_table()
{
    const int W = 13;
    std::cout << "\n"
              << std::left
              << std::setw(14) << "Workload"
              << std::setw(10) << "N"
              << std::setw(W)  << "TV (time)"
              << std::setw(W)  << "AVL (time)"
              << std::setw(W)  << "TV Mem(KB)"
              << std::setw(W)  << "AVL Mem(KB)"
              << std::setw(8)  << "Faster"
              << "\n" << std::string(83, '-') << "\n";

    for (auto& r : results) {
        std::cout << std::left
                  << std::setw(14) << r.workload
                  << std::setw(10) << r.N
                  << std::setw(W)  << fmt_us(r.tv_us)
                  << std::setw(W)  << fmt_us(r.avl_us)
                  << std::setw(W)  << (r.tv_mem  / 1024)
                  << std::setw(W)  << (r.avl_mem / 1024)
                  << std::setw(8)  << (r.tv_us <= r.avl_us ? "TV" : "AVL")
                  << "\n";
    }
    std::cout << "\n";
}

static void print_chart(const std::string& workload)
{
    std::vector<Row*> rows;
    for (auto& r : results)
        if (r.workload == workload) rows.push_back(&r);
    if (rows.empty()) return;

    long long maxv = 1;
    for (auto* r : rows) maxv = std::max(maxv, std::max(r->tv_us, r->avl_us));

    const int BAR = 40;
    std::cout << "\n  === " << workload << " ===\n\n";
    for (auto* r : rows) {
        int tb = (int)(r->tv_us  * BAR / maxv);
        int ab = (int)(r->avl_us * BAR / maxv);
        std::cout << "  N=" << std::setw(9) << r->N << "\n"
                  << "  TV  |" << std::string(tb, '#') << "| " << fmt_us(r->tv_us)  << "\n"
                  << "  AVL |" << std::string(ab, '*') << "| " << fmt_us(r->avl_us) << "\n\n";
    }
}

static void print_memory_analysis()
{
    std::cout << "\n========================================\n"
                 "  Memory Analysis\n"
                 "========================================\n\n"
                 "  Tiered Vector\n"
                 "    Layout  : l tiers, each a Circular_dequeue of capacity l\n"
                 "    Formula : sizeof(TV) + t*sizeof(Circular_dequeue) + t*l*sizeof(int)\n\n"
                 "  AVL Tree\n"
                 "    Per node: value(4)+height(4)+left_size(4)+2*ptr(16) = 28 bytes\n"
                 "    Formula : sizeof(AVLTree) + n*sizeof(Node)\n\n";

    for (auto& r : results) {
        if (r.workload == "SeqAppend" && r.N == 100000) {
            std::cout << "  Measured at N=100,000 (SeqAppend):\n"
                      << "    TV  : " << r.tv_mem  / 1024 << " KB"
                      << "  (" << std::fixed << std::setprecision(1)
                      << (double)r.tv_mem / 100000 << " B/elem)\n"
                      << "    AVL : " << r.avl_mem / 1024 << " KB"
                      << "  (" << (double)r.avl_mem / 100000 << " B/elem)\n\n";
        }
    }
}

int main()
{
    std::cout << "========================================\n"
                 "  Tiered Vector vs AVL Tree Benchmark\n"
                 "========================================\n";

    const std::vector<int> Ns = {10'000, 100'000, 1'000'000};

    for (int N : Ns) {
        std::cout << "\n[N=" << N << "]";

        std::cout << " SeqAppend...";  std::cout.flush(); bench_seq_append(N);
        std::cout << " RandAppend..."; std::cout.flush(); bench_rand_append(N);
        std::cout << " RandAccess..."; std::cout.flush(); bench_rand_access(N);

        // Cap mutation at 100k — TV's O(sqrt N) per op makes 1M very slow
        int mut_N = std::min(N, 100'000);
        std::cout << " FrontMut(N=" << mut_N << ")..."; std::cout.flush();
        bench_front_mutation(mut_N);

        std::cout << " RandMut(N=" << mut_N << ")..."; std::cout.flush();
        bench_rand_mutation(mut_N);

        std::cout << " done.\n";
    }

    print_table();

    for (const char* w : {"SeqAppend","RandAppend","RandAccess","FrontMut","RandMut"})
        print_chart(w);

    print_memory_analysis();

    return 0;
}