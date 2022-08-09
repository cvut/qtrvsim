import os
import subprocess
import sys

# Constants
isa_path = "isa/elf/"
filename = "py-test.py"


# Placeholders
bin_path = "../qtrvsim-bin/qtrvsim_cli"
curr_dir = ""

def max_str_list(list):
    max = 0
    for i in range(len(list)):
        if (max < len(list[i])):
            max = len(list[i])
    return max


def load_filenames():
    global curr_dir
    curr_dir = os.path.realpath(__file__).replace(filename, "")
    isa_dir = os.listdir(curr_dir + isa_path)
    if (len(isa_dir) != 0):
        return isa_dir, True
    else:
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
    nice = False
    if (str(reg_dump.stdout).__contains__("R11:0x00000069")):
        nice = True
    if (str(reg_dump.stdout).__contains__("R11:0x00000096")):
        nice = False
    return nice


def run_tests(sim_bin, tests):
    succ = 0
    max_width = max_str_list(tests)
    for i in range(0, len(tests)):
        test_path = (curr_dir + isa_path + tests[i])
        try:
            reg_dump = subprocess.run([
            sim_bin, test_path, "--d-regs"],
            capture_output=True)
        except subprocess.CalledProcessError as err:
            print(err)
            exit(1)
        test = tests[i].ljust(max_width+2)
        if (check_reg_dump(reg_dump)):
            print(test + ": PASS")
            succ += 1
        else:
            print(test + ": FAIL")
    print(str(succ) + "/" + str(len(tests)) + " tests succesfull.")


sim_bin, chk_res = parse_args(sys.argv)
if (chk_res == False):
    print("Wrong argument!")
    exit(1)
else:
    tests, lf_res = load_filenames()
    if (not lf_res):
        print("No test found!")
        exit(1)
    run_tests(sim_bin, tests)
