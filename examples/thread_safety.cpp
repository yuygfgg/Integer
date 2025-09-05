#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include "../Integer.h"

// Simple workload: compute factorial segments and multiply into a shared result with a mutex-free pattern
// We avoid sharing Integer objects across threads directly to check per-thread safety, and also try a shared structure to stress potential hazards.

struct ThreadResult {
    UnsignedInteger value;
    ThreadResult() : value(1) {}
};

UnsignedInteger factorial_range(unsigned start, unsigned end) {
    UnsignedInteger acc = 1;
    for (unsigned i = start; i <= end; ++i) {
        acc *= i;
    }
    return acc;
}

int main() {
    std::cout << "=== Thread-safety probe for Integer library ===" << std::endl;

    // 1) Per-thread independent use: each thread builds its own numbers and converts to string
    const unsigned num_threads = std::max(4u, std::thread::hardware_concurrency());
    std::cout << "Threads: " << num_threads << std::endl;

    std::atomic<bool> any_exception{false};
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    auto task_independent = [&](unsigned tid) {
        try {
            // Independent sequences
            UnsignedInteger a = 1;
            for (int r = 0; r < 200; ++r) {
                a *= (r + 11);
                a += r;
                a = a * a % UnsignedInteger(1000003);
                // Exercise conversions (string uses thread_local in library):
                volatile size_t len = static_cast<std::string>(a).size();
                (void)len;
            }

            SignedInteger s = -123456789;
            s *= 3;
            s += 42;
            volatile auto s_str_len = static_cast<std::string>(s).size();
            (void)s_str_len;
        } catch (const std::exception& e) {
            std::cerr << "Thread " << tid << " exception (independent): " << e.what() << std::endl;
            any_exception = true;
        }
    };

    for (unsigned t = 0; t < num_threads; ++t) {
        threads.emplace_back(task_independent, t);
    }
    for (auto& th : threads) th.join();
    threads.clear();

    std::cout << "Phase 1 done. Exceptions: " << std::boolalpha << any_exception.load() << std::endl;

    // 2) Shared stress via thread_local buffers inside conversions. We intentionally share std::string destinations only (safe),
    // but we call static_cast<const char*> on different objects concurrently to stress thread_local correctness.
    std::vector<ThreadResult> partial(num_threads);

    auto task_partial = [&](unsigned tid) {
        try {
            // Each thread computes a partial factorial range
            unsigned total_n = 250; // moderate size to run fast
            unsigned chunk = (total_n + num_threads - 1) / num_threads;
            unsigned start = tid * chunk + 1;
            unsigned end = std::min(total_n, (tid + 1) * chunk);
            if (start <= end) {
                partial[tid].value = factorial_range(start, end);
            } else {
                partial[tid].value = 1;
            }

            // Convert repeatedly to string and C-string to exercise thread_local buffers
            for (int r = 0; r < 200; ++r) {
                std::string s = static_cast<std::string>(partial[tid].value);
                const char* cs = static_cast<const char*>(partial[tid].value);
                // Touch data to avoid optimizing away
                if (!cs || s.empty()) {
                    any_exception = true;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Thread " << tid << " exception (partial): " << e.what() << std::endl;
            any_exception = true;
        }
    };

    for (unsigned t = 0; t < num_threads; ++t) {
        threads.emplace_back(task_partial, t);
    }
    for (auto& th : threads) th.join();
    threads.clear();

    // Multiply partials serially to avoid intentional data races on big integers
    UnsignedInteger combined = 1;
    for (unsigned t = 0; t < num_threads; ++t) {
        combined *= partial[t].value;
    }

    std::cout << "Phase 2 done. Combined digits: " << static_cast<std::string>(combined).size() << std::endl;

    // 3) Intentional race test: write to a shared UnsignedInteger without synchronization
    // It is expected to crash or produce inconsistent results, since the library is not thread-safe for shared objects.
    UnsignedInteger shared_val = 1;
    auto task_racy = [&](unsigned tid) {
        try {
            for (int r = 0; r < 1000; ++r) {
                // racy update: not synchronized on purpose
                shared_val *= (tid + 2);
                shared_val += r;
            }
        } catch (const std::exception& e) {
            std::cerr << "Thread " << tid << " exception (racy): " << e.what() << std::endl;
            any_exception = true;
        }
    };

    for (unsigned t = 0; t < num_threads; ++t) {
        threads.emplace_back(task_racy, t);
    }
    for (auto& th : threads) th.join();

    std::cout << "Phase 3 done (racy shared updates)." << std::endl;

    // Summarize
    std::cout << "any_exception = " << std::boolalpha << any_exception.load() << std::endl;
    // Print a small sample conversion to ensure no UB with thread_locals after heavy use
    std::cout << "Sample (combined head len): " << static_cast<std::string>(combined).substr(0, 32) << std::endl;

    std::cout << "=== Done ===" << std::endl;
    return any_exception.load() ? 1 : 0;
}
