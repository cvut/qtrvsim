#!/usr/bin/env python3
"""
stud-support Integration Test Runner for QtRvSim
"""

import argparse
import os
import re
import subprocess
import sys

# Define all stud-support tests
TESTS = [
    {
        "name": "selection_sort",
        "elf_path": "elfs/selection_sort.elf",
        "description": "Selection sort algorithm test",
        "expected_registers": {"s0": 60, "s1": 60},
        "min_cycles": 500, "max_cycles": 5000,
    },
    {
        "name": "vect_add",
        "elf_path": "elfs/vect_add.elf",
        "description": "Vector addition test",
        "expected_registers": {"x8": 0},
        "expected_memory": [(0x400 + 64, 0x11), (0x400 + 68, 0x22), (0x400 + 72, 0x33)],
        "min_cycles": 100, "max_cycles": 2000,
    },
    {
        "name": "vect_add2",
        "elf_path": "elfs/vect_add2.elf",
        "description": "Vector addition variant test",
        "expected_registers": {"t3": 0},
        "min_cycles": 100, "max_cycles": 2000,
    },
    {
        "name": "vect_inc",
        "elf_path": "elfs/vect_inc.elf",
        "description": "Vector increment test",
        "expected_registers": {"t3": 0},
        "min_cycles": 100, "max_cycles": 2000,
    },
    {
        "name": "branchpred_1",
        "elf_path": "elfs/branchpred_1.elf",
        "description": "Branch prediction test",
        "expected_registers": {"s2": 20, "s0": 4, "s1": 5},
        "min_cycles": 50, "max_cycles": 500,
    },
    {
        "name": "ffs_as_log2",
        "elf_path": "elfs/ffs_as_log2.elf",
        "description": "Find first set as log2 test",
        "expected_registers": {"a0": 7, "t1": 7},
        "min_cycles": 20, "max_cycles": 200,
    },
    {
        "name": "uart_echo_irq",
        "elf_path": "elfs/uart_echo_irq.elf",
        "description": "UART echo with interrupts test (hits BREAK early)",
        "min_cycles": 5, "max_cycles": 50,
    },
    {
        "name": "call_clobber",
        "elf_path": "elfs/call_clobber.elf",
        "description": "Function call with register clobbering",
        "expected_registers": {"a0": 0xfffffffffffffffc},
        "min_cycles": 15, "max_cycles": 200,
    },
    {
        "name": "call_save",
        "elf_path": "elfs/call_save.elf",
        "description": "Function call with register saving",
        "expected_registers": {"a0": 0xfffffffffffffffc},
        "min_cycles": 15, "max_cycles": 200,
    },
    {
        "name": "fact_ok",
        "elf_path": "elfs/fact_ok.elf",
        "description": "Correct factorial implementation",
        "expected_registers": {"t0": 24, "a0": 24},
        "min_cycles": 50, "max_cycles": 500,
    },
    {
        "name": "call_10args",
        "elf_path": "elfs/call_10args.elf",
        "description": "Function call with 10 arguments",
        "expected_registers": {"a0": 55},
        "min_cycles": 30, "max_cycles": 300,
    },
    {
        "name": "linus_hello",
        "elf_path": "elfs/linus_hello.elf",
        "description": "Linux-style hello world with syscalls (hits ECALL early)",
        "min_cycles": 5, "max_cycles": 50,
    },
]

# RISC-V ABI name to register number mapping
ABI_TO_NUM = {
    "zero": 0, "ra": 1, "sp": 2, "gp": 3, "tp": 4,
    "t0": 5, "t1": 6, "t2": 7,
    "s0": 8, "fp": 8, "s1": 9,
    "a0": 10, "a1": 11, "a2": 12, "a3": 13, "a4": 14, "a5": 15, "a6": 16, "a7": 17,
    "s2": 18, "s3": 19, "s4": 20, "s5": 21, "s6": 22, "s7": 23, "s8": 24, "s9": 25, "s10": 26, "s11": 27,
    "t3": 28, "t4": 29, "t5": 30, "t6": 31
}

def get_register_value(registers, reg_name):
    """Get register value by name, supporting multiple naming conventions"""
    if reg_name in registers: return registers[reg_name]
    
    # Try as "R" + number format
    r_name = f"R{reg_name}" if not reg_name.startswith('R') else reg_name
    if r_name in registers: return registers[r_name]
        
    # Try converting ABI name to register number
    if reg_name in ABI_TO_NUM:
        r_name = f"R{ABI_TO_NUM[reg_name]}"
        if r_name in registers: return registers[r_name]
            
    # Try as "x" + number format
    if reg_name.startswith('x'):
        try:
            r_name = f"R{int(reg_name[1:])}"
            if r_name in registers: return registers[r_name]
        except ValueError: pass
            
    return None

