#ifndef RADARWIDGET_H
#define RADARWIDGET_H

#include <QColor>
#include <QComboBox>
#include <QDebug>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QPen>
#include <QPointF>
#include <QPushButton>
#include <QRect>
#include <QRectF>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <complex>
#include <QTextBrowser>
//#include "buzzer.h"
//#include "detectiondefines.h"
#include "radar.h"
#include "radarpaintwidget.h"

using namespace IV::Detection::Radar;

class RadarWidget : public QWidget {
  Q_OBJECT
 public:
  explicit RadarWidget(QWidget *parent = nullptr);
  ~RadarWidget();
  void createWidgets();
  void connectCan();
  void closeCan();


 signals:
  void showMsg(QString);

 protected:
  void paintEvent(QPaintEvent *event);
  void wheelEvent(QWheelEvent *event);

 signals:

 public slots:
  void dealConnect();
  void dealStart();
  void dealStop();
  void dealReset();
  void updateCanPortCombobox();
  void updateWidget(radar_data_package list);
  void enableStart();
  void dealCanIndexChanged(int index);
  void dealDeviceWrong();
  void dealThreshChanged();
  void dealBuzzerMinLongChanged();
//  void printdate();

 private:
  QLabel *can_port_label;
  QComboBox *can_port_combobox;
  QLabel *can_channel_label;
  QComboBox *can_channel_combobox;
#ifdef WIN32
  QLabel *can_type_label;
  QComboBox *can_type_combobox;
#endif
  QLabel *can_baud_label;
  QComboBox *can_baud_combobox;

  QPushButton *connnect_btn;
  QLabel *connnect_light_label;
  QWidget *form_widget;
  QLabel *thresh_label;
  QLabel *thresh_value;
  QLabel *buzzer_label;
  QLabel *buzzer_value;
  QPushButton *refresh_btn;
  QPushButton *start_btn;
  QPushButton *stop_btn;
  QPushButton *reset_btn;
  QPushButton *thresh_btn;
  QPushButton *buzzer_btn;
  RadarPaintWidget *content_widget;
public:
  Radar *radar;
private:
  //Buzzer radar_buzzer;
  QTimer *timer_enable_start;
  QPushButton *pushButton;
  QTextBrowser *textBrowser_12;


#ifdef WIN32
  VCI_BOARD_INFO pInfo[50];
#endif
  int current_can_index = 0;
  int can_type = 4;
  int angle_thresh = 30;
  int buzVal = 1;
};

#endif  // RADARWIDGET_H
