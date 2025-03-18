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

	// Load Settings
	imgName = filter2DSettings->value("ImagePath").toString();
	ui.lineEdit_imagePath->setText(imgName);

	qImage = QImage(imgName);
	scene->addPixmap(QPixmap::fromImage(qImage));
	ui.graphicsView_image->setScene(scene);
	ui.graphicsView_image->show();
}

void QtWidgetsFilter2D::SlotButtonImageLoad_Clicked()
{
	imgName = QFileDialog::getOpenFileName(this, "Select Image", QDir::currentPath(), "Img Files (*.png, *.jpg)");
	ui.lineEdit_imagePath->setText(imgName);

	qImage = QImage(imgName);

	scene->clear();
	scene->addPixmap(QPixmap::fromImage(qImage));
	ui.graphicsView_image->setScene(scene);
	ui.graphicsView_image->show();

	filter2DSettings->setValue("ImagePath", imgName);
}