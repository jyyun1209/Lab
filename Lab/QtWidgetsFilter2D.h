#pragma once

#include <Windows.h>
#include <iostream>
#include <experimental/filesystem>

#include <QWidget>
#include <QFileDialog>
#include "ui_QtWidgetsFilter2D.h"

#include "Common/File.h"
#include "Common/Differentiation.h"
#include "Filter2D/MedianFilter.h"
#include "Filter2D/SpatialEdgePreserving.h"
#include "Filter2D/HeatEquation.h"
#include "Reconstruction/ReconstructionCore.h"

class QtWidgetsFilter2D : public QWidget
{
	Q_OBJECT

public:
	QtWidgetsFilter2D(QSettings* settings, QWidget *parent = nullptr);
	~QtWidgetsFilter2D();

	void InitializeUI();
	QSettings* filter2DSettings;

	QString imgName;
	QString calibName;

public slots:
	// Load Image
	void SlotButtonImageLoad_Clicked();

	// Fit Zoom
	void SlotButtonFitZoom_Clicked();

	// Retrieve
	void SlotButtonRetrieve_Clicked();

	// ---------------------------------------------------
	// Extra
	// ---------------------------------------------------

	void SlotCheckboxNothing_Clicked();
	void SlotCheckboxPartialDiffX_Clicked();
	void SlotCheckBoxPartialDiffY_Clicked();
	void SlotComboBoxPartialDiffMode_Changed();

	// ---------------------------------------------------
	// Filters
	// ---------------------------------------------------
	// 
	// Median Filter
	void SlotCheckboxMedian_Clicked();
	void SlotLineEditMedian_Changed();

	// Spatial Edge Preserving
	void SlotCheckboxSpatialEdgePreserving_Clicked();
	void SlotLineEditSpatialEdgePreserving_Changed();

	// Heat Equation
	void SlotCheckboxHeatEquation_Clicked();
	void SlotSliderAlpha_Changed();
	void SlotSliderIteration_Changed();

	// Save Image
	void SlotButtonSave_Clicked();

	// Generate 3D
	void SlotButtonCalibLoad_Clicked();
	void SlotButtonGenerate3D_Clicked();

protected:
	cv::Mat cvImage, cvImage_Display;
	void UpdateImageFromCV(cv::Mat _image, QGraphicsScene* _scene);

private:
	Ui::QtWidgetsFilter2DClass ui;

	QGraphicsScene* scene = new QGraphicsScene();
};
