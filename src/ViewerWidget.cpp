#include   "ViewerWidget.h"

void ViewerWidget::swapPoints(QPoint& point1, QPoint& point2)
{
	int temp = point1.x();
	point1.setX(point2.x());
	point2.setX(temp);
	
	temp = point1.y();
	point1.setY(point2.y());
	point2.setY(temp);
}

void ViewerWidget::drawBresenhamChosenX(QPoint point1, QPoint point2, QColor color)
{
	if (point1.x() > point2.x()) // ak sa klikol prvy bod viac vpravo
		swapPoints(point1, point2);

	int deltaX = point2.x() - point1.x();
	int deltaY = point2.y() - point1.y();
	double slope = static_cast<double>(deltaY) / static_cast<double>(deltaX);
	int k1 = 0;
	int k2 = 0;
	int p = 0;
	int x = 0;
	int y = 0;

	if (slope >= 0 && slope < 1)
	{
		k1 = 2 * deltaY; k2 = 2 * (deltaY - deltaX);
		p = 2 * deltaY - deltaX;

		x = point1.x(); y = point1.y();

		setPixel(x, y, color);

		while (x < point2.x())
		{
			x++;
			if (p > 0)
			{
				y++;
				p += k2;
			}
			else
				p += k1;

			setPixel(x, y, color);
		}
	}
	else if (slope > -1 && slope <= 0)
	{
		k1 = 2 * deltaY; k2 = 2 * (deltaY + deltaX);
		p = 2 * deltaY + deltaX;

		x = point1.x(); y = point1.y();

		setPixel(x, y, color);

		while (x < point2.x())
		{
			x++;
			if (p < 0)
			{
				y--;
				p += k2;
			}
			else
				p += k1;

			setPixel(x, y, color);
		}
	}
}

void ViewerWidget::drawBresenhamChosenY(QPoint point1, QPoint point2, QColor color)
{
	if (point1.y() > point2.y()) // ak sa klikol prvy bod nizsie ako druhy bod
		swapPoints(point1, point2);

	int deltaX = point2.x() - point1.x();
	int deltaY = point2.y() - point1.y();

	double slope = static_cast<double>(deltaY) / static_cast<double>(deltaX);
	int k1 = 0;
	int k2 = 0;
	int p = 0;
	int x = 0;
	int y = 0;

	if (slope >= 1)
	{
		k1 = 2 * deltaX; k2 = 2 * (deltaX - deltaY);
		p = 2 * deltaX - deltaY;

		x = point1.x(); y = point1.y();

		setPixel(x, y, color);

		while (y < point2.y())
		{
			y++;

			if (p > 0)
			{
				x++;
				p += k2;
			}
			else
				p += k1;

			setPixel(x, y, color);
		}
	}
	else if (slope <= -1)
	{
		k1 = 2 * deltaX; k2 = 2 * (deltaX + deltaY);
		p = 2 * deltaX + deltaY;

		x = point1.x(); y = point1.y();

		setPixel(x, y, color);

		while (y < point2.y())
		{
			y++;

			if (p < 0)
			{
				x--;
				p += k2;
			}
			else
				p += k1;

			setPixel(x, y, color);
		}
	}
}

ViewerWidget::ViewerWidget(QString viewerName, QSize imgSize, QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_StaticContents);
	setMouseTracking(true);
	name = viewerName;
	if (imgSize != QSize(0, 0)) {
		img = new QImage(imgSize, QImage::Format_ARGB32);
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
	}
}
ViewerWidget::~ViewerWidget()
{
	delete painter;
	delete img;
}
void ViewerWidget::resizeWidget(QSize size)
{
	this->resize(size);
	this->setMinimumSize(size);
	this->setMaximumSize(size);
}

//Image functions
bool ViewerWidget::setImage(const QImage& inputImg)
{
	if (img != nullptr) {
		delete img;
	}
	img = new QImage(inputImg);
	if (!img) {
		return false;
	}
	resizeWidget(img->size());
	setPainter();
	update();

	return true;
}
bool ViewerWidget::isEmpty()
{
	if (img->size() == QSize(0, 0)) {
		return true;
	}
	return false;
}

//Data function
void ViewerWidget::setPixel(int x, int y, const QColor& color)
{
	if (isInside(x, y)) {
		data[x + y * img->width()] = color.rgb();
	}
}
void ViewerWidget::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	if (isInside(x, y)) {
		QColor color(r, g, b);
		setPixel(x, y, color);
	}
}

void ViewerWidget::clear(QColor color)
{
	for (size_t x = 0; x < img->width(); x++)
	{
		for (size_t y = 0; y < img->height(); y++)
		{
			setPixel(x, y, color);
		}
	}
	update();
}

