#include <benchmark/benchmark.h>
#include <algorithm>
#include <numeric>
#include <list>
#include "zippp/zip.h"


constexpr int num_items = 100;
template<class Col>
void init_col(Col& col) {
    for(std::size_t i = 0; i < num_items; ++i) {
        col.push_back(i);
    }
}

template<class I>
void init_col(std::array<I, num_items>& col) {
    for(std::size_t i = 0; i < num_items; ++i) {
        col[i] = i;
    }
}

template<typename T1, typename T2, typename T3>
struct bench_cols {
    T1 col1;
    T2 col2;
    T3 col3;

    bench_cols() {
        init_col(col1);
        init_col(col2);
        init_col(col3);
    }
};

using bench_t = bench_cols<std::array<int, num_items>, std::vector<bool>, std::vector<long long>>;

static void BM_zipppiter(benchmark::State& state) {
    // bench_cols<std::vector<int>, std::vector<double>, std::vector<long long>> cols;
    bench_t cols;
    for (auto _ : state) {
        int value = 0;
        for(const auto& [val1, val2, val3] : zippp::zip(cols.col1, cols.col2, cols.col3)){
            value += val1 + val2 + val3;
        }
        benchmark::ClobberMemory();
    }
}

static void BM_normaliter(benchmark::State& state) {
    // bench_cols<std::vector<int>, std::vector<double>, std::vector<long long>> cols;
    bench_t cols;
    for (auto _ : state) {
        int value = 0;
        auto it1 = cols.col1.cbegin();
        auto it2 = cols.col2.cbegin();
        auto it3 = cols.col3.cbegin();
        for(; it1 != cols.col1.end(); ++it1, ++it2, ++it3) {
            value += *it1 + *it2 + *it3;
        }
        benchmark::ClobberMemory();
    }
}

static void BM_indexiter(benchmark::State& state) {
    // bench_cols<std::vector<int>, std::vector<double>, std::vector<long long>> cols;
    bench_t cols;
    for (auto _ : state) {
        int value = 0;
        for(std::size_t i = 0; i < cols.col1.size(); ++i) {
            value += cols.col1[i] + cols.col2[i] + cols.col3[i];
        }
        benchmark::ClobberMemory();
    }
}
// Register the function as a benchmark
BENCHMARK(BM_normaliter);
BENCHMARK(BM_indexiter);
BENCHMARK(BM_zipppiter);

BENCHMARK_MAIN();