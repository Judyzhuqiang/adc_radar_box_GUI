#include "mainwindow.h"
#include "candriver.h"
#include "qbytearray.h"

CanDriver::CanDriver(QWidget *parent) : QWidget(parent){

    printf("----------------------------------------------\r\n");
    printf("-                                            -\r\n");
    printf("-         ADC ChuangXin CAN Driver           -\r\n");
    printf("-                                            -\r\n");
    printf("----------------------------------------------\r\n");

    //  init_can1.dev_type = VCI_USBCAN2;

    // test
    // std::vector<can_frame0> sendtest;
    // can1_send_data = {can1_send_frame, can1_send_frame, can1_send_frame};

    //  can_frame0 fram;
    // can0_send_data = {fram, fram, fram};

    //  run();
    //  can0_recv_data.reserve(3500);
    //  can1_recv_data.reserve(3500);
    //  can0_send_data.reserve(3500);
    //  can1_send_data.reserve(3500);


}

CanDriver::~CanDriver() {
    while (VCI_CloseDevice(VCI_USBCAN2, 0) == 1) {
        usleep(1000);
        printf("waitting for close can dev .. /n");
    }


}
bool CanDriver::set_param(can_dev& candev){
    init_can0 = candev;
    //  init_can0.baurdrate = 500;
    init_can0.can_index = 0;
    //  init_can0.dev_type = VCI_USBCAN2;

    init_can1 = candev;
    // init_can1.baurdrate = 500;
    init_can1.can_index = 1;
}
bool CanDriver::close_can(){

    printf(" ready close can dev \n");
    try {
        int i=5;
        while (i--){
            if (VCI_CloseDevice(VCI_USBCAN2, 0) == 1) {
                printf("close can dev seccess  \n");
                return false;
            } else {
                printf("close can dev error  \n");
                usleep(100000);
            }
        }
    } catch (...) {
        printf("try close can error .. /n");
        return false;
    }


    return true;
}



bool CanDriver::run() {
    // set up dev
    if (FindUsb(0, 4)) {
        if (StartCan(init_can0)) {
            printf("setup can0 success ! \n");
        } else {
            printf("setup can0 fail ! \n");
        }

        if (StartCan(init_can1)) {
            printf("setup can1 success ! \n");
        } else {
            printf("setup can1 fail ! \n");
        }
    } else {
        printf("setup USB_CAN dev fail ! \n");
    }

    //  // setup thread
    //  std::thread ThrS0 = CAN0_SendThread(can0_send_data);
    //  // ThrS1.join();
    //  ThrS0.detach();
    //  std::thread ThrS1 = CAN1_SendThread(sdcan1);
    //  ThrS1.detach();

    //  std::thread ThrR0 = CAN0_ReceiveThread();
    //  // ThrS1.join();
    //  ThrR0.detach();
    //  std::thread ThrR1 = CAN1_ReceiveThread();
    //  // ThrS1.join();
    //  ThrR1.join();

    //  std::thread Thrmt = monitorThread();
    //  // ThrS1.join();
    //  Thrmt.join();
}

