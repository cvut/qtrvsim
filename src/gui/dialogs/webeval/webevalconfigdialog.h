#ifndef WEBEVALCONFIGDIALOG_H
#define WEBEVALCONFIGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSettings>

class WebEvalConfigDialog : public QDialog {
    Q_OBJECT

public:
    explicit WebEvalConfigDialog(QSettings *settings, QWidget *parent = nullptr);

private slots:
    void save_and_close();

private:
    QSettings *settings;
    QLineEdit *url_edit;
    QLineEdit *api_key_edit;
};

#endif // WEBEVALCONFIGDIALOG_H
