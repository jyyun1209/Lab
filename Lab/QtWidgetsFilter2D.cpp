#include "QtWidgetsFilter2D.h"

QtWidgetsFilter2D::QtWidgetsFilter2D(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	InitializeUI();
}

QtWidgetsFilter2D::~QtWidgetsFilter2D()
{

}

void QtWidgetsFilter2D::InitializeUI()
{
	connect(ui.pushButton_imagePath, &QPushButton::clicked, this, &QtWidgetsFilter2D::SlotButtonImageLoad_Clicked);
}

void QtWidgetsFilter2D::SlotButtonImageLoad_Clicked()
{
	QString imgName = QFileDialog::getOpenFileName(this, "Select Image", QDir::currentPath(), "Img Files (*.png, *.jpg)");
	ui.lineEdit_imagePath->setText(imgName);

	QImage qImage = QImage(imgName);
	ui.label->setPixmap(QPixmap::fromImage(qImage));
	ui.label->setScaledContents(true);
	ui.label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
}