import argparse


def init_parser():
    parser = argparse.ArgumentParser(
        description="Defeault tests are RV32UI and RV64UI. No pipeline and cache.")
    parser.add_argument('qtrvsim_cli',
                        default='',
                        help="Qtrvsim_cli to run tests. (RVxxUI is default)")

    # Test selection
    bsel = parser.add_argument_group(
        "bit lenght selection for official tests:")
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
    eout.add_argument("--file",
                      action="count",
                      default=0,
                      dest="fileprt",
                      help="Removes most graphical output features.")

    # Test aplication good to have features
    gth = parser.add_argument_group("good to have:")
    gth.add_argument("-R", "--rebuild",
                     action="count",
                     default=0,
                     help="Deletes tests and buidls them anew.")
    gth.add_argument("-S", "--selftest",
                     action="count",
                     default=0,
                     dest="selftest",
                     help="Enable simple tests to check if tester behaves properly.")
    gth.add_argument("--clean",
                     action="count",
                     default=0,
                     dest="clean",
                     help="Deletes all official and seftest elf files.")
    return parser
