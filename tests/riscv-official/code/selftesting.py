import testing as ts
import constants as cn

def self_test(sim_bin, params, src_path, tests):
    sh_pass = []
    sh_fail = []
    error = []
    for test in tests:
        test_res, test_reg_dump = ts.check_reg_dump(
            ts.run_test(sim_bin, params, src_path + cn.SELF_ELF_PATH+"/", test))
        if (test_res == 1 and str(test).__contains__("fail")):   
            sh_fail.append(test)
        if (test_res == 0 and str(test).__contains__("pass")):   
            sh_pass.append(test)
        if(test_res == 2):                                        
            error.append(test)
    print("Test passed but was expected to fail:{0}".format(sh_fail))
    print("Test failed but was expected to pass:{0}".format(sh_pass))
    print("Test ended in error                 :{0}\n".format(error))