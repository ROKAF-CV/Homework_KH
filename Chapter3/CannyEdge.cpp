#define _USE_MATH_DEFINES

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
using namespace std;
using namespace cv;

//https://books.google.co.kr/books?id=O_N8CwAAQBAJ&pg=PT1050&lpg=PT1050&dq=%EC%BA%90%EB%8B%88+%EC%97%90%EC%A7%80+%EB%B9%84%EC%B5%9C%EB%8C%80%EC%96%B5%EC%A0%9C&source=bl&ots=vQR7q8IJ1M&sig=ACfU3U2u9iz2bzKuk20FShHLmo1FCM_zJA&hl=ko&sa=X&ved=2ahUKEwjtud3GmennAhURrZQKHdi-AY4Q6AEwBXoECAkQAQ#v=onepage&q=%EC%BA%90%EB%8B%88%20%EC%97%90%EC%A7%80%20%EB%B9%84%EC%B5%9C%EB%8C%80%EC%96%B5%EC%A0%9C&f=false

/*--------------------------------------------------*/
/*--------------------------------------------------*/
///불완전한 코드입니다.
/*--------------------------------------------------*/
/*--------------------------------------------------*/
void Gaussian(Mat &src, Mat &dst, double sigma);
int RtoD(int dy, int dx);
void nonMax(Mat &strength, Mat &direction);
Mat Tracking(Mat &src, int thresh, int thresl);
void follow_edge(Mat &strength, Mat &visited, Mat &edge, int j, int i, int thresl);
void Cannyedge(Mat &src, Mat &dst, int thresh, int thresl);



int main()
{

	Mat src;
	src = imread("lenna.jpg", IMREAD_GRAYSCALE);
	Mat dst(src.size(), CV_32F);
	Cannyedge(src, dst, 127, 32);

	imshow("origin", src);
	imshow("CannyEdge", dst);

	
	
	
}

void Gaussian(Mat &src, Mat &dst, double sigma)
{
	int k = 6 * sigma;
	if (k % 2 == 0)
		k++;
	int middle = k / 2;
	cout << middle << "\n";
	Mat mask(Size(k, k), CV_32F);

	for (int i = 0; i < mask.cols; i++)
	{
		for (int j = 0; j < mask.rows; j++)
		{
			int y = abs(middle - j);
			int x = abs(middle - i);
			mask.at<float>(j, i) = (1 / (2 * M_PI*pow(sigma, 2))) * exp(-((pow(y, 2) + pow(x, 2)) / (2 * pow(sigma, 2))));
			cout << mask.at<float>(j, i) << "\n";
		}
	}


	for (int i = 1; i < src.cols - 1; i++)
	{
		for (int j = 1; j < src.rows - 1; j++)
		{
			int sum = 0;
			for (int a = -middle; a < middle + 1; a++)
			{
				for (int b = -middle; b < middle + 1; b++)
				{
					sum += mask.at<float>(b + middle, a + middle) * src.at<uchar>(j + b, i + a);
				}
			}
			dst.at<uchar>(j, i) = sum;
		}
	}


}

int RtoD(int dy, int dx)
{
	int qual;
	float degrees = atan2f(dy, dx) * 180 / M_PI;

	if (-22.5 <= degrees < 22.5)
		qual = 0;
	else if (-67.5 <= degrees < -22.5)
		qual = 1;
	else if (-112.5 <= degrees < -67.5)
		qual = 2;
	else if (-157.5 <= degrees < -112.5)
		qual = 3;
	else if( (-180 <= degrees < -157.5) || (157.5 <= degrees <= 180) )
		qual = 0; // 원래는 4
	else if (112.5 <= degrees < 157.5)
		qual = 1; // 원래는 5
	else if (67.5 <= degrees < 112.5)
		qual = 2; // 원래는 6
	else if (22.5 <= degrees < 67.5)
		qual = 3; // 원래는 7
	
	
	//(degrees >= 0 ? degrees : (2 * M_PI + degrees)) * 180 / M_PI; //0~360도로 actan값 계산

	

	return qual;
}

