#ifndef MESSAGESVIEW_H
#define MESSAGESVIEW_H

#include <QListView>
#include <QObject>
#include <QSettings>
#include <QSharedPointer>

class MessagesView : public QListView {
    Q_OBJECT

    using Super = QListView;

public:
    MessagesView(QWidget *parent, QSettings *settings);

    void resizeEvent(QResizeEvent *event) override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    QSettings *settings;
};

#endif // MESSAGESVIEW_H
