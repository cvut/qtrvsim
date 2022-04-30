# Need To Know

**Critical information for every developer.**

## Modal Windows, Dialogues, Message Boxes

In typical, native, Qt setting, modal windows allocated on stack and block upon opening. To manage the modal window, Qt
spins up a second event loop. However, that is not technically possible in Web-Assembly environment. *(This might be
solved in new Qt with “asyncify” build flag, but it does not work with currently used Qt 5.15.2)*

[Read more](http://qtandeverything.blogspot.com/2019/05/exec-on-qt-webassembly.html)

### Broken API usage

```cpp
QMessageBox msg;
msg.exec();
```

```cpp
QMessageBox::critical("Hello");
```

The `.exec()` method and single line call are typical warning signs.

Some parts of code employ special code for Web-Assembly (like HTML file selector) and the native part may use blocking
solution. However, in performance non-critical parts (which most dialogs are), unified asynchronous solution is
preferred.

### Solution

The modal window object has to be allocated dynamically and started using asynchronous method `open`. Answer has to be
obtained via `connect` callback.

For ease of use, wrapper functions are prepared in `gui/helper/async_modal.h`.

Free can be accomplished using special call `msg->setAttribute(Qt::WA_DeleteOnClose)` before opening. This method is
automatically employed in `async_modal.h`.

Docs: [WA_DeleteOnClose](https://doc.qt.io/qt-5/qt.html#WidgetAttribute-enum)
, [QCloseEvent](https://doc.qt.io/qt-5/qcloseevent.html)