#include "fontsize.h"

#include <QFont>
#include <QDebug>
#include <QApplication>
#include <QFontMetrics>

int FontSize::SIZE5 = 5;
int FontSize::SIZE6 = 6;
int FontSize::SIZE7 = 7;
int FontSize::SIZE8 = 8;

void FontSize::init()
{
	int h = QFontMetrics(QApplication::font()).height();
	qDebug() << "Font size:" << h;
	h /= 5;
	int d = h / 10 + 1;
	FontSize::SIZE5 = h - 2*d;
	FontSize::SIZE6 = h - d;
	FontSize::SIZE7 = h;
	FontSize::SIZE8 = h + d;
}
