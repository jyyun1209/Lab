#pragma once

#include <QGraphicsView>
#include <QtWidgets>
#include <QWidget>

class QGraphicsView_Custom : public QGraphicsView
{
public:
	QGraphicsView_Custom(QWidget* parent = 0);
	QImage originalImage;

protected:
	virtual void wheelEvent(QWheelEvent* event);

	QList<QGraphicsTextItem*> overlayTexts;
	void paintEvent(QPaintEvent* event) override;
};