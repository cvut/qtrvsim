#!/usr/bin/env python3
"""
Property-Based Debug Info Test Runner for QtRvSim

This script tests debuginfo (source-level tracing) for compiled C/assembly ELF files.
Instead of checking exact output (which is brittle due to compiler variations),
it verifies semantic properties of the debug info:

1. File presence: Expected source files appear in the trace
2. Line validity: Line numbers are positive integers
3. Coverage: Traced instructions have valid debug info annotations
4. Multi-file support: Multiple source files are correctly attributed

Usage:
    python test_debuginfo.py --qtrvsim-cli /path/to/qtrvsim_cli --build-dir /path/to/elfs

This integrates with the stud-support test infrastructure.
"""

from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
from dataclasses import dataclass
from typing import List, Optional, Tuple


@dataclass
class TraceEntry:
    """A single trace entry from qtrvsim_cli output."""

    stage: str
    instruction: str
    filename: Optional[str]
    line: Optional[int]
    raw_line: str


@dataclass
class TestResult:
    """Result of a single test."""

    name: str
    passed: bool
    message: str
    details: Optional[str] = None


# Define debug info tests
# Each test specifies an ELF and properties to verify
#
# Note: stud-support Makefiles generate intermediate .S files from .c sources,
# so debug info may point to .S files even for C programs. We accept either.
#
# Note: Clang-built ELFs have debug info parsing issues with QtRvSim's libelfin
# (which only supports DWARFv4). Even with -gdwarf-4, the stud-support Makefiles
# may override flags or Clang generates slightly different DWARF that libelfin
# cannot parse. These tests are skipped for Clang builds.
DEBUGINFO_TESTS = [
    {
        "name": "selection_sort_debuginfo",
        "elf": "selection_sort.elf",
        "description": "C program with loops - verify source file appears",
        "expected_files": [".c", ".S"],  # May be .c or generated .S
        "require_debuginfo_coverage": 0.5,  # At least 50% of traces should have debug info
        "skip_clang": True,  # libelfin doesn't parse Clang DWARF correctly
    },
    {
        "name": "vect_add_debuginfo",
        "elf": "vect_add.elf",
        "description": "Vector addition - verify debug info present",
        "expected_files": [".c", ".S"],  # May have C or assembly
        "require_debuginfo_coverage": 0.3,
        "skip_clang": True,  # libelfin doesn't parse Clang DWARF correctly
    },
    {
        "name": "branchpred_debuginfo",
        "elf": "branchpred_1.elf",
        "description": "Branch prediction test - verify debug info",
        "expected_files": [".c", ".S"],  # May be .c or generated .S
        "require_debuginfo_coverage": 0.5,
        "skip_clang": True,  # libelfin doesn't parse Clang DWARF correctly
    },
    {
        "name": "ffs_debuginfo",
        "elf": "ffs_as_log2.elf",
        "description": "Find first set - verify debug info for C code",
        "expected_files": [".c", ".S"],  # May be .c or generated .S
        "require_debuginfo_coverage": 0.5,
        "skip_clang": True,  # libelfin doesn't parse Clang DWARF correctly
    },
    {
        "name": "fact_ok_debuginfo",
        "elf": "fact_ok.elf",
        "description": "Factorial (assembly) - verify .S file appears",
        "expected_files": [".S"],
        "require_debuginfo_coverage": 0.8,  # Assembly should have good coverage
        "skip_clang": True,  # libelfin doesn't parse Clang DWARF correctly
    },
    {
        "name": "call_10args_debuginfo",
        "elf": "call_10args.elf",
        "description": "10-argument call - verify assembly debug info",
        "expected_files": [".S"],
        "require_debuginfo_coverage": 0.8,
        "skip_if_missing": True,  # May be skipped for Clang builds (assembler incompatibility)
        "skip_clang": True,  # libelfin doesn't parse Clang DWARF correctly
    },
]


