#pragma once

#include <QGraphicsView>
#include <QtWidgets>
#include <QWidget>

class QGraphicsView_wheelZoom : public QGraphicsView
{
public:
	QGraphicsView_wheelZoom(QWidget* parent = 0);

protected:
	virtual void wheelEvent(QWheelEvent* event);
};