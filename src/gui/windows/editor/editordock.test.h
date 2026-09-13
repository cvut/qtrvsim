#ifndef EDITORDOCK_TEST_H
#define EDITORDOCK_TEST_H

#include <QtTest>

class TestEditorDock : public QObject {
    Q_OBJECT

private slots:
    void follow_location_data();
    void follow_location();
    void navigate_to_source_data();
    void navigate_to_source();
    void execution_highlight_moves();
    void execution_highlight_clears_data();
    void execution_highlight_clears();
    void execution_highlight_survives_tab_close();
    void failed_open_cache();
};

#endif // EDITORDOCK_TEST_H
