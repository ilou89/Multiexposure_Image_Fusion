#include "zoom_graphics_view.h"

ZoomGraphicsView::ZoomGraphicsView(QWidget *parent)
{
    this->QGraphicsView::setParent(parent);
    this->setDragMode(ScrollHandDrag);
    this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    this->setCursor(Qt::ArrowCursor);
    viewport()->setCursor(Qt::ArrowCursor);
    scale         = 1.0f;
    ctr_pressed   = false;
    minus_pressed = false;
    plus_pressed  = false;
    scale_100     = false;
}

float ZoomGraphicsView::getScale()
{
    return scale;
}

void ZoomGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    viewport()->setCursor(Qt::ClosedHandCursor);
}

void ZoomGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
    viewport()->setCursor(Qt::ArrowCursor);
}

void ZoomGraphicsView::setScale(float val)
{
    scale = val;
    QMatrix matrix;
    matrix.reset();
    matrix.scale(static_cast<qreal>(scale), static_cast<qreal>(scale));
    this->setMatrix(matrix, false);

//    MainWindow *main_window = MainWindow::get_instance();
//    main_window->updateStatusBar();
}

void ZoomGraphicsView::wheelEvent(QWheelEvent *event)
{
    if ( event->modifiers() & Qt::ControlModifier ) {
        event->accept();
        if ( event->delta() > 0 ) {
            if ( scale < 16.f ) {
                scale += 0.1f;
            }
        } else {
            if ( scale > 0.1f ) {
                scale -= 0.1f;
            }
        }

        //correction due to floating-point precission
        if ( scale < 0.1f ) {
            scale = 0.1f;
        } else if ( scale > 16.f ) {
            scale = 16.f;
        }

        setScale(scale);
    }
}

void ZoomGraphicsView::keyPressEvent(QKeyEvent *event)
{
    minus_pressed = false;
    plus_pressed  = false;
    scale_100     = false;
    if ( event->key() == Qt::Key_Control ) {
        ctr_pressed = true;
    } else if ( event->key() == Qt::Key_Minus ) {
        minus_pressed = true;
    } else if ( (event->key() == Qt::Key_Plus) || (event->key() == Qt::Key_Equal) ) {
        plus_pressed = true;
    }else if ( event->key() == Qt::Key_0 ) {
        scale_100 = true;
    }

    if ( (ctr_pressed&minus_pressed) == true ) {
        if ( scale > 0.1f ) {
            scale -= 0.1f;
        }

        if ( scale < 0.1f ) {
            scale = 0.1f;
        }
    } else if ( (ctr_pressed&plus_pressed) == true ) {
        if ( scale < 16.f ) {
            scale += 0.1f;
        }

        if ( scale > 16.f ) {
            scale = 16.f;
        }
    } else if ( (ctr_pressed&scale_100) == true ) {
        scale = 1.f;
    }
    setScale(scale);
}

void ZoomGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_Control ) {
        ctr_pressed = false;
    }else if ( event->key() == Qt::Key_Minus ) {
        minus_pressed = false;
    }else if ( (event->key() == Qt::Key_Plus) || (event->key() == Qt::Key_Equal) ) {
        plus_pressed = false;
    }
}
