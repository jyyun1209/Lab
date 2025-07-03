#pragma once

#include <Windows.h>
#include <iostream>
#include <experimental/filesystem>

#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include "ui_QtWidgetsFilter2D.h"

#include "Common/File.h"
#include "Common/Differentiation.h"
#include "Common/ColorMode.h"
#include "Common/Noise.h"
#include "Filter2D/MedianFilter.h"
#include "Filter2D/SpatialEdgePreserving.h"
#include "Filter2D/Diffusion.h"
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

	void SlotCheckBoxNothing_Clicked();
	void SlotCheckBoxPartialDiffX_Clicked();
	void SlotCheckBoxPartialDiffY_Clicked();
	void SlotComboBoxPartialDiffMode_Changed();
	void SlotComboBoxDiffDirection_Changed();
	void SlotCheckBoxLaplacian_Clicked();
	void SlotComboBoxLaplacianMode_Changed();
	void SlotCheckBoxGrayscale_Clicked();
	void SlotCheckBoxSaltAndPepper_Clicked();
	void SlotCheckBoxSpeckleNoise_Clicked();
	void SlotCheckBoxGaussianNoise_Clicked();
	void SlotCheckBoxRGBGain_Clicked();
	void SlotLineEditR_Changed();
	void SlotLineEditG_Changed();
	void SlotLineEditB_Changed();
	void SlotSliderR_Changed();
	void SlotSliderG_Changed();
	void SlotSliderB_Changed();

	// ---------------------------------------------------
	// Filters
	// ---------------------------------------------------
	
	// Median Filter
	void SlotCheckboxMedian_Clicked();
	void SlotLineEditMedian_Changed();

	// Spatial Edge Preserving
	void SlotCheckboxSpatialEdgePreserving_Clicked();
	void SlotComboBoxSEPMode_Changed();
	void SlotLineEditSpatialEdgePreserving_Changed();

	// Heat Equation
	void SlotCheckboxHeatEquation_Clicked();
	void SlotSliderAlpha_Changed();
	void SlotSliderIteration_Changed();

	// Anisotropic Diffusion
	void SlotCheckboxAnisotropicDiffusion_Clicked();
	void SlotSliderK_Changed();
	void SlotSliderIterationAD_Changed();
	void SlotLineEditK_Changed();
	void SlotLineEditIterationAD_Changed();

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
