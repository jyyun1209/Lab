#include "QtWidgetsFilter2D.h"

QtWidgetsFilter2D::QtWidgetsFilter2D(QSettings* settings, QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	filter2DSettings = settings;
	InitializeUI();
}

QtWidgetsFilter2D::~QtWidgetsFilter2D()
{
	
}

void QtWidgetsFilter2D::UpdateImageFromCV(cv::Mat _image, QGraphicsScene* _scene)
{
	QImage qImage;
	if (_image.type() == CV_8UC1)
	{
		qImage = QImage(_image.data, _image.cols, _image.rows, _image.step, QImage::Format_Grayscale8);
	}
	else if (_image.type() == CV_8UC3)
	{
		qImage = QImage(_image.data, _image.cols, _image.rows, _image.step, QImage::Format_BGR888);
	}
	else if (_image.type() == CV_32FC1)
	{
		cv::Mat mask;
		cv::compare(_image, -INFINITY, mask, cv::CMP_NE);
		cv::normalize(_image, _image, 0, UINT16_MAX, cv::NORM_MINMAX, CV_16UC1, mask);
		qImage = QImage(_image.data, _image.cols, _image.rows, _image.step, QImage::Format_Grayscale16);
	}

	QPixmap pixmap = QPixmap::fromImage(qImage);
	_scene->clear();
	_scene->addPixmap(pixmap);
	_scene->setSceneRect(pixmap.rect());

	ui.graphicsView_image->setScene(_scene);
	ui.graphicsView_image->show();

	ui.label_ImgSize->setText(QString::number(_image.cols) + " x " + QString::number(_image.rows));

	cv::Scalar avgIntensity = cv::mean(_image);
	float avgIntensity_f = (avgIntensity[0] + avgIntensity[1] + avgIntensity[2]) / _image.channels();
	ui.label_AvgIntensity->setText(QString::number(avgIntensity_f, 'f', 2));
}

