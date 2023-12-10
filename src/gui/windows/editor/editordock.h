#ifndef EDITORDOCK_H
#define EDITORDOCK_H

#include "common/memory_ownership.h"
#include "editortab.h"
#include "widgets/hidingtabwidget.h"

#include <QSettings>
#include <optional>

class EditorDock : public HidingTabWidget {
    Q_OBJECT
    using Super = HidingTabWidget;

public:
    /**
     * @param parent_tabs used to update parent tab based on current state
     */
    explicit EditorDock(
        QSharedPointer<QSettings> settings,
        QTabWidget *parent_tabs,
        QWidget *parent = nullptr);
    BORROWED [[nodiscard]] EditorTab *get_tab(int index) const;
    BORROWED EditorTab *create_empty_tab();
    BORROWED EditorTab *open_file(const QString &filename, bool save_as_required = false);
    BORROWED EditorTab *
    open_file_if_not_open(const QString &filename, bool save_as_required = false);
    BORROWED [[nodiscard]] std::optional<int> find_tab_id_by_filename(const QString &filename) const;
    BORROWED [[nodiscard]] EditorTab *find_tab_by_filename(const QString &filename) const;
    BORROWED [[nodiscard]] SrcEditor *get_current_editor() const;
    [[nodiscard]] QStringList get_open_file_list() const;
    bool get_modified_tab_filenames(QStringList &output, bool report_unnamed = false) const;
    bool set_cursor_to(const QString &filename, int line, int column);

protected:
    void tabCountChanged() override;

signals:
    void editor_available_changed(bool available);

public slots:
    void set_show_line_numbers(bool visible);

    void open_file_dialog();
    void save_tab(int index);
    void save_current_tab();
    void save_tab_as(int index);
    void save_current_tab_as();
    void save_tab_to(int index, const QString &filename);
    void save_current_tab_to(const QString &filename);
    void close_tab(int index);
    void close_current_tab();
    void close_tab_by_name(QString &filename, bool ask = false);

private:
    void close_tab_unchecked(int index);
    void confirm_close_tab_dialog(int index);

private:
    QSharedPointer<QSettings> settings;
    bool line_numbers_visible = true;
    size_t unknown_editor_counter = 1;
};

#endif // EDITORDOCK_H
