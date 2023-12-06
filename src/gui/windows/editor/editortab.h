#ifndef EDITORTAB_H
#define EDITORTAB_H

#include "common/memory_ownership.h"
#include "srceditor.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>
#include <QtWidgets/qboxlayout.h>

class EditorTab : public QWidget {
    Q_OBJECT
    using Super = QWidget;

public:
    explicit EditorTab(bool show_line_numbers, QWidget *parent = nullptr);
    [[nodiscard]] SrcEditor *get_editor() const { return editor; }
    QString title();

public slots:
    void set_show_line_number(bool visible);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void elide_file_name();

private:
    QT_OWNED QVBoxLayout *layout;
    QT_OWNED SrcEditor *editor;
    QT_OWNED QHBoxLayout *status_bar_layout;
    QT_OWNED QLabel *status_bar_path;
    QT_OWNED QLabel *status_bar_location;
};

#endif // EDITORTAB_H