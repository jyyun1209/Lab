#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    InitializeUI();
}

MainWindow::~MainWindow()
{}

void MainWindow::InitializeUI()
{
    // Tab
    QTabWidget* tabWidgetMain = new QTabWidget();

    // Widget
    QtWidgetsFilter2D* widgetFilter2D = new QtWidgetsFilter2D();
    tabWidgetMain->addTab(widgetFilter2D, QString("Filter2D"));
    setCentralWidget(tabWidgetMain);
}