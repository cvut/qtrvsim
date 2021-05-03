#ifndef QTRVSIM_CACHE_H
#define QTRVSIM_CACHE_H

#include <QObject>
#include <memory/cache/cache.h>
#include <svgscene/components/simpletextitem.h>
#include <svgscene/utils/memory_ownership.h>

class Cache : public QObject {
public:
    Cache(
        const machine::Cache *cache,
        svgscene::SimpleTextItem *hit_text,
        svgscene::SimpleTextItem *mis_text);

protected slots:
    void hit_update(unsigned value);
    void miss_update(unsigned value);

protected:
    BORROWED svgscene::SimpleTextItem *hit_text;
    BORROWED svgscene::SimpleTextItem *mis_text;
};

#endif // QTRVSIM_CACHE_H
