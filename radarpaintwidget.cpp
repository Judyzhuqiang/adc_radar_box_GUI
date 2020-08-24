#include "radarpaintwidget.h"

RadarPaintWidget::RadarPaintWidget(QWidget *parent) : QWidget(parent) {}
RadarPaintWidget::~RadarPaintWidget() {}
QList<radar_obj> RadarPaintWidget::getObjList() { return obj_list; }

void RadarPaintWidget::setObjList(QList<radar_obj> list) {
    obj_list.clear();
    obj_list = list; }

void RadarPaintWidget::setAngleThresh(int angle) {
    angle_thresh = angle;
    update();
}

void RadarPaintWidget::paintEvent(QPaintEvent *) {
    int size_max = qMin(width() - 20, height() * 2 - 40);
    int size_max_thresh;
    double angle_tmp;
    if (width() / 2 >= height())
        size_max_thresh = height() * 2 - 20;
    else {
        angle_tmp = asin((double)width() / height() / 2.0);
        angle_tmp = angle_tmp * 180 / M_PI;
        if (angle_tmp >= angle_thresh)
            size_max_thresh = height() * 2 - 20;
        else
            size_max_thresh = (width() / 2 - 10) / sin(angle_thresh * M_PI / 180) * 2;
    }
    QPoint pie_center(width() / 2, height() - 10);

    int start_angle = (90 - DATA_SPAN_ANGLE) * 16;
    int span_angle = DATA_SPAN_ANGLE * 16 * 2;
    int start_angle2 = (90 - angle_thresh) * 16;
    int span_angle2 = angle_thresh * 16 * 2;
    QPainter painter(this);
    QPen pen(QColor(Qt::blue));
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    QBrush brush(QColor(Qt::darkGray));
    painter.setBrush(brush);
    painter.drawRect(rect());
    brush.setColor(QColor(Qt::lightGray));
    painter.setBrush(brush);
    QRectF rect_thresh(QPointF((width() - size_max_thresh) / 2,
                               height() - size_max_thresh / 2 - 10),
                       QSize(size_max_thresh, size_max_thresh));
    painter.drawPie(rect_thresh, start_angle2, span_angle2);

    brush.setColor(QColor(0, 0, 0, 0));
    painter.setBrush(brush);
    for (int i = 1; i <= 5; i++) {
        int size = size_max * i / 5;
        QRectF rect_pie(QPointF((width() - size) / 2, height() - size / 2 - 10),
                        QSize(size, size));
        painter.drawPie(rect_pie, start_angle, span_angle);
        painter.drawText(
                    QPoint(pie_center.x() +
                           size / 2 * cos((90 - DATA_SPAN_ANGLE) * M_PI / 180.0) + 10,
                           pie_center.y() -
                           size / 2 * sin((90 - DATA_SPAN_ANGLE) * M_PI / 180.0) + 10),
                    QString::number((float)size / 2 / ratio));
    }

    brush.setColor(QColor(Qt::red));
    pen.setColor(QColor(Qt::blue));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(brush);
    for (int i = 0; i < obj_list.size(); i++) {
        obj_list[i].distance_long =obj_list[i].distance_long ;
        double angle =
                atan(obj_list[i].distance_latitude / obj_list[i].distance_long);
        if (qAbs(angle) * 180 / M_PI > angle_thresh) {
            continue;
        }
        QPoint obj_point(pie_center.x() + obj_list[i].distance_latitude * ratio,
                         pie_center.y() - obj_list[i].distance_long * ratio);
        painter.drawEllipse(obj_point, 4, 4);
        painter.drawText(
                    QPoint(obj_point.x() + 10, obj_point.y()),
                    QString().sprintf("p=(%.3f,%.3f)", obj_list[i].distance_long,
                                      obj_list[i].distance_latitude));
        painter.drawText(
                    QPoint(obj_point.x() + 10, obj_point.y() + 10),
                    QString().sprintf("v=(%.3f,%.3f)", obj_list[i].velocity_long,
                                      obj_list[i].velocity_latitude));
    }
    painter.end();
}

void RadarPaintWidget::wheelEvent(QWheelEvent *event) {
    int delta = event->angleDelta().y();
    if (delta > 0) {
        ratio += 2;
    } else {
        ratio -= 2;
    }
    if (ratio > 500) {
        ratio = 500;
    } else if (ratio < 1) {
        ratio = 1;
    }
    update();
}

/**
 * @brief RadarPaintWidget::buzzerEvent
 */
void RadarPaintWidget::buzzerEvent(int min_long) {
    //  bool is_sp =false;
    //  if (obj_list.size()==0)
    //      //radar_buzzer.speeker(false);
    //  if (!obj_list.size()) return;
    //  for (auto data : obj_list)
    //    if (data.distance_long < min_long){
    //        is_sp = true;
    //        break;
    //    }
    //    else
    //        is_sp = false;
    //  is_sp ? radar_buzzer.speeker(true) : radar_buzzer.speeker(false);
}
// void RadarPaintWidget::pain_chart() {}
