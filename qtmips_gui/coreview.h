#ifndef COREVIEW_H
#define COREVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include "qtmipsmachine.h"
#include "coreview/connection.h"
#include "coreview/programcounter.h"
#include "coreview/multiplexer.h"
#include "coreview/latch.h"
#include "coreview/alu.h"

class CoreView : public QGraphicsView {
public:
    CoreView(QWidget *parent);

protected:
    void resizeEvent(QResizeEvent *event);
private:
    const int scene_width = 800, scene_height = 800;
    void update_scale();
};

class CoreViewScene : public QGraphicsScene {
    Q_OBJECT
public:
    CoreViewScene(CoreView *view, machine::QtMipsMachine *machine);

private:
    machine::QtMipsMachine *machine;

    coreview::ProgramCounter *pc;
    coreview::Alu *alu;
    coreview::Multiplexer *pc_multiplexer;
    coreview::Connection *pc2pc;
    coreview::Latch *testlatch;
};

#else

class CoreView;
class CoreViewScene;

#endif // COREVIEW_H
