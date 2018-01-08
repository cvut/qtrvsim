#ifndef MEMORYVIEW_H
#define MEMORYVIEW_H

#include <QWidget>
#include <QFrame>
#include <QBoxLayout>
#include <QLineEdit>
#include <cstdint>

class MemoryView : public QWidget {
public:
    MemoryView(QWidget *parent = nullptr);
    ~MemoryView();

    void set_center(std::uint32_t address);
    std::uint32_t center();

protected:
    //virtual QWidget *row_widget(std::uint32_t address) = 0;

    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);

    QVBoxLayout *layout;

private slots:
    void go_edit_finish();

private:
    std::uint32_t center_addr;

    QFrame *frame;
    QLineEdit *go_edit;
};

#endif // MEMORYVIEW_H
