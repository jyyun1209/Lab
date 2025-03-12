#pragma once

#include <QWidget>
#include <QFileDialog>
#include "ui_QtWidgetsFilter2D.h"

class QtWidgetsFilter2D : public QWidget
{
	Q_OBJECT

public:
	QtWidgetsFilter2D(QWidget *parent = nullptr);
	~QtWidgetsFilter2D();

	void InitializeUI();

public slots:
	void SlotButtonImageLoad_Clicked();

private:
	Ui::QtWidgetsFilter2DClass ui;

	QGraphicsScene* scene = new QGraphicsScene();
};