void QtWidgetsFilter2D::InitializeUI()
{
	// Connect Signal <-> Slot
	connect(ui.pushButton_imagePath, &QPushButton::clicked, this, &QtWidgetsFilter2D::SlotButtonImageLoad_Clicked);
	connect(ui.pushButton_fitZoom, &QPushButton::clicked, this, &QtWidgetsFilter2D::SlotButtonFitZoom_Clicked);
	connect(ui.pushButton_Retrieve, &QPushButton::clicked, this, &QtWidgetsFilter2D::SlotButtonRetrieve_Clicked);

	connect(ui.checkBox_Nothing, &QCheckBox::clicked, this, &QtWidgetsFilter2D::SlotCheckBoxNothing_Clicked);
	connect(ui.checkBox_PartialDiffX, &QCheckBox::clicked, this, &QtWidgetsFilter2D::SlotCheckBoxPartialDiffX_Clicked);
	connect(ui.checkBox_PartialDiffY, &QCheckBox::clicked, this, &QtWidgetsFilter2D::SlotCheckBoxPartialDiffY_Clicked);
	connect(ui.comboBox_PartialMode, &QComboBox::currentTextChanged, this, &QtWidgetsFilter2D::SlotComboBoxPartialDiffMode_Changed);
	connect(ui.comboBox_DiffDirection, &QComboBox::currentTextChanged, this, &QtWidgetsFilter2D::SlotComboBoxDiffDirection_Changed);
	connect(ui.checkBox_Laplacian, &QCheckBox::clicked, this, &QtWidgetsFilter2D::SlotCheckBoxLaplacian_Clicked);
	connect(ui.comboBox_LaplacianMode, &QComboBox::currentTextChanged, this, &QtWidgetsFilter2D::SlotComboBoxLaplacianMode_Changed);
	connect(ui.checkBox_Grayscale, &QCheckBox::clicked, this, &QtWidgetsFilter2D::SlotCheckBoxGrayscale_Clicked);
	connect(ui.checkBox_SaltAndPepper, &QCheckBox::clicked, this, &QtWidgetsFilter2D::SlotCheckBoxSaltAndPepper_Clicked);
	connect(ui.spinBox_SaltAndPepper, QOverload<int>::of(&QSpinBox::valueChanged), this, &QtWidgetsFilter2D::SlotCheckBoxSaltAndPepper_Clicked);
	connect(ui.groupBox_median, &QGroupBox::clicked, this, &QtWidgetsFilter2D::SlotCheckboxMedian_Clicked);
	connect(ui.lineEdit_medianKernel, &QLineEdit::editingFinished, this, &QtWidgetsFilter2D::SlotLineEditMedian_Changed);
	connect(ui.groupBox_SEP, &QGroupBox::clicked, this, &QtWidgetsFilter2D::SlotCheckboxSpatialEdgePreserving_Clicked);
	connect(ui.lineEdit_sepSigmaS, &QLineEdit::editingFinished, this, &QtWidgetsFilter2D::SlotLineEditSpatialEdgePreserving_Changed);
	connect(ui.lineEdit_sepSigmaR, &QLineEdit::editingFinished, this, &QtWidgetsFilter2D::SlotLineEditSpatialEdgePreserving_Changed);
	connect(ui.lineEdit_sepIterations, &QLineEdit::editingFinished, this, &QtWidgetsFilter2D::SlotLineEditSpatialEdgePreserving_Changed);
	connect(ui.groupBox_heatEquation, &QGroupBox::clicked, this, &QtWidgetsFilter2D::SlotCheckboxHeatEquation_Clicked);
	connect(ui.horizontalSlider_alpha, &QSlider::valueChanged, this, &QtWidgetsFilter2D::SlotSliderAlpha_Changed);
	connect(ui.horizontalSlider_iteration, &QSlider::valueChanged, this, &QtWidgetsFilter2D::SlotSliderIteration_Changed);
	connect(ui.lineEdit_alpha, &QLineEdit::editingFinished, this, &QtWidgetsFilter2D::SlotCheckboxHeatEquation_Clicked);
	connect(ui.lineEdit_iteration, &QLineEdit::editingFinished, this, &QtWidgetsFilter2D::SlotCheckboxHeatEquation_Clicked);
	connect(ui.groupBox_AnisotropicDiffusion, &QGroupBox::clicked, this, &QtWidgetsFilter2D::SlotCheckboxAnisotropicDiffusion_Clicked);
	connect(ui.comboBox_AnisotropicDiffusionMode, &QComboBox::currentTextChanged, this, &QtWidgetsFilter2D::SlotCheckboxAnisotropicDiffusion_Clicked);
	connect(ui.horizontalSlider_k, &QSlider::valueChanged, this, &QtWidgetsFilter2D::SlotSliderK_Changed);
	connect(ui.horizontalSlider_iteration_AD, &QSlider::valueChanged, this, &QtWidgetsFilter2D::SlotSliderIterationAD_Changed);
	connect(ui.lineEdit_k, &QLineEdit::editingFinished, this, &QtWidgetsFilter2D::SlotCheckboxAnisotropicDiffusion_Clicked);
	connect(ui.lineEdit_iteration_AD, &QLineEdit::editingFinished, this, &QtWidgetsFilter2D::SlotCheckboxAnisotropicDiffusion_Clicked);

	connect(ui.pushButton_save, &QPushButton::clicked, this, &QtWidgetsFilter2D::SlotButtonSave_Clicked);
	connect(ui.pushButton_calibPath, &QPushButton::clicked, this, &QtWidgetsFilter2D::SlotButtonCalibLoad_Clicked);
	connect(ui.pushButton_generate3D, &QPushButton::clicked, this, &QtWidgetsFilter2D::SlotButtonGenerate3D_Clicked);


	// Load Settings
	imgName = filter2DSettings->value("ImagePath").toString();
	ui.lineEdit_imagePath->setText(imgName);

	LoadImageToCV(cvImage, imgName.toStdString());

	cvImage_Display = cvImage.clone();
	UpdateImageFromCV(cvImage_Display, scene);
	SlotButtonFitZoom_Clicked();

	int medianKernel = filter2DSettings->value("Median2DKernel").toInt();
	ui.lineEdit_medianKernel->setText(QString::number(medianKernel));

	int sepSigmaS = filter2DSettings->value("SEPSigmaS").toInt();
	ui.lineEdit_sepSigmaS->setText(QString::number(sepSigmaS));

	float sepSigmaR = filter2DSettings->value("SEPSigmaR").toFloat();
	ui.lineEdit_sepSigmaR->setText(QString::number(sepSigmaR));

	int sepIterations = filter2DSettings->value("SEPIterations").toInt();
	ui.lineEdit_sepIterations->setText(QString::number(sepIterations));

	float heatAlpha = filter2DSettings->value("Alpha").toFloat();
	ui.lineEdit_alpha->setText(QString::number(heatAlpha, 'f', 2));

	int heatIteration = filter2DSettings->value("Iteration").toInt();
	ui.lineEdit_iteration->setText(QString::number(heatIteration));

	int saveExt = filter2DSettings->value("SaveExt").toInt();
	ui.comboBox_extension->setCurrentIndex(saveExt);

	calibName = filter2DSettings->value("CalibPath").toString();
	ui.lineEdit_calibPath->setText(calibName);
}

