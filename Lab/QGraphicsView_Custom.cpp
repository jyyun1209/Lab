#include "QGraphicsView_Custom.h"

QGraphicsView_Custom::QGraphicsView_Custom(QWidget* parent) : QGraphicsView(parent)
{

}

void QGraphicsView_Custom::wheelEvent(QWheelEvent* event)
{
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	double scaleFactor = 1.15;
	int delta = 0;

#if QT_VERSION == QT_VERSION_CHECK(5, 14, 0)
{
	delta = event->delta() > 0;
}
#elif QT_VERSION == QT_VERSION_CHECK(5, 15, 0)
{
	delta = event->angleDelta().y();
}
#else
{
	return; // Unsupported Qt version
}
#endif
	
	if (delta > 0)
	{
		scale(scaleFactor, scaleFactor);
	}
	else
	{
		scale(1 / scaleFactor, 1 / scaleFactor);
	}
}

void QGraphicsView_Custom::paintEvent(QPaintEvent* event)
{
	QGraphicsView::paintEvent(event);
	
	QTransform transform = this->transform();
	qreal scaleX = transform.m11();

	for (QGraphicsTextItem* item : overlayTexts) {
		if (scene() && scene()->items().contains(item))
		{
			scene()->removeItem(item);
			delete item;
		}
	}
	overlayTexts.clear();

	if (scaleX > 50)
	{
		QGraphicsPixmapItem* pixmapItem = nullptr;
		QRectF visibleSceneRect = mapToScene(viewport()->rect()).boundingRect();

		int xStart = std::max(0, static_cast<int>(visibleSceneRect.left()));
		int yStart = std::max(0, static_cast<int>(visibleSceneRect.top()));
		int xEnd = std::min(originalImage.width(), static_cast<int>(visibleSceneRect.right())) + 1;
		int yEnd = std::min(originalImage.height(), static_cast<int>(visibleSceneRect.bottom())) + 1;

		for (int y = yStart; y < yEnd; y++)
		{
			for (int x = xStart; x < xEnd; x++)
			{
				QRgb pixelValue = originalImage.pixel(x, y);
				QString text;
				QColor textColor;
				if (originalImage.pixelFormat().channelCount() >= 3)
				{
					text = QString("%1\n%2\n%3")
						.arg(qRed(pixelValue))
						.arg(qGreen(pixelValue))
						.arg(qBlue(pixelValue));

					(qRed(pixelValue) + qGreen(pixelValue) + qBlue(pixelValue)) / 3 < 127 ? textColor = QColor("#BBBBBB") : textColor = QColor("#222222");
				}
				else
				{
					text = QString("%1").arg(qRed(pixelValue));
					qRed(pixelValue) < 127 ? textColor = QColor("#BBBBBB") : textColor = QColor("#222222");
				}

				QGraphicsTextItem* textItem = scene()->addText(text);
				QFont font = textItem->font();

				textItem->setDefaultTextColor(textColor);
				textItem->setScale(1 / scaleX);  // 작게 표시
				font.setBold(true);
				textItem->setFont(font);

				QRectF textRect = textItem->boundingRect();
				qreal scaledWidth = textRect.width() * textItem->scale();
				qreal scaledHeight = textRect.height() * textItem->scale();

				qreal centerX = x + 0.5;
				qreal centerY = y + 0.5;
				textItem->setPos(centerX - scaledWidth / 2.0, centerY - scaledHeight / 2.0);
				textItem->setZValue(1);  // 텍스트가 이미지 위에 표시되도록 설정
				overlayTexts.append(textItem);
			}
		}
	}
}