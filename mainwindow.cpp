#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "radar.h"
#include <QFileDialog>
#include <QFile>
#include <QDebug>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

distCompensation = 0;
angelCompensation = 0;
    ui->setupUi(this);
    radar_widget = new RadarWidget();
       connect(radar_widget, SIGNAL(showMsg(QString)), this,
            SLOT(dealShowMsg(QString)));

    ui->stackedWidget->insertWidget(2, radar_widget);
    QStatusBar *bar = statusBar();

    connect(radar_widget->radar->can->can_driver,SIGNAL(out_log(QString)), this,
            SLOT(dealShowlog(QString)));

    connect(this,SIGNAL(sendCompensationData(float ,float)) ,
            radar_widget->radar,SLOT(getCompensationData(float ,float)));
    connect(ui->pushButton_open,&QPushButton::clicked,[=](){
        //        ui->textBrowser_12->clear();
        QString path = QFileDialog::getOpenFileName(this,tr("回放数据"),"../",tr("TXT (*.txt)"));
        if(!path.isEmpty()){
            QFile file(path);
            bool isOK = file.open(QIODevice::ReadOnly);
            if(isOK == true){
                QTextStream in(&file);
                while(!in.atEnd()){
                    QString line = in.readLine();
                    QByteArray ba = line.toLatin1();
                    ui->textBrowser_12->append(ba);
                    usleep(10000);
                    QApplication::processEvents();

                    if(!line.isEmpty()){
                        QString sid = line.mid(32,3);
                        QByteArray id = QString(sid).toUtf8();
                        id = id.toHex();
                        radar_obj obj;
                        char data[8];
                        int num_of_objs;
                        if (sid =="60A"){
                            QByteArray data0 = line.mid(62,2).toLatin1();
                            data[0] = data0.toUInt(Q_NULLPTR,16);
                            num_of_objs =data[0];
                            package.objs.clear();

                            if(num_of_objs==0){
                                radar_widget->updateWidget(package);

                            }
                        }

                        if (sid =="60B") {

                            QByteArray data1 = line.mid(65,2).toLatin1();
                            QByteArray data2 = line.mid(68,2).toLatin1();
                            QByteArray data3 = line.mid(71,2).toLatin1();
                            QByteArray data4 = line.mid(74,2).toLatin1();
                            QByteArray data5 = line.mid(77,2).toLatin1();
                            QByteArray data6 = line.mid(80,2).toLatin1();
                            QByteArray data7 = line.mid(83,2).toLatin1();

                            data[1] = data1.toUInt(Q_NULLPTR,16);
                            data[2] = data2.toUInt(Q_NULLPTR,16);
                            data[3] = data3.toUInt(Q_NULLPTR,16);
                            data[4] = data4.toUInt(Q_NULLPTR,16);
                            data[5] = data5.toUInt(Q_NULLPTR,16);
                            data[6] = data6.toUInt(Q_NULLPTR,16);
                            data[7] = data7.toUInt(Q_NULLPTR,16);


                            int distlat=((data[2]&7)<<8)|data[3];
                            obj.distance_latitude=distlat*0.2f-204.6f;
                            int distlong=data[1]<<5|(data[2]>>3);
                            obj.distance_long=distlong*0.2f-500;
                            int virelong=data[4]<<2|(data[5]>>6);
                            obj.velocity_long=virelong*0.25f-128;
                            int virelat=(data[5]&0x3f)<<3|(data[6]>>5);
                            obj.velocity_latitude=virelat*0.25f-64;
                            obj.dynamic_prop=data[6]&7;
                            int objrcs=data[7];
                            obj.obj_RCS=objrcs*0.5f-64;
                            package.objs.push_back(obj);
                            if(package.objs.size()>=num_of_objs){
                                radar_widget->updateWidget(package);
                            }

                        }
                        //                        else{
                        //                            package.objs.clear();
                        ////                            usleep(100000);
                        //                        }
                    }


                }
                file.close();
            }
        }
    });

    connect(ui->pushButton_save,&QPushButton::clicked,[=](){
        QString fileName = QFileDialog::getSaveFileName(this,tr("保存数据"),"../",tr("TXT (*.txt)"));

        if (!fileName.isNull()){
            QFile file(fileName);
            bool isOK = file.open(QIODevice::ReadWrite | QIODevice::Append);
            if(isOK == true){
                QByteArray array = file.readAll();
                QString str = ui->textBrowser_12->toPlainText();
                file.write(str.toUtf8());

                file.close();
                QMessageBox::information(this,"保存文件","保存文件成功",QMessageBox::Ok);
            }
        }
        else
        {
            //点的是取消
        }

    });