void QtWidgetsFilter2D::SlotButtonImageLoad_Clicked()
{
	QString currPath = QDir::currentPath();
	if (!imgName.isEmpty()) {
		QFileInfo info(imgName);
		currPath = info.absolutePath();
	}

	imgName = QFileDialog::getOpenFileName(this, "Select Image", currPath, "All Files (*.*);;Img Files (*.png, *.PNG, *.jpg, *.JPG)");
	ui.lineEdit_imagePath->setText(imgName);

	LoadImageToCV(cvImage, imgName.toStdString());

	cvImage_Display = cvImage.clone();
	UpdateImageFromCV(cvImage_Display, scene);
	SlotButtonFitZoom_Clicked();

	filter2DSettings->setValue("ImagePath", imgName);
}

// Fit Zoom
void QtWidgetsFilter2D::SlotButtonFitZoom_Clicked()
{
	QTimer::singleShot(0, this, [this]() {
		ui.graphicsView_image->resetTransform();
		ui.graphicsView_image->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
	});
}

// Retrieve
void QtWidgetsFilter2D::SlotButtonRetrieve_Clicked()
{
	if (cvImage.empty())
	{
		return;
	}

	cvImage_Display = cvImage.clone();
	UpdateImageFromCV(cvImage_Display, scene);

	// Reset all filters
	ui.checkBox_Nothing->setChecked(true);
	ui.groupBox_median->setChecked(false);
	ui.groupBox_SEP->setChecked(false);
	ui.groupBox_heatEquation->setChecked(false);
}


// Extra
void QtWidgetsFilter2D::SlotCheckBoxNothing_Clicked()
{
	if (ui.checkBox_Nothing->isChecked())
	{
		cvImage_Display = cvImage.clone();
		UpdateImageFromCV(cvImage_Display, scene);
	}
}

void QtWidgetsFilter2D::SlotCheckBoxPartialDiffX_Clicked()
{
	if (ui.checkBox_PartialDiffX->isChecked())
	{
		cvImage_Display = cvImage.clone();
		Diff_Partial_X(cvImage_Display, cvImage_Display, DIFF_PARTIAL_MODE(ui.comboBox_PartialMode->currentIndex()), DIFF_DIRECTION(ui.comboBox_DiffDirection->currentIndex()));
		UpdateImageFromCV(cvImage_Display, scene);
	}
	else
	{
		cvImage_Display = cvImage.clone();
		UpdateImageFromCV(cvImage, scene);
	}
}

void QtWidgetsFilter2D::SlotCheckBoxPartialDiffY_Clicked()
{
	if (ui.checkBox_PartialDiffY->isChecked())
	{
		cvImage_Display = cvImage.clone();
		Diff_Partial_Y(cvImage_Display, cvImage_Display, DIFF_PARTIAL_MODE(ui.comboBox_PartialMode->currentIndex()), DIFF_DIRECTION(ui.comboBox_DiffDirection->currentIndex()));
		UpdateImageFromCV(cvImage_Display, scene);
	}
	else
	{
		cvImage_Display = cvImage.clone();
		UpdateImageFromCV(cvImage, scene);
	}
}

void QtWidgetsFilter2D::SlotComboBoxPartialDiffMode_Changed()
{
	if (ui.checkBox_PartialDiffX->isChecked())
	{
		SlotCheckBoxPartialDiffX_Clicked();
	}
	else if (ui.checkBox_PartialDiffY->isChecked())
	{
		SlotCheckBoxPartialDiffY_Clicked();
	}
}

