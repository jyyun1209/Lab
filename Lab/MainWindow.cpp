#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    std::string settingFile = std::experimental::filesystem::current_path().string() + "\\config.ini";
    settings = new QSettings(QString::fromStdString(settingFile), QSettings::IniFormat);

    InitializeUI();
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::InitializeUI()
{
    // Tab
    tabWidgetMain = new QTabWidget();

    // Widget
    widgetFilter2D = new QtWidgetsFilter2D(settings, this);
    tabWidgetMain->addTab(widgetFilter2D, QString("Filter2D"));

    setCentralWidget(tabWidgetMain);
}