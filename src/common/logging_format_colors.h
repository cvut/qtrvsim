#ifndef LOGGING_FORMAT_COLORS_H
#define LOGGING_FORMAT_COLORS_H

#include <QtGlobal>

static void set_default_log_pattern() {
    qSetMessagePattern("%{if-info}\033[34m[INFO]  %{endif}"
                       "%{if-debug}\033[37m[DEBUG] %{endif}"
                       "%{if-warning}\033[33m[WARN]  %{endif}"
                       "%{if-critical}\033[31m[ERROR] %{endif}"
                       "%{if-fatal}\033[31m[FATAL ERROR] %{endif}"
                       "%{if-category}%{category}:%{endif}"
                       "\033[0m\t%{message}");
}

#endif // LOGGING_FORMAT_COLORS_H
