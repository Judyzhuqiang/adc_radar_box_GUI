#include "can_driver.h"

CAN::CAN() {

    can_driver = new CanDriver();
}
CAN::~CAN(){
    delete can_driver;
}

bool CAN::Recv_msg(can_frame0& fram, int can_chn) {
    can_driver->RcvMsg(fram, can_chn);
}
bool CAN::Send_msg(can_frame0& fram, int can_chn) {
    can_driver->SdMsg(fram, can_chn);
}
void CAN::view_msg(can_frame0& fram, int can_chn, int mode) {
    can_driver->viewMsg(fram, can_chn, mode);
}
bool CAN::set_param(can_dev& dev){
    this->candev = dev;
}
bool CAN::start_can(){
    can_driver->run();
}
bool CAN::close_can(){
    can_driver->close_can();
}
