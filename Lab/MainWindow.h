#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

#include "QtWidgetsFilter2D.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void InitializeUI();

private:
    Ui::MainWindowClass ui;
};