bool CanDriver::SetCanParam(can_dev& candev, VCI_INIT_CONFIG& config) {
    //初始化参数，严格参 数二次开发函数库说明书。
    config.AccCode = candev.AccCode;
    config.AccMask = candev.AccMask;
    config.Filter = candev.Filter;  //接收所有帧
    config.Mode = candev.mode;      //正常模式

    switch (candev.baurdrate) {
    case 10:  // 10k
        config.Timing0 = 0x31;
        config.Timing1 = 0x1C;
        break;
    case 20:
        config.Timing0 = 0x18;
        config.Timing1 = 0x1C;
        break;

    case 40:
        config.Timing0 = 0x87;
        config.Timing1 = 0xFF;
        break;

    case 50:
        config.Timing0 = 0x09;
        config.Timing1 = 0x1C;
        break;
    case 80:
        config.Timing0 = 0x83;
        config.Timing1 = 0xFF;
        break;

    case 100:
        config.Timing0 = 0x04;
        config.Timing1 = 0x1C;
        break;
    case 125:
        config.Timing0 = 0x03;
        config.Timing1 = 0x1C;
        break;

    case 200:
        config.Timing0 = 0x81;
        config.Timing1 = 0xFA;
        break;
    case 250:
        config.Timing0 = 0x01;
        config.Timing1 = 0x1C;
        break;

    case 400:
        config.Timing0 = 0x80;
        config.Timing1 = 0xFA;
        break;

    case 500:
        config.Timing0 = 0x00;
        config.Timing1 = 0x1C;
        break;
    case 666:
        config.Timing0 = 0x80;
        config.Timing1 = 0xB6;
        break;

    case 800:
        config.Timing0 = 0x00;
        config.Timing1 = 0x16;
        break;
    case 1000:
        config.Timing0 = 0x00;
        config.Timing1 = 0x14;
        break;

        //    case 33.33:
        //      config.Timing0 = 0x09;
        //      config.Timing1 = 0x6F;
        //      break;
        //    case 66.66:
        //      config.Timing0 = 0x04;
        //      config.Timing1 = 0x6F;
        //      break;

        //    case 83.33:
        //      config.Timing0 = 0x03;
        //      config.Timing1 = 0x6F;
        //      break;

    default:  // 500K
        config.Timing0 = 0x00;
        config.Timing1 = 0x1C;
        break;
    }
    return true;
}

/**
 * @brief CanDriver::OpenDev
 * @param dev_index
 * @return
 */
bool CanDriver::OpenDev(int dev_index, int dev_type) {
    int openDevCout = 0;
    bool is_close_dev = false;
    while (true) {
        try {
            openDevCout++;
            printf("try open dev times %d \n", openDevCout);
            if (openDevCout >= 100) {
                printf("open can dev over 100 times ! \n");
                return false;
            }

            if (!is_close_dev) {
                if (VCI_OpenDevice(dev_type, dev_index, 0) == 1) {
                    printf("USB-CAN device reset success ! \n");
                    // init can
                    if (InitCan(init_can0) && InitCan(init_can1)) {
                        printf("USB-CAN device can0 and can1 init success  ! \n");
                        return true;
                    } else {
                        is_close_dev = true;
                    }

                } else {
                    is_close_dev = true;
                }
            } else {
                printf("open can dev fail, try close dev \n");
                try {
                    if (VCI_CloseDevice(VCI_USBCAN2, 0) == 1) {
                        printf("close can dev seccess  \n");
                        return false;
                    } else {
                        printf("close can dev error  \n");
                        usleep(1000);
                        is_close_dev = false;
                    }
                } catch (...) {
                    printf("try close can error .. /n");
                }
            }
        } catch (...) {
            printf("start USB-CAN device fatal error ! \n");
        }
    }
}
bool CanDriver::InitCan(can_dev& candev) {
    int dev_index = candev.dev_index;
    int can_index = candev.can_index;
    int dev_type = candev.dev_type;
    VCI_INIT_CONFIG config;

    if (SetCanParam(candev, config))
        printf("set can %d param succes ! \n", can_index);
    else {
        printf("set can %d param fail ! \n", can_index);
        return false;
    };

    int cout_initcan = 0;
    while (true) {
        try {
            cout_initcan++;
            if (cout_initcan > 10) return false;

            int rtn = VCI_InitCAN(dev_type, dev_index, can_index, &config);
            usleep(100);

            if (rtn == 1) {
                printf("int can %d succes ! \n ", can_index);
                return true;
            } else {
                printf("int can %d fail ! \n ", can_index);
            }
        } catch (...) {
            printf("init USB-CAN %d  device fatal error \n", can_index);
            return false;
        }
    }
}

