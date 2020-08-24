#ifndef RADARPAINTWIDGET_H
#define RADARPAINTWIDGET_H

#include <QBrush>
#include <QColor>
#include <QDebug>
#include <QPaintDevice>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QWidget>
//#include "buzzer.h"
#include "radar.h"
#include "detectiondefines.h"

using namespace IV::Detection::Radar;

class RadarPaintWidget : public QWidget {
    Q_OBJECT
public:
    explicit RadarPaintWidget(QWidget *parent = nullptr);
    ~RadarPaintWidget();
    QList<radar_obj> getObjList();
    void setObjList(QList<radar_obj> list);
    void setAngleThresh(int angle);
    void buzzerEvent(int min_long);

protected:
    void paintEvent(QPaintEvent *);
    void wheelEvent(QWheelEvent *);

signals:

public slots:
private:
    int ratio = 20;
    QList<radar_obj> obj_list;
    int angle_thresh = 30;

public:
//   Buzzer radar_buzzer;
};

#endif  // RADARPAINTWIDGET_H
