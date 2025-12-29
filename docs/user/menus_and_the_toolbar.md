### Menu Bar

The menu bar provides access to all features of QtRvSim. It is divided into standard menus: **File**, **Machine**, **Windows**, **Options**, and **Help**.

---

#### File Menu
Handles operations related to simulation setup, source files, and the application itself.

- <img src="images/icons/document-import.png" style="vertical-align: middle; width: 20px; height: 20px"> **New simulation...** (Ctrl+N)
  Opens the *Pre-sets and ELF File* configuration dialog to start a new simulation.
- <img src="images/icons/reload.png" style="vertical-align: middle; width: 20px; height: 20px"> **Reload simulation** (Ctrl+Shift+R)
  Reassembles the source code currently in the editor and resets the simulation state (registers, memory, cycle count).
- **Print** (Ctrl+Alt+P)
  Prints the contents of the active panel or editor.
- <img src="images/icons/new.png" style="vertical-align: middle; width: 20px; height: 20px"> **New source** (Ctrl+F)
  Opens a new editor tab to write a fresh assembly program.
- <img src="images/icons/open.png" style="vertical-align: middle; width: 20px; height: 20px"> **Open source** (Ctrl+O)
  Loads an existing RISC-V assembly source file (`.s`) into the editor.
- <img src="images/icons/save.png" style="vertical-align: middle; width: 20px; height: 20px"> **Save source** (Ctrl+S)
  Saves the current editor contents. If previously unsaved, behaves like "Save source as."
- **Save source as**
  Prompts for a filename and saves the editor contents to that path.
- <img src="images/icons/closetab.png" style="vertical-align: middle; width: 20px; height: 20px"> **Close source** (Ctrl+W)
  Closes the current editor tab (prompts to save unsaved work).
- **Examples**
  Opens a submenu of example RISC-V programs bundled with QtRvSim. Selecting one loads it into the editor.
- <img src="images/icons/application-exit.png" style="vertical-align: middle; width: 20px; height: 20px"> **Exit** (Ctrl+Q)
  Exits the QtRvSim application.

---

#### Machine Menu
Controls simulation execution and machine-specific options.

- <img src="images/icons/play.png" style="vertical-align: middle; width: 20px; height: 20px"> **Run** (Ctrl+R)
  Starts continuous execution at the configured speed.
- <img src="images/icons/pause.png" style="vertical-align: middle; width: 20px; height: 20px"> **Pause** (Ctrl+P)
  Halts execution if running.
- <img src="images/icons/next.png" style="vertical-align: middle; width: 20px; height: 20px"> **Step** (Ctrl+T)
  Executes only one instruction.
- **Speed Controls**
  Set the execution rate when using **Run**:
  - 1 instruction/s (Ctrl+1)
  - 2 instructions/s (Ctrl+2)
  - 5 instructions/s (Ctrl+5)
  - 10 instructions/s (Ctrl+0)
  - 25 instructions/s (Ctrl+F5)
  - Unlimited (Ctrl+U) – fastest possible speed
  - Max (Ctrl+A) – fastest speed, with reduced GUI updates (higher performance)
- **Restart**
  Resets registers, memory, peripherals, and counters to state after the last assembly.
- **Mnemonics Registers** (checkbox)
  Toggle register view to show mnemonic names (`ra`, `sp`) instead of raw (`x1`, `x2`).
- **Show Symbol**
  ? to be added ?
- <img src="images/icons/compfile-256.png" style="vertical-align: middle; width: 20px; height: 20px"> **Compile Source** (Ctrl+E)
  Assembles source code in the editor using the built-in assembler.
- <img src="images/icons/build-256.png" style="vertical-align: middle; width: 20px; height: 20px"> **Build Executable** (Ctrl+B)
  Builds a standalone ELF file using an external RISC-V toolchain (e.g., GCC/Clang).

#### Windows Menu
Manages visibility of all simulation panels.

