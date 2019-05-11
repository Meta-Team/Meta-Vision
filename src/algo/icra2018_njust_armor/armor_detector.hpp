#ifndef ALGO_ICRA2018_NJUST_ARMOR_DETECTOR_HPP
#define ALGO_ICRA2018_NJUST_ARMOR_DETECTOR_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "../../global.hpp"

//#include "armor_detector.hpp"
using namespace cv;
using namespace std;

namespace ICRA2018_NJUST_Armor {
    #define POINT_DIST(p1,p2) sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y))

    struct ArmorParam {
        int min_light_gray;	            // 板灯最小灰度值
        int min_light_height;			// 板灯最小高度值
        int avg_contrast_threshold;	    // 对比度检测中平均灰度差阈值，大于该阈值则为显著点
        int light_slope_offset;		    // 允许灯柱偏离垂直线的最大偏移量，单位度
        int max_light_delta_h;          // 左右灯柱在水平位置上的最大差值，像素单位
        int min_light_delta_h;		    // 左右灯柱在水平位置上的最小差值，像素单位
        int max_light_delta_v;		    // 左右灯柱在垂直位置上的最大差值，像素单位
        int max_light_delta_angle;	    // 左右灯柱在斜率最大差值，单位度
        int avg_board_gray_threshold;   // 矩形区域平均灰度阈值，小于该阈值则选择梯度最小的矩阵
        int avg_board_grad_threshold;   // 矩形区域平均梯度阈值，小于该阈值则选择梯度最小的矩阵
        int grad_threshold;			    // 矩形区域梯度阈值，在多个矩形区域中选择大于该阈值像素个数最少的区域  (not used)
        int br_threshold;				// 红蓝通道相减后的阈值

        ArmorParam(){
            min_light_gray = 210;
            min_light_height = 8;
            avg_contrast_threshold = 110;
            light_slope_offset = 30;
            max_light_delta_h = 450;
            min_light_delta_h = 12;
            max_light_delta_v = 50;
            max_light_delta_angle = 30;
            avg_board_gray_threshold = 80;
            avg_board_grad_threshold = 25;
            grad_threshold = 25;
            br_threshold = 30;
        }
    };

    class ArmorDetector {
    public:
        ArmorDetector(const ArmorParam & para = ArmorParam()){
            _para = para;
            _res_last = cv::RotatedRect();
            _dect_rect = cv::Rect();
            _lost_cnt = 0;
            _is_lost = true;
        }
        void setPara(const ArmorParam & para) {
            _para = para;
        }
        void initTemplate(const cv::Mat & _template);
        cv::RotatedRect getTargetAera(const cv::Mat & src);
        void setLastResult(const cv::RotatedRect & rect){
            _res_last = rect;
        }
        const cv::RotatedRect & getLastResult() const{
            return _res_last;
        }

        void setEnemyColor(int enemyColor) {
            _enemy_color = enemyColor;
        }
        
    private:
        /**
         * @brief setImage Pocess the input (set the green component and sub of blue and red component)
         * @param src
         */
        void setImage(const cv::Mat & src);

        /**
         * @brief templateDist Compute distance between template and input image
         * @param img input image
         * @param is_smarect_pnp_solverll true if input image is a samll armor, otherwise, false
         * @return distance
         */
        int templateDist(const cv::Mat & img);

        /**
         * @brief findContourInEnemyColor Find contour in _max_color
         * @param left output left contour image (probably left lamp of armor)
         * @param right output righe edge (probably right lamp of armor)
         * @param contours_left output left contour
         * @param contours_right output right contour
         */
        void findContourInEnemyColor(
            cv::Mat & left, cv::Mat & right,
            vector<vector<cv::Point2i> > &contours_left,
            vector<vector<cv::Point2i> > &contours_right);

        /**
         * @brief findTargetInContours Find target rectangles
         * @param contours_left input left contour
         * @param contours_right input right contour
         * @param rects target rectangles (contains wrong area)
         */
        void findTargetInContours(
            const vector<vector<cv::Point> > & contours_left,
            const vector<vector<cv::Point> > & contours_right,
            vector<cv::RotatedRect> & rects,
            vector<double> & score);

        /**
         * @brief chooseTarget Choose the most possible rectangle among all the rectangles
         * @param rects candidate rectangles
         * @return the most likely armor (RotatedRect() returned if no proper one)
         */
        cv::RotatedRect chooseTarget(const vector<cv::RotatedRect> & rects,
                                     const vector<double> & score);

        /**
         * @brief boundingRRect Bounding of two ratate rectangle (minumum area that contacts two inputs)
         * @param left left RotatedRect
         * @param right right RotatedRect
         * @return minumum area that contacts two inputs
         */
        cv::RotatedRect boundingRRect(const cv::RotatedRect & left,
                                      const cv::RotatedRect & right);

        /**
         * @brief adjustRRect Adjust input angle
         * @param rect input
         * @return adjusted rotate rectangle
         */
        cv::RotatedRect adjustRRect(const cv::RotatedRect & rect);

        bool makeRectSafe(cv::Rect & rect, cv::Size size){
            if (rect.x < 0)
                rect.x = 0;
            if (rect.x + rect.width > size.width)
                rect.width = size.width - rect.x;
            if (rect.y < 0)
                rect.y = 0;
            if (rect.y + rect.height > size.height)
                rect.height = size.height - rect.y;
            if (rect.width <= 0 || rect.height <= 0)
                return false;
            return true;
        }

        bool broadenRect(cv::Rect & rect, int width_added, int height_added, cv::Size size){
            rect.x -= width_added;
            rect.width += width_added * 2;
            rect.y -= height_added;
            rect.height += height_added * 2;
            return makeRectSafe(rect, size);
        }

    public:
        bool _is_lost;
        int _lost_cnt;
        cv::RotatedRect _res_last;      // last detect result
        cv::Rect _dect_rect;            // detect reigon of original image
        ArmorParam _para;               // parameter of alg
        int _enemy_color;               // color of enemy, see global.hpp
        cv::Mat _src;                   // source image around the interesting reigon according to last result
        cv::Mat _g;                     // green component of source image
        cv::Mat _ec;                    // enemy color component of source image
        cv::Mat _max_color;             // binary image of sub between blue and red component
        cv::Mat _binary_template;       // armor template binary image
    };
}

#endif // ALGO_ICRA2018_NJUST_ARMOR_DETECTOR_HPP
