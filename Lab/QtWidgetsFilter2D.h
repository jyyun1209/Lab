#pragma once

#include <iostream>
#include <experimental/filesystem>

#include <QWidget>
#include <QFileDialog>
#include "ui_QtWidgetsFilter2D.h"

#include "Common/File.h"
#include "Filter2D/MedianFilter.h"
#include "Filter2D/SpatialEdgePreserving.h"

class QtWidgetsFilter2D : public QWidget
{
	Q_OBJECT

public:
	QtWidgetsFilter2D(QSettings* settings, QWidget *parent = nullptr);
	~QtWidgetsFilter2D();

	void InitializeUI();
	QSettings* filter2DSettings;

	QString imgName;

public slots:
	// Load Image
	void SlotButtonImageLoad_Clicked();

	// Median Filter
	void SlotCheckboxMedian_Clicked();
	void SlotLineEditMedian_Changed();

	// Spatial Edge Preserving
	void SlotCheckboxSpatialEdgePreserving_Clicked();
	void SlotLineEditSpatialEdgePreserving_Changed();

	// Save Image
	void SlotButtonSave_Clicked();

protected:
	QImage qImage;
	cv::Mat cvImage;

private:
	Ui::QtWidgetsFilter2DClass ui;

	QGraphicsScene* scene = new QGraphicsScene();
};
