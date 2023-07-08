#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QWheelEvent>

class GraphicsView : public QGraphicsView {
    Q_OBJECT
    using Super = QGraphicsView;

public:
    explicit GraphicsView(QWidget *parent);
    void setScene(QGraphicsScene *scene);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void update_scale();
    int prev_height;
    int prev_width;
};

#endif // GRAPHICSVIEW_H
