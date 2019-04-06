#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>

#if defined(__arm__) || defined(__aarch64__)
    #include "sse_to_neon.hpp"
#endif

#include <stdlib.h>
#include "armor_detector.hpp"

using namespace cv;
using namespace std;

#define SKIP_UNSAFE_RECT(rect, max_size) {if (makeRectSafe(rect, max_size) == false) continue;}

namespace ICRA2018_NJUST_Armor {
    void ArmorDetector::setImage(const cv::Mat& src) {
        // cout << __PRETTY_FUNCTION__ << endl;
        const cv::Point& last_result = _res_last.center;
        if(last_result.x == 0 || last_result.y == 0){
            _src = src;
            _dect_rect = Rect(0, 0, src.cols, src.rows);
        } else{
            Rect rect =_res_last.boundingRect();
            int max_half_w = _para.max_light_delta_h * 1.3;
            int max_half_h = 300;

            double scale = 1.8;
            int exp_half_w = min(max_half_w / 2, int(rect.width * scale));
            int exp_half_h = min(max_half_h / 2, int(rect.height * scale));

            int w = min(max_half_w, exp_half_w);
            int h = min(max_half_h, exp_half_h);

            Point center = last_result;
            int x = max(center.x - w, 0);
            int y = max(center.y -h, 0);
            Point lu = Point(x, y);

            x = min(center.x + w, src.cols);
            y = min(center.y + h, src.rows);
            Point rd = Point(x, y);

            _dect_rect = Rect(lu, rd);
            if (makeRectSafe(_dect_rect, src.size()) == false){
                _res_last = cv::RotatedRect();
                _dect_rect = Rect(0, 0, src.cols, src.rows);
                _src = src;
            } else {
                src(_dect_rect).copyTo(_src);
            }
        }

        int total_pixel = _src.cols * _src.rows;
        const uchar * ptr_src = _src.data;
        const uchar * ptr_src_end = _src.data + total_pixel * 3;

        _g.create(_src.size(), CV_8UC1);
        _ec.create(_src.size(), CV_8UC1);
        _max_color = cv::Mat(_src.size(), CV_8UC1, cv::Scalar(0));
        uchar *ptr_g = _g.data, *ptr_ec = _ec.data, *ptr_max_color = _max_color.data;

        for(; ptr_src != ptr_src_end; ++ptr_src, ++ptr_g, ++ptr_max_color, ++ptr_ec){
            uchar b = *ptr_src;
            uchar g = *(++ptr_src);
            uchar r = *(++ptr_src);
            *ptr_g = g;
            *ptr_ec = (_enemy_color == RED) ? r : b;
            //*ptr_g = b;
            if (r > _para.min_light_gray) {
                *ptr_max_color = 255;
            }
            //if (r - b > _para.br_threshold && r >= g)
            //   *ptr_max_color = 255;
        }
    }

    void ArmorDetector::initTemplate(const Mat &_template, const Mat &_template_small){
        // cout << __PRETTY_FUNCTION__ << endl;
        vector<cv::Mat> bgr;
        bgr.resize(3);
        Mat temp_green;

        split(_template_small, bgr);
        resize(bgr[1], temp_green, Size(100, 25));
        cv::threshold(temp_green, _binary_template_small, 128, 255, THRESH_OTSU);

        split(_template, bgr);
        resize(bgr[1], temp_green, Size(100, 25));
        cv::threshold(temp_green, _binary_template, 128, 255, THRESH_OTSU);
    }

    int ArmorDetector::templateDist(const Mat &img, bool is_small){
        // cout << __PRETTY_FUNCTION__ << endl;
        int dist = 0;
        const uchar threshold_value = _para.min_light_gray - 15;
        int total_pixel = img.rows * img.cols;
        const uchar * p1 = is_small ? _binary_template_small.data :_binary_template.data;
        const uchar * p2 = img.data;
        for(int i = 0; i < total_pixel;  ++i, p1+=1, p2+=3){
            uchar v = (*p2+1) > threshold_value ? 255 : 0;//green component
            dist += (*p1) == v ? 0 : 1;
        }
        return dist;
    }

