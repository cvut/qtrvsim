/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionNewMachine;
    QAction *actionRestart;
    QAction *actionNew;
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionSaveAs;
    QAction *actionClose;
    QAction *actionCompileSource;
    QAction *actionBuildExe;
    QAction *actionExit;
    QAction *actionRun;
    QAction *actionStep;
    QAction *actionPause;
    QAction *ips1;
    QAction *ips5;
    QAction *ips10;
    QAction *ips25;
    QAction *ipsUnlimited;
    QAction *actionMemory;
    QAction *actionProgram_memory;
    QAction *actionRegisters;
    QAction *actionCsrShow;
    QAction *actionReload;
    QAction *actionPrint;
    QAction *actionProgram_Cache;
    QAction *actionData_Cache;
    QAction *actionL2_Cache;
    QAction *actionResetWindows;
    QAction *ips2;
    QAction *actionAbout;
    QAction *ipsMax;
    QAction *actionAboutQt;
    QAction *actionPeripherals;
    QAction *actionTerminal;
    QAction *actionLcdDisplay;
    QAction *actionShow_Symbol;
    QAction *actionCore_View_show;
    QAction *actionMessages;
    QAction *actionMnemonicRegisters;
    QAction *actionEditorShowLineNumbers;
    QAction *actionBranch_Predictor_History_table;
    QAction *actionBranch_Predictor_Target_table;
    QAction *actionBranch_Predictor_Info;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QWidget *widget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuExamples;
    QMenu *menuWindows;
    QMenu *menuMachine;
    QMenu *menuHelp;
    QMenu *menuOptions;
    QStatusBar *statusBar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(900, 600);
        QIcon icon;
        icon.addFile(QString::fromUtf8("\n"
"           :/icons/gui.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setDockNestingEnabled(false);
        MainWindow->setUnifiedTitleAndToolBarOnMac(true);
        actionNewMachine = new QAction(MainWindow);
        actionNewMachine->setObjectName(QString::fromUtf8("actionNewMachine"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/document-import.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNewMachine->setIcon(icon1);
        actionRestart = new QAction(MainWindow);
        actionRestart->setObjectName(QString::fromUtf8("actionRestart"));
        actionNew = new QAction(MainWindow);
        actionNew->setObjectName(QString::fromUtf8("actionNew"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/new.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNew->setIcon(icon2);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon3);
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        actionSave->setEnabled(false);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/icons/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon4);
        actionSaveAs = new QAction(MainWindow);
        actionSaveAs->setObjectName(QString::fromUtf8("actionSaveAs"));
        actionSaveAs->setEnabled(false);
        actionClose = new QAction(MainWindow);
        actionClose->setObjectName(QString::fromUtf8("actionClose"));
        actionClose->setEnabled(false);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/icons/closetab.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionClose->setIcon(icon5);
        actionCompileSource = new QAction(MainWindow);
        actionCompileSource->setObjectName(QString::fromUtf8("actionCompileSource"));
        actionCompileSource->setEnabled(false);
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/icons/compfile-256.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCompileSource->setIcon(icon6);
        actionBuildExe = new QAction(MainWindow);
        actionBuildExe->setObjectName(QString::fromUtf8("actionBuildExe"));
        actionBuildExe->setEnabled(true);
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/icons/build-256.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionBuildExe->setIcon(icon7);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/icons/application-exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionExit->setIcon(icon8);
        actionRun = new QAction(MainWindow);
        actionRun->setObjectName(QString::fromUtf8("actionRun"));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/icons/play.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRun->setIcon(icon9);
        actionStep = new QAction(MainWindow);
        actionStep->setObjectName(QString::fromUtf8("actionStep"));
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/icons/next.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionStep->setIcon(icon10);
        actionPause = new QAction(MainWindow);
        actionPause->setObjectName(QString::fromUtf8("actionPause"));
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/icons/pause.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPause->setIcon(icon11);
        ips1 = new QAction(MainWindow);
        ips1->setObjectName(QString::fromUtf8("ips1"));
        ips1->setCheckable(true);
        ips5 = new QAction(MainWindow);
        ips5->setObjectName(QString::fromUtf8("ips5"));
        ips5->setCheckable(true);
        ips10 = new QAction(MainWindow);
        ips10->setObjectName(QString::fromUtf8("ips10"));
        ips10->setCheckable(true);
        ips25 = new QAction(MainWindow);
        ips25->setObjectName(QString::fromUtf8("ips25"));
        ips25->setCheckable(true);
        ipsUnlimited = new QAction(MainWindow);
        ipsUnlimited->setObjectName(QString::fromUtf8("ipsUnlimited"));
        ipsUnlimited->setCheckable(true);
        actionMemory = new QAction(MainWindow);
        actionMemory->setObjectName(QString::fromUtf8("actionMemory"));
        actionProgram_memory = new QAction(MainWindow);
        actionProgram_memory->setObjectName(QString::fromUtf8("actionProgram_memory"));
        actionRegisters = new QAction(MainWindow);
        actionRegisters->setObjectName(QString::fromUtf8("actionRegisters"));
        actionCsrShow = new QAction(MainWindow);
        actionCsrShow->setObjectName(QString::fromUtf8("actionCsrShow"));
        actionReload = new QAction(MainWindow);
        actionReload->setObjectName(QString::fromUtf8("actionReload"));
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/icons/reload.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionReload->setIcon(icon12);
        actionPrint = new QAction(MainWindow);
        actionPrint->setObjectName(QString::fromUtf8("actionPrint"));
        actionProgram_Cache = new QAction(MainWindow);
        actionProgram_Cache->setObjectName(QString::fromUtf8("actionProgram_Cache"));
        actionData_Cache = new QAction(MainWindow);
        actionData_Cache->setObjectName(QString::fromUtf8("actionData_Cache"));
        actionL2_Cache = new QAction(MainWindow);
        actionL2_Cache->setObjectName(QString::fromUtf8("actionL2_Cache"));
        actionResetWindows = new QAction(MainWindow);
        actionResetWindows->setObjectName(QString::fromUtf8("actionResetWindows"));
        ips2 = new QAction(MainWindow);
        ips2->setObjectName(QString::fromUtf8("ips2"));
        ips2->setCheckable(true);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        ipsMax = new QAction(MainWindow);
        ipsMax->setObjectName(QString::fromUtf8("ipsMax"));
        ipsMax->setCheckable(true);
        actionAboutQt = new QAction(MainWindow);
        actionAboutQt->setObjectName(QString::fromUtf8("actionAboutQt"));
        actionPeripherals = new QAction(MainWindow);
        actionPeripherals->setObjectName(QString::fromUtf8("actionPeripherals"));
        actionTerminal = new QAction(MainWindow);
        actionTerminal->setObjectName(QString::fromUtf8("actionTerminal"));
        actionLcdDisplay = new QAction(MainWindow);
        actionLcdDisplay->setObjectName(QString::fromUtf8("actionLcdDisplay"));
        actionShow_Symbol = new QAction(MainWindow);
        actionShow_Symbol->setObjectName(QString::fromUtf8("actionShow_Symbol"));
        actionCore_View_show = new QAction(MainWindow);
        actionCore_View_show->setObjectName(QString::fromUtf8("actionCore_View_show"));
        actionCore_View_show->setCheckable(true);
        actionCore_View_show->setChecked(true);
        actionMessages = new QAction(MainWindow);
        actionMessages->setObjectName(QString::fromUtf8("actionMessages"));
        actionMnemonicRegisters = new QAction(MainWindow);
        actionMnemonicRegisters->setObjectName(QString::fromUtf8("actionMnemonicRegisters"));
        actionMnemonicRegisters->setCheckable(true);
        actionMnemonicRegisters->setChecked(false);
        actionEditorShowLineNumbers = new QAction(MainWindow);
        actionEditorShowLineNumbers->setObjectName(QString::fromUtf8("actionEditorShowLineNumbers"));
        actionEditorShowLineNumbers->setCheckable(true);
        actionEditorShowLineNumbers->setChecked(true);
        actionBranch_Predictor_History_table = new QAction(MainWindow);
        actionBranch_Predictor_History_table->setObjectName(QString::fromUtf8("actionBranch_Predictor_History_table"));
        actionBranch_Predictor_Target_table = new QAction(MainWindow);
        actionBranch_Predictor_Target_table->setObjectName(QString::fromUtf8("actionBranch_Predictor_Target_table"));
        actionBranch_Predictor_Info = new QAction(MainWindow);
        actionBranch_Predictor_Info->setObjectName(QString::fromUtf8("actionBranch_Predictor_Info"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        widget = new QWidget(centralWidget);
        widget->setObjectName(QString::fromUtf8("widget"));

        horizontalLayout->addWidget(widget);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 900, 20));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuExamples = new QMenu(menuFile);
        menuExamples->setObjectName(QString::fromUtf8("menuExamples"));
        menuWindows = new QMenu(menuBar);
        menuWindows->setObjectName(QString::fromUtf8("menuWindows"));
        menuMachine = new QMenu(menuBar);
        menuMachine->setObjectName(QString::fromUtf8("menuMachine"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuOptions = new QMenu(menuBar);
        menuOptions->setObjectName(QString::fromUtf8("menuOptions"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setMovable(false);
        toolBar->setAllowedAreas(Qt::TopToolBarArea);
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuMachine->menuAction());
        menuBar->addAction(menuWindows->menuAction());
        menuBar->addAction(menuOptions->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionNewMachine);
        menuFile->addAction(actionReload);
        menuFile->addAction(actionPrint);
        menuFile->addAction(actionNew);
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionSaveAs);
        menuFile->addAction(actionClose);
        menuFile->addAction(menuExamples->menuAction());
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuWindows->addAction(actionRegisters);
        menuWindows->addAction(actionProgram_memory);
        menuWindows->addAction(actionMemory);
        menuWindows->addAction(actionProgram_Cache);
        menuWindows->addAction(actionData_Cache);
        menuWindows->addAction(actionL2_Cache);
        menuWindows->addAction(actionBranch_Predictor_History_table);
        menuWindows->addAction(actionBranch_Predictor_Target_table);
        menuWindows->addAction(actionBranch_Predictor_Info);
        menuWindows->addAction(actionPeripherals);
        menuWindows->addAction(actionTerminal);
        menuWindows->addAction(actionLcdDisplay);
        menuWindows->addAction(actionCsrShow);
        menuWindows->addAction(actionCore_View_show);
        menuWindows->addAction(actionMessages);
        menuWindows->addAction(actionResetWindows);
        menuMachine->addAction(actionRun);
        menuMachine->addAction(actionPause);
        menuMachine->addAction(actionStep);
        menuMachine->addSeparator();
        menuMachine->addAction(ips1);
        menuMachine->addAction(ips2);
        menuMachine->addAction(ips5);
        menuMachine->addAction(ips10);
        menuMachine->addAction(ips25);
        menuMachine->addAction(ipsUnlimited);
        menuMachine->addAction(ipsMax);
        menuMachine->addSeparator();
        menuMachine->addAction(actionRestart);
        menuMachine->addAction(actionMnemonicRegisters);
        menuMachine->addAction(actionShow_Symbol);
        menuMachine->addAction(actionCompileSource);
        menuMachine->addAction(actionBuildExe);
        menuHelp->addAction(actionAbout);
        menuHelp->addAction(actionAboutQt);
        menuOptions->addAction(actionEditorShowLineNumbers);
        toolBar->addAction(actionNewMachine);
        toolBar->addAction(actionReload);
        toolBar->addSeparator();
        toolBar->addAction(actionRun);
        toolBar->addAction(actionPause);
        toolBar->addAction(actionStep);
        toolBar->addSeparator();
        toolBar->addAction(ips1);
        toolBar->addAction(ips2);
        toolBar->addAction(ips5);
        toolBar->addAction(ips10);
        toolBar->addAction(ips25);
        toolBar->addAction(ipsUnlimited);
        toolBar->addAction(ipsMax);
        toolBar->addAction(actionNew);
        toolBar->addAction(actionOpen);
        toolBar->addAction(actionSave);
        toolBar->addAction(actionClose);
        toolBar->addAction(actionCompileSource);
        toolBar->addAction(actionBuildExe);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionNewMachine->setText(QCoreApplication::translate("MainWindow", "&New simulation...", nullptr));
#if QT_CONFIG(shortcut)
        actionNewMachine->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        actionRestart->setText(QCoreApplication::translate("MainWindow", "R&estart", nullptr));
        actionNew->setText(QCoreApplication::translate("MainWindow", "Ne&w source", nullptr));
#if QT_CONFIG(tooltip)
        actionNew->setToolTip(QCoreApplication::translate("MainWindow", "New source", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionNew->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+F", nullptr));
#endif // QT_CONFIG(shortcut)
        actionOpen->setText(QCoreApplication::translate("MainWindow", "&Open source", nullptr));
#if QT_CONFIG(tooltip)
        actionOpen->setToolTip(QCoreApplication::translate("MainWindow", "Open source", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionOpen->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSave->setText(QCoreApplication::translate("MainWindow", "&Save source", nullptr));
#if QT_CONFIG(tooltip)
        actionSave->setToolTip(QCoreApplication::translate("MainWindow", "Save source", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionSave->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSaveAs->setText(QCoreApplication::translate("MainWindow", "Save source &as", nullptr));
#if QT_CONFIG(tooltip)
        actionSaveAs->setToolTip(QCoreApplication::translate("MainWindow", "Save source as", nullptr));
#endif // QT_CONFIG(tooltip)
        actionClose->setText(QCoreApplication::translate("MainWindow", "&Close source", nullptr));
#if QT_CONFIG(tooltip)
        actionClose->setToolTip(QCoreApplication::translate("MainWindow", "Close source", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionClose->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+W", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCompileSource->setText(QCoreApplication::translate("MainWindow", "&Compile Source", nullptr));
#if QT_CONFIG(tooltip)
        actionCompileSource->setToolTip(QCoreApplication::translate("MainWindow", "Compile source and update memory", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionCompileSource->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+E", nullptr));
#endif // QT_CONFIG(shortcut)
        actionBuildExe->setText(QCoreApplication::translate("MainWindow", "&Build Executable", nullptr));
#if QT_CONFIG(tooltip)
        actionBuildExe->setToolTip(QCoreApplication::translate("MainWindow", "Build executable by external make", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionBuildExe->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+B", nullptr));
#endif // QT_CONFIG(shortcut)
        actionExit->setText(QCoreApplication::translate("MainWindow", "E&xit", nullptr));
#if QT_CONFIG(tooltip)
        actionExit->setToolTip(QCoreApplication::translate("MainWindow", "Exit program", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionExit->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Q", nullptr));
#endif // QT_CONFIG(shortcut)
        actionRun->setText(QCoreApplication::translate("MainWindow", "&Run", nullptr));
#if QT_CONFIG(shortcut)
        actionRun->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+R", nullptr));
#endif // QT_CONFIG(shortcut)
        actionStep->setText(QCoreApplication::translate("MainWindow", "&Step", nullptr));
#if QT_CONFIG(shortcut)
        actionStep->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+T", nullptr));
#endif // QT_CONFIG(shortcut)
        actionPause->setText(QCoreApplication::translate("MainWindow", "&Pause", nullptr));
#if QT_CONFIG(shortcut)
        actionPause->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+P", nullptr));
#endif // QT_CONFIG(shortcut)
        ips1->setText(QCoreApplication::translate("MainWindow", "&1 instruction per second", nullptr));
        ips1->setIconText(QCoreApplication::translate("MainWindow", "1x", nullptr));
#if QT_CONFIG(tooltip)
        ips1->setToolTip(QCoreApplication::translate("MainWindow", "Run CPU in speed of single instruction per second", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        ips1->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+1", nullptr));
#endif // QT_CONFIG(shortcut)
        ips5->setText(QCoreApplication::translate("MainWindow", "&5 instructions per second", nullptr));
        ips5->setIconText(QCoreApplication::translate("MainWindow", "5x", nullptr));
#if QT_CONFIG(tooltip)
        ips5->setToolTip(QCoreApplication::translate("MainWindow", "Run CPU in speed of 5 instructions per second", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        ips5->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+5", nullptr));
#endif // QT_CONFIG(shortcut)
        ips10->setText(QCoreApplication::translate("MainWindow", "1&0 instructions per second", nullptr));
        ips10->setIconText(QCoreApplication::translate("MainWindow", "10x", nullptr));
#if QT_CONFIG(tooltip)
        ips10->setToolTip(QCoreApplication::translate("MainWindow", "Run CPU in speed of 10 instructions per second", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        ips10->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+0", nullptr));
#endif // QT_CONFIG(shortcut)
        ips25->setText(QCoreApplication::translate("MainWindow", "2&5 instructions per second", nullptr));
        ips25->setIconText(QCoreApplication::translate("MainWindow", "25x", nullptr));
#if QT_CONFIG(tooltip)
        ips25->setToolTip(QCoreApplication::translate("MainWindow", "Run CPU in speed of 25 instructions per second", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        ips25->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+F5", nullptr));
#endif // QT_CONFIG(shortcut)
        ipsUnlimited->setText(QCoreApplication::translate("MainWindow", "&Unlimited", nullptr));
#if QT_CONFIG(tooltip)
        ipsUnlimited->setToolTip(QCoreApplication::translate("MainWindow", "Run CPU without any clock constrains", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        ipsUnlimited->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+U", nullptr));
#endif // QT_CONFIG(shortcut)
        actionMemory->setText(QCoreApplication::translate("MainWindow", "&Memory", nullptr));
#if QT_CONFIG(tooltip)
        actionMemory->setToolTip(QCoreApplication::translate("MainWindow", "Data memory view", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionMemory->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+M", nullptr));
#endif // QT_CONFIG(shortcut)
        actionProgram_memory->setText(QCoreApplication::translate("MainWindow", "&Program", nullptr));
#if QT_CONFIG(tooltip)
        actionProgram_memory->setToolTip(QCoreApplication::translate("MainWindow", "Program memory view", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionProgram_memory->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+P", nullptr));
#endif // QT_CONFIG(shortcut)
        actionRegisters->setText(QCoreApplication::translate("MainWindow", "&Registers", nullptr));
#if QT_CONFIG(shortcut)
        actionRegisters->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+D", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCsrShow->setText(QCoreApplication::translate("MainWindow", "C&ontrol and Status Registers", nullptr));
#if QT_CONFIG(shortcut)
        actionCsrShow->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+I", nullptr));
#endif // QT_CONFIG(shortcut)
        actionReload->setText(QCoreApplication::translate("MainWindow", "&Reload simulation", nullptr));
#if QT_CONFIG(shortcut)
        actionReload->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+R", nullptr));
#endif // QT_CONFIG(shortcut)
        actionPrint->setText(QCoreApplication::translate("MainWindow", "&Print", nullptr));
#if QT_CONFIG(shortcut)
        actionPrint->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Alt+P", nullptr));
#endif // QT_CONFIG(shortcut)
        actionProgram_Cache->setText(QCoreApplication::translate("MainWindow", "Pro&gram Cache", nullptr));
#if QT_CONFIG(shortcut)
        actionProgram_Cache->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+P", nullptr));
#endif // QT_CONFIG(shortcut)
        actionData_Cache->setText(QCoreApplication::translate("MainWindow", "&Data Cache", nullptr));
#if QT_CONFIG(shortcut)
        actionData_Cache->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+M", nullptr));
#endif // QT_CONFIG(shortcut)
        actionL2_Cache->setText(QCoreApplication::translate("MainWindow", "L2 Cache", nullptr));
        actionResetWindows->setText(QCoreApplication::translate("MainWindow", "Reset Windows", nullptr));
        ips2->setText(QCoreApplication::translate("MainWindow", "&2 instructions per second", nullptr));
        ips2->setIconText(QCoreApplication::translate("MainWindow", "2x", nullptr));
#if QT_CONFIG(tooltip)
        ips2->setToolTip(QCoreApplication::translate("MainWindow", "Run CPU in speed of two instructions per second", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        ips2->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+2", nullptr));
#endif // QT_CONFIG(shortcut)
        actionAbout->setText(QCoreApplication::translate("MainWindow", "About Qt&RVSim", nullptr));
        ipsMax->setText(QCoreApplication::translate("MainWindow", "&Max", nullptr));
#if QT_CONFIG(tooltip)
        ipsMax->setToolTip(QCoreApplication::translate("MainWindow", "Run at maximal speed, skip visualization for 100 msec", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        ipsMax->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+A", nullptr));
#endif // QT_CONFIG(shortcut)
        actionAboutQt->setText(QCoreApplication::translate("MainWindow", "About &Qt", nullptr));
        actionPeripherals->setText(QCoreApplication::translate("MainWindow", "P&eripherals", nullptr));
        actionTerminal->setText(QCoreApplication::translate("MainWindow", "&Terminal", nullptr));
        actionLcdDisplay->setText(QCoreApplication::translate("MainWindow", "&LCD display", nullptr));
        actionShow_Symbol->setText(QCoreApplication::translate("MainWindow", "Sh&ow Symbol", nullptr));
#if QT_CONFIG(tooltip)
        actionShow_Symbol->setToolTip(QCoreApplication::translate("MainWindow", "Show Symbol", nullptr));
#endif // QT_CONFIG(tooltip)
        actionCore_View_show->setText(QCoreApplication::translate("MainWindow", "&Core View", nullptr));
        actionMessages->setText(QCoreApplication::translate("MainWindow", "Me&ssages", nullptr));
#if QT_CONFIG(tooltip)
        actionMessages->setToolTip(QCoreApplication::translate("MainWindow", "Show compile/build messages", nullptr));
#endif // QT_CONFIG(tooltip)
        actionMnemonicRegisters->setText(QCoreApplication::translate("MainWindow", "M&nemonics Registers", nullptr));
        actionEditorShowLineNumbers->setText(QCoreApplication::translate("MainWindow", "Show &Line Numbers", nullptr));
#if QT_CONFIG(tooltip)
        actionEditorShowLineNumbers->setToolTip(QCoreApplication::translate("MainWindow", "Show line number in the code editor.", nullptr));
#endif // QT_CONFIG(tooltip)
        actionBranch_Predictor_History_table->setText(QCoreApplication::translate("MainWindow", "Branch Predictor (History table)", nullptr));
        actionBranch_Predictor_Target_table->setText(QCoreApplication::translate("MainWindow", "Branch Predictor (Target table)", nullptr));
        actionBranch_Predictor_Info->setText(QCoreApplication::translate("MainWindow", "Branch Predictor (Info)", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "&File", nullptr));
        menuExamples->setTitle(QCoreApplication::translate("MainWindow", "&Examples", nullptr));
        menuWindows->setTitle(QCoreApplication::translate("MainWindow", "&Windows", nullptr));
        menuMachine->setTitle(QCoreApplication::translate("MainWindow", "M&achine", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "&Help", nullptr));
        menuOptions->setTitle(QCoreApplication::translate("MainWindow", "Options", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
