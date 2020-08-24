#ifndef DETECTIONDEFINES_H
#define DETECTIONDEFINES_H
#include <qglobal.h>
#include <QList>
#include <cmath>
#include <string>

namespace IV {

namespace Detection {
namespace Radar {
const int DATA_SPAN_ANGLE = 60;
struct can_msg {
    uint id;
    bool is_extern;
    bool is_remote;
    uchar nLen;
    uchar data[8];
};
struct radar_obj {
    int id;
    float distance_long;
    float distance_latitude;
    float velocity_long;
    float velocity_latitude;
    int dynamic_prop;
    float obj_RCS;
    float x;
    float y;
};
struct radar_data_package {
    QList<radar_obj> objs;
};
}  // namespace Radar
namespace IMU {
struct imu_unit {
    float X = 0;
    float Y = 0;
    float Z = 0;
    float abs = 0;
};

struct imu_data {
    imu_unit acc_speed;
    imu_unit angular_speed;
    imu_unit magnetic;
    imu_unit angle;
    int timestep;
    float T;
};
}  // namespace IMU
namespace Video {
struct darknet_obj {
    std::string name;
    float width;
    float height;
    float x;
    float y;
    float prob;
    float mask;
    float objectness;
    int sort_class;
};
}  // namespace Video
namespace Wave {
struct wave_data {
    int hall;
    int light_electric;
    int wave_distance;
    float temperature;
    int timestep;
};
}  // namespace Wave
namespace RPLidar {

const quint8 ORDER_START_MOTOR[6] = {0xa5, 0xf0, 0x02, 0x94, 0x02, 0xc1};
const quint8 ORDER_STOP_MOTOR[6] = {0xa5, 0xf0, 0x02, 0x00, 0x00, 0x57};
const quint8 ORDER_STOP[2] = {0xa5, 0x25};
const quint8 ORDER_RESET[2] = {0xa5, 0x40};
const quint8 ORDER_SCAN[2] = {0xa5, 0x20};
const quint8 ORDER_EXPRESS_SCAN_TRADITION[9] = {0xa5, 0x82, 05,   0x00, 0x00,
                                                0x00, 0x00, 0x00, 0x22};
const quint8 ORDER_EXPRESS_SCAN_EXTEND[9] = {0xa5, 0x82, 05,   0x00, 0x00,
                                             0x00, 0x00, 0x00, 0x22};
const quint8 ORDER_EXPRESS_SCAN_DENSE[9] = {0xa5, 0x82, 05,   0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x22};
const quint8 ORDER_FORCE_SCAN[2] = {0xa5, 0x21};
const quint8 ORDER_GET_INFO[2] = {0xa5, 0x50};
const quint8 ORDER_GET_HEALTH[2] = {0xa5, 0x52};
const quint8 ORDER_GET_AMPLERATE[2] = {0xa5, 0x59};
const quint8 ORDER_GET_CONF[2] = {0xa5, 0x84};

const int STATUS_WAIT = 0;
const int STATUS_SCAN = 1;
const int STATUS_EXPRESS_SCAN = 2;

const int LENGTH_SCAN_RESPONSE_DATA = 5;
const int LENGTH_EXPRESS_SCAN_RESPONSE_DATA = 84;

struct scan_data {
    int quality;
    float angle;
    float distance;
    float x;
    float y;
};

}  // namespace RPLidar
namespace CAN {
// struct canMsg {
//  unsigned int id;
//  bool isExtern;
//  bool isRemote;
//  unsigned char nlen;
//  unsigned char data[8];
//};

}  // namespace CAN
namespace BUZZER {
struct speek {
    bool IS_SPEEK;
};
struct canMsg {
    int data_len;
    int can_id;
    unsigned char data[8] = {0};
};

}  // namespace BUZZER
}  // namespace Detection
}  // namespace IV
#endif  // DETECTIONDEFINES_H
