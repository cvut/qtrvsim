#!/usr/bin/env python3
"""Verify source-level debuginfo tracing on compiled ELF binaries."""

import argparse
import os
import re
import subprocess
import sys

# Test definitions: ELF name, expected source extensions, minimum required coverage
TESTS = [
    {"elf": "selection_sort.elf", "ext": (".c", ".S"), "min_cov": 0.5},
    {"elf": "vect_add.elf", "ext": (".c", ".S"), "min_cov": 0.5},
    {"elf": "branchpred_1.elf", "ext": (".c", ".S"), "min_cov": 0.5},
    {"elf": "ffs_as_log2.elf", "ext": (".c", ".S"), "min_cov": 0.5},
    {"elf": "fact_ok.elf", "ext": (".S",), "min_cov": 0.8},
]

TRACE_RE = re.compile(
    r"^(Fetch|Decode|Execute|Memory|Writeback):\s+(.+?)(?:\s+\[([^:]+):(\d+)\])?$"
)


def run_test(qtrvsim_cli: str, elf_path: str, test: dict) -> tuple:
    if not os.path.isfile(elf_path):
        return None, f"SKIP (ELF not found: {elf_path})"

    cmd = [qtrvsim_cli, elf_path, "--trace-fetch", "--trace-source"]
    try:
        res = subprocess.run(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            timeout=30,
        )
    except subprocess.TimeoutExpired:
        return False, "Timed out after 30s"

    if res.returncode != 0:
        return False, f"CLI exited with code {res.returncode}: {res.stderr.strip()}"

    lines = [line for line in res.stdout.splitlines() if line.strip()]
    if not lines:
        return False, "Empty trace output"

    annotated = 0
    matched_ext = False
    for line in lines:
        m = TRACE_RE.match(line)
        if not m:
            continue
        _, _, file, lineno = m.groups()
        if file and lineno:
            annotated += 1
            if int(lineno) <= 0:
                return False, f"Invalid line number {lineno} in: {line}"
            if any(file.endswith(ext) for ext in test["ext"]):
                matched_ext = True

    cov = annotated / len(lines) if lines else 0.0
    if not matched_ext:
        return False, f"Expected source extension {test['ext']} not found in trace"
    if cov < test["min_cov"]:
        return False, f"Coverage too low: {cov:.1%} < {test['min_cov']:.1%}"

    return True, f"PASS ({annotated}/{len(lines)} lines, {cov:.1%} coverage)"


def main():
    parser = argparse.ArgumentParser(description="Test debuginfo tracing on ELFs")
    parser.add_argument("--qtrvsim-cli", required=True, help="Path to qtrvsim_cli")
    parser.add_argument("--build-dir", required=True, help="Directory containing test ELFs")
    args = parser.parse_args()

    failed = 0
    for test in TESTS:
        elf_path = os.path.join(args.build_dir, test["elf"])
        ok, msg = run_test(args.qtrvsim_cli, elf_path, test)
        status = "[SKIP]" if ok is None else "[OK]  " if ok else "[FAIL]"
        print(f"{status} {test['elf']:<25} {msg}")
        if ok is False:
            failed += 1

    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