def parse_trace_output(output: str) -> List[TraceEntry]:
    """
    Parse qtrvsim_cli trace output into structured entries.

    Expected format: "Fetch: addi x1, x0, 1 [filename.c:42]"
    The [filename:line] part is optional.
    """
    entries = []

    # Pattern for trace lines with optional debug info
    # Matches: "Stage: instruction [file:line]" or "Stage: instruction"
    trace_pattern = re.compile(
        r"^(Fetch|Decode|Execute|Memory|Writeback):\s+"  # Stage
        r"(.+?)"  # Instruction (non-greedy)
        r"(?:\s+\[([^:]+):(\d+)\])?"  # Optional [file:line]
        r"\s*$"
    )

    for line in output.splitlines():
        line = line.strip()
        if not line:
            continue

        match = trace_pattern.match(line)
        if match:
            stage = match.group(1)
            instruction = match.group(2).strip()
            filename = match.group(3)
            line_num = int(match.group(4)) if match.group(4) else None

            entries.append(
                TraceEntry(
                    stage=stage,
                    instruction=instruction,
                    filename=filename,
                    line=line_num,
                    raw_line=line,
                )
            )

    return entries


def check_file_presence(
    entries: List[TraceEntry], expected_patterns: List[str]
) -> Tuple[bool, str]:
    """
    Check that at least one of the expected file patterns appears in the trace.

    Args:
        entries: Parsed trace entries
        expected_patterns: List of patterns (e.g., [".c", ".S"]) where at least one should appear

    Returns:
        (passed, message) tuple
    """
    found_files = set()
    for entry in entries:
        if entry.filename:
            found_files.add(entry.filename)

    if not found_files:
        return False, "No source files found in trace"

    # Check if ANY of the expected patterns match (not ALL)
    matched_patterns = []
    for pattern in expected_patterns:
        if any(pattern in f for f in found_files):
            matched_patterns.append(pattern)

    if not matched_patterns:
        return (
            False,
            f"No expected file patterns {expected_patterns} found. Found: {sorted(found_files)}",
        )

    return True, f"Found files: {sorted(found_files)} (matched: {matched_patterns})"


def check_line_validity(entries: List[TraceEntry]) -> Tuple[bool, str]:
    """
    Check that all line numbers are valid (positive integers).
    """
    invalid_entries = []

    for entry in entries:
        if entry.line is not None:
            if entry.line <= 0:
                invalid_entries.append(f"Line {entry.line} in {entry.filename}")

    if invalid_entries:
        return False, f"Invalid line numbers: {invalid_entries[:5]}"

    return True, "All line numbers are valid"


def check_debuginfo_coverage(
    entries: List[TraceEntry], min_coverage: float
) -> Tuple[bool, str]:
    """
    Check that at least min_coverage fraction of trace entries have debug info.
    """
    if not entries:
        return False, "No trace entries found"

    with_debuginfo = sum(1 for e in entries if e.filename is not None)
    total = len(entries)
    coverage = with_debuginfo / total

    if coverage < min_coverage:
        return (
            False,
            f"Debug info coverage {coverage:.1%} < required {min_coverage:.1%} ({with_debuginfo}/{total} entries)",
        )

    return (
        True,
        f"Debug info coverage: {coverage:.1%} ({with_debuginfo}/{total} entries)",
    )


def check_no_instruction_without_stage(entries: List[TraceEntry]) -> Tuple[bool, str]:
    """
    Verify trace format is correct - all entries should have a stage.
    """
    invalid = [e for e in entries if not e.stage]
    if invalid:
        return False, f"Found {len(invalid)} entries without stage"
    return True, "All entries have valid stage"


def is_clang_build(build_dir: str) -> bool:
    """Check if the build directory indicates a Clang build."""
    return "clang" in os.path.basename(build_dir).lower()


