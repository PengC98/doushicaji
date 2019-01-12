#include "opencv2/opencv.hpp"
#include <iostream>
#include "pnp_solver.h"
using namespace cv;
class Aim_ball{
    public:
        Aim_ball();
        ~Aim_ball();
    public:  
        Point3f getDistance(Mat depth);
    private:

};