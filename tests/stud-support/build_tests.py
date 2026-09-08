#!/usr/bin/env python3
"""
Build stud-support ELFs for integration testing

This script compiles selected stud-support examples into RISC-V ELFs
using Clang/LLVM toolchain or GCC.
"""

import argparse
import os
import shutil
import subprocess
import sys
import glob

# List of tests to build: dict with keys:
#   name: output name
#   dir: source directory
#   file: optional single file to build
#   skip_clang: optional, skip when using Clang (assembler incompatibility)
TESTS_TO_BUILD = [
    {"name": "selection_sort", "dir": "seminaries/qtrvsim/selection-sort"},
    {"name": "vect_add", "dir": "seminaries/qtrvsim/vect-add"},
    {"name": "vect_add2", "dir": "seminaries/qtrvsim/vect-add2"},
    {"name": "vect_inc", "dir": "seminaries/qtrvsim/vect-inc"},
    {"name": "branchpred_1", "dir": "seminaries/qtrvsim/branchpred-1"},
    {"name": "ffs_as_log2", "dir": "seminaries/qtrvsim/ffs-as-log2"},
    {"name": "uart_echo_irq", "dir": "seminaries/qtrvsim/uart-echo-irq"},
    {
        "name": "call_clobber",
        "dir": "seminaries/qtrvsim/call-syscall",
        "file": "lec10-01-call4-clobber.S",
    },
    {
        "name": "call_save",
        "dir": "seminaries/qtrvsim/call-syscall",
        "file": "lec10-02-call4-save.S",
    },
    {
        "name": "fact_buggy",
        "dir": "seminaries/qtrvsim/call-syscall",
        "file": "lec10-03-fact-buggy.S",
    },
    {
        "name": "fact_ok",
        "dir": "seminaries/qtrvsim/call-syscall",
        "file": "lec10-04-fact-ok.S",
    },
    # skip_clang: Clang's integrated assembler doesn't accept `la` with preprocessor macro
    {
        "name": "call_10args",
        "dir": "seminaries/qtrvsim/call-syscall",
        "file": "lec10-05-call-10args.S",
        "skip_clang": True,
    },
    {
        "name": "linus_hello",
        "dir": "seminaries/qtrvsim/call-syscall",
        "file": "lec10-06-linus-hello.S",
    },
]


def get_toolchain_config(use_clang=False, xlen=32):
    """Get toolchain configuration

    Args:
        use_clang: Force Clang/LLVM toolchain
        xlen: Target XLEN (32 or 64)
    """
    arch_prefix = os.getenv("ARCH", "riscv64-unknown-elf")

    if xlen == 32:
        march = "rv32g"
        mabi = "ilp32"
        clang_target = "riscv32"
    else:
        march = "rv64g"
        mabi = "lp64"
        clang_target = "riscv64"

    # Prefer GCC unless Clang is forced or GCC is missing and Clang is present
    if use_clang or (not shutil.which(f"{arch_prefix}-gcc") and shutil.which("clang")):
        cc = os.getenv("CC", "clang")
        # Note: -gdwarf-4 is required because QtRvSim uses libelfin which only supports DWARFv4.
        # Modern Clang defaults to DWARF5 which won't be parsed correctly.
        cflags = os.getenv(
            "CFLAGS",
            f"--target={clang_target} -march={march} -nostdlib -static -fuse-ld=lld -g -gdwarf-4",
        )
        return {
            "name": f"Clang/LLVM (RV{xlen})",
            "cc": cc,
            "cflags": cflags,
            "ld": cc,
            "ldflags": cflags,
            "objcopy": "llvm-objcopy",
            "required": [cc, "ld.lld", "llvm-objcopy"],
            "xlen": xlen,
        }

    cc = os.getenv("CC", f"{arch_prefix}-gcc")
    return {
        "name": f"GNU (RV{xlen})",
        "cc": cc,
        "cflags": os.getenv(
            "CFLAGS", f"-g -static -nostdlib -march={march} -mabi={mabi}"
        ),
        "ld": f"{arch_prefix}-ld",
        "ldflags": "",
        "objcopy": f"{arch_prefix}-objcopy",
        "required": [cc],
        "xlen": xlen,
    }


def check_toolchain(config):
    """Check if required tools are available"""
    for tool in config["required"]:
        if not shutil.which(tool):
            return False, f"Tool not found: {tool}"
    return True, "OK"


