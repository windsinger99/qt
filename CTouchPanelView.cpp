/*
 * CTouchPanelView.cpp
 */
#include <QApplication>
#include <QMainWindow>
#include <QStatusBar>
#include <QClipboard>
#include <QScrollBar>
#include <QAction>
#include <qmath.h>
#include <QDebug>
#include <QMenu>
#include <QFile>

#include <QMimeData>
#include "CTouchPanelView.h"

CTouchPanelView::CTouchPanelView(QGraphicsView *parent) :
    QGraphicsView(parent)
{
    viewScale = 1;
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setRenderHint(QPainter::Antialiasing, false);
    setViewportUpdateMode(SmartViewportUpdate);
    setDragMode(RubberBandDrag);
}

CTouchPanelView::~CTouchPanelView()
{
}

void CTouchPanelView::wheelEvent(QWheelEvent *e)
{
    if(this->dragMode() == QGraphicsView::ScrollHandDrag)
    {
        qreal factor = qPow(1.2, e->delta() / 240.0);
        viewScale *= factor;
        scale(factor, factor);
        e->accept();
    }
}
/* end of file */
