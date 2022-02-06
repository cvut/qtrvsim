# Logging

The project uses the Qt logging framework. For quick introduction
see [this KDAB presentation](https://www.kdab.com/wp-content/uploads/stories/slides/Day2/KaiKoehne_Qt%20Logging%20Framework%2016_9_0.pdf)
.

To enable logging in a `cpp` file, add the following snippet.

```c++
#include "common/logging.h"

LOG_CATEGORY("<project>.<file/class>");
```

This creates a category object in the scope and enables seamless use of logging macros `DEBUG` `LOG` `WARN` and `INFO`.
If you need to use logging in a header file, call the `LOG_CATEGORY` macro only in a local scope to avoid interfering
with log categories in source files.

Debug logs are intended to provide fine level of information and should be hidden by default and enabled only for
currently debugged category. Debug logs are removed from all `release` builds.

## Using the log

Qt supports both C++ streams and `printf`-like interface. **In this project, only `printf` style logs should be used.**
Logs are considered to be part of the documentation and stream are too hard to read. The only exception is compatibility
with used libraries (curretly *svgscene*).

To print any Qt type, just wrap it with `qPrintable` macro.

**Example:**

```
QT_LOGGING_CONF=/data/Dev/school/QtRvSim/qtlogging.ini;
```

```cpp
DEBUG("Link triggered (href: %s).", qPrintable(getTargetName()));
```

## Configuring the log

To filter shown logs, modify `qtlogging.ini` file in the root of the project. If this config is not found automatically,
use `QT_LOGGING_CONF`environment variable.

**Example:**

```shell
QT_LOGGING_CONF=/data/Dev/school/QtRvSim/qtlogging.ini
```

**Configuration example** (shows debug logs from `instruction.cpp` only):

```ini
[Rules]
*.debug=false
machine.Instruction.debug=true
```