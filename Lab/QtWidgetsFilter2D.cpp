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

	scene->clear();
	scene->addPixmap(QPixmap::fromImage(qImage));
	ui.graphicsView_image->setScene(scene);
	ui.graphicsView_image->show();
}

void QtWidgetsFilter2D::InitializeUI()
{
	// Connect Signal <-> Slot
	connect(ui.pushButton_imagePath, &QPushButton::clicked, this, &QtWidgetsFilter2D::SlotButtonImageLoad_Clicked);
	connect(ui.groupBox_median, &QGroupBox::clicked, this, &QtWidgetsFilter2D::SlotCheckboxMedian_Clicked);
	connect(ui.lineEdit_medianKernel, &QLineEdit::editingFinished, this, &QtWidgetsFilter2D::SlotLineEditMedian_Changed);
	connect(ui.groupBox_SEP, &QGroupBox::clicked, this, &QtWidgetsFilter2D::SlotCheckboxSpatialEdgePreserving_Clicked);
	connect(ui.lineEdit_sepSigmaS, &QLineEdit::editingFinished, this, &QtWidgetsFilter2D::SlotLineEditSpatialEdgePreserving_Changed);
	connect(ui.lineEdit_sepSigmaR, &QLineEdit::editingFinished, this, &QtWidgetsFilter2D::SlotLineEditSpatialEdgePreserving_Changed);
	connect(ui.lineEdit_sepIterations, &QLineEdit::editingFinished, this, &QtWidgetsFilter2D::SlotLineEditSpatialEdgePreserving_Changed);
	connect(ui.pushButton_save, &QPushButton::clicked, this, &QtWidgetsFilter2D::SlotButtonSave_Clicked);

	// Load Settings
	imgName = filter2DSettings->value("ImagePath").toString();
	ui.lineEdit_imagePath->setText(imgName);

	LoadImageToCV(cvImage, imgName.toStdString());
	UpdateImageFromCV(cvImage, scene);

	int medianKernel = filter2DSettings->value("Median2DKernel").toInt();
	ui.lineEdit_medianKernel->setText(QString::number(medianKernel));

	int sepSigmaS = filter2DSettings->value("SEPSigmaS").toInt();
	ui.lineEdit_sepSigmaS->setText(QString::number(sepSigmaS));

	float sepSigmaR = filter2DSettings->value("SEPSigmaR").toFloat();
	ui.lineEdit_sepSigmaR->setText(QString::number(sepSigmaR));

	int sepIterations = filter2DSettings->value("SEPIterations").toInt();
	ui.lineEdit_sepIterations->setText(QString::number(sepIterations));

	int saveExt = filter2DSettings->value("SaveExt").toInt();
	ui.comboBox_extension->setCurrentIndex(saveExt);
}

void QtWidgetsFilter2D::SlotButtonImageLoad_Clicked()
{
	imgName = QFileDialog::getOpenFileName(this, "Select Image", QDir::currentPath(), "All Files (*.*);;Img Files (*.png, *.PNG, *.jpg, *.JPG)");
	ui.lineEdit_imagePath->setText(imgName);

	LoadImageToCV(cvImage, imgName.toStdString());
	UpdateImageFromCV(cvImage, scene);

	filter2DSettings->setValue("ImagePath", imgName);
}


// Median Filter
void QtWidgetsFilter2D::SlotCheckboxMedian_Clicked()
{
	if (ui.groupBox_median->isChecked())
	{
		cv::Mat cvImageFiltered = cvImage.clone();
		MedianFilter2D(cvImage, cvImageFiltered, ui.lineEdit_medianKernel->text().toInt(), ui.lineEdit_medianKernel->text().toInt(), MODE_MEDIAN2D::CUSTOM_CUDA);
		UpdateImageFromCV(cvImageFiltered, scene);
	}
	else
	{
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
		cv::Mat cvImageFiltered = cvImage.clone();
		SpatialEdgePreservingFilter(cvImage, cvImageFiltered, ui.lineEdit_sepSigmaS->text().toInt(), ui.lineEdit_sepSigmaR->text().toFloat(), ui.lineEdit_sepIterations->text().toInt());
		UpdateImageFromCV(cvImage, scene);
	}
	else
	{
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


// Save Image
void QtWidgetsFilter2D::SlotButtonSave_Clicked()
{
	filter2DSettings->setValue("SaveExt", ui.comboBox_extension->currentIndex());
	SaveImage(cvImage, ".", ui.comboBox_extension->currentText().toStdString());
}