- **Registers** (Ctrl+D) – Register state viewer.
![alt text](images/gui_elements/registers.webp)
- **Program** (Ctrl+P) – Code view with current instruction highlighting.
![alt text](images/gui_elements/program.webp)
- **Memory** (Ctrl+M) – Memory contents.
![alt text](images/gui_elements/memory.webp)
- **Program Cache** (Ctrl+Shift+P) – Instruction cache visualization (if enabled).
![alt text](images/gui_elements/program_cache.webp)
- **Data Cache** (Ctrl+Shift+M) – Data cache visualization (if enabled).
![alt text](images/gui_elements/data_cache.webp)
- **L2 Cache** – Second-level cache view (if enabled).
![alt text](images/gui_elements/l2_cache.webp)
- **Branch Predictor** – Displays prediction tables and data (if configured).
![alt text](images/gui_elements/branch_predictor.webp)
- **Peripherals** – Container for simulated I/O devices.
![alt text](images/gui_elements/peripherals.webp)
- **Terminal** – Simulated serial terminal.
![alt text](images/gui_elements/terminal.webp)
- **LCD Display** – Simulated LCD.
![alt text](images/gui_elements/lcd_display.webp)
- **Control and Status Registers (CSR)** (Ctrl+I) – CSR panel.
![alt text](images/gui_elements/control_and_status_registers.webp)
- **Core View** – Datapath visualization (CPU core).
![alt text](images/gui_elements/core.webp)
- **Messages** – Log and feedback panel.
![alt text](images/gui_elements/messages.webp)
- **Reset Windows** – Restores all panels to their default positions.

---

#### Options Menu
- **Show Line Numbers** (checkbox) – Toggle line numbers in the code editor.

---

#### Help Menu
Provides information about QtRvSim, version details, and licensing.

---

### Toolbar
![Toolbar screenshot](images/gui_elements/toolbar.webp)

The toolbar provides quick access to frequently used actions:

- <img src="images/icons/document-import.png" style="vertical-align: middle; width: 20px; height: 20px"> **New Simulation** – Equivalent to *File → New simulation...* (Ctrl+N).
- <img src="images/icons/reload.png" style="vertical-align: middle; width: 20px; height: 20px"> **Reload** – Resets simulator state (equivalent to *Machine → Restart*).
- <img src="images/icons/play.png" style="vertical-align: middle; width: 20px; height: 20px"> **Run** – Start program execution (Ctrl+R).
- <img src="images/icons/pause.png" style="vertical-align: middle; width: 20px; height: 20px"> **Pause** – Pause execution (Ctrl+P).
- <img src="images/icons/next.png" style="vertical-align: middle; width: 20px; height: 20px"> **Step** – Execute one instruction (Ctrl+T).
- **Speed Controls** – Select execution speed (same as *Machine → Speed Controls*).
- <img src="images/icons/new.png" style="vertical-align: middle; width: 20px; height: 20px"> **New Source** – Open a new editor tab (Ctrl+F).
- <img src="images/icons/open.png" style="vertical-align: middle; width: 20px; height: 20px"> **Open Source** – Load an existing `.s` or `.elf` file (Ctrl+O).
- <img src="images/icons/save.png" style="vertical-align: middle; width: 20px; height: 20px"> **Save Source** – Save current file (Ctrl+S).
- <img src="images/icons/closetab.png" style="vertical-align: middle; width: 20px; height: 20px"> **Close Source** – Close active code editor tab (Ctrl+W).
- <img src="images/icons/compfile-256.png" style="vertical-align: middle; width: 20px; height: 20px"> **Compile Source** – Assemble current RISC-V program with the built-in assembler (Ctrl+E).
- <img src="images/icons/build-256.png" style="vertical-align: middle; width: 20px; height: 20px"> **Build Executable** – Build ELF with external RISC-V toolchain (Ctrl+B).
