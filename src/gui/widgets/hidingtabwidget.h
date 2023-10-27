#ifndef HIDINGTABWIDGET_H
#define HIDINGTABWIDGET_H

#include <QObject>
#include <QTabWidget>

/** A widget that hides itself when it has no tabs. */
class HidingTabWidget : public QTabWidget {
    Q_OBJECT
    using Super = QTabWidget;

public:
    explicit HidingTabWidget(QWidget *parent = nullptr) : Super(parent) {};

    void tabInserted(int index) override;
    void tabRemoved(int index) override;

signals:
    void requestAddRemoveTab(QWidget *tab, bool visible);

public slots:
    void addRemoveTabRequested(QWidget *tab, bool exists);

protected:
    virtual void tabCountChanged() {};
};

#endif // HIDINGTABWIDGET_H
