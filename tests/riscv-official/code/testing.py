import os
import sys
import pathlib
import subprocess

import helpers as hp
import constants as cn


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


def load_filenames(dir_path, rebuild):
    test_files = list(filter(
        lambda x: not str(x).startswith("."), os.listdir(dir_path+"/elf")))
    if (len(test_files) != 0 and not rebuild):
        return test_files, True
    else:
        try:
            if (rebuild):
                print("Building {0}".format(dir_path))
            else:
                print("No tests found. Trying to build tests.")
            tests_built = subprocess.run(
                ["make", "-C", dir_path],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL)
            return load_filenames(dir_path, False)
        except:
            print("Failed to build tests. {0} ; {1}".format(
                dir_path, test_files))
            sys.exit(1)


# If output of the qtrvsim-cli is changed this will probably break !!!
def check_reg_dump(reg_dump):
    res = 2
    stdout = str(reg_dump.stdout)
    if (stdout.__contains__("R11:")):
        index = stdout.find("R11:")
        if (stdout[index:index+18].__contains__("600d")):
            res = 1
        elif (stdout[index:index+18].__contains__("bad")):
            res = 0
    return res, reg_dump


def get_RVxx(tests, set):
    set = str(set).lower()
    tests32 = list(filter(lambda t: str(t).__contains__("32"+set), tests))
    tests64 = list(filter(lambda t: str(t).__contains__("64"+set), tests))
    tests32.sort()
    tests64.sort()
    return tests32, tests64


def run_test(sim_bin, params, dir_path, filename):
    test_path = dir_path + filename
    try:
        param_bin = [sim_bin, test_path, "--d-regs"]
        if (params.pipeline):
            param_bin.append("--pipelined")
        if (params.cache):
            param_bin.extend(cn.CACHE_SETTINGS)
        return subprocess.run(param_bin, capture_output=True)
    except subprocess.CalledProcessError as err:
        print(err)
        exit(1)


def run_tests(sim_bin, params, dir_path, tests):
    succ = 0
    max_width = 0
    if (not params.fileprt):
        max_width = hp.max_str_list(tests)
    for i in range(0, len(tests)):
        reg_dump = run_test(sim_bin, params, dir_path, tests[i])
        test = tests[i].ljust(max_width)
        test_res, test_reg_dump = check_reg_dump(reg_dump)
        succ += hp.res_print(test, test_res, params)
    if (not params.fileprt):
        print(str(succ) + "/" + str(len(tests)) + " tests succesfull.\n")


def run_official_tests(sim_bin, params, src_path, tests):
    if (not params.no32):
        if (not params.fileprt):
            print("--- 32 bit register tests ---")
        run_tests(sim_bin, params, src_path + cn.ELF_PATH, tests[0])
    if (not params.no64):
        if (not params.fileprt):
            print("--- 64 bit register tests ---")
        run_tests(sim_bin, params, src_path + cn.ELF_PATH, tests[1])


def self_test(sim_bin, params, src_path, tests):
    wrong = []
    for i in range(0, len(tests)):
        test_res, test_reg_dump = check_reg_dump(
            run_test(sim_bin, params, src_path + cn.SELF_ELF_PATH+"/", tests[i]))
        if ((test_res == 1 and str(tests[i]).__contains__("fail")) or
            (test_res == 0 and str(tests[i]).__contains__("pass")) or
                test_res == 2):
            wrong.append(tests[i])
            # if (not params.nodump or params.dregs):
            #    hp.print_formated_output(test_reg_dump)
    print("Undesired behaviour:{0}".format(wrong))


def test_selector(sim_bin, params, src_path, tests):
    if (params.pipeline):
        print("Simulator runs in pipelined mode.")
    if (params.cache):
        print("Simulator runs in cache mode.")
    if (params.fileprt):
        line = ""
    else:
        line = "-+-+-+-+-+-+-+-+-"
    print(line+"RVxxUI"+line)
    run_official_tests(sim_bin, params, src_path, get_RVxx(tests, "ui"))

    if (params.multiply):
        print(line+"RVxxUM"+line)
        run_official_tests(sim_bin, params, src_path, get_RVxx(tests, "um"))

    if (params.atomic):
        print(line+"RVxxUA"+line)
        run_official_tests(sim_bin, params, src_path, get_RVxx(tests, "ua"))

    if (params.CSR):
        print(line+"RVxxSI"+line)
        run_official_tests(sim_bin, params, src_path, get_RVxx(tests, "si"))
        print(line+"RVxxMI"+line)
        run_official_tests(sim_bin, params, src_path, get_RVxx(tests, "mi"))

    if (len(params.external) > 0):
        print(line+"External Tests"+line)
        dir_path = params.external[0]
        test_files = os.listdir(dir_path)
        test_files = list(
            filter(lambda x: not str(x).startswith("."), test_files))
        run_tests(dir_path, test_files, params)


def delete_elf(src_path):
    try:
        subprocess.run( ["make", "-C", src_path + cn.ISA_PATH, "clean"],
                        stdout = subprocess.DEVNULL,
                        stderr = subprocess.DEVNULL)
        subprocess.run(["make", "-C", src_path + cn.SELF_PATH, "clean"],
                        stdout = subprocess.DEVNULL,
                        stderr = subprocess.DEVNULL)
    except subprocess.CalledProcessError as err:
        print("Unable to delete elf files.")
        print(err)
    sys.exit(0)