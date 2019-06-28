#include "opticalFlow.h"

OpticalFlow::OpticalFlow()
{
	int max_corners = 50;
	double quality_level = 0.01;
	double min_dist = 10.0;
}

OpticalFlow::~OpticalFlow()
{
}

OpticalFlow::OpticalFlow(int _max_corners = 50, double _quality_level = 0.01, double _min_dist = 10)
{
	max_corners = _max_corners;
	quality_level = _quality_level;
	min_dist = _min_dist;
}

//-------------------------------------------------------------------------------------------------
// @brief: ����
// @param: frame	�������Ƶ֡
// @return: void
//-------------------------------------------------------------------------------------------------
void OpticalFlow::tracking(Mat &frame)
{
	cvtColor(frame, gray, COLOR_BGR2GRAY);//�˾�����OpenCV2��Ϊ��cvtColor(frame, gray,CV_BGR2GRAY);
	frame.copyTo(output);
	displacement.clear();
	// ���������
	if (addNewPoints())
	{
		goodFeaturesToTrack(gray, features, max_corners, quality_level, min_dist);
		points[0].insert(points[0].end(), features.begin(), features.end());
		initial.insert(initial.end(), features.begin(), features.end());
	}

	if (!features.empty())
	{
		if (gray_prev.empty())
			gray.copyTo(gray_prev);

		// l-k�������˶�����
		calcOpticalFlowPyrLK(gray_prev, gray, points[0], points[1], status, err);

		// ȥ��һЩ���õ�������
		int k = 0;
		for (size_t i = 0; i < points[1].size(); i++)
		{
			if (acceptTrackedPoint(i))
			{
				initial[k] = initial[i];
				points[1][k] = points[1][i];
				k++;
			}
		}
		points[1].resize(k);
		initial.resize(k);

		pointsNum = points[1].size();
		if (pointsNum > 0)
		{
			motionObject = minAreaRect(points[1]);
			movingPoints = points[1];

			// ��ʾ��������˶��켣
			for (size_t i = 0; i < pointsNum; i++)
			{
				displacement.push_back(points[1][i] - initial[i]);
				line(output, initial[i], points[1][i], Scalar(0, 0, 255));
				circle(output, points[1][i], 3, Scalar(0, 255, 0), -1);
			}
		}
		// ���£��ѵ�ǰ���ٽ����Ϊ��һ�βο�
		swap(points[1], points[0]);
		swap(gray_prev, gray);
	}
	else
	{
		cout << "warning:can't find any features!!\n";//��ֹ���Ҳ����������ʱ�򱨴�
	}

}

//-------------------------------------------------------------------------------------------------
// @brief: ����µ��Ƿ�Ӧ�ñ����
// @param:��
// @return: �Ƿ���ӱ�־
//-------------------------------------------------------------------------------------------------
bool OpticalFlow::addNewPoints()
{
	return points[0].size() <= 10;
}

//-------------------------------------------------------------------------------------------------
// @brief: ������Щ���ٵ㱻���ܣ�
// @param:
// @return:
//-------------------------------------------------------------------------------------------------
bool OpticalFlow::acceptTrackedPoint(int i)
{
	return status[i] && ((abs(points[0][i].x - points[1][i].x) + abs(points[0][i].y - points[1][i].y)) > 15);
}


bool OpticalFlow::isDroneAppear()
{
	if (10 * pointsNum > max_corners &&  1.5 * pointsNum < max_corners)
	{
		//if (motionObject.size.height*motionObject.size.width <= 8000)
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}
