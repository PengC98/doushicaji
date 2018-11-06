////////////////////////////////////////////////////////////////////////////////
///Copyright(c)      UESTC MBZIRC     Model Code for UAV
///ALL RIGHTS RESERVED
///@file:control_model.cpp
///@brief: UAV 控制模型，包含对所有应用的管理，创建应用，并改变应用
/// 
///@vesion 1.0
///@author: PC
///@email: 694977655@qq.com
///@date: 18-11-6
///修订历史：
////////////////////////////////////////////////////////////////////////////////
#include "image_tool.h"
#include "control_model.h"
#include "basic_tool.h"
#include "serial_port_debug.h"
#include "usb_capture_with_thread.h"
using namespace cv;

ControlModel::ControlModel(){}

ControlModel::~ControlModel(){}

void ControlModel::init(RobotModel* robotModel){
    pRobotModel=robotModel;
    autoAim = new AutoAim(1280, 720`);
    //配置文件
    cv::FileStorage f("../res/main_config.yaml", cv::FileStorage::READ);
    f["enemy_is_red"] >> mEnemyIsRed;//自瞄敌方颜色
    f.release();
    usleep(1000000);//等待1s，等摄像头稳定
    //初始模式初始化
    mSetMode=ROBOT_MODE_AUTOAIM;
}

//串口数据接收处理入口
void ControlModel::serialListenDataProcess(SerialPacket recvPacket) {
//复杂自定义数据包，需要自定义析单独处理
    pRobotModel->DataUpdate(recvPacket);
}

void ControlModel::processFSM(){
    //控制部分

}

void ControlModel::Aim(){
        Mat src;
        Point2f current_angle;
        UsbCaptureWithThread cap = pRobotModel->getpUsbCapture();
        if(cap.getImg(src)!=0) cout<<"src is error"<<endl;
        SerialInterface *interface = pRobotModel->getpSerialInterface();
        interface->getAbsYunTaiDelta();
        current_angle.x = pRobotModel->getCurrentPitch();
        current_angle.y = pRobotModel->getCurrentYaw();
        cout<<current_angle<<endl;
        Point2f angle;
        if(autoAim->aim(src, current_angle.x, current_angle.y, angle,0.0)==BaseAim::AIM_TARGET_FOUND){
            interface->YunTaiDeltaSet(angle.x, angle.y);
        }
}