void ViewerWidget::drawLineDDA(QPoint point1, QPoint point2, QColor color)
{
	// trochu prerobeny DDA algoritmus podla: http://www.st.fmph.uniba.sk/~vojtova5/PG/Rast-use-DDA.html
	int deltaX = point2.x() - point1.x();
	int deltaY = point2.y() - point1.y();
	int steps = 0;

	double xIncrement = 0.0, yIncrement = 0.0;
	double x = point1.x(), y = point1.y();

	if (qAbs(deltaX) > qAbs(deltaY))
		steps = qAbs(deltaX);
	else
		steps = qAbs(deltaY);

	xIncrement = deltaX / static_cast<double>(steps);
	yIncrement = deltaY / static_cast<double>(steps);

	setPixel(static_cast<int>(x), static_cast<int>(y), color);

	for (int i = 0; i < steps; i++)
	{
		x += xIncrement;
		y += yIncrement;

		setPixel(static_cast<int>(x), static_cast<int>(y), color);
	}
	
	update();
}
void ViewerWidget::drawLineBresenham(QPoint point1, QPoint point2, QColor color)
{
	int deltaX = point2.x() - point1.x();
	int deltaY = point2.y() - point1.y();
	double slope = static_cast<double>(deltaY) / static_cast<double>(deltaX);

	if ((slope >= 0 && slope < 1) || (slope > -1 && slope <= 0)) // riadiaca os x
		drawBresenhamChosenX(point1, point2, color);
	else if ((slope >= 1) || (slope <= -1)) // riadiaca os y
		drawBresenhamChosenY(point1, point2, color);
	
	update();
}
void ViewerWidget::drawCircumference(QPoint point1, QPoint point2, QColor color)
{
	int deltaX = point2.x() - point1.x();
	int deltaY = point2.y() - point1.y();

	int radius = static_cast<int>(qSqrt(static_cast<double>(deltaX) * deltaX + static_cast<double>(deltaY) * deltaY));
	int p = 1 - radius;
	int x = 0;
	int y = radius;
	int dvaX = 3;
	int dvaY = 2 * radius + 2;
	int Sx = point1.x();
	int Sy = point1.y();

	while (x <= y)
	{
		// pixely hore
		setPixel(Sx - y, Sy + x, color);
		setPixel(Sx - y, Sy - x, color);
		
		// pixely dole
		setPixel(Sx + y, Sy + x, color);
		setPixel(Sx + y, Sy - x, color);

		// pixely vpravo
		setPixel(Sx + x, Sy - y, color);
		setPixel(Sx + x, Sy + y, color);

		// pixely na lavo
		setPixel(Sx - x, Sy - y, color);
		setPixel(Sx - x, Sy + y, color);

		if (p > 0)
		{
			p -= dvaY;
			y--;
			dvaY -= 2;
		}

		p += dvaX;
		dvaX += 2;
		x++;
	}

	update();
}

void ViewerWidget::drawGeometry(QVector<QPoint>& geometryPoints, QColor color, int algorithm)
{
	if (geometryPoints.size() == 2) // usecka
		createLineWithAlgorithm(geometryPoints.at(0), geometryPoints.at(1), color, algorithm);
	else if (geometryPoints.size() > 2) // polygon
	{
		for (int i = 1; i <= geometryPoints.size(); i++)
		{
			if (i == geometryPoints.size())
				createLineWithAlgorithm(geometryPoints.at(0), geometryPoints.at(i - 1), color, algorithm);
			else
				createLineWithAlgorithm(geometryPoints.at(i), geometryPoints.at(i - 1), color, algorithm);
		}
	}
}

void ViewerWidget::createLineWithAlgorithm(QPoint point1, QPoint point2, QColor color, int algorithm)
{
	//qDebug() << "line drawn:" << point1 << point2;
	if (algorithm == 0) // DDA
		drawLineDDA(point1, point2, color);
	else if (algorithm == 1) // mr. Bresenham
		drawLineBresenham(point1, point2, color);
	else
		qDebug() << "Incorrect algorithm";
}

