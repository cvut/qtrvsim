# RISC-V
- Update README based on instruction support.
- Finish and test macos bundle.
# From QtMips
- Full unprivileged instruction set support.
- On cache also allow calculate what time it would take with stalls.
- Unit tests for hazard unit
- Consider to notify MemoryView and ProgramView about changed range to limit update and redraw to required region only.
- Use background color to mark program and data in cache
- There seems to be some problem with layout recalculation when dock is pulled out of main window. When it's resized
  then it's immediately correctly recalculated.
- Implement and test more SPIM compatible syscalls.
