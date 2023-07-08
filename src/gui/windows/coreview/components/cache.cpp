#include "cache.h"

Cache::Cache(
    const machine::Cache *cache,
    svgscene::SimpleTextItem *hit_text,
    svgscene::SimpleTextItem *mis_text)
    : QObject()
    , hit_text(hit_text)
    , mis_text(mis_text) {
    connect(cache, &machine::Cache::hit_update, this, &Cache::hit_update);
    connect(cache, &machine::Cache::miss_update, this, &Cache::miss_update);
}
void Cache::hit_update(unsigned value) {
    hit_text->setText(QString::number(value));
}
void Cache::miss_update(unsigned int value) {
    mis_text->setText(QString::number(value));
}
