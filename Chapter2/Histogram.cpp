#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
void countHist(Mat gray, double hist[]);
Mat draw_histogram_gray(double hist[]);


int main()
{
	double hist[256];
	Mat src, gray, dst;
	src = imread("lenna.jpg", IMREAD_COLOR);
	

	cvtColor(src, gray, CV_BGR2GRAY);

	countHist(gray, hist);

	dst = draw_histogram_gray(hist);

	imshow("gray", gray);
	imshow("histogram", dst);
	
	waitKey(0);

	return 0;

	
}

void countHist(Mat gray,  double hist[])
{
	uchar j;
	for (int i = 0; i < 256; i++)
	{
		hist[i] = 0;
	}
	for (int r = 0; r < gray.rows; r++)
	{
		for (int c = 0; c < gray.cols; c++)
		{
			j = gray.at<uchar>(r, c);
			hist[j]++;
		}
	}

	for (int i = 0; i < 256; i++)
	{
		hist[i] = hist[i] / (gray.rows*gray.cols);
	}
	 
}

Mat draw_histogram_gray(double hist[])
{
	double vmax= -1.0;
	Mat himg(256, 256, CV_8U, Scalar(255));

	for (int i = 0; i < 256; i++) vmax = (vmax>hist[i]) ? vmax : hist[i];

	for (int i = 0; i < 256; i++){
		Point p1(i, 256), p2(i, 256 - (int)((hist[i] / vmax)*(0.8 * 256)));
		line(himg, p1, p2, Scalar(0));
	}
	return himg;
}