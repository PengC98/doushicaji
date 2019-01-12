#include "aim.h"
using namespace cv;
using namespace std;

Aim_ball::Aim_ball(){

}

Aim_ball::~Aim_ball(){

}

Point3f Aim_ball::getDistance(Mat depth){
    int a = 0;
    int b = 0;
    Point3f distance;
    for(int i = 0; i <depth.rows; i++){
        for(int j = 0; j < depth.cols; j++){
            if(depth.at<ushort>(i,j) < depth.at<ushort>(a,b) && (depth.at<ushort>(i,j) > 100 && depth.at<ushort>(a,b) > 100)){
                a = i;
                b = j;
            }
        }
    }
    distance.z = float(depth.at<ushort>(a,b));
    distance.x = b - depth.cols/2;
    distance.y = a - depth.rows/2;
    return distance;
}

