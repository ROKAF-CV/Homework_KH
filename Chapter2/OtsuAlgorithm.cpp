#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
using namespace std;
using namespace cv;
void countHist(Mat gray, double hist[]);
Mat draw_histogram_gray(double hist[]);
int OtsuThresh(double hist[]);

//Stack overflow ÂüÁ¶
//https://stackoverflow.com/questions/33693423/otsu-thresholding

int main()
{
	double hist[256];
	Mat src, gray, histo, dst;
	src = imread("lenna.jpg", IMREAD_COLOR);


	cvtColor(src, gray, CV_BGR2GRAY);

	countHist(gray, hist);

	histo = draw_histogram_gray(hist);

	

	double thres = OtsuThresh(hist);

	threshold(gray, dst, thres, 255, THRESH_BINARY);

	imshow("gray", gray);
	imshow("histogram", histo);
	imshow("dst", dst);

	waitKey(0);

	return 0;


}

void countHist(Mat gray, double hist[])
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
	double vmax = -1.0;
	Mat himg(256, 256, CV_8U, Scalar(255));

	for (int i = 0; i < 256; i++) vmax = (vmax>hist[i]) ? vmax : hist[i];

	for (int i = 0; i < 256; i++){
		Point p1(i, 256), p2(i, 256 - (int)((hist[i] / vmax)*(0.8 * 256)));
		line(himg, p1, p2, Scalar(0));
	}
	return himg;
}

int OtsuThresh(double hist[])
{
	double probability[256], mean[256], between[256];
	double vmax;
	int threshold;
	

	for (int i = 0; i < 256; i++)
	{
		probability[i] = 0.0;
		mean[i] = 0.0;
		between[i] = 0.0;
	}

	probability[0] = hist[0];

	for (int i = 1; i < 256; i++)
	{
		probability[i] = probability[i - 1] + hist[i];
		mean[i] = mean[i - 1] + i*hist[i];	
	}

	threshold = 0;
	vmax = 0.0;

	for (int i = 0; i < 256; i++)
	{
		if (probability[i] != 0.0 && probability[i] != 1.0)
			between[i] = pow(mean[255] * probability[i] - mean[i], 2) / (probability[i] * (1 - probability[i]));
		else
			between[i] = 0.0;
		if (between[i] > vmax){
			vmax = between[i];
			threshold = i;
		}
	}

	cout << "Threshold: " << threshold << "\n" << endl;

	return threshold;
}