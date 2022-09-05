def max_str_list(list):
    if (len(list) > 0):
        return len(max(list, key=lambda x: len(x)))
    return 0


# Decorative method, if output of the qtrvsim-cli is changed this will probably break!!!
def print_formated_output(reg_dump):
    stdout = reg_dump.stdout.decode("utf-8")
    stderr = reg_dump.stderr.decode("utf-8")
    print(stderr)
    print(stdout)


def res_translate(res):
    if (res == 0):
        return "FAIL"
    elif (res == 1):
        return "PASS"
    else:
        return "ERROR"


def res_print(test, test_res, test_reg_dump, params):
    if (test_res == 1):
        if (not params.nopass):
            if (not params.fileprt):
                print(test + ": " + '\033[92m' + "PASS" + '\033[0m')
            else:
                print(test+":PASS")
            if (params.dregs):
                print_formated_output(test_reg_dump)
        return 1
    elif (test_res == 0):
        if (not params.fileprt):
            print(test + ": " + '\033[93m' + "FAIL" + '\033[0m')
        else:
            print(test+":FAIL")
        if (not params.nodump or params.dregs):
            print_formated_output(test_reg_dump)
        return 0
    else:
        if (not params.fileprt):
            print(test + ": " + '\033[91m' + "ERROR" + '\033[0m')
        else:
            print(test+":ERROR")
        if (not params.nodump or params.dregs):
            print_formated_output(test_reg_dump)
        return 0


def get_RVxx(tests, isa):
    isa = str(isa).lower()
    tests32 = list(filter(lambda t: str(t).__contains__("32"+isa), tests))
    tests64 = list(filter(lambda t: str(t).__contains__("64"+isa), tests))
    tests32.sort()
    tests64.sort()
    return tests32, tests64