#ifdef __linux__
    bar->showMessage("current platform: linux");
#endif
}

MainWindow::~MainWindow() { delete ui; delete radar_widget;}

void MainWindow::dealShowMsg(QString msg) { statusBar()->showMessage(msg); }

void MainWindow::dealShowlog(QString u)
{
    ui->textBrowser_12->document()->setMaximumBlockCount(200);
    ui->textBrowser_12->append(u);
}
//距离标定计算
void MainWindow::on_pushButton_clicked()
{
    QString str0,str1,str2,str3,str4;
    float data[10][10]={0};
    float piancha[10];
    float buchang[10];
    float buchangZong=0;
    int count=0;
    int columnCout=ui->tableWidget_2->columnCount();//table colunm count


    //计算距离偏差数值
    for(int i=0;i<columnCout;i++)
    {

        if(ui->tableWidget_2->item(0,i)!=0
                && ui->tableWidget_2->item(1,i)!=0)

        {
            str0 = ui->tableWidget_2->item(0,i)->text();
            str1=ui->tableWidget_2->item(1,i)->text();
            data[0][i]=str0.toFloat();
            data[1][i]=str1.toFloat();
            piancha[i]=data[0][i]-data[1][i];
            str2=QString::number(piancha[i],'g',6);
            ui->tableWidget_2->setItem(2,i,new QTableWidgetItem(str2));
        }

    }

    //根据偏差数值，评估补偿数值
    for(int i=0;i<columnCout-1;i++)
    {
        if(piancha[i]<1 && piancha[i]>-1 &&
                ui->tableWidget_2->item(0,i)!=0 &&
                ui->tableWidget_2->item(1,i)!=0)
        {
            buchang[i]=-piancha[i];
            buchangZong+=buchang[i];
            count++;
            str3=QString::number(buchang[i],'g',6);
            ui->tableWidget_2->setItem(3,i,new QTableWidgetItem(str3));
            ui->tableWidget_2->setItem(4,i,new QTableWidgetItem("补偿"));
            ui->tableWidget_2->item(4,i)->setTextColor(Qt::green);
        }
        else
        {
            buchang[i]=0;
            ui->tableWidget_2->setItem(4,i,new QTableWidgetItem("不补偿"));
            ui->tableWidget_2->item(4,i)->setTextColor(Qt::red);
        }
    }
    if(count !=0)
    {
        buchangZong=buchangZong/count;
        str4=QString::number(buchangZong,'g',6);
        ui->tableWidget_2->setItem(3,columnCout-1,new QTableWidgetItem(str4));
        ui->tableWidget_2->setItem(4,columnCout-1,new QTableWidgetItem("补偿"));
        ui->tableWidget_2->item(4,columnCout-1)->setTextColor(Qt::green);
    }
    else
        //        ui->tableWidget_2->setItem(3,columnCout-1,new QTableWidgetItem(str4));
    {
        ui->tableWidget_2->setItem(4,columnCout-1,new QTableWidgetItem("不补偿"));
        ui->tableWidget_2->item(4,columnCout-1)->setTextColor(Qt::red);
    }
}

//角度标定计算
void MainWindow::on_pushButton_2_clicked()
{
    QString str0,str1,str2,str3,str4;
    float data[10][10]={0};
    float piancha[10];
    float buchang[10];
    float buchangZong=0;
    int count=0;
    int columnCout=ui->tableWidget_3->columnCount();//table colunm count


    //计算角度偏差数值
    for(int i=0;i<columnCout;i++)
    {

        if(ui->tableWidget_3->item(0,i)!=0
                && ui->tableWidget_3->item(1,i)!=0)

        {
            str0 = ui->tableWidget_3->item(0,i)->text();
            str1=ui->tableWidget_3->item(1,i)->text();
            data[0][i]=str0.toFloat();
            data[1][i]=str1.toFloat();
            piancha[i]=data[0][i]-data[1][i];
            str2=QString::number(piancha[i],'g',6);
            ui->tableWidget_3->setItem(2,i,new QTableWidgetItem(str2));
        }

    }

    //根据角度数值，评估补偿数值，5度作为约束条件
    for(int i=0;i<columnCout-1;i++)
    {
        if(piancha[i]<5 && piancha[i]>-5 &&
                ui->tableWidget_3->item(0,i)!=0 &&
                ui->tableWidget_3->item(1,i)!=0)
        {
            buchang[i]=-piancha[i];
            buchangZong+=buchang[i];
            count++;
            str3=QString::number(buchang[i],'g',6);
            ui->tableWidget_3->setItem(3,i,new QTableWidgetItem(str3));
            ui->tableWidget_3->setItem(4,i,new QTableWidgetItem("补偿"));
            ui->tableWidget_3->item(4,i)->setTextColor(Qt::green);
        }
        else
        {
            buchang[i]=0;
            ui->tableWidget_3->setItem(4,i,new QTableWidgetItem("不补偿"));
            ui->tableWidget_3->item(4,i)->setTextColor(Qt::red);
        }
    }
    if(count !=0)
    {
        buchangZong=buchangZong/count;
        str4=QString::number(buchangZong,'g',6);
        ui->tableWidget_3->setItem(3,columnCout-1,new QTableWidgetItem(str4));
        ui->tableWidget_3->setItem(4,columnCout-1,new QTableWidgetItem("补偿"));
        ui->tableWidget_3->item(4,columnCout-1)->setTextColor(Qt::green);
    }
    else
        //        ui->tableWidget_2->setItem(3,columnCout-1,new QTableWidgetItem(str4));
    {
        ui->tableWidget_3->setItem(4,columnCout-1,new QTableWidgetItem("不补偿"));
        ui->tableWidget_3->item(4,columnCout-1)->setTextColor(Qt::red);
    }

}

