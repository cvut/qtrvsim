import os
import subprocess
import sys

# Constants
ISA_PATH = "isa"
ELF_PATH = "isa/elf/"
FILENAME = "py-test.py"


# Placeholders
CURR_DIR = ""

def max_str_list(list):
    max = 0
    for i in range(len(list)):
        if (max < len(list[i])):
            max = len(list[i])
    return max

# Decorative method, if output of the qtrvsim-cli is changed this will probably break!!!
def print_formated_output(reg_dump):
    stdout = reg_dump.stdout.decode("utf-8")
    stderr = reg_dump.stderr.decode("utf-8")
    print(stderr)
    print(stdout)


def load_filenames():
    global CURR_DIR
    CURR_DIR = os.path.realpath(__file__).replace(FILENAME, "")
    isa_dir = os.listdir(CURR_DIR + ELF_PATH)
    if (len(isa_dir) > 5):
        return isa_dir, True
    else:
        try:
            print("No tests found. Trying to build tests.")
            tests_built = subprocess.run(["cd {0} && make clean && make".format(CURR_DIR + ISA_PATH)], shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            return load_filenames()
        except subprocess.CalledProcessError as err:
            print("Failed to build tests.")
            print(err)
            return [], False


def parse_args(args):
    argv = len(args)
    if (argv == 2):  # .py + sim_bin
        sim_bin = os.path.realpath(args[1])
        try:
            sim_test = subprocess.run(
                [sim_bin], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        except subprocess.CalledProcessError as err:
            print(err)
            exit(1)
    if (sim_test.returncode == 0):
        return sim_bin, True
    return "", False


def check_reg_dump(reg_dump):
    res = False
    if (str(reg_dump.stdout).__contains__("R11:0x0600d000")):
        res = True
    if (str(reg_dump.stdout).__contains__("R11:0x0bad0000")):
        res = False
    return res, reg_dump


def order_tests(tests):
    tests32 = list(filter(lambda t: str(t).__contains__("32"), tests))
    tests64 = list(filter(lambda t: str(t).__contains__("64"), tests))
    tests32.sort()
    tests64.sort()
    return tests32, tests64


def run_tests(sim_bin, tests):
    succ = 0
    max_width = max_str_list(tests)
    for i in range(0, len(tests)):
        test_path = (CURR_DIR + ELF_PATH + tests[i])
        try:
            reg_dump = subprocess.run([
            sim_bin, test_path, "--d-regs"],
            capture_output=True)
        except subprocess.CalledProcessError as err:
            print(err)
            exit(1)
        test = tests[i].ljust(max_width+2)
        test_res, test_reg_dump = check_reg_dump(reg_dump)
        if (test_res):
            print(test + ": " +'\033[92m' + "PASS" + '\033[0m')
            succ += 1
        else:
            print(test + ": " +'\033[91m' + "FAIL" + '\033[0m')
            print_formated_output(test_reg_dump)
    print(str(succ) + "/" + str(len(tests)) + " tests succesfull.\n")


sim_bin, chk_res = parse_args(sys.argv)
if (chk_res == False):
    print("Error! Please use qtrvsim command line binary.")
    exit(1)
else:
    tests, lf_res = load_filenames()
    if (not lf_res):
        print("No test found!")
        exit(1)
    tests32, tests64 = order_tests(tests)
    print("--- 32 bit register tests ---")
    run_tests(sim_bin, tests32)
    print("--- 64 bit register tests ---")
    run_tests(sim_bin, tests64)

