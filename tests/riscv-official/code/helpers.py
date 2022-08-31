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


def res_translate(res):
    if (res == 0):
        return "FAIL"
    elif (res == 1):
        return "PASS"
    else:
        return "ERROR"


def res_print(test, test_res, params):
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
