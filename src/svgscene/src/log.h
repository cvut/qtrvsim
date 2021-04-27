#ifndef LOG_H
#define LOG_H

#include "necrolog/libnecrolog/necrolog.h"

#include <QStringRef>

inline NecroLog &operator<<(NecroLog log, const QString &s)
{
	return log << s.toStdString();
}

inline NecroLog &operator<<(NecroLog log, const QStringRef &s)
{
	return log << s.toString().toStdString();
}

#endif // LOG_H
