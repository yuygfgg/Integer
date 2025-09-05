// Simple standalone benchmark and correctness checker between Integer and GMP

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <utility>

#include "../Integer.h"
#include <gmpxx.h>

namespace chrono = std::chrono;

struct CommandLineArgs {
    std::size_t numDecimalDigits = 2000; // digits for a,b used in add/sub/mul/div/mod
    unsigned powExponent = 1000;         // exponent for pow(base, exponent)
    unsigned factorialN = 1000;          // n for n!
    unsigned seed = 123456789u;          // RNG seed
};

static bool parseUnsigned(const char* s, std::size_t& out) {
    try {
        out = static_cast<std::size_t>(std::stoull(s));
        return true;
    } catch (...) {
        return false;
    }
}
static bool parseUnsigned(const char* s, unsigned& out) {
    try {
        out = static_cast<unsigned>(std::stoul(s));
        return true;
    } catch (...) {
        return false;
    }
}

static CommandLineArgs parseArgs(int argc, char** argv) {
    CommandLineArgs args;
    for (int i = 1; i < argc; ++i) {
        std::string key = argv[i];
        if ((key == "--digits" || key == "-d") && i + 1 < argc) {
            std::size_t v{};
            if (parseUnsigned(argv[i + 1], v)) args.numDecimalDigits = v;
            ++i;
        } else if ((key == "--exp" || key == "-e") && i + 1 < argc) {
            unsigned v{};
            if (parseUnsigned(argv[i + 1], v)) args.powExponent = v;
            ++i;
        } else if ((key == "--fact" || key == "-f") && i + 1 < argc) {
            unsigned v{};
            if (parseUnsigned(argv[i + 1], v)) args.factorialN = v;
            ++i;
        } else if (key == "--seed" && i + 1 < argc) {
            unsigned v{};
            if (parseUnsigned(argv[i + 1], v)) args.seed = v;
            ++i;
        } else if (key == "--help" || key == "-h") {
            std::cout << "Usage: ./bench_integer_vs_gmp [--digits N] [--exp E] [--fact N] [--seed S]\n";
            std::exit(0);
        }
    }
    return args;
}

// Generate a random base-10 string with exactly numDigits digits, no leading zero
static std::string generateRandomDecimalString(std::size_t numDigits, std::mt19937_64& rng) {
    if (numDigits == 0) return std::string("0");
    std::uniform_int_distribution<int> distFirst(1, 9);
    std::uniform_int_distribution<int> dist(0, 9);
    std::string s;
    s.reserve(numDigits);
    s.push_back(static_cast<char>('0' + distFirst(rng)));
    for (std::size_t i = 1; i < numDigits; ++i) s.push_back(static_cast<char>('0' + dist(rng)));
    return s;
}

template <typename Func, typename Result>
static double measureMillis(Func&& func, Result& out) {
    auto t0 = chrono::high_resolution_clock::now();
    out = func();
    auto t1 = chrono::high_resolution_clock::now();
    return chrono::duration<double, std::milli>(t1 - t0).count();
}

// Power by exponentiation-by-squaring, shared for both libraries
template <typename Big>
static Big powerBySquaring(Big base, unsigned exponent) {
    Big result = 1;
    while (exponent > 0) {
        if (exponent & 1u) result *= base;
        exponent >>= 1u;
        if (exponent) base *= base;
    }
    return result;
}

// Factorial, shared for both libraries
template <typename Big>
static Big factorial(unsigned n) {
    Big acc = 1;
    for (unsigned i = 2; i <= n; ++i) acc *= i;
    return acc;
}

static std::string toDecimalString(const UnsignedInteger& x) {
    return static_cast<std::string>(x);
}
static std::string toDecimalString(const mpz_class& x) {
    return x.get_str();
}

