#!/usr/bin/env python3
import os
import sys
import subprocess
import random
import string
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CLI_SIMD = ROOT / "tests" / "integer_cli"
CLI_FALLBACK = ROOT / "tests" / "integer_cli_fallback"
SRC = ROOT / "tests" / "integer_cli.cpp"
HDR = ROOT / "Integer.h"

CLANG = os.environ.get("CLANG", "clang++")
COMMON_FLAGS = [
    "-std=c++17",
    "-O2",
    "-Wall",
    "-Wextra",
    "-Wpedantic",
    "-DENABLE_VALIDITY_CHECK",
]
ASAN_UBSAN_FLAGS = [
    "-fsanitize=address,undefined",
    "-fno-omit-frame-pointer",
]


def build_target(out_path: Path, extra_flags=None):
    flags = COMMON_FLAGS + ASAN_UBSAN_FLAGS + (extra_flags or [])
    cmd = [CLANG, str(SRC), "-o", str(out_path)] + flags
    print("[BUILD] ", " ".join(cmd))
    subprocess.check_call(cmd)


def build_all():
    build_target(CLI_SIMD)
    build_target(CLI_FALLBACK, extra_flags=["-U__AVX2__", "-U__ARM_NEON__"])


def run_cli(cli_path: Path, lines):
    proc = subprocess.Popen(
        [str(cli_path)],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    out, err = proc.communicate("\n".join(lines) + "\n", timeout=120)
    return proc.returncode, out.strip().splitlines(), err


def expect_ok(line):
    if not line.startswith("OK "):
        return None, line
    return line[3:], None


def cxx_div_trunc(a, b):
    if (a > 0) != (b > 0):
        return - (abs(a) // abs(b))
    else:
        return a // b


def cxx_mod(a, b):
    return a - cxx_div_trunc(a, b) * b


def test_deterministic(cli_path: Path):
    lines = []
    lines += [
        "U to_str 0",
        "U to_str 12345678901234567890",
        "S to_str -0",
        "S to_str -12345678901234567890",
        "U to_u64 18446744073709551615",
        "S to_s64 -9223372036854775808",
    ]
    lines += [
        "U add 123 456",
        "U sub 1000 1",
        "U mul 12345 67890",
        "U div 1000 3",
        "U mod 1000 3",
        "U cmp 1000 1001",
        "S add -5 7",
        "S sub -5 7",
        "S mul -12 34",
        "S div -100 7",
        "S mod -100 7",
        "S cmp -100 7",
    ]
    lines += [
        "U add 0 0",
        "U mul 0 123456",
        "S add 0 -0",
        "U div 1 1",
        "U mod 1 1",
    ]
    rc, out, err = run_cli(cli_path, lines)
    assert rc == 0, f"CLI exited {rc}, stderr={err}"
    idx = 0

    def ok():
        nonlocal idx
        s, e = expect_ok(out[idx])
        idx += 1
        if e:
            raise AssertionError(f"Expected OK, got: {e}")
        return s

    assert ok() == "0"
    assert ok() == "12345678901234567890"
    assert ok() == "0"
    assert ok() == "-12345678901234567890"
    assert ok() == str(18446744073709551615)
    assert ok() == str(-9223372036854775808)

    assert ok() == str(123 + 456)
    assert ok() == str(1000 - 1)
    assert ok() == str(12345 * 67890)
    assert ok() == str(1000 // 3)
    assert ok() == str(1000 % 3)
    assert ok() == str(-1)

    assert ok() == str(-5 + 7)
    assert ok() == str(-5 - 7)
    assert ok() == str(-12 * 34)
    assert ok() == str(cxx_div_trunc(-100, 7))
    assert ok() == str(cxx_mod(-100, 7))
    assert ok() == str(-1)

    assert ok() == "0"
    assert ok() == "0"
    assert ok() == "0"
    assert ok() == "1"
    assert ok() == "0"

    print(f"[OK] deterministic tests passed on {cli_path.name}")


def rand_bigint_str(max_digits=513):
    digits = 513
    s = "".join(random.choice(string.digits) for _ in range(digits))
    s = s.lstrip("0")
    return s if s else "0"


def rand_signed_str(max_digits=513):
    s = rand_bigint_str(max_digits)
    if s != "0" and random.random() < 0.5:
        s = "-" + s
    return s


def test_random(cli_path: Path, seed=0xC0FFEE, cases=2000):
    random.seed(seed)
    lines = []
    refs = []

    for _ in range(cases):
        a = rand_bigint_str()
        b = rand_bigint_str()
        op = random.choice(["add", "sub", "mul", "div", "mod", "cmp"])
        if op == "sub":
            if len(a) < len(b) or (len(a) == len(b) and a < b):
                a, b = b, a
        if op in ("div", "mod") and b == "0":
            b = "1"
        lines.append(f"U {op} {a} {b}")
        refs.append(("U", op, a, b))

    for _ in range(cases):
        a = rand_signed_str()
        b = rand_signed_str()
        op = random.choice(["add", "sub", "mul", "div", "mod", "cmp"])
        if op in ("div", "mod") and b == "0":
            b = "1"
        lines.append(f"S {op} {a} {b}")
        refs.append(("S", op, a, b))

    rc, out, err = run_cli(cli_path, lines)
    assert rc == 0, f"CLI exited {rc}, stderr={err}"

    mismatches = 0
    for i, (typ, op, a, b) in enumerate(refs):
        res, exc = expect_ok(out[i]) if i < len(out) else (None, "missing output")
        if exc:
            print(f"[ERR] [{cli_path.name}] line {i}: {typ} {op} {a} {b} => {exc}")
            mismatches += 1
            continue
        try:
            aa = int(a)
            bb = int(b)
            if typ == "U":
                if op == "add":
                    expected = aa + bb
                elif op == "sub":
                    expected = aa - bb
                elif op == "mul":
                    expected = aa * bb
                elif op == "div":
                    expected = aa // bb
                elif op == "mod":
                    expected = aa % bb
                else:
                    expected = -1 if aa < bb else (0 if aa == bb else 1)
            else:
                if op == "add":
                    expected = aa + bb
                elif op == "sub":
                    expected = aa - bb
                elif op == "mul":
                    expected = aa * bb
                elif op == "div":
                    expected = cxx_div_trunc(aa, bb)
                elif op == "mod":
                    expected = cxx_mod(aa, bb)
                else:
                    expected = -1 if aa < bb else (0 if aa == bb else 1)
            if str(expected) != res:
                print(
                    f"[MISMATCH][{cli_path.name}][{typ}] {a} {op} {b}: got {res[:120]}... vs expected {str(expected)[:120]}..."  # pyright: ignore[reportOptionalSubscript]
                )
                mismatches += 1
        except Exception as e:
            print(f"[EXC-REF][{cli_path.name}] {typ} {op} {a} {b}: {e}")
            mismatches += 1

    if mismatches == 0:
        print(f"[OK] random tests passed on {cli_path.name}")
    else:
        print(f"[WARN] random tests mismatches on {cli_path.name}: {mismatches}")
    return mismatches


def main():
    build_all()

    for cli in (CLI_SIMD, CLI_FALLBACK):
        rc, out, err = run_cli(cli, ["U add 1 2"])
        if rc != 0:
            print(f"[CLI-ERR] {cli.name}", err)
            sys.exit(1)
        print(out[0])

    test_deterministic(CLI_SIMD)
    test_deterministic(CLI_FALLBACK)

    m_simd = test_random(CLI_SIMD)
    m_fallback = test_random(CLI_FALLBACK)

    if m_simd or m_fallback:
        print(f"[SUMMARY] mismatches: SIMD={m_simd}, fallback={m_fallback}")
        sys.exit(2)

    print("[DONE] all tests passed with ASan+UBSan on both SIMD and fallback")


if __name__ == "__main__":
    main()