def build_test(
    source_dir,
    output_name,
    toolchain,
    stud_support_root,
    output_dir,
    verbose=False,
    single_file=None,
):
    """Build a single test"""
    full_source_dir = os.path.join(stud_support_root, source_dir)
    output_elf = os.path.join(output_dir, f"{output_name}.elf")

    print(f"Building {output_name}...")
    if not os.path.isdir(full_source_dir):
        print(f"  ERROR: Source directory not found: {full_source_dir}")
        return False, "source not found"

    try:
        if single_file:
            source_file = os.path.join(full_source_dir, single_file)
            if not os.path.isfile(source_file):
                print(f"  ERROR: Source file not found: {source_file}")
                return False, "source file not found"

            _, ext = os.path.splitext(source_file)
            cmd = [toolchain["cc"]]
            if ext in [".S", ".s"]:
                cmd.append("-D__ASSEMBLY__")
            if toolchain["cflags"]:
                cmd.extend(toolchain["cflags"].split())
            elif toolchain["name"] == "GNU":
                # Default flags for GCC if not specified
                cmd.extend(
                    ["-march=rv32im", "-mabi=ilp32", "-nostdlib", "-static", "-g"]
                )

            cmd.extend(["-o", output_elf, source_file])

            result = subprocess.run(
                cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True
            )
            if result.returncode != 0:
                print(f"  ERROR: Build failed with return code {result.returncode}")
                print("  Output:")
                print(result.stdout)
                print("  Error:")
                print(result.stderr)
                return False, "build error"

            print(f"  SUCCESS: {output_elf}")
            return True, None

        else:
            # Makefile build
            subprocess.run(
                ["make", "clean"],
                cwd=full_source_dir,
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
                check=False,
            )

            make_vars = [
                f"CC={toolchain['cc']}",
                f"AS={toolchain['cc']}",
                f"CXX={toolchain['cc']}",
                f"LD={toolchain['ld']}",
                f"OBJCOPY={toolchain['objcopy']}",
                f"LOADLIBES=",
            ]

            if toolchain["cflags"]:
                make_vars.extend(
                    [
                        f"CFLAGS={toolchain['cflags']}",
                        f"AFLAGS={toolchain['cflags']}",
                        f"CXXFLAGS={toolchain['cflags']}",
                    ]
                )

            if toolchain["ldflags"]:
                make_vars.append(f"LDFLAGS={toolchain['ldflags']}")

            result = subprocess.run(
                ["make"] + make_vars,
                cwd=full_source_dir,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
            )
            if result.returncode != 0:
                print(f"  ERROR: Build failed with return code {result.returncode}")
                print("  Output:")
                print(result.stdout)
                print("  Error:")
                print(result.stderr)
                return False, "build error"

            # Find built ELF
            candidates = [
                os.path.join(full_source_dir, os.path.basename(source_dir))
            ] + glob.glob(os.path.join(full_source_dir, "*.elf"))

            for candidate in candidates:
                if os.path.isfile(candidate) and os.access(candidate, os.X_OK):
                    # Simple check if it looks like an ELF (starts with \x7fELF)
                    with open(candidate, "rb") as f:
                        if f.read(4) == b"\x7fELF":
                            shutil.copy2(candidate, output_elf)
                            print(f"  SUCCESS: {output_elf}")
                            return True, None

            print(f"  ERROR: Built ELF not found in {full_source_dir}")
            return False, "ELF not found"

    except Exception as e:
        print(f"  ERROR: Exception: {e}")
        return False, f"exception: {e}"


def main():
    parser = argparse.ArgumentParser(description="Build stud-support ELFs")
    parser.add_argument(
        "--verbose", "-v", action="store_true", help="Show detailed output"
    )
    parser.add_argument("--use-gcc", action="store_true", help="Force GNU toolchain")
    parser.add_argument(
        "--use-clang", action="store_true", help="Force Clang toolchain"
    )
    parser.add_argument(
        "--xlen",
        type=int,
        choices=[32, 64],
        default=32,
        help="Target XLEN: 32 or 64 (default: 32)",
    )
    parser.add_argument("--output-dir", help="Output directory")
    parser.add_argument(
        "--stud-support-path", required=True, help="Path to stud-support repo"
    )
    args = parser.parse_args()

    output_dir = args.output_dir or os.path.join(
        os.path.dirname(os.path.realpath(__file__)), "elfs"
    )
    if not os.path.isdir(args.stud_support_path):
        print(f"Error: stud-support not found at {args.stud_support_path}")
        return 1

    toolchain = get_toolchain_config(args.use_clang, args.xlen)
    if args.use_gcc and "GNU" not in toolchain["name"]:
        # User forced GCC but we defaulted to Clang (shouldn't happen with current logic but good to be safe)
        toolchain = get_toolchain_config(False, args.xlen)

    available, msg = check_toolchain(toolchain)
    if not available:
        print(f"Error: {msg}\nPlease install riscv64-unknown-elf-gcc or clang/lld.")
        return 1

    os.makedirs(output_dir, exist_ok=True)
    print(f"Using toolchain: {toolchain['name']} ({toolchain['cc']})")
    print(f"Output: {output_dir}\n")

    is_clang = "Clang" in toolchain["name"]
    tests_to_run = [
        t for t in TESTS_TO_BUILD if not (is_clang and t.get("skip_clang", False))
    ]
    skipped = len(TESTS_TO_BUILD) - len(tests_to_run)

    results = [
        build_test(
            t["dir"],
            t["name"],
            toolchain,
            args.stud_support_path,
            output_dir,
            args.verbose,
            t.get("file"),
        )
        for t in tests_to_run
    ]
    success_count = sum(1 for r in results if r[0])

    print(f"\nBuild Summary: {success_count}/{len(tests_to_run)} successful", end="")
    if skipped:
        print(f" ({skipped} skipped for {toolchain['name']})")
    else:
        print()
    if success_count < len(tests_to_run):
        print("Failed tests:")
        for i, (success, reason) in enumerate(results):
            if not success:
                print(f"  - {tests_to_run[i]['name']} ({reason})")
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