    void ArmorDetector::findContourInEnemyColor(
        cv::Mat & left, cv::Mat & right,
        vector<vector<Point2i>> &contours_left,
        vector<vector<Point2i>> &contours_right
    ) {
        // cout << __PRETTY_FUNCTION__ << endl;
        vector<vector<Point2i>> contours_br;
        vector<Vec4i> hierarchy;
        findContours(_max_color, contours_br, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        // left lamp template
        // o o x x x x x x
        // o o x x x x x x
        // o o x x x x x x

        // right lamp template
        // x x x x x x o o
        // x x x x x x o o
        // x x x x x x o o

        // margin_o -> col of 'o'
        // margin_x -> col of 'x'
        // margin_h -> row of 'o' or 'x'

        // using average gray value of 'x' minus average gray value of 'o'
        // if the value lager than threshold, the point is consider as a lamp point

        left = Mat::zeros(_max_color.size(), CV_8UC1);
        right = Mat::zeros(_max_color.size(), CV_8UC1);
        const int margin_o = 1, margin_x = 16;
        const int margin_h = 3;

        const __m128i vk0 = _mm_setzero_si128();       // constant vector of all 0s for use with _mm_unpacklo_epi8/_mm_unpackhi_epi8

        for(vector<vector<Point2i>>::const_iterator it = contours_br.begin(); it != contours_br.end(); it++) {
            Rect rect =cv::boundingRect(*it);
            Rect b_rect = rect;
            if (broadenRect(b_rect, 3, 3, _src.size()) == false) continue;
            Scalar m = mean(_src(b_rect));
            if (_enemy_color == RED && (m[0] > m[2] || m[1] > m[2])){
                continue;
            }
            else if(_enemy_color == BLUE && (m[2] > m[0] || m[1] > m[0])){
                continue;
            }

            size_t min_i = rect.x;
            size_t max_i = min(_max_color.cols - margin_o, rect.x + rect.width);
            size_t min_j = rect.y;
            size_t max_j = min(_max_color.rows - margin_h, rect.y + rect.height);

            int count_left = 0, count_right = 0;
            const uchar * ptr_gray_base = _g.data;
            const uchar * ptr_ec_base = _ec.data;

            for (size_t j = min_j; j < max_j; ++j)	{
                size_t offset = j * _g.cols;
                const uchar * ptr_gray = ptr_gray_base + offset;
                const uchar * ptr_ec = ptr_ec_base + offset;
                uchar * ptr_left = left.data + offset;
                uchar * ptr_right = right.data + offset;

                for (size_t i = min_i; i < max_i; ++i){
                    if (*(ptr_ec + i) < _para.min_light_gray)
                        continue;

                    if((*(ptr_left + i) != 0) && (*(ptr_right + i) != 0))
                        continue;

                    int offset_0 = i;
                    int offset_1 = offset_0 + _g.cols;
                    int offset_2 = offset_1 + _g.cols;

                    const uchar * cur_point_0 = ptr_gray + offset_0;
                    const uchar * cur_point_1 = ptr_gray + offset_1;
                    const uchar * cur_point_2 = ptr_gray + offset_2;

                    int sum_o = ((ptr_ec + offset_0)[0]) +
                                ((ptr_ec + offset_1)[0]) +
                                ((ptr_ec + offset_2)[0]);
                    sum_o /= 3;

                    if ((int) i + margin_o + margin_x < _max_color.cols){
                        __m128i vsum = _mm_setzero_si128();
                        __m128i v = _mm_loadu_si128((__m128i*)(cur_point_0 + margin_o));
                        __m128i vl = _mm_unpacklo_epi8(v, vk0);
                        __m128i vh = _mm_unpackhi_epi8(v, vk0);
                        vsum = _mm_add_epi16(vsum, _mm_add_epi16(vl, vh));

                        v = _mm_loadu_si128((__m128i*)(cur_point_1 + margin_o));
                        vl = _mm_unpacklo_epi8(v, vk0);
                        vh = _mm_unpackhi_epi8(v, vk0);
                        vsum = _mm_add_epi16(vsum, _mm_add_epi16(vl, vh));

                        v = _mm_loadu_si128((__m128i*)(cur_point_2 + margin_o));
                        vl = _mm_unpacklo_epi8(v, vk0);
                        vh = _mm_unpackhi_epi8(v, vk0);
                        vsum = _mm_add_epi16(vsum, _mm_add_epi16(vl, vh));

                        int sum_x  = _mm_extract_epi16(vsum,0) + _mm_extract_epi16(vsum,1) + _mm_extract_epi16(vsum,2) + _mm_extract_epi16(vsum,3)+
                                _mm_extract_epi16(vsum,4) + _mm_extract_epi16(vsum,5) + _mm_extract_epi16(vsum,6) + _mm_extract_epi16(vsum,7);

                        int avgdist = sum_o - sum_x/48;
                        // cout << "sum_o, sum_x, avgdist:\t" << sum_o/16 << ", " << sum_x/128 << ", " << avgdist << "\n";
                        if (avgdist > _para.avg_contrast_threshold){
                            *(unsigned char*)(ptr_left+offset_0) = 0xff;
                            *(unsigned char*)(ptr_left+offset_1) = 0xff;
                            *(unsigned char*)(ptr_left+offset_2) = 0xff;

                            count_left += 3;
                            // cout << "sum_x:" << sum_x << "\tsum_o:" << sum_o << endl;
                        }
                    }//End of "if (i +margin_o + margin_x < _max_color.cols)"

                    if (i > margin_x) {
                    //if (*(ptr_right + i) == 0 && i > margin_x) {
                        __m128i vsum = _mm_setzero_si128();
                        __m128i v = _mm_loadu_si128((__m128i*)(cur_point_0 - margin_x));
                        __m128i vl = _mm_unpacklo_epi8(v, vk0);
                        __m128i vh = _mm_unpackhi_epi8(v, vk0);
                        vsum = _mm_add_epi16(vsum, _mm_add_epi16(vl, vh));

                        v = _mm_loadu_si128((__m128i*)(cur_point_1 - margin_x));
                        vl = _mm_unpacklo_epi8(v, vk0);
                        vh = _mm_unpackhi_epi8(v, vk0);
                        vsum = _mm_add_epi16(vsum, _mm_add_epi16(vl, vh));

                        v = _mm_loadu_si128((__m128i*)(cur_point_2 - margin_x));
                        vl = _mm_unpacklo_epi8(v, vk0);
                        vh = _mm_unpackhi_epi8(v, vk0);
                        vsum = _mm_add_epi16(vsum, _mm_add_epi16(vl, vh));

                        int sum_x = _mm_extract_epi16(vsum,0) + _mm_extract_epi16(vsum,1)
                                  + _mm_extract_epi16(vsum,2) + _mm_extract_epi16(vsum,3)
                                  + _mm_extract_epi16(vsum,4) + _mm_extract_epi16(vsum,5)
                                  + _mm_extract_epi16(vsum,6) + _mm_extract_epi16(vsum,7);

                        int avgdist = sum_o - sum_x / 48;
                        // cout << "sum_o, sum_x, avgdist:\t" << sum_o/16 << ", " << sum_x/128 << ", " << avgdist << "\n";
                        if (avgdist > _para.avg_contrast_threshold){
                            *(unsigned char*)(ptr_right+offset_0) = 0xff;
                            *(unsigned char*)(ptr_right+offset_1) = 0xff;
                            *(unsigned char*)(ptr_right+offset_2) = 0xff;

                            count_right += 3;
                            // cout << "sum_x:" << sum_x << "\tsum_o:" << sum_o << endl;
                        }
                    }
                }
            }
        }

        findContours(left, contours_left, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
        findContours(right, contours_right, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

        // cout << "contours_br:" << contours_br.size() << endl;
        // cout << "contours_left:" << contours_left.size() << endl;
        // cout << "contours_right:" << contours_right.size() << endl;
    }

    cv::RotatedRect ArmorDetector::boundingRRect(
        const cv::RotatedRect & left,
        const cv::RotatedRect & right
    ){
        // cout << __PRETTY_FUNCTION__ << endl;
        const Point & pl = left.center, & pr = right.center;
        Point2f center = (pl + pr) / 2.0;
        cv::Size2f wh_l = left.size;
        cv::Size2f wh_r = right.size;
        float width = POINT_DIST(pl, pr) - (wh_l.width + wh_r.width) / 2.0;
        float height = max(wh_l.height, wh_r.height);
//        float height = (wh_l.height + wh_r.height) / 2.0;
        float angle = atan2(right.center.y - left.center.y, right.center.x - left.center.x);
        return RotatedRect(center, Size2f(width, height), angle * 180 / CV_PI);
    }

    RotatedRect ArmorDetector::adjustRRect(const RotatedRect & rect){
        // cout << __PRETTY_FUNCTION__ << endl;
        const Size2f & s = rect.size;
        if (s.width < s.height)
                return rect;
        return RotatedRect(rect.center, Size2f(s.height, s.width), rect.angle + 90.0);
    }

    void ArmorDetector::findTargetInContours(
        const vector<vector<cv::Point> > & contours_left,
        const vector<vector<cv::Point> > & contours_right,
        vector<cv::RotatedRect> & rects,
        vector<double> & score
    ) {
        // cout << __PRETTY_FUNCTION__ << endl;
        // 用直线拟合轮廓，找出符合斜率范围的轮廓
        vector<RotatedRect> final_contour_rect_left, final_contour_rect_right;
        vector<double> score_left, score_right;

        for(size_t i =0; i < contours_left.size(); i++){
            RotatedRect rrect = minAreaRect(contours_left[i]);
            rrect = adjustRRect(rrect);
            double angle = rrect.angle;
            angle = 90 -angle;
            angle = angle < 0 ? angle +180 : angle;

            // the contour must be near-vertical
            float delta_angle = abs(angle - 90);
            if (delta_angle <_para.light_slope_offset){
                final_contour_rect_left.push_back(rrect);
                score_left.push_back(delta_angle);
            }
        }

        for (size_t i = 0; i < contours_right.size(); ++i){
            // fit the lamp contour as a eclipse
            RotatedRect rrect = minAreaRect(contours_right[i]);
            rrect = adjustRRect(rrect);
            double angle = rrect.angle;
            angle = 90 - angle;
            angle = angle < 0 ? angle + 180 : angle;

            // the contour must be near-vertical
            float delta_angle = abs(angle - 90);
            if (delta_angle < _para.light_slope_offset){
                final_contour_rect_right.push_back(rrect);
                score_right.push_back(delta_angle);
            }
        }

        // using all the left edge and right edge to make up rectangles
        for (size_t i = 0; i < final_contour_rect_left.size(); ++i) {
            const RotatedRect & rect_i = final_contour_rect_left[i];
            const Point & center_i = rect_i.center;
            float xi = center_i.x;
            float yi = center_i.y;

            for (size_t j = 0; j < final_contour_rect_right.size(); j++) {
                const RotatedRect & rect_j = final_contour_rect_right[j];
                const Point & center_j = rect_j.center;
                float xj = center_j.x;
                float yj = center_j.y;
                float delta_h = xj - xi;
                float delta_angle = abs(rect_j.angle - rect_i.angle);

                // if rectangle is match condition, put it in candidate vector
                if (delta_h > _para.min_light_delta_h && delta_h < _para.max_light_delta_h &&
                    abs(yi - yj) < _para.max_light_delta_v &&
                    delta_angle < _para.max_light_delta_angle) {
                        RotatedRect rect = boundingRRect(rect_i, rect_j);
                        rects.push_back(rect);
                        score.push_back((score_right[j] + score_left[i]) / 6.0 + delta_angle);
                }
            }
        }
    }

    cv::RotatedRect ArmorDetector::chooseTarget(
        const vector<cv::RotatedRect> & rects,
        const vector<double> & score
    ) {
        // cout << __PRETTY_FUNCTION__ << endl;
        if (rects.size() < 1){
            _is_lost = true;
            return RotatedRect();
        }

        int ret_idx = -1;
        double avg_score = 0.0;
        for(size_t i = 0; i < score.size(); ++i){
            avg_score += score[i];
        }
        avg_score /= score.size();
        double template_dist_threshold = 0.20;
        double percent_large_grad_threshold = 0.25;
        double max_wh_ratio = 5.2, min_wh_ratio = 1.25;
        const double avg_slope = 4.0;
        const double degree2rad_scale = 3.1415926 / 180.0;
        const double exp_weight_scale = 15.0;
        if (_is_lost == false){
            template_dist_threshold = 0.4;
            percent_large_grad_threshold = 0.5;
            max_wh_ratio += 0.5;
            min_wh_ratio -= 0.2;
        }
        double weight = template_dist_threshold *percent_large_grad_threshold
                /exp(-(avg_slope + avg_score) * degree2rad_scale * exp_weight_scale);
        bool is_small = true;

        for(size_t i = 0; i < rects.size(); ++i){
            const RotatedRect & rect = rects[i];

            // the ratio of width and height must be matched
            double w = rect.size.width;
            double h = rect.size.height;
            double wh_ratio = w / h;
            if (wh_ratio > max_wh_ratio || wh_ratio < min_wh_ratio)
                continue;

            // width must close to the last result
            const Size2f size_last = _res_last.size;
            if(_is_lost == false && size_last.width > _para.min_light_delta_h){
                double percent = 0.50 * size_last.width;
                if (abs(w - size_last.width) > percent){
                    // cout << "refused 0 : size_last.width: " << size_last.width << "\tcur width: "  << w << endl;
                    continue;
                }
            }

            // rotate the area
            int lamp_width = max((int)w / 12, 1);
            cv::Rect bounding_roi = rect.boundingRect();
            bounding_roi.x -= w / 8;
            bounding_roi.width += w / 4;
            SKIP_UNSAFE_RECT(bounding_roi, _src.size());

            Point2f new_center = rect.center - Point2f(bounding_roi.x, bounding_roi.y);
            Mat roi_src = _src(bounding_roi);
            Mat rotation = getRotationMatrix2D(new_center, rect.angle, 1);
            Mat rectify_target;
            cv::warpAffine(roi_src, rectify_target, rotation, bounding_roi.size());

            // get the black board of the armor
            cv::Point ul = Point(max(int(new_center.x - (w / 2.0)) + 1, 0), max((int)(new_center.y - h / 2.0), 0));
            cv::Point dr = Point(new_center.x + w / 2.0, new_center.y + h / 2.0);
            Rect roi_black = Rect(cv::Point(ul.x, ul.y), cv::Point(dr.x, dr.y));
            // get the left lamp and right lamp of the armor
            Rect roi_left_lamp = Rect(Point(max(0, ul.x - lamp_width), ul.y), Point(max(rectify_target.cols, ul.x), dr.y));
            Rect roi_right_lamp = Rect(Point(dr.x , ul.y), Point(min(dr.x + lamp_width, rectify_target.cols), dr.y));

            SKIP_UNSAFE_RECT(roi_left_lamp, rectify_target.size());
            SKIP_UNSAFE_RECT(roi_right_lamp, rectify_target.size());
            SKIP_UNSAFE_RECT(roi_black, rectify_target.size());

            // valid the gray value of black area
            Mat black_part;
            rectify_target(roi_black).copyTo(black_part);
            int black_side = min(_para.min_light_delta_h / 2, 4);
            Mat gray_mid_black(Size(roi_black.width - black_side * 2, roi_black.height), CV_8UC1);
            const uchar * ptr = black_part.data;
            uchar * ptr_gray = gray_mid_black.data;
            int avg_green_mid = 0;
            int avg_red_side = 0;
            int avg_blue_side = 0;
            int avg_green_side = 0;
            int cf = black_part.cols - black_side;
            for(int j = 0; j < black_part.rows; ++j){
                for (int k = 0; k < black_side; ++k, ++ptr){
                    uchar b = *ptr;
                    uchar g = *(++ptr);
                    uchar r = *(++ptr);
                    avg_red_side += r;
                    avg_blue_side += b;
                    avg_green_side += g;
                }
                for (int k = black_side; k < cf; ++k, ++ptr, ++ptr_gray){
                    uchar b = *ptr;
                    uchar g = *(++ptr);
                    uchar r = *(++ptr);
                    avg_green_mid += g;
                    *ptr_gray = (uchar)((r * 38 + g * 75 + b * 15) >> 7);
                }
                for (int k = cf; k < black_part.cols; ++k, ++ptr){
                    uchar b = *ptr;
                    uchar g = *(++ptr);
                    uchar r = *(++ptr);
                    avg_red_side += r;
                    avg_blue_side += b;
                    avg_green_side += g;
                }
            }
            avg_green_mid /= (gray_mid_black.cols * gray_mid_black.rows);
            int side_total = black_side * 2 * gray_mid_black.rows;
            avg_green_side /= side_total;
            if (avg_green_mid > _para.avg_board_gray_threshold){
                // cout << "refused 1 : avg_green: " << avg_green_mid << "\tavg_board_gray_threshold: "  << (int)_para.avg_board_gray_threshold << endl;
                continue;
            }

            if (_enemy_color == RED && avg_red_side - 10 < avg_blue_side){
                // cout << "refused 1.1 : red < blue:  red:" << avg_red_side/side_total << "\tblue: "  << avg_blue_side/side_total << endl;
                continue;
            } else if (_enemy_color == BLUE && avg_blue_side - 10 < avg_red_side){
                // cout << "refused 1.2 : red > blue:  red:" << avg_red_side/side_total << "\tblue: "  << avg_blue_side/side_total << endl;
                continue;
            }

            // valid the gradient of the black area
            Mat gradX, gradY;
            cv::Sobel(gray_mid_black, gradX, CV_16S, 1, 0);
            cv::Sobel(gray_mid_black, gradY, CV_16S, 0, 1);

            int y_grad = 0, x_grad = 0;
            int large_grad_count = 0;
            int side_width = gray_mid_black.cols * 10.0 / 100; // jump over the side；
            short * ptr_x = (short *)gradX.data;
            short * ptr_y = (short *)gradY.data;
            for (int j = 0; j < gradX.rows; ++j){
                // jump over left side part
                ptr_x+= side_width;
                ptr_y+= side_width;

                // compute the middle part
                size_t up_b = gradX.cols - side_width;
                for (size_t k = side_width; k < up_b; ++k, ++ptr_x, ++ptr_y)	{
                    int x = abs(*ptr_x);
                    int y = abs(*ptr_y);
                    x_grad += x;
                    y_grad += y;
                    large_grad_count += y / _para.grad_threshold;
                }
                // jump over right side part
                ptr_x+= gradX.cols-up_b;
                ptr_y+= gradX.cols-up_b;
            }

            // kick out the area of large gradients
            int total_pixel = (gradX.cols - (side_width << 1)) * gradX.rows;
            double large_grad_percent =(double)large_grad_count/total_pixel;
            if(large_grad_percent > percent_large_grad_threshold){
                // cout << "refused 2: large_grad_percent: " << large_grad_percent <<  endl;
                continue;
            }


            // valid the average gradient of the black area
            double avg_x = (double)x_grad / total_pixel;
            double avg_y = (double)y_grad / total_pixel;
            if (avg_x < _para.avg_board_grad_threshold + 30 && avg_y < _para.avg_board_grad_threshold ){
                Point p1(roi_left_lamp.x, roi_left_lamp.y);
                Point p2(roi_right_lamp.x+roi_right_lamp.width, roi_right_lamp.y+roi_right_lamp.height);
                // get the whole area of armor
                Rect armor_rect(p1, p2);
                SKIP_UNSAFE_RECT(armor_rect, rectify_target.size());
                Mat armor = rectify_target(armor_rect);

                // compute the distance of template
                cv::Size cur_size;
                if (is_small) {
                    cur_size = _binary_template_small.size();
                } else {
                    cur_size = _binary_template.size();
                }

                resize(armor, armor, cur_size);

                double dist = templateDist(armor, is_small);
                dist = dist / (cur_size.width * cur_size.height);
                if(dist >  template_dist_threshold){
                    // cout << "refused 3: dist: " << dist << "\tdist threshold:" << (cur_size.width * cur_size.height) / 4 << endl;
                    continue;
                }

                // choose the best rectangle with minimum (large_grad_percent * dist)
                dist = max(dist, 0.001);
                large_grad_percent = max(large_grad_percent, 0.001);
                double cur_weight = large_grad_percent * dist / exp(-(abs(rect.angle) + score[i]) * degree2rad_scale * exp_weight_scale);
                if(cur_weight < weight){
                    weight = cur_weight;
                    ret_idx = i;
                    _is_small_armor = is_small;
                } else {
                    // cout << "refused 4: cur_weight: " << cur_weight << "\tweight threshold:" << weight << endl;
                }
            } else {
                // cout << "refused 3: (x_grad, y_grad): (" << avg_x << ", " << avg_y << ")\t avg_grad_threshold: " <<  (int)_para.avg_board_grad_threshold << endl;
            }
        }

        if (ret_idx == -1){
            _is_lost = true;
            return RotatedRect();
        }
        _is_lost = false;
        // broaden the height of target
        RotatedRect ret_rect = rects[ret_idx];
        Rect ret_rect1 = ret_rect.boundingRect();
        if (broadenRect(ret_rect1, 3, 3, _src.size()) == false) {
            return ret_rect;
        }

        _is_small_armor = true;
        return ret_rect;
    }

    cv::RotatedRect ArmorDetector::getTargetAera(const cv::Mat & src){
        // cout << __PRETTY_FUNCTION__ << endl;
        setImage(src);
        cv::Mat contrast_left, contrast_right;
        vector<vector<Point2i> > contours_left;
        vector<vector<Point2i> > contours_right;
        findContourInEnemyColor(contrast_left, contrast_right, contours_left, contours_right);
        vector<RotatedRect> rects;
        vector<double> score;
        findTargetInContours(contours_left, contours_right, rects, score);
        RotatedRect final_rect = chooseTarget(rects, score);

        if(final_rect.size.width != 0){
            final_rect.center.x += _dect_rect.x;
            final_rect.center.y += _dect_rect.y;
            _res_last = final_rect;
            _lost_cnt = 0;
        } else{
            ++_lost_cnt;

            if (_lost_cnt < 3)
                _res_last.size =Size2f(_res_last.size.width * 2, _res_last.size.height * 1.5);
            else if(_lost_cnt == 6)
                _res_last.size =Size2f(_res_last.size.width * 1.5, _res_last.size.height * 1.5);
            else if(_lost_cnt == 12)
                _res_last.size =Size2f(_res_last.size.width * 1.5, _res_last.size.height * 1.5);
            else if(_lost_cnt == 18)
                _res_last.size =Size2f(_res_last.size.width * 1.5, _res_last.size.height * 1.5);
            else if (_lost_cnt > 60 )
                _res_last = RotatedRect();
        }
        return final_rect;
    }
}