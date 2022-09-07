import sys
import os

FILENAME = "qtrvsim_tester.py"
SRC_DIR = os.path.realpath(__file__).replace(FILENAME, "")

sys.path.append(SRC_DIR+"code")
import constants as cn
import myparse as mp
import testing as ts
import selftesting as sts

parser = mp.init_parser()
params = parser.parse_args()
if (params.dregs):
    params.nodump = 0
    params.nopass = 0

if(params.clean):
    ts.delete_elf(SRC_DIR)
    sys.exit(0)
        
sim_bin, bin_check = ts.test_sim_bin(params.qtrvsim_cli)

if(not bin_check):
    print("Problem with qtrvsim_cli binary!")
    sys.exit(1)

self_files, s_file_check = ts.load_filenames(
    SRC_DIR + cn.SELF_PATH, bool(params.rebuild))
test_files, t_file_check = ts.load_filenames(
    SRC_DIR + cn.ISA_PATH, bool(params.rebuild))

if (params.selftest):
    sts.self_test(sim_bin, params, SRC_DIR, self_files)

ts.test_selector(sim_bin, params, SRC_DIR, test_files)

sys.exit(0)
