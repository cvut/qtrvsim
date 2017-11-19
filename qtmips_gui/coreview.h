#ifndef COREVIEW_H
#define COREVIEW_H

#include <QGraphicsView>
#include <QGraphicsItem>
#include <QList>
#include "machineconfig.h"

class CoreView : public QGraphicsView {
    Q_OBJECT
public:
    CoreView(QWidget *parent);

private:

};

/*
class CoreViewBlock : public QGraphicsItem {
    Q_OBJECT
public:
    CoreViewBlock();
};

class CoreViewLine : public QGraphicsItem {
    Q_OBJECT
public:
    struct point {
        int x1, y1, x2, y2;
    };

    CoreViewLine();
    CoreViewLine(struct point start, struct point end, QList<struct point> axis);
    ~CoreViewLine();

    void set_start(struct point);
    void set_end(struct point);
    void set_axis(QList<struct point>);

protected:
    struct point start, end;
    QList<struct point> axis;
};
*/

#endif // COREVIEW_H
