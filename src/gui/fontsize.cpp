#include "fontsize.h"

#include <QApplication>
#include <QFont>
#include <QFontMetrics>
#include "common/logging.h"

LOG_CATEGORY("gui.font");

int FontSize::SIZE5 = 5;
int FontSize::SIZE6 = 6;
int FontSize::SIZE7 = 7;
int FontSize::SIZE8 = 8;

void FontSize::init() {
    int h = QFontMetrics(QApplication::font()).height();
    LOG() << "Font size:" << h;
    h /= 3;
    int d = h / 4 + 1;
    FontSize::SIZE5 = h - 2 * d;
    FontSize::SIZE6 = h - d;
    FontSize::SIZE7 = h;
    FontSize::SIZE8 = h + d / 2;
}
