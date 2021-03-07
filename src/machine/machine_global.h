#ifndef MACHINE_GLOBAL_H
#define MACHINE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MACHINE_LIBRARY)
    #define MACHINE_SHARED_EXPORT Q_DECL_EXPORT
#else
    #define MACHINE_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MACHINE_GLOBAL_H
