#pragma once

#include <iostream>
#include <experimental/filesystem>

#include <QWidget>
#include <QFileDialog>
#include "ui_QtWidgetsFilter2D.h"

#include "Filter2D/MedianFilter.h"

class QtWidgetsFilter2D : public QWidget
{
	Q_OBJECT

public:
	QtWidgetsFilter2D(QSettings* settings, QWidget *parent = nullptr);
	~QtWidgetsFilter2D();

	void InitializeUI();
	QSettings* filter2DSettings;

	QString imgName;
	int medianKernel;

public slots:
	void SlotButtonImageLoad_Clicked();
	void SlotCheckboxMedian_Clicked();

protected:
	QImage qImage;

private:
	Ui::QtWidgetsFilter2DClass ui;

	QGraphicsScene* scene = new QGraphicsScene();
};
