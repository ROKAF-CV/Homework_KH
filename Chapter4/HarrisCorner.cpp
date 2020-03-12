#define _USE_MATH_DEFINES

#include <iostream>
#include <queue>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <cmath>
using namespace std;
using namespace cv;

class Harris{

public:

	void initialize(Mat &src, Mat &dst) //해리스 코너 식을 계산하기 위해 모든 컬러값을 1로 바꾼다.
	{
		for (int i = 1; i<src.cols-1; i++)
		{
			for (int j = 1; j<src.rows-1; j++)
			{
				if (src.at<uchar>(j, i) > 0)
					dst.at<uchar>(j, i) = 1;
				else if (src.at<uchar>(j,i) == 0);
				dst.at<uchar>(j, i) = 0;
			}
		}

	}

	void calcul(Mat &src, Mat &dys, Mat &dxs, Mat &dydx)
	{
		Mat dy = Mat::zeros(src.size(), CV_32S); //dy의 값이 -1이 나올수 있기 때문에 int로 설정
		Mat dx = Mat::zeros(src.size(), CV_32S);


		for (int i = 1; i < src.cols - 1; i++)
		{
			for (int j = 1; j < src.rows - 1; j++)
			{
				dy.at<int>(j, i) = src.at<uchar>(j + 1, i) - src.at<uchar>(j - 1, i);
				dx.at<int>(j, i) = src.at<uchar>(j, i + 1) - src.at<uchar>(j, i - 1);
				dys.at<int>(j, i) = pow(dy.at<int>(j, i), 2);
				dxs.at<int>(j, i) = pow(dx.at<int>(j, i), 2);
				dydx.at<int>(j, i) = (dy.at<int>(j, i))*(dx.at<int>(j, i));

			}
		}

	}
	
	Mat GaussianCalc(Mat &src)
	{

		Mat dst = Mat::zeros(src.size(), CV_64F);
		for (int i = 1; i < src.cols - 1; i++)
		{
			for (int j = 1; j < src.rows - 1; j++)
			{
				for (int a = -1; a < 2; a++)
				{
					for (int b = -1; b < 2; b++)
					{
						dst.at<double>(j + b, i + a) += (src.at<int>(j, i))*(gsort.at<double>(b + 1, a + 1));
							//(gsort[b + 1][a + 1]);
					}
				}
			}
		}
		return dst;
	}


	void MomMat(Mat &src, Mat &dys, Mat &dxs, Mat &dydx)
	{
		
		double p, q, r, trace, det, charter;
		//double Matrix[2][2]; //원래 해리스 코너 함수의 경우 2차 모멘트를 구하지만, 구현하는데 필요하지 않다고 생각되어 주석처리함
		dys = GaussianCalc(dys);
		dxs = GaussianCalc(dxs);
		dydx = GaussianCalc(dydx);

		for (int i = 1; i < src.cols-1; i++)
		{
			for (int j = 1; j < src.rows-1; j++)
			{
				p = dys.at<double>(j, i);
				q = dxs.at<double>(j, i);
				r = dydx.at<double>(j, i);

				trace = p + q;
				det = p*q - pow(r, 2);

				charter = det - 0.04*pow(trace, 2);

				if (charter > 0.02)
				{
					que.push(make_pair(j, i));
				}
			}
		}
		
	}

	void hop(){
		while (!que.empty())
		{
			pair<int, int> n = que.front();
			que.pop();

			cout << "y좌표: " << n.first << ", x좌표: " << n.second << '\n';
		}
	}


private:
	
	Mat gsort = (Mat_<double>(3, 3) << 
		0.0751, 0.1238, 0.0751,
		0.1238, 0.2042, 0.1238,
		0.0751, 0.1238, 0.0751);

	//double gsort[3][3] = {	//가우시안 시그마 1.0;
	//	{ 0.0751, 0.1238, 0.0751 },
	//	{ 0.1238, 0.2042, 0.1238 },
	//	{ 0.0751, 0.1238, 0.0751 } 
	//};

	queue<pair<int, int>> que;

};

void harrisop()
{
	Mat input_image = (Mat_<uchar>(12, 12) <<
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 255, 255, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 255, 255, 255, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 0,
		0, 0, 0, 255, 255, 255, 255, 255, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		);
	//Mat src = imread("lenna.jpg", IMREAD_GRAYSCALE);
	//Mat mora(src.size(), CV_8U);
	Mat dst(input_image.size(), CV_8U);
	Mat dys = Mat(input_image.size(), CV_32S);
	Mat dxs = Mat(input_image.size(), CV_32S);
	Mat dydx = Mat(input_image.size(), CV_32S);
	const int rate = 50;

	Harris ha;
	ha.initialize(input_image, dst);
	ha.calcul(dst, dys, dxs, dydx);
	ha.MomMat(dst, dys, dxs, dydx);

	ha.hop();





	resize(input_image, input_image, Size(), rate, rate, INTER_NEAREST);
	imshow("Original", input_image);


}

int main(void)
{
	harrisop();

	return 0;
}