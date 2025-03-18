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
	QtWidgetsFilter2D(QSettings* settings, QWidget *parent = nullptr);
	~QtWidgetsFilter2D();

	void InitializeUI();
	QString imgName;
	QSettings* filter2DSettings;

public slots:
	void SlotButtonImageLoad_Clicked();

protected:
	QImage qImage;

private:
	Ui::QtWidgetsFilter2DClass ui;

	QGraphicsScene* scene = new QGraphicsScene();
};