bool CanDriver::StartCan(can_dev& candev) {
    int dev_index = candev.dev_index;
    int can_index = candev.can_index;
    int dev_type = candev.dev_type;

    bool is_reset_can = false;
    int cout_reset_can = 0;
    while (true) {
        try {
            cout_reset_can++;
            if (cout_reset_can > 10) return false;

            if (!is_reset_can) {
                if (VCI_StartCAN(dev_type, dev_index, can_index) == 1) {
                    printf("Start CAN%d success \n", can_index);
                    return true;

                } else {
                    printf("start &&  CAN%d  error\n", can_index);
                    is_reset_can = true;
                }
            } else {
                if (VCI_ResetCAN(dev_type, dev_index, can_index) == 1) {
                    printf("reset CAN %d success \n", can_index);
                } else {
                    printf("reset CAN %d error\n", can_index);
                }
                is_reset_can = false;
            }
        } catch (...) {
            printf("start USB-CAN %d  device fatal error \n", can_index);
            return false;
        }
    }
}

/**
 * @brief CanDriver::FindUsb
 * @param dev_index
 * @param dev_type
 * @return
 */
bool CanDriver::FindUsb(int dev_index, int dev_type) {
    int findUsbCout = 0;
    bool is_reset_usb = false;

    while (true) {
        try {
            findUsbCout++;
            if (findUsbCout >= 10000) {
                printf("open can dev over 100 times ! \n");
                return false;
            }

            if (!is_reset_usb) {
                int num = VCI_FindUsbDevice2(pInfo1);
                usleep(100);

                if (num > 0)  // open one dev
                {
                    printf("[ USBCAN DEVICE NUM ]: %d PCS \n", num);
                    //打开设备
                    if (OpenDev(dev_index, dev_type)) {
                        return true;
                    } else {
                        is_reset_usb = true;
                    }

                } else {
                    is_reset_usb = true;
                }
                //
            } else {
                printf("Not Find USB-CAN device, try reset usb dev : %d \n",
                       findUsbCout);
                if (VCI_UsbDeviceReset(dev_type, dev_index, 0) == 1) {
                    printf("USB-CAN device reset sucess \n");
                    usleep(1000);
                } else {
                    printf("USB-CAN device reset fail, try again \n");
                }

                is_reset_usb = false;
            }

        } catch (...) {
            printf("USB-CAN device fatal error \n");
            return false;
        }
    }
}

/**
 * @brief CanDriver::ReadDevInfo
 * @param DevType
 * @param pInfo
 * @return
 */
bool CanDriver::ReadDevInfo(DWORD& DevType, VCI_BOARD_INFO& pInfo) {
    //读取设备序列号、版本等信息。
    if (VCI_ReadBoardInfo(DevType, 0, &pInfo) == 1) {
        printf(">>Get VCI_ReadBoardInfo success!\n");

        printf(" %08X \n", pInfo.hw_Version);
        printf(" %08X \n", pInfo.fw_Version);
        printf(" %08X \n", pInfo.dr_Version);
        printf(" %08X \n", pInfo.in_Version);
        printf(" %08X \n", pInfo.irq_Num);
        printf(" %08X \n", pInfo.can_Num);

        printf(">>Serial_Num:%c", pInfo.str_Serial_Num[0]);
        printf("%c", pInfo.str_Serial_Num[1]);
        printf("%c", pInfo.str_Serial_Num[2]);
        printf("%c", pInfo.str_Serial_Num[3]);
        printf("%c", pInfo.str_Serial_Num[4]);
        printf("%c", pInfo.str_Serial_Num[5]);
        printf("%c", pInfo.str_Serial_Num[6]);
        printf("%c", pInfo.str_Serial_Num[7]);
        printf("%c", pInfo.str_Serial_Num[8]);
        printf("%c", pInfo.str_Serial_Num[9]);
        printf("%c", pInfo.str_Serial_Num[10]);
        printf("%c", pInfo.str_Serial_Num[11]);
        printf("%c", pInfo.str_Serial_Num[12]);
        printf("%c", pInfo.str_Serial_Num[13]);
        printf("%c", pInfo.str_Serial_Num[14]);
        printf("%c", pInfo.str_Serial_Num[15]);
        printf("%c", pInfo.str_Serial_Num[16]);
        printf("%c", pInfo.str_Serial_Num[17]);
        printf("%c", pInfo.str_Serial_Num[18]);
        printf("%c", pInfo.str_Serial_Num[19]);
        printf("\n");

        printf(">>hw_Type:%c", pInfo.str_hw_Type[0]);
        printf("%c", pInfo.str_hw_Type[1]);
        printf("%c", pInfo.str_hw_Type[2]);
        printf("%c", pInfo.str_hw_Type[3]);
        printf("%c", pInfo.str_hw_Type[4]);
        printf("%c", pInfo.str_hw_Type[5]);
        printf("%c", pInfo.str_hw_Type[6]);
        printf("%c", pInfo.str_hw_Type[7]);
        printf("%c", pInfo.str_hw_Type[8]);
        printf("%c", pInfo.str_hw_Type[9]);
        printf("\n");

        printf(">>Firmware Version:V");
        printf("%x", (pInfo.fw_Version & 0xF00) >> 8);
        printf(".");
        printf("%x", (pInfo.fw_Version & 0xF0) >> 4);
        printf("%x", pInfo.fw_Version & 0xF);
        printf("\n");
        return true;
    } else {
        printf(">>Get VCI_ReadBoardInfo error!\n");
        return false;
    }
}

