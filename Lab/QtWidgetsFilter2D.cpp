#include "QtWidgetsFilter2D.h"

QtWidgetsFilter2D::QtWidgetsFilter2D(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	InitializeUI();
}

QtWidgetsFilter2D::~QtWidgetsFilter2D()
{}

void QtWidgetsFilter2D::InitializeUI()
{
	connect(ui.pushButton_imagePath, &QPushButton::clicked, this, &QtWidgetsFilter2D::SlotButtonImageLoad_Clicked);
}

void QtWidgetsFilter2D::SlotButtonImageLoad_Clicked()
{
	ui.lineEdit_imagePath->setText(QString("Hi Junyoung"));
}