//OpenCV headers
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <stdlib.h>
#include <thread>

#include "settings.hpp"
#include "armor_detect.hpp"
#include "interface.hpp"

using namespace std;
using namespace cv;

namespace RM2018_Xidian_Armor {



    MetaInterface::MetaInterface(const YAML::Node &root) {
        settings = new Settings(root);

        armorDetector = new ArmorDetector(settings->armor);
        armorDetector->initArmorHist(settings->small_armor_pic_file, settings->big_armor_pic_file);

        armorRecorder = new ArmorRecorder;

        cameraInfo = new CameraInfo(settings->camera_info_file);

        angleSolver = new AngleSolver(cameraInfo->camera_matrix, cameraInfo->dist_coeffs, 21.6, 5.4, cameraInfo->z_scale, 20.0, 800.0);
        angleSolver->setRelationPoseCameraPTZ(cameraInfo->r_camera_ptz, cameraInfo->t_camera_ptz, cameraInfo->barrel_ptz_offset_y);

        angleSolverFactory = new AngleSolverFactory;
        angleSolverFactory->setTargetSize(21.6, 5.4, AngleSolverFactory::TARGET_ARMOR);
        angleSolverFactory->setTargetSize(12.4, 5.4, AngleSolverFactory::TARGET_SAMLL_ARMOR);
        angleSolverFactory->setSolver(angleSolver);
    }

    MetaInterface::~MetaInterface() {
        delete settings;
        delete armorDetector;
        delete armorRecorder;
    }

    void MetaInterface::setEnemyColor(int enemyColor) {
        armorDetector->setEnemyColor(enemyColor);
    }

    RotatedRect MetaInterface::analyze(const Mat &src) {

        std::vector<ArmorInfo> armors;
        ArmorInfo finalArmor;
        ArmorPos armorPos;

        if (armorDetector->detect(src, armors) && !armors.empty())
        {
            armorPos = armorRecorder->SelectFinalArmor(armors, *angleSolver, *angleSolverFactory, src, finalArmor);
            // TODO: try to use decoded angle
//            serial_mul::publish2car(armor_pos_, current_yaw, current_pitch);
        }
        else{
            armorPos.reset_pos();
//            serial_mul::publish2car(armor_pos_, current_yaw, current_pitch);
        }

        if (armorPos.Flag)
        {
            armorRecorder->miss_detection_cnt = 0;
            armorRecorder->setLastResult(armorPos, std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() -  armorRecorder->recorder_time).count() / 1000.0);
            std::cout << "Pitch: "<< armorPos.angle_y << ", Yaw " << armorPos.angle_x << endl;
            return finalArmor.rect;
        }
        else {
            armorRecorder->miss_detection_cnt++;
            if (armorRecorder->miss_detection_cnt > 3) armorRecorder->clear();
            return RotatedRect();
        }
    }
}