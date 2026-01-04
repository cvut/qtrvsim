# Environment Variables

QtRvSim supports several environment variables to customize its behavior. These are primarily useful for advanced users, portable deployments, and development purposes.

---

## GUI Configuration

### Portable Application Mode

By default, QtRvSim GUI stores application state (window positions, last selected configuration, etc.) using the system's standard configuration location. To make the simulator fully portable (e.g., running from a USB drive), you can specify a custom configuration file path.

**`QTRVSIM_CONFIG_FILE`**

Set this variable to the path of a `.ini` file where QtRvSim should store its GUI state.

**Example (Linux/macOS):**
```bash
export QTRVSIM_CONFIG_FILE=/path/to/portable/qtrvsim_config.ini
./qtrvsim_gui
```

**Example (Windows):**
```cmd
set QTRVSIM_CONFIG_FILE=D:\QtRvSim\config.ini
qtrvsim_gui.exe
```

---

### Scale GUi Fonts for Classroom Projector

The fonts can be to small for students to see them from the more distant places or projector resolution can be interpolated or scaled that text in editors and menus is hard to read. The next option allows to scale fonts for the whole application.

```bash
QT_SCALE_FACTOR=1.4 ./qtrvsim_gui
```

---

## Logging

Logging environment variables are primarily intended for development and debugging purposes. For detailed information about available logging options, please refer to the developer documentation.