void QtWidgetsFilter2D::SlotComboBoxDiffDirection_Changed()
{
	if (ui.checkBox_PartialDiffX->isChecked())
	{
		SlotCheckBoxPartialDiffX_Clicked();
	}
	else if (ui.checkBox_PartialDiffY->isChecked())
	{
		SlotCheckBoxPartialDiffY_Clicked();
	}
}

void QtWidgetsFilter2D::SlotCheckBoxLaplacian_Clicked()
{
	if (ui.checkBox_Laplacian->isChecked())
	{
		cvImage_Display = cvImage.clone();
		Laplacian(cvImage_Display, cvImage_Display, LAPLACIAN_MODE(ui.comboBox_LaplacianMode->currentIndex()));
		UpdateImageFromCV(cvImage_Display, scene);
	}
	else
	{
		cvImage_Display = cvImage.clone();
		UpdateImageFromCV(cvImage, scene);
	}
}

void QtWidgetsFilter2D::SlotComboBoxLaplacianMode_Changed()
{
	if (ui.checkBox_Laplacian->isChecked())
	{
		SlotCheckBoxLaplacian_Clicked();
	}
}

void QtWidgetsFilter2D::SlotCheckBoxGrayscale_Clicked()
{
	if (ui.checkBox_Grayscale->isChecked())
	{
		cvImage_Display = cvImage.clone();
		Color2Grayscale(cvImage_Display, cvImage_Display);
		UpdateImageFromCV(cvImage_Display, scene);
	}
	else
	{
		cvImage_Display = cvImage.clone();
		UpdateImageFromCV(cvImage, scene);
	}
}

void QtWidgetsFilter2D::SlotCheckBoxSaltAndPepper_Clicked()
{
	if (ui.checkBox_SaltAndPepper->isChecked())
	{
		cvImage_Display = cvImage.clone();
		SaltAndPepper(cvImage_Display, cvImage_Display, ui.spinBox_SaltAndPepper->value());
		UpdateImageFromCV(cvImage_Display, scene);
	}
	else
	{
		cvImage_Display = cvImage.clone();
		UpdateImageFromCV(cvImage, scene);
	}
}


// Median Filter
void QtWidgetsFilter2D::SlotCheckboxMedian_Clicked()
{
	if (ui.groupBox_median->isChecked())
	{
		cvImage_Display = cvImage.clone();
		MedianFilter2D(cvImage, cvImage_Display, ui.lineEdit_medianKernel->text().toInt(), ui.lineEdit_medianKernel->text().toInt(), MODE_MEDIAN2D::CUSTOM_CUDA);
		UpdateImageFromCV(cvImage_Display, scene);
	}
	else
	{
		cvImage_Display = cvImage.clone();
		UpdateImageFromCV(cvImage, scene);
	}
}

void QtWidgetsFilter2D::SlotLineEditMedian_Changed()
{
	QtWidgetsFilter2D::SlotCheckboxMedian_Clicked();
	filter2DSettings->setValue("Median2DKernel", ui.lineEdit_medianKernel->text().toInt());
}


// Spatial Edge Preserving
void QtWidgetsFilter2D::SlotCheckboxSpatialEdgePreserving_Clicked()
{
	if (ui.groupBox_SEP->isChecked())
	{
		cvImage_Display = cvImage.clone();
		SpatialEdgePreservingFilter(cvImage, cvImage_Display, ui.lineEdit_sepSigmaS->text().toInt(), ui.lineEdit_sepSigmaR->text().toFloat(), ui.lineEdit_sepIterations->text().toInt());
		UpdateImageFromCV(cvImage_Display, scene);
	}
	else
	{
		cvImage_Display = cvImage.clone();
		UpdateImageFromCV(cvImage, scene);
	}
}

void QtWidgetsFilter2D::SlotLineEditSpatialEdgePreserving_Changed()
{
	QtWidgetsFilter2D::SlotCheckboxSpatialEdgePreserving_Clicked();
	filter2DSettings->setValue("SEPSigmaS", ui.lineEdit_sepSigmaS->text().toInt());
	filter2DSettings->setValue("SEPSigmaR", ui.lineEdit_sepSigmaR->text().toFloat());
	filter2DSettings->setValue("SEPIterations", ui.lineEdit_sepIterations->text().toInt());
}


