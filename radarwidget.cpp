#include "radarwidget.h"

RadarWidget::RadarWidget(QWidget *parent) : QWidget(parent) {
    createWidgets();
    qRegisterMetaType<radar_data_package>("radar_data_package");
    radar = new Radar;
    connect(radar, SIGNAL(getData(radar_data_package)), this,
            SLOT(updateWidget(radar_data_package)));

    connect(radar, SIGNAL(deviceWrong()), this, SLOT(dealDeviceWrong()));
    updateCanPortCombobox();
    timer_enable_start = new QTimer;
    connect(timer_enable_start, SIGNAL(timeout()), this, SLOT(enableStart()));

}
RadarWidget::~RadarWidget() {
    delete radar;
}

void RadarWidget::createWidgets() {
    can_port_label = new QLabel("设备选择");
    can_port_combobox = new QComboBox;
    connect(can_port_combobox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(dealCanIndexChanged(int)));
#ifdef WIN32
    can_type_label = new QLabel("设备类型");
    can_type_combobox = new QComboBox;
    can_type_combobox->insertItem(0, "CANalyst-II");
#endif
    can_baud_label = new QLabel("波特率");
    can_baud_combobox = new QComboBox;
    can_baud_combobox->addItem("10Kbps");
    can_baud_combobox->addItem("20Kbps");
    can_baud_combobox->addItem("40Kbps");
    can_baud_combobox->addItem("50Kbps");
    can_baud_combobox->addItem("80Kbps");
    can_baud_combobox->addItem("100Kbps");
    can_baud_combobox->addItem("125Kbps");
    can_baud_combobox->addItem("200Kbps");
    can_baud_combobox->addItem("250Kbps");
    can_baud_combobox->addItem("400Kbps");
    can_baud_combobox->addItem("500Kbps");
    can_baud_combobox->addItem("666Kbps");
    can_baud_combobox->addItem("800Kbps");
    can_baud_combobox->addItem("1000Kbps");
    can_baud_combobox->addItem("33.33Kbps");
    can_baud_combobox->addItem("66.66Kbps");
    can_baud_combobox->addItem("83.33Kbps");
    can_baud_combobox->setCurrentText("500Kbps");
    can_channel_label = new QLabel("通道");
    can_channel_combobox = new QComboBox;
    connnect_light_label = new QLabel("未连接");
    connnect_btn = new QPushButton("连接");
    connect(connnect_btn, SIGNAL(clicked()), this, SLOT(dealConnect()));
//    connect(connnect_btn,SIGNAL(clicked(bool)),this,SLOT(printdate()));
    thresh_label = new QLabel("角度范围");
    thresh_value = new QLabel(QString::number(angle_thresh));
    thresh_btn = new QPushButton("角度设置");
    connect(thresh_btn, SIGNAL(clicked(bool)), this, SLOT(dealThreshChanged()));
    buzzer_label = new QLabel("报警阈值");
    buzzer_value = new QLabel(QString::number(buzVal));
    buzzer_btn = new QPushButton("报警设置");
    connect(buzzer_btn, SIGNAL(clicked(bool)), this,
            SLOT(dealBuzzerMinLongChanged()));


    form_widget = new QWidget(this);
    form_widget->setGeometry(0, 0, 120, 300);
    form_widget->setFixedWidth(120);
    QFormLayout *form_layout = new QFormLayout(form_widget);
    form_layout->addWidget(can_port_label);
    form_layout->addWidget(can_port_combobox);
#ifdef WIN32
    form_layout->addWidget(can_type_label);
    form_layout->addWidget(can_type_combobox);
#endif
    form_layout->addWidget(can_baud_label);
    form_layout->addWidget(can_baud_combobox);
    form_layout->addWidget(can_channel_label);
    form_layout->addWidget(can_channel_combobox);
    form_layout->addWidget(connnect_light_label);
    form_layout->addWidget(connnect_btn);
    start_btn = new QPushButton("启动");
    QPixmap start_icon(":/resource/icon/start.png");
    refresh_btn = new QPushButton("刷新");
    start_btn->setIcon(QIcon(start_icon));
    stop_btn = new QPushButton("停止");
    stop_btn->setIcon(QIcon(":/resource/icon/stop.png"));
    reset_btn = new QPushButton("重置");
    reset_btn->setIcon(QIcon(":/resource/icon/reset.png"));
    connect(refresh_btn, SIGNAL(clicked(bool)), this,
            SLOT(updateCanPortCombobox()));
    connect(start_btn, SIGNAL(clicked(bool)), this, SLOT(dealStart()));
    connect(stop_btn, SIGNAL(clicked(bool)), this, SLOT(dealStop()));
    connect(reset_btn, SIGNAL(clicked(bool)), this, SLOT(dealReset()));
    form_layout->addWidget(refresh_btn);
    form_layout->addWidget(start_btn);
    form_layout->addWidget(stop_btn);
    form_layout->addWidget(reset_btn);
    form_layout->addWidget(thresh_label);
    form_layout->addWidget(thresh_value);
    form_layout->addWidget(thresh_btn);

    form_layout->addWidget(buzzer_label);
    form_layout->addWidget(buzzer_value);
    form_layout->addWidget(buzzer_btn);

    start_btn->setEnabled(false);
    stop_btn->setEnabled(false);
    reset_btn->setEnabled(false);

    content_widget = new RadarPaintWidget;
    QHBoxLayout *main_layout = new QHBoxLayout(this);
    main_layout->addWidget(form_widget);
    main_layout->addWidget(content_widget);
}