def verify_result(test, result, verbose):
    """Verify test result against expectations"""
    if result.returncode != 0:
        return False, f"Exit code {result.returncode} (expected 0)" + (f"\nStdout: {result.stdout}" if verbose else "")

    # Parse output
    registers = {m.group(1): int(m.group(2), 16) for m in re.finditer(r'([A-Za-z0-9_]+):0x([0-9a-fA-F]+)', result.stdout)}
    cycles_match = re.search(r'cycles:\s*(\d+)', result.stdout)
    cycles = int(cycles_match.group(1)) if cycles_match else None

    # Verify registers
    for reg, expected in test.get("expected_registers", {}).items():
        actual = get_register_value(registers, reg)
        if actual is None:
            return False, f"Register {reg} not found" + (f"\nAvailable: {list(registers.keys())}" if verbose else "")
        if actual != expected:
            return False, f"Register {reg}: expected 0x{expected:x}, got 0x{actual:x}" + (f"\nAll: {registers}" if verbose else "")

    # Verify cycles
    if cycles is not None:
        if cycles < test.get("min_cycles", 0):
            return False, f"Cycles {cycles} < min {test['min_cycles']}"
        if test.get("max_cycles") and cycles > test["max_cycles"]:
            return False, f"Cycles {cycles} > max {test['max_cycles']}"

    # Verify marker
    if test.get("marker") and test["marker"] not in result.stdout:
        return False, f"Marker '{test['marker']}' not found" + (f"\nStdout: {result.stdout}" if verbose else "")

    return True, f"PASS ({cycles} cycles)" if cycles else "PASS"

def run_test(qtrvsim_cli, test, elf_dir, verbose=False, pipeline=False, cache=False):
    """Run a single test"""
    elf_path = os.path.join(elf_dir, os.path.basename(test["elf_path"]))
    if not os.path.exists(elf_path): return False, f"ELF not found: {elf_path}"
    
    cmd = [qtrvsim_cli, elf_path, "--dump-registers", "--dump-cycles"]
    if pipeline: cmd.append("--pipelined")
    if cache: cmd.extend(["--d-cache", "lru,2,2,2", "--i-cache", "lru,2,2,2"])
    
    try:
        result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, timeout=30)
        return verify_result(test, result, verbose)
    except subprocess.TimeoutExpired:
        return False, "Test timed out (30s)"
    except Exception as e:
        return False, f"Exception: {e}"

def main():
    parser = argparse.ArgumentParser(description="Run stud-support integration tests")
    parser.add_argument("--build-dir", required=True, help="Directory containing test ELFs")
    parser.add_argument("--qtrvsim-cli", required=True, help="Path to qtrvsim_cli executable")
    parser.add_argument("--stud-support-path", required=True, help="stud-support repo path")
    parser.add_argument("-v", "--verbose", action="store_true", help="Verbose output")
    parser.add_argument("-f", "--filter", help="Filter tests")
    parser.add_argument("--pipeline", action="store_true", help="Pipelined mode")
    parser.add_argument("--cache", action="store_true", help="Cache enabled")
    args = parser.parse_args()
    
    elf_dir = args.build_dir
    
    def is_exe(fpath):
        return os.path.isfile(fpath) and (os.name == 'nt' or os.access(fpath, os.X_OK))

    qtrvsim_cli = args.qtrvsim_cli
    if not is_exe(qtrvsim_cli):
        # Try appending .exe for Windows if not present
        if os.name == 'nt' and not qtrvsim_cli.endswith('.exe') and is_exe(qtrvsim_cli + '.exe'):
            qtrvsim_cli += '.exe'
        else:
            print(f"Error: qtrvsim_cli not found or not executable at {qtrvsim_cli}")
            return 1
        
    tests = [t for t in TESTS if not args.filter or args.filter in t["name"]]
    if not tests:
        print(f"No tests match filter: {args.filter}")
        return 1
        
    mode = "pipelined+cached" if args.pipeline and args.cache else "pipelined" if args.pipeline else "cached" if args.cache else "single-cycle"
    print(f"Running {len(tests)} tests ({mode})\nUsing: {qtrvsim_cli}\nELFs: {elf_dir}\n" + "="*70)
    
    results = [run_test(qtrvsim_cli, t, elf_dir, args.verbose, args.pipeline, args.cache) for t in tests]
    passed = sum(1 for r in results if r[0])
    
    for i, (success, msg) in enumerate(results):
        status = "[OK]  " if success else "[FAIL]"
        print(f"{status} {tests[i]['name']:40s} {msg}")
        
    print("="*70 + f"\nResults: {passed}/{len(tests)} passed")
    if passed < len(tests):
        print("\nFailed tests:")
        for i, (success, _) in enumerate(results):
            if not success: print(f"  - {tests[i]['name']}")
            
    return 1 if passed < len(tests) else 0

if __name__ == "__main__":
    sys.exit(main())
