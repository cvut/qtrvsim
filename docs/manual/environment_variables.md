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

## Logging

Logging environment variables are primarily intended for development and debugging purposes. For detailed information about available logging options, please refer to the developer documentation.