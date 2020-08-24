#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidget>
#include <QMainWindow>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTableWidgetItem>
#include <math.h>
#include "radarwidget.h"
#include "candriver.h"
#include "radar.h"
using namespace IV;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void dealShowMsg(QString msg);

public slots:
    void dealShowlog(QString u);


signals:
//    void getData(radar_data_package);
    void sendCompensationData(float ,float);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_DistButton_clicked();
    void on_AngelButton_clicked();

private:
    Ui::MainWindow *ui;
    RadarWidget *radar_widget;
    radar_data_package package;
    float distCompensation;
    float angelCompensation;


};

#endif  // MAINWINDOW_H