void RadarWidget::connectCan() {
    QString can_index = can_port_combobox->currentText();
    if (can_index == "") {
        QMessageBox::warning(this, "警告", "没有选中设备");
        start_btn->setEnabled(false);
        stop_btn->setEnabled(false);
        reset_btn->setEnabled(false);
        return;
    }

    QString can_baud_str = can_baud_combobox->currentText();
    can_baud_str = can_baud_str.remove("Kbps");
    if (can_baud_str == "") {
        QMessageBox::warning(this, "警告", "波特率为空");
        start_btn->setEnabled(false);
        stop_btn->setEnabled(false);
        reset_btn->setEnabled(false);
        return;
    }
    QString can_channel_str = can_channel_combobox->currentText();
    if (can_channel_str == "") {
        QMessageBox::warning(this, "警告", "通道号为空");
        start_btn->setEnabled(false);
        stop_btn->setEnabled(false);
        reset_btn->setEnabled(false);
        return;
    }
#ifdef WIN32
    int ret =
            radar->connect_can(can_type, can_index.toULong(), can_baud_str.toULong(),
                               can_channel_str.toULong());
#endif
#ifdef __unix__
    int ret = radar->connectCan(can_type, can_index.toUInt(),
                                can_baud_str.toUInt(), can_channel_str.toUInt());
#endif
    if (ret == 0) {
        QMessageBox::critical(this, "错误", "打开设备失败");
        start_btn->setEnabled(false);
        stop_btn->setEnabled(false);
        reset_btn->setEnabled(false);
        return;
    }
    if (ret == -1) {
        dealDeviceWrong();
        return;
    }
    connnect_btn->setText("断开");
    QPalette pe = connnect_light_label->palette();
    pe.setColor(QPalette::WindowText, QColor(Qt::red));
    connnect_light_label->setPalette(pe);
    connnect_light_label->setText("已连接");
    start_btn->setEnabled(false);
    stop_btn->setEnabled(true);
    reset_btn->setEnabled(true);
    refresh_btn->setEnabled(false);
    radar->start();
    // setup buzzer
    //  sleep(1);
    content_widget->buzzerEvent(buzVal);
    emit showMsg("radar connect");
    return;
}

void RadarWidget::closeCan() {

    int ret = radar->closeCan();

    connnect_btn->setText("连接");
    QPalette pe = connnect_light_label->palette();
    pe.setColor(QPalette::WindowText, QColor(Qt::black));
    connnect_light_label->setPalette(pe);
    connnect_light_label->setText("未连接");
    start_btn->setEnabled(false);
    stop_btn->setEnabled(false);
    reset_btn->setEnabled(false);
    refresh_btn->setEnabled(true);
    emit showMsg("radar close");
}

void RadarWidget::paintEvent(QPaintEvent *) {}

void RadarWidget::wheelEvent(QWheelEvent *) {}

void RadarWidget::dealConnect() {
    QString btn_text = connnect_btn->text();
    if (btn_text == "连接") {
        connectCan();
        content_widget->buzzerEvent(buzVal);
    } else {
        closeCan();
        //radar_buzzer.speeker(false);
    }
}

