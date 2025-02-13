#ifndef ZOOM_GRAPHICS_VIEW_H
#define ZOOM_GRAPHICS_VIEW_H
#include "QGraphicsView"
#include "QtWidgets"
#include"QTransform"


class ZoomGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ZoomGraphicsView(QWidget *parent = nullptr);
    void  setScale(float val);
    float getScale();

private:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);
    float scale;
    bool ctr_pressed;
    bool minus_pressed;
    bool plus_pressed;
    bool scale_100;
};

#endif // ZOOM_GRAPHICS_VIEW_H