def run_debuginfo_test(
    qtrvsim_cli: str,
    elf_path: str,
    test_config: dict,
    verbose: bool = False,
    build_dir: str = "",
) -> TestResult:
    """
    Run a single debug info test.
    """
    test_name = test_config["name"]

    # Skip Clang builds if configured (libelfin doesn't parse Clang DWARF correctly)
    if test_config.get("skip_clang") and is_clang_build(build_dir):
        return TestResult(
            name=test_name,
            passed=True,
            message="SKIPPED (Clang debug info not supported)",
        )

    if not os.path.exists(elf_path):
        if test_config.get("skip_if_missing"):
            return TestResult(
                name=test_name, passed=True, message="SKIPPED (ELF not built)"
            )
        return TestResult(
            name=test_name, passed=False, message=f"ELF not found: {elf_path}"
        )

    # Run qtrvsim_cli with trace options
    cmd = [qtrvsim_cli, elf_path, "--trace-fetch", "--trace-source"]

    try:
        result = subprocess.run(
            cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, timeout=30
        )
    except subprocess.TimeoutExpired:
        return TestResult(name=test_name, passed=False, message="Test timed out (30s)")
    except Exception as e:
        return TestResult(
            name=test_name, passed=False, message=f"Failed to run CLI: {e}"
        )

    # Parse output
    entries = parse_trace_output(result.stdout)

    if not entries:
        return TestResult(
            name=test_name,
            passed=False,
            message="No trace entries parsed from output",
            details=result.stdout[:500] if verbose else None,
        )

    # Run all property checks
    checks = []
    all_passed = True

    # Check 1: File presence
    if "expected_files" in test_config:
        passed, msg = check_file_presence(entries, test_config["expected_files"])
        checks.append(f"File presence: {'PASS' if passed else 'FAIL'} - {msg}")
        all_passed = all_passed and passed

    # Check 2: Line validity
    passed, msg = check_line_validity(entries)
    checks.append(f"Line validity: {'PASS' if passed else 'FAIL'} - {msg}")
    all_passed = all_passed and passed

    # Check 3: Debug info coverage
    if "require_debuginfo_coverage" in test_config:
        passed, msg = check_debuginfo_coverage(
            entries, test_config["require_debuginfo_coverage"]
        )
        checks.append(f"Coverage: {'PASS' if passed else 'FAIL'} - {msg}")
        all_passed = all_passed and passed

    # Check 4: Format validity
    passed, msg = check_no_instruction_without_stage(entries)
    checks.append(f"Format: {'PASS' if passed else 'FAIL'} - {msg}")
    all_passed = all_passed and passed

    return TestResult(
        name=test_name,
        passed=all_passed,
        message="All checks passed" if all_passed else "Some checks failed",
        details="\n".join(checks) if verbose or not all_passed else None,
    )


def main():
    parser = argparse.ArgumentParser(
        description="Property-based debug info tests for QtRvSim"
    )
    parser.add_argument("--qtrvsim-cli", help="Path to qtrvsim_cli executable")
    parser.add_argument("--build-dir", help="Directory containing test ELF files")
    parser.add_argument(
        "-v", "--verbose", action="store_true", help="Show detailed output"
    )
    parser.add_argument("-f", "--filter", help="Filter tests by name substring")
    parser.add_argument(
        "--list", action="store_true", help="List available tests and exit"
    )

    args = parser.parse_args()

    if args.list:
        print("Available debug info tests:")
        for test in DEBUGINFO_TESTS:
            print(f"  {test['name']}: {test['description']}")
        return 0

    # Validate required arguments when not just listing
    if not args.qtrvsim_cli:
        print("Error: --qtrvsim-cli is required")
        return 1
    if not args.build_dir:
        print("Error: --build-dir is required")
        return 1

    # Helper to check if file is executable
    def is_exe(fpath):
        return os.path.isfile(fpath) and (os.name == "nt" or os.access(fpath, os.X_OK))

    # Validate CLI path (handle Windows .exe extension)
    qtrvsim_cli = args.qtrvsim_cli
    if not is_exe(qtrvsim_cli):
        # Try appending .exe for Windows if not present
        if (
            os.name == "nt"
            and not qtrvsim_cli.endswith(".exe")
            and is_exe(qtrvsim_cli + ".exe")
        ):
            qtrvsim_cli += ".exe"
        else:
            print(f"Error: qtrvsim_cli not found or not executable at {qtrvsim_cli}")
            return 1

    # Filter tests
    tests = DEBUGINFO_TESTS
    if args.filter:
        tests = [t for t in tests if args.filter in t["name"]]
        if not tests:
            print(f"No tests match filter: {args.filter}")
            return 1

    print(f"Running {len(tests)} debug info tests")
    print(f"Using CLI: {qtrvsim_cli}")
    print(f"ELF dir: {args.build_dir}")
    print("=" * 70)

    # Run tests
    results = []
    for test_config in tests:
        elf_path = os.path.join(args.build_dir, test_config["elf"])
        result = run_debuginfo_test(
            qtrvsim_cli, elf_path, test_config, args.verbose, args.build_dir
        )
        results.append(result)

        # Print result
        status = "[OK]  " if result.passed else "[FAIL]"
        print(f"{status} {result.name:40s} {result.message}")
        if result.details:
            for line in result.details.splitlines():
                print(f"       {line}")

    # Summary
    passed = sum(1 for r in results if r.passed)
    total = len(results)

    print("=" * 70)
    print(f"Results: {passed}/{total} passed")

    if passed < total:
        print("\nFailed tests:")
        for result in results:
            if not result.passed:
                print(f"  - {result.name}")
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