bool CanDriver::ReceiveMsg(can_dev& candev, can_frame0& recv_fram, int& reclen) {
    // int reclen = 3000;  // buf size

    ind++;

    int a;
    length = VCI_Receive(candev.dev_type, candev.dev_index, candev.can_index, rec,
                         reclen, WaitTime);
    if (length > 0) {
        // memcpy(&recv_fram, rec, length);

        recv_fram.can_id = rec->ID;
        memset(&recv_fram.data, 0, sizeof(recv_fram.data));
        memcpy(&recv_fram.data, rec->Data, rec->DataLen);
        recv_fram.DataLen = rec->DataLen;
        recv_fram.SendType = rec->SendType;
        recv_fram.TimeFlag = rec->TimeFlag;
        recv_fram.TimeStamp = rec->TimeStamp;
        recv_fram.ExternFlag = rec->ExternFlag;
        recv_fram.RemoteFlag = rec->RemoteFlag;



        //输出到UI，数据给到全局变量
        a=rec->TimeStamp;
        s.clear();
        s+="TimeStamp:"+QString::number(a,10).toUpper();
        s+=" Index:"+QString::number(ind,10).toUpper();
        a=rec->ID;
        s+=" ID:"+QString::number(a,16).toUpper()+" ";//输出ID
        s+=" Standard  Data   data:0x ";
        char data[40];
        sprintf(data,"%02x %02x %02x %02x %02x %02x %02x %02x \n",
                rec->Data[0],rec->Data[1],rec->Data[2],rec->Data[3]
                ,rec->Data[4],rec->Data[5],rec->Data[6],rec->Data[7]);

        s.append(data);

        emit out_log(s);
        //printf("log :%s \n",qPrintable(s));


        //        MainWindow::radardata=s;


        usleep(100);

        // out scream
//        for (int i = 0; i < length; i++) {
            // viewMsg(recv_fram, candev.can_index, 0);

            //        printf("Receive: ");
            //        printf(" TimeStamp:0x%08X", rec[i].TimeStamp);  //时间标识。
            //        printf("Index:%04d  ", count_recv);

            //        count_recv++;
            //        printf("CAN%d RX ID:0x%08X ", candev.can_index, rec[0].ID);
            //        if (rec[i].ExternFlag == 0) printf(" Standard ");
            //        if (rec[i].ExternFlag == 1) printf(" Extend   ");
            //        if (rec[i].RemoteFlag == 0) printf(" Data   ");
            //        if (rec[i].RemoteFlag == 1) printf(" Remote ");
            //        printf("DLC:0x%02X", rec[i].DataLen);
            //        printf(" data:0x");

            //        for (auto& data : rec[i].Data) {
            //          printf(" %02X", data);
            //        }

            //        printf("\n");
//        }

    } else {
        usleep(10000);
        // printf("NOT receive can%d data, please check dev state .. \n",
        // candev.can_index);
    }
}
bool CanDriver::SendMsg(can_dev& candev, can_frame0& send_fram, int& length) {
    PVCI_CAN_OBJ send = (PVCI_CAN_OBJ)&send_fram;
    if (VCI_Transmit(candev.dev_type, candev.dev_index, candev.can_index, send,
                     length) == 1) {
        // printf("can%d send msg success ! \n", candev.can_index);

        usleep(1000);

        // out scream
        // for (int i = 0; i < length; i++) {
        // viewMsg(send_fram, candev.can_index, 1);

        //      printf("Send: ");
        //      printf(" TimeStamp:0x%08X", send[i].TimeStamp);  //时间标识。
        //      printf("Index:%04d  ", count_send);

        //      count_send++;
        //      printf("CAN%d TX ID:0x%08X ", candev.can_index, send[0].ID);
        //      if (send[i].ExternFlag == 0) printf(" Standard ");
        //      if (send[i].ExternFlag == 1) printf(" Extend   ");
        //      if (send[i].RemoteFlag == 0) printf(" Data   ");
        //      if (send[i].RemoteFlag == 1) printf(" Remote ");
        //      printf("DLC:0x%02X", send[i].DataLen);
        //      printf(" data:0x");

        //      for (auto& data : send[i].Data) {
        //        printf(" %02X", data);
        //      }

        //      printf("\n");
        //      send[i].ID += 1;
        //}
        return true;
    } else {
        printf("message send fail ! \n");
        return false;
    }
}
bool CanDriver::RcvMsg(can_frame0& recv_fram, int can_chn) {
    int length = 1;
    if (can_chn == 0)
        return ReceiveMsg(init_can0, recv_fram, length);
    else if (can_chn == 1)
        return ReceiveMsg(init_can1, recv_fram, length);
    else {
        printf("NOT this CAN chanel \n");
    }
}

