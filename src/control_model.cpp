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
#include "control_model.h"
#include "basic_tool.h"
#include "usb_capture_with_thread.h"
using namespace cv;

ControlModel::ControlModel(){}

ControlModel::~ControlModel(){}

void ControlModel::init(RobotModel* robotModel){
    pRobotModel=robotModel;
    //配置文件
    usleep(1000000);//等待1s，等摄像头稳定
    //初始模式初始化
    mSetMode=ROBOT_MODE_TRACKBALL;
}

//串口数据接收处理入口
void ControlModel::serialListenDataProcess(SerialPacket recvPacket) {
//复杂自定义数据包，需要自定义析单独处理
    pRobotModel->DataUpdate(recvPacket);
}

void ControlModel::processFSM(){
    //模式切换预处理
    if(mSetMode!=pRobotModel->getCurrentMode()){
        pRobotModel->setCurrentMode(mSetMode);
        switch (mSetMode){
            case ROBOT_MODE_TRACKBALL:{
                pid_x.init(0.001,0,0,0.0f,3,0,AUTOMATIC,DIRECT);
                pid_y.init(0.001,0,0,0.0f,3,0,AUTOMATIC,DIRECT);
                pid_z.init(0.001,0,0,0.0f,3,0,AUTOMATIC,DIRECT);
                pid_x.PIDSetpointSet(0);
                pid_y.PIDSetpointSet(0);
                pid_z.PIDSetpointSet(500);
                cout<<"[control model mode ]:Switch to BALL TRACKING Mode!"<<endl;
                break;
            }
            case ROBOT_MODE_RETURN:{
                cout<<"[control model mode ]:Switch to RETURN Mode!"<<endl;
                break;
            }
            case ROBOT_MODE_EMPTY:{
                cout<<"[control model mode ]:Do Nothing"<<endl;
                break;
            }
        }
    }

    switch(pRobotModel->getCurrentMode()){
        case ROBOT_MODE_TRACKBALL:{
            trackBall();
            break;
        }
        case ROBOT_MODE_RETURN:{

            break;
        }
        
    }

}

void ControlModel::trackBall(){
    RealsenseInterface* cap = pRobotModel->getRealsenseCpature();
    SerialInterface* interface = pRobotModel->getpSerialInterface();
    Mat src;
    Point3f distance;
    distance.x = distance .y = distance.z = 0xff;
    if(cap->getDepthImg(src) == 0){
        distance = ball_aim.getDistance(src);
        pid_x.PIDInputSet(distance.x);
        pid_y.PIDInputSet(distance.y);
        pid_z.PIDInputSet(distance.z);
        float velocity_x = pid_x.PIDOutputGet();
        float velocity_y = pid_y.PIDOutputGet();
        float velocity_z = pid_z.PIDOutputGet();
        if(abs(distance.x)<50&&abs(distance.y)<50)
            interface->movebyVelocity(velocity_x,velocity_y,velocity_z,0);
        else interface->movebyVelocity(velocity_x,velocity_y,0,0);
        if(distance.z<800)  cout<<"收网!!!!"<<endl;
    }


}
