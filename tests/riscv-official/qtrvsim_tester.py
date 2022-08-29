import os
import sys
import pathlib
import argparse
import subprocess

# Constants
ISA_PATH = "isa"
ELF_PATH = "isa/elf/"
FILENAME = "qtrvsim_tester.py"
LINE_DESIGN = "+-+-+-+-+-+-+-+-+-+-+-+-+-+"


# Placeholders
CURR_DIR = ""


# Decorative method, if output of the qtrvsim-cli is changed this will probably break!!!
def print_formated_output(reg_dump):
    stdout = reg_dump.stdout.decode("utf-8")
    stderr = reg_dump.stderr.decode("utf-8")
    print(stderr)
    print(stdout)


def max_str_list(list):
    max = 0
    for i in range(len(list)):
        if (max < len(list[i])):
            max = len(list[i])
    return max


def test_sim_bin(sim_bin):
    try:
        sim_test = subprocess.run(
            [sim_bin], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    except:
        parser.print_help()
        sys.exit(1)
    if (sim_test.returncode == 0):
        return sim_bin, True
    return "", False


# If output of the qtrvsim-cli is changed this will probably break !!!
def check_reg_dump(reg_dump):
    res = False
    if (str(reg_dump.stdout).__contains__("R11:0x0600d000")):
        res = True
    if (str(reg_dump.stdout).__contains__("R11:0x0bad0000")):
        res = False
    return res, reg_dump


def get_RVUI(tests):
    tests32ui = list(filter(lambda t: str(t).__contains__("32ui"), tests))
    tests64ui = list(filter(lambda t: str(t).__contains__("64ui"), tests))
    tests32ui.sort()
    tests64ui.sort()
    return tests32ui, tests64ui


def get_RVUM(tests):
    tests32um = list(filter(lambda t: str(t).__contains__("32um"), tests))
    tests64um = list(filter(lambda t: str(t).__contains__("64um"), tests))
    tests32um.sort()
    tests64um.sort()
    return tests32um, tests64um


def get_RVUA(tests):
    tests32ua = list(filter(lambda t: str(t).__contains__("32ua"), tests))
    tests64ua = list(filter(lambda t: str(t).__contains__("64ua"), tests))
    tests32ua.sort()
    tests64ua.sort()
    return tests32ua, tests64ua


def get_RVSI(tests):
    tests32si = list(filter(lambda t: str(t).__contains__("32si"), tests))
    tests64si = list(filter(lambda t: str(t).__contains__("64si"), tests))
    tests32si.sort()
    tests64si.sort()
    return tests32si, tests64si


def get_RVMI(tests):
    tests32mi = list(filter(lambda t: str(t).__contains__("32mi"), tests))
    tests64mi = list(filter(lambda t: str(t).__contains__("64mi"), tests))
    tests32mi.sort()
    tests64mi.sort()
    return tests32mi, tests64mi


def load_filenames(rebuild):
    global CURR_DIR
    CURR_DIR = os.path.realpath(__file__).replace(FILENAME, "")
    isa_dir = os.listdir(CURR_DIR + ELF_PATH)
    if (len(isa_dir) > 5 and not rebuild):
        return isa_dir, True
    else:
        try:
            if (rebuild):
                print("Rebuilding tests.")
            else:
                print("No tests found. Trying to build tests.")
            tests_built = subprocess.run(["cd {0} && make clean && make".format(
                CURR_DIR + ISA_PATH)], shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            return load_filenames(False)
        except:
            print("Failed to build tests.")
            parser.print_help()
            sys.exit(1)


def run_tests(dir_path, tests, params):
    succ = 0
    max_width = max_str_list(tests)
    for i in range(0, len(tests)):
        test_path = (dir_path + tests[i])
        try:
            param_bin = [sim_bin, test_path, "--d-regs"]
            if (params.pipeline):
                param_bin.append("--pipelined")
            if (params.cache):
                param_bin.append("--d-cache")
                param_bin.append("lru,2,2,2,wb")
                param_bin.append("--i-cache")
                param_bin.append("lru,2,2,2")
            reg_dump = subprocess.run(param_bin, capture_output=True)
        except subprocess.CalledProcessError as err:
            print(err)
            exit(1)
        test = tests[i].ljust(max_width+2)
        test_res, test_reg_dump = check_reg_dump(reg_dump)
        if (test_res):
            if (not params.nopass):
                print(test + ": " + '\033[92m' + "PASS" + '\033[0m')
                if (params.dregs):
                    print_formated_output(test_reg_dump)
            succ += 1
        else:
            print(test + ": " + '\033[91m' + "FAIL" + '\033[0m')
            if (not params.nodump or params.dregs):
                print_formated_output(test_reg_dump)
    print(str(succ) + "/" + str(len(tests)) + " tests succesfull.\n")


def run_official_tests(sim_bin, tests, params):
    if(not params.no32):
        print("--- 32 bit register tests ---")
        run_tests(CURR_DIR + ELF_PATH, tests[0], params)
    if(not params.no64):
        print("--- 64 bit register tests ---")
        run_tests(CURR_DIR + ELF_PATH, tests[1], params)
        

def test_selector(sim_bin, params, tests):
    if (params.pipeline):
        print("Simulator runs in pipelined mode.")
    if (params.cache):
        print("Simulator runs in cache mode.")
    print(LINE_DESIGN+" RVxxUI "+LINE_DESIGN)
    run_official_tests(sim_bin, get_RVUI(tests), params)
    if (params.multiply):
        print(LINE_DESIGN+" RVxxUM "+LINE_DESIGN)
        run_official_tests(sim_bin, get_RVUM(tests), params)
    if (params.atomic):
        print(LINE_DESIGN+" RVxxUA "+LINE_DESIGN)
        run_official_tests(sim_bin, get_RVUA(tests), params)
    if (params.CSR):
        print(LINE_DESIGN+" RVxxSI "+LINE_DESIGN)
        run_official_tests(sim_bin, get_RVSI(tests), params)
        print(LINE_DESIGN+" RVxxMI "+LINE_DESIGN)
        run_official_tests(sim_bin, get_RVMI(tests), params)
    if (len(params.external) > 0):
        print(LINE_DESIGN+" External Tests "+LINE_DESIGN)
        dir_path = params.external[0]
        test_files = os.listdir(dir_path)
        test_files = list(
            filter(lambda x: not str(x).startswith("."), test_files))
        run_tests(dir_path, test_files, params)


parser = argparse.ArgumentParser(description="Defeault tests are RV32UI and RV64UI. No pipeline and cache.")
parser.add_argument('qtrvsim_cli',
                    default='',
                    help="Qtrvsim_cli to run tests. (RVxxUI is default)")

# Test selection
bsel = parser.add_argument_group("bit lenght selection for official tests:")
bsel.add_argument("--no-32",
                    action="count",
                    default=0,
                    dest="no32",
                    
                    help="Disables 32-bit tests. (Only for official tests)")
bsel.add_argument("--no-64",
                    action="count",
                    default=0,
                    dest="no64",
                    help="Disables 64-bit tests. (Only for official tests)")

# Additional tests
tsel = parser.add_argument_group("additional set of tests:")
tsel.add_argument("-E", "--external",
                    nargs=1,
                    action="store",
                    default="",
                    help="Path to additional tests. Required to adhere to PASS,FAIL behavior of edited test enviroment.")
tsel.add_argument("-M", "--multiply",
                    action="count",
                    default=0,
                    help="Additional set of tests for multiplication.")
tsel.add_argument("-A", "--atomic",
                    action="count",
                    default=0,
                    help="Additional set of tests for atomic instructions.")
tsel.add_argument("--CSR",
                    action="count",
                    default=0,
                    help="Additional set of tests for control and status registers.")

# QtRVSim_cli settings
opts = parser.add_argument_group("QtRVSim options:")
opts.add_argument("--pipeline",
                    action="count",
                    default=0,
                    help="Simulator runs in pipelined configuration.")
opts.add_argument("--cache",
                    action="count",
                    default=0,
                    help="Simulator runs with d-cache and i-cache implemented. (d lru,2,2,2,wb ; i lru,2,2,2)")

# Test aplication output options
eout = parser.add_argument_group("output print options:")
eout.add_argument("--no-pass",
                    action="count",
                    default=0,
                    dest="nopass",
                    help="Prints only failed tests for readability.")
eout.add_argument("--no-dump",
                    action="count",
                    default=0,
                    dest="nodump",
                    help="Omits printing of registers in failed test.")
eout.add_argument("--d-regs",
                    action="count",
                    default=0,
                    dest="dregs",
                    help="Prints output of qtrvsim --d-regs param. (Higher priority than other output options.)")

# Test aplication good to have features
gth = parser.add_argument_group("good to have:")
gth.add_argument("-R", "--rebuild",
                    action="count",
                    default=0,
                    help="Deletes tests and buidls them anew.")
args = parser.parse_args()
if(args.dregs):
    args.nodump = 0
    args.nopass = 0

sim_bin, bin_check = test_sim_bin(args.qtrvsim_cli)
test_files, file_check = load_filenames(bool(args.rebuild))
test_selector(sim_bin, args, test_files)
