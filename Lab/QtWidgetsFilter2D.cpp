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

void QtWidgetsFilter2D::InitializeUI()
{
	// Connect Signal <-> Slot
	connect(ui.pushButton_imagePath, &QPushButton::clicked, this, &QtWidgetsFilter2D::SlotButtonImageLoad_Clicked);
	connect(ui.groupBox_median, &QGroupBox::clicked, this, &QtWidgetsFilter2D::SlotCheckboxMedian_Clicked);
	connect(ui.lineEdit_medianKernel, &QLineEdit::textChanged, this, &QtWidgetsFilter2D::SlotLineEditMedian_Changed);
	connect(ui.groupBox_SEP, &QGroupBox::clicked, this, &QtWidgetsFilter2D::SlotCheckboxSpatialEdgePreserving_Clicked);
	connect(ui.lineEdit_sepKernel, &QLineEdit::textChanged, this, &QtWidgetsFilter2D::SlotLineEditSpatialEdgePreserving_Changed);
	connect(ui.pushButton_save, &QPushButton::clicked, this, &QtWidgetsFilter2D::SlotButtonSave_Clicked);

	// Load Settings
	imgName = filter2DSettings->value("ImagePath").toString();
	ui.lineEdit_imagePath->setText(imgName);
	qImage = QImage(imgName);
	scene->addPixmap(QPixmap::fromImage(qImage));
	ui.graphicsView_image->setScene(scene);
	ui.graphicsView_image->show();

	int medianKernel = filter2DSettings->value("Median2DKernel").toInt();
	ui.lineEdit_medianKernel->setText(QString::number(medianKernel));

	int sepKernel = filter2DSettings->value("SEPKernel").toInt();
	ui.lineEdit_sepKernel->setText(QString::number(sepKernel));
}

void QtWidgetsFilter2D::SlotButtonImageLoad_Clicked()
{
	imgName = QFileDialog::getOpenFileName(this, "Select Image", QDir::currentPath(), "All Files (*.*);;Img Files (*.png, *.PNG, *.jpg, *.JPG)");
	ui.lineEdit_imagePath->setText(imgName);

	qImage = QImage(imgName);

	scene->clear();
	scene->addPixmap(QPixmap::fromImage(qImage));
	ui.graphicsView_image->setScene(scene);
	ui.graphicsView_image->show();

	filter2DSettings->setValue("ImagePath", imgName);
}


// Median Filter
void QtWidgetsFilter2D::SlotCheckboxMedian_Clicked()
{
	if (ui.groupBox_median->isChecked())
	{
		//cv::Mat cvImage = cv::imread(imgName.toStdString(), cv::IMREAD_UNCHANGED);
		cvImage = cv::imread(imgName.toStdString(), cv::IMREAD_GRAYSCALE);
		cvImage.convertTo(cvImage, CV_32F);
		MedianFilter2D(cvImage, cvImage, ui.lineEdit_medianKernel->text().toInt(), ui.lineEdit_medianKernel->text().toInt(), MODE_MEDIAN2D::CUSTOM_CPP);
		cvImage.convertTo(cvImage, CV_8UC1);
		//cv::cvtColor(cvImage, cvImage, cv::COLOR_BGR2RGB);

		scene->clear();
		//scene->addPixmap(QPixmap::fromImage(QImage(cvImage.data, cvImage.cols, cvImage.rows, cvImage.step, QImage::Format_RGB888)));
		scene->addPixmap(QPixmap::fromImage(QImage(cvImage.data, cvImage.cols, cvImage.rows, cvImage.step, QImage::Format_Grayscale8)));
		ui.graphicsView_image->setScene(scene);
		ui.graphicsView_image->show();
	}
	else
	{
		scene->clear();
		scene->addPixmap(QPixmap::fromImage(qImage));
		ui.graphicsView_image->setScene(scene);
		ui.graphicsView_image->show();
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
		//cv::Mat cvImage = cv::imread(imgName.toStdString(), cv::IMREAD_UNCHANGED);
		cvImage = cv::imread(imgName.toStdString(), cv::IMREAD_GRAYSCALE);
		cvImage.convertTo(cvImage, CV_32F);
		SpatialEdgePreservingFilter(cvImage, cvImage, 3, 0.1, ui.lineEdit_sepKernel->text().toInt());
		cvImage.convertTo(cvImage, CV_8UC1);
		//cv::cvtColor(cvImage, cvImage, cv::COLOR_BGR2RGB);

		scene->clear();
		//scene->addPixmap(QPixmap::fromImage(QImage(cvImage.data, cvImage.cols, cvImage.rows, cvImage.step, QImage::Format_RGB888)));
		scene->addPixmap(QPixmap::fromImage(QImage(cvImage.data, cvImage.cols, cvImage.rows, cvImage.step, QImage::Format_Grayscale8)));
		ui.graphicsView_image->setScene(scene);
		ui.graphicsView_image->show();
	}
	else
	{
		scene->clear();
		scene->addPixmap(QPixmap::fromImage(qImage));
		ui.graphicsView_image->setScene(scene);
		ui.graphicsView_image->show();
	}
}

void QtWidgetsFilter2D::SlotLineEditSpatialEdgePreserving_Changed()
{
	QtWidgetsFilter2D::SlotCheckboxSpatialEdgePreserving_Clicked();
	filter2DSettings->setValue("SEPKernel", ui.lineEdit_sepKernel->text().toInt());
}


// Save Image
void QtWidgetsFilter2D::SlotButtonSave_Clicked()
{
	saveImage(cvImage, "Output.png");
}