bool CanDriver::SdMsg(can_frame0& recv_fram, int can_chn) {
    int length = 1;
    if (can_chn == 0)
        return SendMsg(init_can0, recv_fram, length);
    else if (can_chn == 1)
        return SendMsg(init_can1, recv_fram, length);
    else {
        printf("NOT this CAN chanel \n");
    }
}
bool CanDriver::viewMsg(const can_frame0& fram, int can_chn, int mode) {
    // out scream
    printf("Msg: ");
    if (mode == 0)
        printf("-RCV-");
    else if (mode == 1) {
        printf("-SD -");
    } else {
        printf("-  -");
    }

    printf(" TimeStamp:0x%08X", fram.TimeStamp);  //时间标识。
    printf("Index:%04d  ", count_send);

    count_send++;

    if (mode == 0)
        printf("CAN%d RX ID:0x%08X ", can_chn, fram.can_id);
    else if (mode == 1) {
        printf("CAN%d TX ID:0x%08X ", can_chn, fram.can_id);
    } else {
        printf("CAN%d    ID:0x%08X ", can_chn, fram.can_id);
    }

    if (fram.ExternFlag == 0) printf(" Standard ");
    if (fram.ExternFlag == 1) printf(" Extend   ");
    if (fram.RemoteFlag == 0) printf(" Data   ");
    if (fram.RemoteFlag == 1) printf(" Remote ");
    printf("DLC:0x%02X", fram.DataLen);
    printf(" data:0x");

    for (auto& data : fram.data) {
        printf(" %02X", data);
    }

    printf("\n");
    // fram.ID += 1;
}
// bool CanDriver::can0_send_msg(std::vector<can_frame0> smsg) {
//  printf("can0 ready to send data .. \n");
//  int once_send_length = 1;
//  int cout_send = 0;
//  // printf("size of smag : %d \t\n", smsg.size());
//  while (true) {
//    for (auto& send : smsg) {
//      cout_send = 0;
//      while (cout_send < 5) {
//        cout_send++;
//        if (SendMsg(init_can0, send, once_send_length)) {
//          // printf("can0 send msg success .. \n");
//          break;
//        } else {
//          printf("can0 send msg fail, try again .. \n");
//        }
//      }
//      if (cout_send >= 5) {
//        printf("can0 send msg fail, stop try, try next  \n");
//      }
//    }
//    smsg.clear();
//    usleep(100);
//  }
//}

