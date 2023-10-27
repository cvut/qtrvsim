#ifndef SAVECHANGED_H
#define SAVECHANGED_H

#include <QDialog>
#include <QList>
#include <QStandardItemModel>
#include <QStringList>

class SaveChangedDialog : public QDialog {
    Q_OBJECT

public:
    explicit SaveChangedDialog(QStringList &changedlist, QWidget *parent = nullptr);
signals:
    void user_decision(bool cancel, QStringList tosavelist);
private slots:
    void cancel_clicked();
    void ignore_clicked();
    void save_clicked();

private:
    QStandardItemModel *model;
};

#endif // SAVECHANGED_H
