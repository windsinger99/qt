#ifndef CTOUCHPANELVIEW_H
#define CTOUCHPANELVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QMap>

#include <QString>

class CTouchPanelView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit CTouchPanelView(QGraphicsView *parent = 0);
    ~CTouchPanelView();

protected:
    void wheelEvent(QWheelEvent *e);

private:
    qreal viewScale;

    QPointF lastPos;
};

#endif // CTOUCHPANELVIEW_H