void ViewerWidget::trimLine(QVector<QPoint>& currentLine, QColor color, int algorithm)
{
	int imgHeight = getImgHeight();
	int imgWidth = getImgWidth();
	QPoint E[4]; // pole vrcholov obrazka
	E[0] = QPoint(0, 0); E[3] = QPoint(imgWidth, 0);
	E[1] = QPoint(0, imgHeight); E[2] = QPoint(imgWidth, imgHeight);
	QPoint P1 = currentLine[0], P2 = currentLine[1];
	QPoint newP1(0, 0), newP2(0, 0);
	QVector<QPoint> newLine;
	QPoint vectorD = P2 - P1, vectorW(0, 0), vectorE(0, 0), normalE(0, 0);
	double tL = 0.0, tU = 1.0, t = 0.0;
	int dotProductDN = 0, dotProductWN = 0;

	bool areInside = false;
	bool shouldTrim = false;

	if ((P1.x() >= 0 && P1.x() <= imgWidth && P1.y() >= 0 && P1.y() <= imgHeight) || (P2.x() >= 0 && P2.x() <= imgWidth && P2.y() >= 0 && P2.y() <= imgHeight))
		areInside = true;

	//https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
	if (!areInside) // ci sa usecka nachadza v nejakom rohu tak, ze oba body su uz mimo, ale mala by sa este osekavat
	{
		int intersections = 0;
		double s = 0.0, t = 0.0;
		int denom = 0;
		int upperS = 0;
		int upperT = 0;

		for (int i = 0; i < 4; i++)
		{
			if (i == 3)
				vectorE = E[0] - E[3];
			else
				vectorE = E[i + 1] - E[i];

			denom = -vectorE.x() * vectorD.y() + vectorD.x() * vectorE.y();
			upperS = -vectorD.y() * (P1.x() - E[i].x()) + vectorD.x() * (P1.y() - E[i].y());
			upperT = vectorE.x() * (P1.y() - E[i].y()) - vectorE.y() * (P1.x() - E[i].x());
			s = static_cast<double>(upperS) / denom;
			t = static_cast<double>(upperT) / denom;

			if (s >= 0.0 && s <= 1.0 && t >= 0 && t <= 1.0)
				intersections++;
		}

		if (intersections != 2)
			shouldTrim = false;
		else
			shouldTrim = true;
	}

	if (areInside || shouldTrim)
	{
		qDebug() << "trimming line";

		for (int i = 0; i < 4; i++)
		{
			// direct vector hrany obrazka
			if (i == 3)
				vectorE = E[0] - E[3];
			else
				vectorE = E[i + 1] - E[i];

			// z direct vectora spravime normalu: (x, y) -> (y, -x)
			normalE.setX(vectorE.y()); normalE.setY(-vectorE.x());
			vectorW = P1 - E[i];

			// skalarne suciny
			dotProductDN = QPoint::dotProduct(vectorD, normalE);
			dotProductWN = QPoint::dotProduct(vectorW, normalE);

			if (dotProductDN != 0)
			{
				t = static_cast<double>(-dotProductWN) / dotProductDN;

				if (dotProductDN > 0 && t <= 1.0)
					tL = std::max(t, tL);
				else if (dotProductDN < 0 && t >= 0.0)
					tU = std::min(t, tU);
			}
		}
		qDebug() << "tL:" << tL << "\ttU:" << tU;
		if (tL == 0.0 && tU == 1.0)
			drawGeometry(currentLine, color, algorithm);
		else if (tL < tU)
		{
			newP1.setX(static_cast<int>(P1.x() + ((double)P2.x() - P1.x()) * tL));
			newP1.setY(static_cast<int>(P1.y() + ((double)P2.y() - P1.y()) * tL));

			newP2.setX(static_cast<int>(P1.x() + ((double)P2.x() - P1.x()) * tU));
			newP2.setY(static_cast<int>(P1.y() + ((double)P2.y() - P1.y()) * tU));

			newLine.push_back(newP1); newLine.push_back(newP2);

			drawGeometry(newLine, color, algorithm);
		}
	}
	else
		drawGeometry(currentLine, color, algorithm);
}

void ViewerWidget::trimPolygon(QVector<QPoint>& polygonPoints, QColor color, int algorithm)
{
	QVector<QPoint> V = polygonPoints; // kopia bodov
	QVector<QPoint> W;
	QPoint S(0, 0);
	int xMin[4] = { 0,0, -getImgWidth() + 1, -getImgHeight() + 1 }; // poznamka pre autora: obrazok v poznamkach ku tymto hodnotam; z nejakeho dovodu ak tam nie je +1, to nekresli na spodnu hranu obrazka
	int temp = 0;

	for (int i = 0; i < 4; i++)
	{
		if (V.size() != 0)
			S = V[V.size() - 1];

		for (int j = 0; j < V.size(); j++)
		{
			if (V.at(j).x() >= xMin[i])
			{
				if (S.x() >= xMin[i])
					W.push_back(V[j]);
				else
				{
					temp = static_cast<int>(S.y() + ((double)xMin[i] - S.x()) * ((double)V[j].y() - S.y()) / ((double)V[j].x() - S.x()) + 0.5);
					W.push_back(QPoint(xMin[i], temp)); // priesecnik P
					W.push_back(V[j]);
				}
			}
			else
			{
				if (S.x() >= xMin[i])
				{
					temp = static_cast<int>(S.y() + ((double)xMin[i] - S.x()) * ((double)V[j].y() - S.y()) / ((double)V[j].x() - S.x()) + 0.5);
					W.push_back(QPoint(xMin[i], temp)); // priesecnik P
				}
			}

			S = V.at(j);
		}

		V.clear();

		for (int k = 0; k < W.size(); k++)
			V.push_back(QPoint(W[k].y(), -W[k].x()));

		W.clear();
	}
	drawGeometry(V, color, algorithm);
}

void ViewerWidget::trimGeometry(QVector<QPoint>& geometryPoints, QColor color, int algorithm)
{
	if (geometryPoints.size() == 2)
		trimLine(geometryPoints, color, algorithm);
	else if (geometryPoints.size() > 2)
		trimPolygon(geometryPoints, color, algorithm);
}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}