#pragma once
#include <QtWidgets>

#define EPSILON 0.000000001;

struct Edge
{
	QPoint startPoint;
	QPoint endPoint;
	int deltaY;
	double x;
	double w;

	bool operator>(const Edge& edge) // courtesy of Alex Filip
	{
		return startPoint.y() > edge.startPoint.y();
	}
};

class ViewerWidget :public QWidget {
	Q_OBJECT
private:
	QString name = "";
	QSize areaSize = QSize(0, 0);
	QImage* img = nullptr;
	QRgb* data = nullptr;
	QPainter* painter = nullptr;

	// cv5 stuff
	void swapPoints(QPoint& point1, QPoint& point2);
	void printEdges(QVector<Edge> polygonEdges);

	void bubbleSortEdgesY(QVector<Edge>& polygonEdges);
	void bubbleSortEdgesX(QList<Edge>& polygonEdges);
	void setEdgesOfPolygon(QVector<QPoint> polygonPoints, QVector<Edge>& polygonEdges);

	// kreslenie
	void drawBresenhamChosenX(QPoint point1, QPoint point2, QColor color);
	void drawBresenhamChosenY(QPoint point1, QPoint point2, QColor color);
	
	void drawGeometry(QVector<QPoint> geometryPoints, QColor penColor, QColor fillColor, int algorithm);
	void trimLine(QVector<QPoint> currentLine, QColor color, int algorithm);
	void trimPolygon(QVector<QPoint> V, QColor penColor, QColor fillColor, int algorithm);
	void fillPolygonScanLineAlgorithm(QVector<QPoint> polygonPoints, QColor fillColor);

public:
	ViewerWidget(QString viewerName, QSize imgSize, QWidget* parent = Q_NULLPTR);
	~ViewerWidget();
	void resizeWidget(QSize size);

	// funkcie na kreslenie
	void drawLineDDA(QPoint point1, QPoint point2, QColor color);
	void drawLineBresenham(QPoint point1, QPoint point2, QColor color);
	void createLineWithAlgorithm(QPoint point1, QPoint point2, QColor color, int algorithm);
	void drawCircumference(QPoint point1, QPoint point2, QColor color);

	void trimGeometry(QVector<QPoint>& geometryPoints, QColor color, QColor fillColor, int algorithm);

	//Image functions
	bool setImage(const QImage& inputImg);
	QImage* getImage() { return img; };
	bool isEmpty();

	//Data functions
	QRgb* getData() { return data; }
	void setPixel(int x, int y, const QColor& color);
	void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
	bool isInside(int x, int y) { return (x >= 0 && y >= 0 && x < img->width() && y < img->height()) ? true : false; }

	//Get/Set functions
	QString getName() { return name; }
	void setName(QString newName) { name = newName; }

	void setPainter() { painter = new QPainter(img); }
	void setDataPtr() { data = reinterpret_cast<QRgb*>(img->bits()); }

	int getImgWidth() { return img->width(); };
	int getImgHeight() { return img->height(); };

	void clear(QColor color = Qt::white);

public slots:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};