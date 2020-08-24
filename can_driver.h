#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include <QtCore/qglobal.h>
#include <QDateTime>
#include <functional>
#include <queue>
#include "candriver.h"

#if defined(MODULECOMM_LIBRARY)
#define MODULECOMMSHARED_EXPORT Q_DECL_EXPORT
#else
#define MODULECOMMSHARED_EXPORT Q_DECL_IMPORT
#endif

class MODULECOMMSHARED_EXPORT CAN {
public:
    CAN();
    ~CAN();
    bool start_can();
    bool close_can();
    bool set_param(can_dev& dev);
    bool Recv_msg(can_frame0& fram, int can_chn);
    bool Send_msg(can_frame0& fram, int can_chn);
    void view_msg(can_frame0& fram, int can_chn, int mode = 0);

public:
    CanDriver* can_driver;
    can_dev candev;
};
#endif  // CAN_DRIVER_H
