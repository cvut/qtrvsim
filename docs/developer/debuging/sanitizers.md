# Runtime sanitizers

## Using sanitizers in this project

All debug builds are by default built with address and undefined runtime
sanitizers. To disable them (which is strongly discouraged) run cmake with
options none.

```shell
cmake -DSANITIZERS=none
```

To run other selection of sanitiser use, pass colon separated list to the
command.

```shell
cmake -DSANITIZERS=memory,undefined
```

NOTE: Some sanitizer cannot be used together, like address and memory.

### Sanitizer debug info and Clang

If you are using sanitizers with clang and you don't files and line numbers,
make sure that `llvm-symbolizer` is installed and if that does not help, add
env `ASAN_SYMBOLIZER_PATH=` with path to your symbolizer executable. Most of the
time, having it in `PATH` should be enough. 

