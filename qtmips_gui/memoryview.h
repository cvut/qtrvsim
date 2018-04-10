#ifndef MEMORYVIEW_H
#define MEMORYVIEW_H

#include <QWidget>
#include <QAbstractScrollArea>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QList>
#include <QVector>
#include <cstdint>
#include <QResizeEvent>
#include "qtmipsmachine.h"
#include "statictable.h"

class MemoryView : public QWidget {
    Q_OBJECT
public:
    MemoryView(QWidget *parent = nullptr);

    virtual void setup(machine::QtMipsMachine*);

    void set_focus(std::uint32_t address);
    std::uint32_t focus();

    void edit_load_focus(); // Set current focus to edit field

protected:
    const machine::Memory *memory;

    virtual QList<QWidget*> row_widget(std::uint32_t address, QWidget *parent) = 0;

    QVBoxLayout *layout;

    void reload_content(); // reload displayed data
    void update_content(int count, int shift); // update content to match given count and shift

private slots:
    void go_edit_finish();

private:
    unsigned count;
    std::uint32_t addr_0; // First address in view

    class Frame : public QAbstractScrollArea {
    public:
        Frame(MemoryView *parent);

        StaticTable *widg;
        void focus(unsigned i); // Focus on given item in widget
        unsigned focussed(); // What item is in focus
        void check_update(); // Update widget size and content if needed

    protected:
        MemoryView *mv;
        int content_y;

        bool viewportEvent(QEvent*);
        void resizeEvent(QResizeEvent*);
        void wheelEvent(QWheelEvent *event);
    };
    Frame *memf;

    QWidget *ctl_widg;
    QLineEdit *go_edit;
};

#endif // MEMORYVIEW_H
