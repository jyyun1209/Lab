#pragma once

#include <QWidget>
#include "ui_QtWidgetsFilter2D.h"

class QtWidgetsFilter2D : public QWidget
{
	Q_OBJECT

public:
	QtWidgetsFilter2D(QWidget *parent = nullptr);
	~QtWidgetsFilter2D();

private:
	Ui::QtWidgetsFilter2DClass ui;
};
