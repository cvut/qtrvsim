#!/usr/bin/env python3
"""
Stud-Support Integration Test Runner
Builds and runs all stud-support integration tests for QtRvSim

Usage: ./run_all.py [OPTIONS]

Options:
    --stud-support PATH   Path to stud-support repository (required)
    --qtrvsim-cli PATH    Path to qtrvsim_cli (default: ../../build/target/qtrvsim_cli)
    --build-only          Only build ELFs, don't run tests
    --skip-build          Skip building, use existing ELFs
    --verbose, -v         Show detailed output
    --help, -h            Show this help message
"""

import argparse
import os
import subprocess
import sys
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(
        description="Stud-Support Integration Test Runner",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "--stud-support",
        dest="stud_support_path",
        required=True,
        help="Path to stud-support repository (required)",
    )
    parser.add_argument(
        "--qtrvsim-cli",
        dest="qtrvsim_cli",
        default=None,
        help="Path to qtrvsim_cli (default: ../../build/target/qtrvsim_cli)",
    )
    parser.add_argument(
        "--build-only",
        action="store_true",
        help="Only build ELFs, don't run tests",
    )
    parser.add_argument(
        "--skip-build",
        action="store_true",
        help="Skip building, use existing ELFs",
    )
    parser.add_argument(
        "--verbose",
        "-v",
        action="store_true",
        help="Show detailed output",
    )

    args = parser.parse_args()

    # Determine paths
    script_dir = Path(__file__).parent.resolve()
    qtrvsim_cli = (
        Path(args.qtrvsim_cli)
        if args.qtrvsim_cli
        else script_dir / "../../build/target/qtrvsim_cli"
    )
    qtrvsim_cli = qtrvsim_cli.resolve()
    stud_support_path = Path(args.stud_support_path).resolve()
    build_dir = script_dir / "elfs"

    # Validate stud-support path
    if not stud_support_path.is_dir():
        print(
            f"Error: stud-support directory not found: {stud_support_path}",
            file=sys.stderr,
        )
        sys.exit(1)

    verbose_args = ["-v"] if args.verbose else []

    # Build ELFs if not skipping
    if not args.skip_build:
        print("==========================================")
        print("Building test ELFs...")
        print("==========================================")

        build_cmd = [
            sys.executable,
            str(script_dir / "build_tests.py"),
            "--stud-support-path",
            str(stud_support_path),
            "--output-dir",
            str(build_dir),
        ] + verbose_args

        result = subprocess.run(build_cmd)
        if result.returncode != 0:
            print("Error: Build failed", file=sys.stderr)
            sys.exit(1)

        if args.build_only:
            print()
            print(f"Build complete. ELFs are in: {build_dir}")
            sys.exit(0)

    # Check qtrvsim_cli exists
    if not qtrvsim_cli.exists():
        print(f"Error: qtrvsim_cli not found: {qtrvsim_cli}", file=sys.stderr)
        print("Build QtRvSim first or specify --qtrvsim-cli PATH", file=sys.stderr)
        sys.exit(1)

    if not os.access(qtrvsim_cli, os.X_OK):
        print(f"Error: qtrvsim_cli is not executable: {qtrvsim_cli}", file=sys.stderr)
        sys.exit(1)

    # Check ELF directory exists
    if not build_dir.is_dir():
        print(f"Error: ELF directory not found: {build_dir}", file=sys.stderr)
        print("Run without --skip-build to build the ELFs first", file=sys.stderr)
        sys.exit(1)

    # Run functional tests
    print()
    print("==========================================")
    print("Running functional tests...")
    print("==========================================")

    func_cmd = [
        sys.executable,
        str(script_dir / "run_tests.py"),
        "--qtrvsim-cli",
        str(qtrvsim_cli),
        "--build-dir",
        str(build_dir),
        "--stud-support-path",
        str(stud_support_path),
    ] + verbose_args

    func_result = subprocess.run(func_cmd)

    # Run debug info tests
    print()
    print("==========================================")
    print("Running debug info tests...")
    print("==========================================")

    debug_cmd = [
        sys.executable,
        str(script_dir / "test_debuginfo.py"),
        "--qtrvsim-cli",
        str(qtrvsim_cli),
        "--build-dir",
        str(build_dir),
    ] + verbose_args

    debug_result = subprocess.run(debug_cmd)

    # Summary
    print()
    print("==========================================")
    print("Summary")
    print("==========================================")

    func_passed = func_result.returncode == 0
    debug_passed = debug_result.returncode == 0

    print(f"Functional tests:  {'PASSED' if func_passed else 'FAILED'}")
    print(f"Debug info tests:  {'PASSED' if debug_passed else 'FAILED'}")

    if not func_passed or not debug_passed:
        sys.exit(1)

    print()
    print("All tests passed!")
    sys.exit(0)


if __name__ == "__main__":
    main()