void SobelOper(Mat &src, Mat &strength, Mat &direction)
{
	
	strength = Mat(src.size(), CV_32F);
	direction = Mat(src.size(), CV_8U);
	
	int mx[3][3] = {
		{-1, 0 ,1},
		{-2, 0 ,2},
		{-1, 0, 1}
	};

	int my[3][3] = {
		{ -1, -2, -1 },
		{ 0, 0, 0 },
		{ 1, 2, 1 }
	};
	
	for (int i = 1; i < src.cols - 1; i++)
	{
		for (int j = 1; j < src.rows - 1; j++)
		{
			int dx = 0;
			int dy = 0;
			for (int a = -1; a < 2; a++)
			{
				for (int b = -1; b < 2; b++)
				{
					dx += mx[b + 1][a + 1] * src.at<uchar>(j + b, i + a);
					dy += my[b + 1][a + 1] * src.at<uchar>(j + b, i + a);
				}
			}
			strength.at<float>(j,i) = sqrt(pow(dx, 2) + pow(dy, 2));
			direction.at<uchar>(j,i) = RtoD(dy, dx);
		}
	}

}



void nonMax(Mat &strength, Mat &direction)
{
	uchar dec = direction.at<uchar>(240, 240);
	for (int i = 1; i < direction.cols-1; i++)
	{
		for (int j = 1; j < direction.rows - 1; j++)
		{

			uchar dir = direction.at<uchar>(j, i);
			switch (dir){
			case 0:
				if ((strength.at<uchar>(j, i) <= strength.at<uchar>(j - 1, i)) ||
					(strength.at<uchar>(j, i) <= strength.at<uchar>(j + 1, i)) )
						strength.at<uchar>(j, i) = 0;
				break;
			case 1:
				if ((strength.at<uchar>(j, i) <= strength.at<uchar>(j - 1, i + 1)) ||
					(strength.at<uchar>(j, i) <= strength.at<uchar>(j + 1, i - 1)))
					strength.at<uchar>(j, i) = 0;
				break;
			case 2:
				if ((strength.at<uchar>(j, i) <= strength.at<uchar>(j, i-1)) ||
					(strength.at<uchar>(j, i) <= strength.at<uchar>(j, i+1)))
					strength.at<uchar>(j, i) = 0;
				break;
			case 3:
				if ((strength.at<uchar>(j, i) <= strength.at<uchar>(j - 1, i - 1)) ||
					(strength.at<uchar>(j, i) <= strength.at<uchar>(j + 1, i + 1)))
					strength.at<uchar>(j, i) = 0;
				break;

			}
		}
		
	}
}

Mat Tracking(Mat &strength, int thresh, int thresl)
{
	Mat edge(strength.size(), CV_8U);
	Mat visited(strength.size(), CV_8U);
	for (int i = 0; i < strength.cols; i++)
	{
		for (int j = 0; j < strength.rows; j++)
		{
			edge.at<uchar>(j, i) = 0;
			visited.at<uchar>(j, i) = 0;

		}
	}

	for (int i = 1; i < strength.cols - 1; i++)
	{
		for (int j = 1; j < strength.rows - 1; j++)
		{
			if ((strength.at<uchar>(j, i) > thresh) || (visited.at<uchar>(j, i) == 0))
				follow_edge(strength, visited , edge, j, i, thresl);
		}
	}

	return edge;
}

void follow_edge(Mat &strength, Mat &visited, Mat &edge, int j,int i, int thresl)
{
	visited.at<uchar>(j, i) = 1;
	edge.at<uchar>(j, i) = 255;

	for (int a = -1; a < 2; a++)
	{
		for (int b = -1; b < 2; b++)
		{
			if (((strength.at<uchar>(j + b, i + a)) > thresl) && (visited.at<uchar>(j + b, i + a) == 0))
				follow_edge(strength, visited, edge, j + b, i + a, thresl);
		}
	}

}

void Cannyedge(Mat &src, Mat &dst, int thresh, int thresl)
{
	Mat edge;
	Mat gaus(src.size(), CV_8U);
	Mat strength(src.size(), CV_32F);
	Mat direction(src.size(), CV_8U);

	Gaussian(src, gaus, 0.5);
	SobelOper(gaus, strength, direction);
	nonMax(strength, direction);
	edge = Tracking(strength, thresh, thresl);
	
	dst = edge;
}