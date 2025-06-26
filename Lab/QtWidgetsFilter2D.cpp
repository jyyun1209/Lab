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
}

void QtWidgetsFilter2D::InitializeUI()
{
	// Connect Signal <-> Slot
	connect(ui.pushButton_imagePath, &QPushButton::clicked, this, &QtWidgetsFilter2D::SlotButtonImageLoad_Clicked);
	connect(ui.pushButton_fitZoom, &QPushButton::clicked, this, &QtWidgetsFilter2D::SlotButtonFitZoom_Clicked);
	connect(ui.pushButton_Retrieve, &QPushButton::clicked, this, &QtWidgetsFilter2D::SlotButtonRetrieve_Clicked);

	connect(ui.checkBox_Nothing, &QCheckBox::clicked, this, &QtWidgetsFilter2D::SlotCheckboxNothing_Clicked);
	connect(ui.checkBox_PartialDiffX, &QCheckBox::clicked, this, &QtWidgetsFilter2D::SlotCheckboxPartialDiffX_Clicked);
	connect(ui.checkBox_PartialDiffY, &QCheckBox::clicked, this, &QtWidgetsFilter2D::SlotCheckBoxPartialDiffY_Clicked);

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
void QtWidgetsFilter2D::SlotCheckboxNothing_Clicked()
{
	if (ui.checkBox_Nothing->isChecked())
	{
		cvImage_Display = cvImage.clone();
		UpdateImageFromCV(cvImage_Display, scene);
	}
}

void QtWidgetsFilter2D::SlotCheckboxPartialDiffX_Clicked()
{
	if (ui.checkBox_PartialDiffX->isChecked())
	{
		cvImage_Display = cvImage.clone();
		Diff_Partial_X(cvImage_Display, cvImage_Display);
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
		Diff_Partial_Y(cvImage_Display, cvImage_Display);
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


// Save Image
void QtWidgetsFilter2D::SlotButtonSave_Clicked()
{
	filter2DSettings->setValue("SaveExt", ui.comboBox_extension->currentIndex());
	SaveImage(cvImage_Display, ".", ui.comboBox_extension->currentText().toStdString());
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