void MainWindow::on_pushButton_3_clicked()
{
    QString str0,str1,str2,str3,str4,str5;
    QString str[10];
    float junfangcha[10]={0};

    float jingdu[3]={0};
    float temp=0;

    //均方差计算
    //距离 角度 速度
    for(int j=0;j<9;j++)//3列
    {
        if(ui->tableWidget_4->item(0,j)!=0)//距离真值存在
        {
            str0=ui->tableWidget_4->item(0,j)->text();
            float juli1Zhen=str0.toFloat();
            float juli1;
            float fangchahe1=0;
            int count=0;
            for(int i=1;i<8;i++)
            {
                if(ui->tableWidget_4->item(i,j)!=0)
                {
                    str[i]=ui->tableWidget_4->item(i,j)->text();
                    juli1=str[i].toFloat();
                    fangchahe1+=(juli1-juli1Zhen)*(juli1-juli1Zhen);
                    count++;
                }

            }
            if(count>=1)
            {
                junfangcha[j]=sqrt(fangchahe1/count);
                str1=QString::number(junfangcha[j],'g',6);
                ui->tableWidget_4->setItem(8,j,new QTableWidgetItem(str1));
            }
            else
            {
                ui->tableWidget_4->setItem(8,j,new QTableWidgetItem("数据不符"));
            }
        }
        else
        {
            //            ui->tableWidget_4->setItem(8,j,new QTableWidgetItem("数据不符"));
        }
    }

    //精度值计算及打印 距离

    if(ui->tableWidget_4->item(9,0)!=0)
    {
        str2=ui->tableWidget_4->item(9,0)->text();
        temp=str2.toFloat();
        temp=junfangcha[0]*temp;
        jingdu[0]+=temp;
    }
    if(ui->tableWidget_4->item(9,1)!=0)
    {
        str2=ui->tableWidget_4->item(9,1)->text();
        jingdu[0]+=junfangcha[1]*str2.toFloat();
    }
    if(ui->tableWidget_4->item(9,2)!=0)
    {
        str2=ui->tableWidget_4->item(9,2)->text();
        jingdu[0]+=junfangcha[2]*str2.toFloat();
    }
    str3=QString::number(jingdu[0],'g',6);
    ui->tableWidget_4->setItem(10,1,new QTableWidgetItem(str3));

    //精度值计算及打印 角度

    if(ui->tableWidget_4->item(9,0+3)!=0)
    {
        str2=ui->tableWidget_4->item(9,0+3)->text();
        temp=str2.toFloat();
        temp=junfangcha[0+3]*temp;
        jingdu[1]+=temp;
    }
    if(ui->tableWidget_4->item(9,1+3)!=0)
    {
        str2=ui->tableWidget_4->item(9,1+3)->text();
        jingdu[1]+=junfangcha[1+3]*str2.toFloat();
    }
    if(ui->tableWidget_4->item(9,2+3)!=0)
    {
        str2=ui->tableWidget_4->item(9,2+3)->text();
        jingdu[1]+=junfangcha[2+3]*str2.toFloat();
    }
    str4=QString::number(jingdu[1],'g',6);
    ui->tableWidget_4->setItem(10,1+3,new QTableWidgetItem(str4));

    //精度值计算及打印 速度

    if(ui->tableWidget_4->item(9,0+6)!=0)
    {
        str2=ui->tableWidget_4->item(9,0+6)->text();
        temp=str2.toFloat();
        temp=junfangcha[0+6]*temp;
        jingdu[2]+=temp;
    }
    if(ui->tableWidget_4->item(9,1+6)!=0)
    {
        str2=ui->tableWidget_4->item(9,1+6)->text();
        jingdu[2]+=junfangcha[1+6]*str2.toFloat();
    }
    if(ui->tableWidget_4->item(9,2+6)!=0)
    {
        str2=ui->tableWidget_4->item(9,2+6)->text();
        jingdu[2]+=junfangcha[2+6]*str2.toFloat();
    }
    str5=QString::number(jingdu[2],'g',6);
    ui->tableWidget_4->setItem(10,1+6,new QTableWidgetItem(str5));

    //评估结果显示
    ui->tableWidget_4->setItem(11,0,new QTableWidgetItem("标准：2m"));
    ui->tableWidget_4->setItem(11,0+3,new QTableWidgetItem("标准：5度"));
    ui->tableWidget_4->setItem(11,0+6,new QTableWidgetItem("标准：5km/h"));

    //距离部分
    if(jingdu[0]!=0)
    {
        ui->tableWidget_4->setItem(11,1,new QTableWidgetItem("实验数据有效"));//请确认实验数据或再次标定设备
        ui->tableWidget_4->item(11,1)->setTextColor(Qt::green);

        if(jingdu[0]>2)
        {
            ui->tableWidget_4->setItem(11,2,new QTableWidgetItem("超过标准"));//请确认实验数据或再次标定设备
            ui->tableWidget_4->item(11,2)->setTextColor(Qt::red);
        }
        else
        {
            ui->tableWidget_4->setItem(11,2,new QTableWidgetItem("符合标准"));//请确认实验数据或再次标定设备
            ui->tableWidget_4->item(11,2)->setTextColor(Qt::green);
        }
    }

    else
    {
        ui->tableWidget_4->setItem(11,1,new QTableWidgetItem("实验数据无效"));//请确认实验数据或再次标定设备
        ui->tableWidget_4->item(11,1)->setTextColor(Qt::red);
    }

    //角度部分
    if(jingdu[1]!=0)
    {
        ui->tableWidget_4->setItem(11,1+3,new QTableWidgetItem("实验数据有效"));//请确认实验数据或再次标定设备
        ui->tableWidget_4->item(11,1+3)->setTextColor(Qt::green);

        if(jingdu[0]>2)
        {
            ui->tableWidget_4->setItem(11,2+3,new QTableWidgetItem("超过标准"));//请确认实验数据或再次标定设备
            ui->tableWidget_4->item(11,2+3)->setTextColor(Qt::red);
        }
        else
        {
            ui->tableWidget_4->setItem(11,2+3,new QTableWidgetItem("符合标准"));//请确认实验数据或再次标定设备
            ui->tableWidget_4->item(11,2+3)->setTextColor(Qt::green);
        }
    }

    else
    {
        ui->tableWidget_4->setItem(11,1+3,new QTableWidgetItem("实验数据无效"));//请确认实验数据或再次标定设备
        ui->tableWidget_4->item(11,1+3)->setTextColor(Qt::red);
    }

    //速度部分
    if(jingdu[2]!=0)
    {
        ui->tableWidget_4->setItem(11,1+6,new QTableWidgetItem("实验数据有效"));//请确认实验数据或再次标定设备
        ui->tableWidget_4->item(11,1+6)->setTextColor(Qt::green);

        if(jingdu[0]>2)
        {
            ui->tableWidget_4->setItem(11,2+6,new QTableWidgetItem("超过标准"));//请确认实验数据或再次标定设备
            ui->tableWidget_4->item(11,2+6)->setTextColor(Qt::red);
        }
        else
        {
            ui->tableWidget_4->setItem(11,2+6,new QTableWidgetItem("符合标准"));//请确认实验数据或再次标定设备
            ui->tableWidget_4->item(11,2+6)->setTextColor(Qt::green);
        }
    }

    else
    {
        ui->tableWidget_4->setItem(11,1+6,new QTableWidgetItem("实验数据无效"));//请确认实验数据或再次标定设备
        ui->tableWidget_4->item(11,1+6)->setTextColor(Qt::red);
    }
}

void MainWindow::on_DistButton_clicked()
{
    QString str;
    str = ui->tableWidget_2->item(3,6)->text();
    distCompensation = str.toFloat();
            emit sendCompensationData(distCompensation ,angelCompensation);
//    qDebug()<<distCompensation;
}

void MainWindow::on_AngelButton_clicked()
{
    QString str;
    str = ui->tableWidget_3->item(3,6)->text();
    angelCompensation = str.toFloat();
                emit sendCompensationData(distCompensation ,angelCompensation);
//    qDebug()<<angelCompensation;
}
