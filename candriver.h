#ifndef CANDRIVER_H
#define CANDRIVER_H
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include "include/controlcan.h"

#include <unistd.h>
#include <condition_variable>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <unistd.h>
#include <QWidget>

#include <QObject>

typedef struct {
    unsigned int can_id;
    unsigned int TimeStamp;
    unsigned char TimeFlag;
    unsigned char SendType;
    unsigned char RemoteFlag;  //是否是远程帧
    unsigned char ExternFlag;  //是否是扩展帧
    unsigned char DataLen;
    unsigned char data[8];
    unsigned char Reserved[3];
} can_frame0;

typedef struct {
    unsigned int dev_type = 4;
    unsigned int dev_index = 0;
    unsigned int can_index = 0;
    unsigned int AccCode = 0;
    unsigned int AccMask = 0xFFFFFFFF;
    unsigned int Filter = 1;
    unsigned int baurdrate = 500;
    unsigned int mode = 0;

} can_dev;

typedef void (*send_can1)(can_frame0);

static std::mutex mut;
static std::condition_variable data_cond;

class CanDriver: public QWidget {
    Q_OBJECT
public:
    explicit CanDriver(QWidget *parent = nullptr);
//    CanDriver();
    ~CanDriver();

    // setup
    bool run();
    bool set_param(can_dev& candev);
    bool RcvMsg(can_frame0& recv_fram, int can_chn);
    bool SdMsg(can_frame0& recv_fram, int can_chn);
    bool close_can();
public:
    bool SetCanParam(can_dev& candev, VCI_INIT_CONFIG& config);
    bool FindUsb(int dev_index, int dev_type);
    bool OpenDev(int dev_index, int dev_type = 4);
    bool ReadDevInfo(DWORD& DevType, VCI_BOARD_INFO& pInfo);
    bool InitCan(can_dev& candev);
    bool StartCan(can_dev& candev);
    bool SendMsg(can_dev& candev, can_frame0& send_fram, int& length);
    bool ReceiveMsg(can_dev& candev, can_frame0& recv_fram, int& recv_len);
    bool viewMsg(const can_frame0& recv_fram, int can_chn = 0, int mode = 0);
    QString s=" ";//取值，界面显示
    //  bool can0_send_msg(std::vector<can_frame> smsg);
    //  std::thread CAN0_SendThread(std::vector<can_frame> smsg) {
    //    printf("CAN0_SendThread setup .. \n");
    //    return std::thread(&CanDriver::can0_send_msg, this, smsg);
    //  };

    //  bool can1_send_msg(send_can1 can);
    //  std::thread CAN1_SendThread(send_can1 can) {
    //    printf("CAN1_SendThread setup .. \n");
    //    return std::thread(&CanDriver::can1_send_msg, this, can);
    //  };

    //  bool can0_recv_msg();
    //  std::thread CAN0_ReceiveThread() {
    //    printf("CAN0_ReceiveThread setup .. \n");
    //    return std::thread(&CanDriver::can0_recv_msg, this);
    //  };
    //  bool can1_recv_msg();
    //  std::thread CAN1_ReceiveThread() {
    //    printf("CAN1_ReceiveThread setup .. \n");
    //    return std::thread(&CanDriver::can1_recv_msg, this);
    //  };
    //  bool monitor();

    //  std::thread monitorThread() {
    //    return std::thread(&CanDriver::monitor, this);
    //};
signals:
    void out_log(QString);

private:
    VCI_BOARD_INFO pInfo;  //用来获取设备信息。
    int count_send = 0;    //数据列表中，用来存储列表序号。
    int count_recv = 0;
    VCI_BOARD_INFO pInfo1[50];

    // public:
    //  // init can dev
    can_dev init_can0;
    can_dev init_can1;

    //  // receive candata vector
    //  std::vector<can_frame> can0_recv_data;
    //  std::vector<can_frame> can1_recv_data;

    //  // send candata vector
    //  std::vector<can_frame> can0_send_data;
    //  std::queue<can_frame> can1_send_data;

    //  send_can1 sdcan1;

    int length = 0;  // back the length
    int WaitTime = 100;
    int ind = 0;
    VCI_CAN_OBJ rec[3000];  //接收缓存，设为3000为佳。

};

#endif  // CANDRIVER_H