int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const CommandLineArgs args = parseArgs(argc, argv);
    std::mt19937_64 rng(args.seed);

    std::cout << "=== Integer vs GMP (C++) - Simple Benchmark & Correctness Check ===\n";
    std::cout << "Config: digits=" << args.numDecimalDigits
              << ", pow_exp=" << args.powExponent
              << ", fact_n=" << args.factorialN
              << ", seed=" << args.seed << "\n\n";

    // Prepare random operands a,b (ensure a >= b to satisfy UnsignedInteger subtraction precondition)
    const std::string aStr = generateRandomDecimalString(args.numDecimalDigits, rng);
    const std::string bStr = generateRandomDecimalString(args.numDecimalDigits, rng);

    UnsignedInteger uiA(aStr.c_str());
    UnsignedInteger uiB(bStr.c_str());
    mpz_class giA(aStr, 10);
    mpz_class giB(bStr, 10);

    if (giA < giB) {
        std::swap(uiA, uiB);
        std::swap(giA, giB);
    }

    auto printRow = [](const std::string& name, double msInteger, double msGmp, bool equal, std::size_t digits = 0) {
        std::cout << std::left << std::setw(12) << name
                  << "  Integer: " << std::setw(10) << std::fixed << std::setprecision(3) << msInteger << " ms"
                  << "  GMP: " << std::setw(10) << std::fixed << std::setprecision(3) << msGmp << " ms"
                  << "  Correct: " << (equal ? "yes" : "NO");
        if (digits != 0) std::cout << "  digits: " << digits;
        std::cout << "\n";
    };

    // add
    UnsignedInteger uiAdd;
    mpz_class giAdd;
    const double tAddUI = measureMillis([&] { return uiA + uiB; }, uiAdd);
    const double tAddGI = measureMillis([&] { return giA + giB; }, giAdd);
    const bool okAdd = toDecimalString(uiAdd) == toDecimalString(giAdd);
    printRow("add", tAddUI, tAddGI, okAdd);

    // sub (uiA >= uiB guaranteed)
    UnsignedInteger uiSub;
    mpz_class giSub;
    const double tSubUI = measureMillis([&] { return uiA - uiB; }, uiSub);
    const double tSubGI = measureMillis([&] { return giA - giB; }, giSub);
    const bool okSub = toDecimalString(uiSub) == toDecimalString(giSub);
    printRow("sub", tSubUI, tSubGI, okSub);

    // mul
    UnsignedInteger uiMul;
    mpz_class giMul;
    const double tMulUI = measureMillis([&] { return uiA * uiB; }, uiMul);
    const double tMulGI = measureMillis([&] { return giA * giB; }, giMul);
    const bool okMul = toDecimalString(uiMul) == toDecimalString(giMul);
    printRow("mul", tMulUI, tMulGI, okMul);

    // div (uiB != 0 by construction)
    UnsignedInteger uiDiv;
    mpz_class giDiv;
    const double tDivUI = measureMillis([&] { return uiA / uiB; }, uiDiv);
    const double tDivGI = measureMillis([&] { return giA / giB; }, giDiv);
    const bool okDiv = toDecimalString(uiDiv) == toDecimalString(giDiv);
    printRow("div", tDivUI, tDivGI, okDiv);

    // mod
    UnsignedInteger uiMod;
    mpz_class giMod;
    const double tModUI = measureMillis([&] { return uiA % uiB; }, uiMod);
    const double tModGI = measureMillis([&] { return giA % giB; }, giMod);
    const bool okMod = toDecimalString(uiMod) == toDecimalString(giMod);
    printRow("mod", tModUI, tModGI, okMod);

    // pow (use a small base to keep size moderate; exponent possibly large)
    const std::string baseStr = "123456789"; // ~9-digit base -> result ~ 9*exp digits
    UnsignedInteger uiBase(baseStr.c_str());
    mpz_class giBase(baseStr, 10);

    UnsignedInteger uiPow;
    mpz_class giPow;
    const double tPowUI = measureMillis([&] { return powerBySquaring(uiBase, args.powExponent); }, uiPow);
    const double tPowGI = measureMillis([&] { return powerBySquaring(giBase, args.powExponent); }, giPow);
    const bool okPow = toDecimalString(uiPow) == toDecimalString(giPow);
    const std::size_t powDigits = toDecimalString(uiPow).size();
    printRow("pow", tPowUI, tPowGI, okPow, powDigits);

    // factorial
    UnsignedInteger uiFact;
    mpz_class giFact;
    const double tFacUI = measureMillis([&] { return factorial<UnsignedInteger>(args.factorialN); }, uiFact);
    const double tFacGI = measureMillis([&] { return factorial<mpz_class>(args.factorialN); }, giFact);
    const bool okFac = toDecimalString(uiFact) == toDecimalString(giFact);
    const std::size_t facDigits = toDecimalString(uiFact).size();
    printRow("factorial", tFacUI, tFacGI, okFac, facDigits);

    std::cout << "\nDone.\n";
    return (okAdd && okSub && okMul && okDiv && okMod && okPow && okFac) ? 0 : 1;
}
