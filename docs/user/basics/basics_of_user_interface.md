# User Interface Overview

After selecting a configuration preset (or loading an example) in the initial dialog, the main **QtRvSim window** appears.
This chapter introduces the primary functional areas of the interface necessary for basic operation.
![QtRvSim main window (empty workspace)](media/gui_elements/user_interface.webp)
The interface can be divided into five main areas:
![The QtRvSim interface, divided into main sections](media/gui_elements/user_interface_annotated.webp)

## 1. Menu Bar & Toolbar (Blue Area)

- Located at the **top of the window**.
- The **Menu Bar** provides full access to all commands (`File`, `Machine`, `Windows`, etc.).
- The **Toolbar** below offers quick access to the most commonly used commands via icons.

**Key Toolbar Buttons (basic simulation):**
- <img src="../media/icons/compfile-256.png" style="vertical-align: middle; width: 20px; height: 20px"> **Compile Source** – Assemble your code.
- <img src="../media/icons/play.png" style="vertical-align: middle; width: 20px; height: 20px"> **Run** – Execute the program continuously.
- <img src="../media/icons/next.png" style="vertical-align: middle; width: 20px; height: 20px"> **Step** – Advance execution one instruction (or one cycle) at a time.
- <img src="../media/icons/reload.png" style="vertical-align: middle; width: 20px; height: 20px"> **Reset** – Reset the simulation state to the beginning.

## 2. Program Panel (Green Area)

- Toggles via **`Windows -> Program`** or **Ctrl+P**.
- Displays the **sequence of assembly instructions** for the currently loaded program.
- During execution, the **currently processed instruction** is highlighted (blue).
- A dropdown menu allows you to follow a particular **pipeline stage** (fetch, decode, execute, etc.):
  - Default: **Follow Fetch** – ideal to get started.

This panel makes it easy to trace the flow of instructions and observe where execution currently is.

## 3. Datapath Visualization / Core View (Red Area)

- The central and most distinctive panel of QtRvSim.
- Visually represents the processor’s **datapath** based on the selected configuration (single-cycle/pipelined).
- Shows how instructions flow through components such as the **registers, ALU and memory**.

This view makes abstract computer architecture concepts concrete by showing the processor "at work."

## 4. Simulation Counters (Purple Area)

- Located in the **top-right corner** of the window.
- Displays important **statistics** about the simulation:
  - **Cycles** – Total number of clock cycles since the last reset.
  - **Stalls** – Number of pipeline stalls encountered (in pipelined configurations).

Values update continuously during execution (`Run`) or after each `Step`.
Counters reset when you **Reset** or **Reload** the simulation.

## 5. Additional Information Windows

QtRvSim offers several **dockable panels** to inspect system state in detail.
These can be opened from the **`Windows`** menu:

![QtRvSim with the windows menu open](media/gui_elements/windows_menu.webp)

You can rearrange, dock, tab, or float these windows to customize your workspace. Use **`Windows -> Reset Windows`** to restore the default layout.

**Essential Panels for Beginners:**
- **Registers** (`Windows -> Registers` or **Ctrl+D**) – Displays general-purpose registers **x0–x31** and the Program Counter.
- **Memory** (`Windows -> Memory` or **Ctrl+M**) – Displays the contents of simulated memory.
- **Terminal** (`Windows -> Terminal`) – For text I/O when programs interact with the console.

## Next Steps

In the following chapter, we will look more closely at the available menus and toolbar options.
*(If you are just starting out, you may skip ahead to chapter _____ and return later as needed.)*
