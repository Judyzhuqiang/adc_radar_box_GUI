
#include "radar.h"


Radar::Radar(QObject *parent) : QThread(parent)
{
    can = new CAN();
}

int Radar::connectCan(int DevType,int DevIndex,int DevBaud,int DevChannel)
{

    //set
    dev.baurdrate=DevBaud;
    dev.can_index=DevChannel;
    dev.dev_index=DevIndex;

    can->set_param(dev);
    can->start_can();
    is_start=true;
    return 1;

}
void Radar::getCompensationData(float distCompensation0,float angelCompensation0)
{
    distCompensation = distCompensation0;
    angelCompensation = angelCompensation0;
}
int Radar::closeCan()
{
    can->close_can();
    is_start=false;
    //delete can;
}
void Radar::stop()
{
    is_start=false;
}

int Radar::reset()
{

}

void Radar::updateInitConfig()
{

}

void Radar::run()
{

    while (is_start) {
        int dwRel=1;

        //        can_frame0 frame;
        //        dwRel=read(sock,&frame,sizeof(frame));
        can->Recv_msg(frame, 0);
        //can->view_msg(frame, 0, 0);


        if(dwRel==-1){
            msleep(100);
            continue;
        }
        if(dwRel > 0){
            int id=frame.can_id;
            switch (id) {
            case 0x60a:{
                num_of_objs=frame.data[0];
                measure_count=frame.data[1]<<8|frame.data[2];
                interface_version=frame.data[3]<<4;
                package.objs.clear();

                if(num_of_objs == 0){
                    emit getData(package);
                }
                break;
            }
            case 0x60b:{
                radar_obj obj;

                int distlong = frame.data[1]<<5|(frame.data[2]>>3);
                obj.distance_long = distlong*0.2f-500 + distCompensation;//距离补偿
                  qDebug()<<obj.distance_long;
                int distlat = ((frame.data[2]&7)<<8)|frame.data[3];
                obj.distance_latitude = distlat*0.2f-204.6f + angelCompensation *
                        obj.distance_long * Pi /180;//角度补偿
                int virelong = frame.data[4]<<2|(frame.data[5]>>6);
                obj.velocity_long = virelong*0.25f-128;
                int virelat = (frame.data[5]&0x3f)<<3|(frame.data[6]>>5);
                obj.velocity_latitude = virelat*0.25f-64;
                obj.dynamic_prop = frame.data[6]&7;
                int objrcs = frame.data[7];
                obj.obj_RCS = objrcs*0.5f-64;
                package.objs.push_back(obj);
                if(package.objs.size() >= num_of_objs){
                    emit getData(package);
                }
                break;
            }
            default:
                break;
            }
        }


    }
//    is_start=true;
}

void Radar::setBaud(int index, int baud)
{
    QString down=QString().sprintf("sudo ifconfig can%d down",index);
    QString up=QString().sprintf("sudo ifconfig can%d up",index);
    QString command=QString().sprintf("sudo ip link set can0 type can bitrate %d000",baud);
    system(down.toStdString().c_str());
    system(command.toStdString().c_str());
    system(up.toStdString().c_str());
}