//void RadarWidget::printdate(){
//    QString s="sss";
//    textBrowser_12->setText(s);
//}
void RadarWidget::dealStart() {
    radar->start();
    update();
    start_btn->setEnabled(false);
    stop_btn->setEnabled(true);
    emit showMsg("radar start");
    //radar_buzzer.speeker(true);
    // 启动buzzer
    content_widget->buzzerEvent(buzVal);
}

void RadarWidget::dealStop() {
    radar->stop();
    update();
    start_btn->setEnabled(false);
    stop_btn->setEnabled(false);
    timer_enable_start->start(500);
    emit showMsg("radar stop");

    //radar_buzzer.speeker(false);
}

void RadarWidget::dealReset() {
    int ret = QMessageBox::question(this, "确认", "确认重置？");
    switch (ret) {
    case QMessageBox::Yes: {
        ret = radar->reset();
        if (ret == 1) {
            QMessageBox::information(this, "确认", "重置成功");
            emit showMsg("radar reset");
        } else if (ret == 0) {
            QMessageBox::warning(this, "警告", "重置失败");
        } else {
            dealDeviceWrong();
            emit showMsg("radar reset wrong");
        }
        break;
    }
    default:
        break;
    }
}

void RadarWidget::updateCanPortCombobox() {
    if (connnect_light_label->text() == "已连接") return;
    int num = 1;
#ifdef WIN32
    num = VCI_FindUsbDevice2(pInfo);
    if (num == 0) {
        current_can_index = 0;
    } else {
        current_can_index = -1;
        can_channel_combobox->clear();
        int channel_num = pInfo[0].can_Num;
        for (int i = 0; i < channel_num; i++) {
            can_channel_combobox->addItem(QString::number(i + 1));
        }
    }
    can_port_combobox->clear();
    for (int i = 0; i < num; i++) {
        can_port_combobox->addItem(QString::number(i));
    }
#endif
#ifdef __linux__
    can_port_combobox->clear();
    can_channel_combobox->clear();
    can_channel_combobox->addItem(QString::number(0));
    for (int i = 0; i < num + 2; i++) {
        can_port_combobox->addItem(QString::number(i));
    }
#endif
    emit showMsg("update radar can port");
}

void RadarWidget::updateWidget(radar_data_package list) {
    //qDebug() << "get data" << list.objs.size() << list.objs[0].x;
    content_widget->setObjList(list.objs);
    content_widget->update();
    bool is_sp;

    //content_widget->buzzerEvent(buzVal);

    //    for (auto data : list.objs){
    //        if (data.distance_long <= buzVal)
    //        {
    //            radar_buzzer.speeker(true);
    //            break;
    //        }
    //        {
    //            radar_buzzer.speeker(false);
    //        }
    //        qDebug()<<data.distance_long<<endl;
    //    }
    //radar_buzzer.speeker(true);
}

void RadarWidget::enableStart() {
    start_btn->setEnabled(true);
    timer_enable_start->stop();
}

void RadarWidget::dealCanIndexChanged(int index) { current_can_index = index; }

void RadarWidget::dealDeviceWrong() {
    QMessageBox::critical(this, "警告", "设备不存在或USB掉线");
    updateCanPortCombobox();
    connnect_btn->setText("连接");
    QPalette pe = connnect_light_label->palette();
    pe.setColor(QPalette::WindowText, QColor(Qt::black));
    connnect_light_label->setPalette(pe);
    connnect_light_label->setText("未连接");
    start_btn->setEnabled(false);
    stop_btn->setEnabled(false);
    reset_btn->setEnabled(false);
    emit showMsg("radar wrong");
}

void RadarWidget::dealThreshChanged() {
    bool ok;
    int value = QInputDialog::getInt(this, "设置扫描范围", "请输入扫描范围",
                                     angle_thresh, 10, 60, 1, &ok);
    if (ok) {
        angle_thresh = value;
        content_widget->setAngleThresh(angle_thresh);
        thresh_value->setText(QString::number(angle_thresh));
    }
}
void RadarWidget::dealBuzzerMinLongChanged() {
//    bool ok;
//    int value = QInputDialog::getInt(
//                this, "设置最小报警距离", "请输入最小报警距离", buzVal, 0, 200, 2, &ok);
//    if (ok) {
//        buzVal = value;
//        content_widget->buzzerEvent(buzVal);
//        buzzer_value->setText(QString::number(buzVal));
//    }
}