// bool CanDriver::can1_send_msg(send_can1 can) {
//  printf("can1 ready to send data .. \n");
//  int once_send_length = 1;
//  int cout_send = 0;
//  while (true) {
//    (*can)(can1_send_data);

//    if (can1_send_data.size() == 0) {
//      sleep(1);
//      printf("No data /n");
//      continue;
//    }

//    // defined a lock
//    //    std::unique_lock<std::mutex> qlock(mut);
//    if (!can1_send_data.empty()) {
//      // data_cond.wait(qlock, [is_empty] { return !is_empty; });
//      auto& send = can1_send_data.front();

//      cout_send = 0;
//      while (cout_send < 5) {
//        cout_send++;
//        if (SendMsg(init_can1, send, once_send_length)) {
//          // printf("can1 send msg success .. %d \n", cout_send);
//          break;
//        } else {
//          printf("can1 send msg fail, try again .. \n");
//        }
//      }
//      if (cout_send >= 5) {
//        printf("can1 send msg fail, stop try, try next  \n");
//      }
//      // notifuy another thread
//      //      data_cond.notify_one();
//      usleep(10000);
//      can1_send_data.pop();
//      //      qlock.unlock();  // unlock
//    }
//  }
//  usleep(100);
//}

// bool CanDriver::can0_recv_msg() {
//  can0_recv_data.clear();
//  int cout_recv = 0;
//  int recv_len = 3000;
//  can_frame0 fram[recv_len];
//  printf("can0 ready to recv data .. \n");
//  while (true) {
//    if (can0_recv_data.size() > 3500)
//      can0_recv_data.erase(can0_recv_data.begin());

//    while (cout_recv < 5) {
//      if (ReceiveMsg(init_can0, fram, recv_len)) {
//        usleep(100);
//        for (int i = 0; i < recv_len; i++) {
//          can0_recv_data.push_back(fram[i]);
//        }
//        break;
//      } else {
//        printf("can0 recv msg fail, try again .. \n");
//      }
//    }
//    if (cout_recv >= 5) {
//      printf("can0 send msg fail, stop try, try next  \n");
//    }
//  }
//}
// bool CanDriver::can1_recv_msg() {
//  can1_recv_data.clear();
//  int cout_recv = 0;
//  int recv_len = 3000;
//  can_frame0 fram[recv_len];
//  printf("can1 ready to recv data .. \n");
//  while (true) {
//    if (can1_recv_data.size() > 3500)
//      can1_recv_data.erase(can1_recv_data.begin());

//    while (cout_recv < 5) {
//      if (ReceiveMsg(init_can1, fram, recv_len)) {
//        usleep(100);
//        for (int i = 0; i < recv_len; i++) {
//          can1_recv_data.push_back(fram[i]);
//        }
//        break;
//      } else {
//        printf("can0 recv msg fail, try again .. \n");
//      }
//    }
//    if (cout_recv >= 5) {
//      printf("can0 send msg fail, stop try, try next  \n");
//    }
//  }
//}
// bool CanDriver::monitor() {  //
//                             //  sleep(1000000);

//  //  int i;
//  //  can_frame0 fram;
//  //  while (i < 200) {
//  //    std::lock_guard<std::mutex> qlock(mut);
//  //    can1_send_data.push(fram);
//  //    // printf("can1 send data size: %d \n", can1_send_data.size());

//  //    i++;
//  //    mut.unlock();
//  //    usleep(3000);
//  //  }

//  sleep(100000);
//}
