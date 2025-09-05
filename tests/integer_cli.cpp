#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include "../Integer.h"

// Simple CLI to exercise UnsignedInteger and SignedInteger
// Protocol (per line, whitespace separated):
//   <type> <op> <a> [b]
// where:
//   <type>: U | S   (UnsignedInteger or SignedInteger)
//   <op>: add sub mul div mod cmp to_str to_u64 to_s64 to_double
//   <a>, <b>: base-10 integer strings (for S may start with '-')
// Output:
//   On success:  "OK <result>" (result is decimal string or scalar)
//   On exception: "EXC <what>"

static inline void trim(std::string &s) {
    size_t p = 0; while (p < s.size() && std::isspace(static_cast<unsigned char>(s[p]))) ++p; s.erase(0, p);
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string line;
    while (std::getline(std::cin, line)) {
        trim(line);
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string type, op, a, b;
        iss >> type >> op >> a;
        if (type.empty() || op.empty() || a.empty()) {
            std::cout << "EXC invalid input" << '\n';
            continue;
        }
        if (op == "add" || op == "sub" || op == "mul" || op == "div" || op == "mod" || op == "cmp") {
            if (!(iss >> b)) { std::cout << "EXC missing operand" << '\n'; continue; }
        }
        try {
            if (type == "U") {
                if (op == "to_str") {
                    UnsignedInteger ua(a.c_str());
                    std::cout << "OK " << ua << '\n';
                } else if (op == "to_u64") {
                    UnsignedInteger ua(a.c_str());
                    unsigned long long v = static_cast<unsigned long long>(ua);
                    std::cout << "OK " << v << '\n';
                } else if (op == "to_double") {
                    UnsignedInteger ua(a.c_str());
                    double v = static_cast<double>(ua);
                    std::ostringstream os; os.setf(std::ios::fixed); os.precision(0); os << v; // no frac
                    std::cout << "OK " << os.str() << '\n';
                } else if (op == "add" || op == "sub" || op == "mul" || op == "div" || op == "mod") {
                    UnsignedInteger ua(a.c_str());
                    UnsignedInteger ub(b.c_str());
                    if (op == "add") {
                        auto r = ua + ub;
                        std::cout << "OK " << r << '\n';
                    } else if (op == "sub") {
                        auto r = ua - ub; // may throw if ua < ub
                        std::cout << "OK " << r << '\n';
                    } else if (op == "mul") {
                        auto r = ua * ub;
                        std::cout << "OK " << r << '\n';
                    } else if (op == "div") {
                        auto r = ua / ub; // may throw if ub == 0
                        std::cout << "OK " << r << '\n';
                    } else { // mod
                        auto r = ua % ub; // may throw if ub == 0
                        std::cout << "OK " << r << '\n';
                    }
                } else if (op == "cmp") {
                    UnsignedInteger ua(a.c_str());
                    UnsignedInteger ub(b.c_str());
                    int sgn = (ua < ub) ? -1 : (ua == ub ? 0 : 1);
                    std::cout << "OK " << sgn << '\n';
                } else {
                    std::cout << "EXC unknown op" << '\n';
                }
            } else if (type == "S") {
                if (op == "to_str") {
                    SignedInteger sa(a.c_str());
                    std::cout << "OK " << sa << '\n';
                } else if (op == "to_s64") {
                    SignedInteger sa(a.c_str());
                    long long v = static_cast<long long>(sa);
                    std::cout << "OK " << v << '\n';
                } else if (op == "to_u64") {
                    SignedInteger sa(a.c_str());
                    unsigned long long v = static_cast<unsigned long long>(sa); // may throw if negative
                    std::cout << "OK " << v << '\n';
                } else if (op == "to_double") {
                    SignedInteger sa(a.c_str());
                    double v = static_cast<double>(sa);
                    std::ostringstream os; os.setf(std::ios::fixed); os.precision(0); os << v; // no frac
                    std::cout << "OK " << os.str() << '\n';
                } else if (op == "add" || op == "sub" || op == "mul" || op == "div" || op == "mod") {
                    SignedInteger sa(a.c_str());
                    SignedInteger sb(b.c_str());
                    if (op == "add") {
                        auto r = sa + sb;
                        std::cout << "OK " << r << '\n';
                    } else if (op == "sub") {
                        auto r = sa - sb;
                        std::cout << "OK " << r << '\n';
                    } else if (op == "mul") {
                        auto r = sa * sb;
                        std::cout << "OK " << r << '\n';
                    } else if (op == "div") {
                        auto r = sa / sb; // may throw if sb == 0
                        std::cout << "OK " << r << '\n';
                    } else { // mod
                        auto r = sa % sb; // may throw if sb == 0
                        std::cout << "OK " << r << '\n';
                    }
                } else if (op == "cmp") {
                    SignedInteger sa(a.c_str());
                    SignedInteger sb(b.c_str());
                    int sgn = (sa < sb) ? -1 : (sa == sb ? 0 : 1);
                    std::cout << "OK " << sgn << '\n';
                } else {
                    std::cout << "EXC unknown op" << '\n';
                }
            } else {
                std::cout << "EXC unknown type" << '\n';
            }
        } catch (const std::exception &e) {
            std::cout << "EXC " << e.what() << '\n';
        }
    }
    return 0;
}