// Heat Equation
void QtWidgetsFilter2D::SlotCheckboxHeatEquation_Clicked()
{
	if (ui.groupBox_heatEquation->isChecked())
	{
		cvImage_Display = cvImage.clone();
		HeatEquationFilter(cvImage, cvImage_Display, ui.lineEdit_alpha->text().toFloat(), ui.lineEdit_iteration->text().toInt());
		UpdateImageFromCV(cvImage_Display, scene);
	}
	else
	{
		cvImage_Display = cvImage.clone();
		UpdateImageFromCV(cvImage, scene);
	}
}

void QtWidgetsFilter2D::SlotSliderAlpha_Changed()
{
	ui.lineEdit_alpha->setText(QString::number(ui.horizontalSlider_alpha->value() / 100.0, 'f', 2));
	filter2DSettings->setValue("Alpha", ui.lineEdit_alpha->text().toFloat());
	SlotCheckboxHeatEquation_Clicked();
}

void QtWidgetsFilter2D::SlotSliderIteration_Changed()
{
	ui.lineEdit_iteration->setText(QString::number(ui.horizontalSlider_iteration->value()));
	filter2DSettings->setValue("Iteration", ui.lineEdit_iteration->text().toInt());
	SlotCheckboxHeatEquation_Clicked();
}


// Anisotropic Diffusion
void QtWidgetsFilter2D::SlotCheckboxAnisotropicDiffusion_Clicked()
{
	if (ui.groupBox_AnisotropicDiffusion->isChecked())
	{
		cvImage_Display = cvImage.clone();
		AnisotropicDiffusion(cvImage, cvImage_Display, ui.lineEdit_k->text().toFloat(), ui.lineEdit_iteration_AD->text().toInt(), ANISOTROPIC_DIFFUSION_MODE(ui.comboBox_AnisotropicDiffusionMode->currentIndex()));
		UpdateImageFromCV(cvImage_Display, scene);
	}
	else
	{
		cvImage_Display = cvImage.clone();
		UpdateImageFromCV(cvImage, scene);
	}
}

void QtWidgetsFilter2D::SlotSliderK_Changed()
{
	ui.lineEdit_k->setText(QString::number(ui.horizontalSlider_k->value() / 100.0, 'f', 2));
	filter2DSettings->setValue("K", ui.lineEdit_k->text().toFloat());
	SlotCheckboxAnisotropicDiffusion_Clicked();
}

void QtWidgetsFilter2D::SlotSliderIterationAD_Changed()
{
	ui.lineEdit_iteration_AD->setText(QString::number(ui.horizontalSlider_iteration_AD->value()));
	filter2DSettings->setValue("Iteration_AD", ui.lineEdit_iteration_AD->text().toInt());
	SlotCheckboxAnisotropicDiffusion_Clicked();
}


// Save Image
void QtWidgetsFilter2D::SlotButtonSave_Clicked()
{
	filter2DSettings->setValue("SaveExt", ui.comboBox_extension->currentIndex());
	bool ret = SaveImage(cvImage_Display, ".", ui.comboBox_extension->currentText().toStdString());

	if (ret)
	{
		QMessageBox::information(this, "Information", "Image saved successfully.");
	}
	else
	{
		QMessageBox::warning(this, "Warning", "Image save failed.");
	}
}


// Generate 3D
void QtWidgetsFilter2D::SlotButtonCalibLoad_Clicked()
{
	calibName = QFileDialog::getOpenFileName(this, "Select Calibration File", QDir::currentPath(), "All Files (*.*);;Calibration Files (*.yml)");
	ui.lineEdit_calibPath->setText(calibName);

	filter2DSettings->setValue("CalibPath", calibName);
}

void QtWidgetsFilter2D::SlotButtonGenerate3D_Clicked()
{
	// Load Calibration Data
	Reconstruction::CalibrationParams calibParams;
	LoadCalibData(calibName.toStdString(), calibParams);

	// Generate 3D Point Cloud
	cv::Mat pointCloud;
	DepthToPoint(cvImage_Display, pointCloud, calibParams.cameraMatrixL);

	// Save Point Cloud
	SavePointCloud(pointCloud, ".", ".ply");
}