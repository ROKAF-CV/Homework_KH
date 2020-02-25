#define _USE_MATH_DEFINES

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

void Gaussian(Mat src, Mat dst, double sigma);

void Lapla(Mat src, Mat dst);

void LOGfilter(Mat src, Mat dst);
void EDetection(Mat dst, Mat detect, int thres);

int main()
{
	Mat src, gauori, abs_dst;

	int thres = 0;
	src = (imread("lenna.jpg", IMREAD_GRAYSCALE));
	
	Mat gaus(src.size(), CV_8U);

	Mat dst(src.size(), CV_8U);
	//Mat detect(src.size(), CV_8U);

	Gaussian(src, gaus, 0.5);
	GaussianBlur(src, gauori, Size(3, 3),0,0 );
	
	LOGfilter(src, dst);
	convertScaleAbs(dst, abs_dst);
	
	//EDetection(dst, detect, thres);

	cout << src.type() << endl;
	imshow("src", src);
	imshow("Gaussian", gaus);
	imshow("Gaussian Original", gauori);

	imshow("dst", abs_dst);
	//imshow("Edge Detection", detect);

	waitKey(0);
	return 0;
}

void Gaussian(Mat src, Mat dst, double sigma)
{
	int k = 6 * sigma;
	if (k % 2 == 0)
		k++;
	int middle = k / 2 ;
	cout << middle << "\n";
	Mat mask(Size(k, k), CV_32F);

	for (int i = 0; i < mask.cols; i++)
	{
		for (int j = 0; j < mask.rows; j++)
		{
			int y = abs(middle  - j);
			int x = abs(middle - i);
			mask.at<float>(j, i) = (1 / (2 * M_PI*pow(sigma, 2) ) ) * exp( - ( (pow(y,2)+ pow(x,2))/(2*pow(sigma,2)) ) );
			cout << mask.at<float>(j, i)<< "\n";
		}
	}
	

	for (int i = 1; i < src.cols - 1; i++)
	{
		for (int j = 1; j < src.rows - 1; j++)
		{
			int sum = 0;
			for (int a = -middle; a < middle+1; a++)
			{
				for (int b = -middle; b < middle+1; b++)
				{
					sum += mask.at<float>(b + middle, a + middle) * src.at<uchar>(j + b, i + a);
				}
			}
			dst.at<uchar>(j, i) = sum;
		}
	}


}


void Lapla(Mat src, Mat dst)
{
	int mask[3][3] = {
		{0, 1, 0},
		{1, -4, 1},
		{0, 1, 0}
	};
	for (int i = 1; i < src.cols - 1; i++)
	{
		for (int j = 1; j < src.rows - 1; j++)
		{
			int sum = 0;
			for (int a = -1; a < 2; a++)
			{
				for (int b = -1; b < 2; b++)
				{
					sum += mask[b+1][a+1] * src.at<uchar>(j + b, i + a);
				}
			}
			dst.at<uchar>(j, i) = saturate_cast<uchar>(sum);
			
		}
	}
}

void LOGfilter(Mat src, Mat dst)
{
	Gaussian(src, src, 0.5);
	Lapla(src, dst);
	

	//GaussianBlur(src, src, Size(3, 3), 0, 0);
	//Laplacian(src, dst, ddepth, 3);

	//convertScaleAbs(dst, abs_dst);

	//return abs_dst;
}

bool IsPositive(Mat dst, int i, int j, int thres)
{ 
	int cnt = 0;

	if (dst.at <uchar>(i - 1, j - 1)*dst.at <uchar>(i + 1, j + 1) < 0)
	{
		//if (abs(dst.at <uchar>(i - 1, j - 1) - dst.at <uchar>(i + 1, j + 1)) > thres)
			cnt++;
	}

	if (dst.at <uchar>(i - 1, j)*dst.at <uchar>(i + 1, j) < 0)
	{
		//if (abs(dst.at <uchar>(i - 1, j) - dst.at <uchar>(i + 1, j)) > thres)
			cnt++;
	}

	if (dst.at <uchar>(i - 1, j + 1)*dst.at <uchar>(i + 1, j - 1) < 0)
	{
		//if (abs(dst.at <uchar>(i - 1, j + 1) - dst.at <uchar>(i + 1, j - 1)) > thres)
			cnt++;
	}

	if (dst.at <uchar>(i, j + 1)*dst.at <uchar>(i, j - 1) < 0)
	{
		//if (abs(dst.at <uchar>(i, j + 1) - dst.at <uchar>(i, j - 1)) > thres)
			cnt++;
	}
	cout << "cnt°ª: " << cnt << endl;
	
	if (cnt >= 2)
	{
		return true;
	}
	else
		return false;
}

void EDetection(Mat dst, Mat detect, int thres)
{
	
	for (int i = 1; i < dst.cols-1; i++)
	{
		for (int j = 1; j < dst.rows-1; j++)
		{
			if (IsPositive(dst, i, j, thres)==true)
				detect.at<uchar>(i, j) = 255;
			else
				detect.at<uchar>(i, j) = 0;
		}
	}
}

