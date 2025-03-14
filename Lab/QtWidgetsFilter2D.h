#pragma once

#include <iostream>
#include <experimental/filesystem>

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
	QSettings* settings;

public slots:
	void SlotButtonImageLoad_Clicked();

protected:
	QString imgName;
	QImage qImage;

private:
	Ui::QtWidgetsFilter2DClass ui;

	QGraphicsScene* scene = new QGraphicsScene();
};
