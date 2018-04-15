#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>

class GraphicsView : public QGraphicsView {
public:
    GraphicsView(QWidget *parent);

    void setScene(QGraphicsScene *scene);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    void update_scale();
};

#endif // GRAPHICSVIEW_H
