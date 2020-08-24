#ifndef RADAR_H
#define RADAR_H

#include <QDebug>
#include <QObject>
#include <QSerialPort>
#include <QThread>
#include <QTimer>
#include "detectiondefines.h"
#ifdef WIN32
#include "ControlCAN.h"
#endif
#ifdef __linux__
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#endif

#include "can_driver.h"

using namespace IV::Detection::Radar;
using namespace std;
using namespace IV;
class Radar : public QThread {
    Q_OBJECT
public:
    explicit Radar(QObject *parent = nullptr);
    int connectCan(int DevType, int DevIndex, int DevBaud, int DevChannel);
    int closeCan();
    void stop();
    int reset();
    void updateInitConfig();
    void run();
    bool speeker(bool is_speek);



#ifdef __linux__
    void setBaud(int index, int baud);
#endif

signals:
    void getData(radar_data_package);
    void deviceWrong();
    void out_msg(QString);

public slots:

void getCompensationData(float distCompensation , float angelCompensation);

private:
    radar_data_package package;
    bool is_start = false;
    int measure_count;
    int num_of_objs;
    int interface_version;
    float distCompensation;
    float angelCompensation;

#ifdef __unix__
    int sock;
    sockaddr_can addr;
    ifreq ifr;
    bool is_open = false;
    const float Pi = 3.1415926;
#endif
#ifdef WIN32
    int device_type = 4;
    int device_index = 0;
    int device_baud = 500;
    int device_channel = 1;
    VCI_INIT_CONFIG vic;
    VCI_BOARD_INFO vbi;
#endif
public:
    CAN *can;
    can_dev dev; //
    can_frame0 frame;

};
//class buzzer{
//    int buzmsg(bool is_speek,int sock) {
//        can_frame msg;
//        msg.can_id = 0x20;
//        msg.can_dlc =8;
//        if (is_speek) {
//            msg.data[8] = {0};
//            msg.data[0] = 0x1;
//            msg.data[1] = 0x0;
//        } else {
//            msg.data[8] = {0};
//            msg.data[0] = 0x0;
//            msg.data[1] = 0x0;
//        }

//        int rtn =0;
//        rtn = write(sock,&msg,sizeof(msg));
//        if (rtn==-1 ){
//            qDebug()<<"error"<<endl;
//        }
//        return rtn;
//    }
//};
#endif  // RADAR_H
