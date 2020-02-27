#define _USE_MATH_DEFINES

#include <iostream>
#include <queue>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <cmath>
using namespace std;
using namespace cv;

class Moravec{
public:
	void moravec(Mat &src, int thresh)
	{
		///Mat mask(Size(3, 3), CV_8U); // private에서 생성 불가능??
		
		for (int i = 1; i < src.cols - 1; i++)
		{
			for (int j = 1; j < src.rows - 1; j++)
			{
				int min = 100000000;
						
				int s[4] = { 0, };
												
						for (int c = -1; c < 2; c++)
						{
							for (int d = -1; d < 2; d++)
							{

								s[0] += pow(src.at<uchar>(j + 1 + d, i + c) - src.at<uchar>(j + 1, i), 2);
								s[1] += pow(src.at<uchar>(j + d, i + 1 + c) - src.at<uchar>(j, i + 1), 2);
								s[2] += pow(src.at<uchar>(j - 1 + d, i + c) - src.at<uchar>(j - 1, i), 2);
								s[3] += pow(src.at<uchar>(j + d, i - 1 + c) - src.at<uchar>(j, i - 1), 2);

							}
						}
						for (int count = 0; count < 4; count++){
							if (min > s[count]) //모든 방향에서 커야 한다는 조건 추가??
							{
								min = s[count];
							}
						}

						if (min>thresh)
							q.push(make_pair(j, i));
			}
		}
	}
	
	void mop(){
		while (!q.empty())
		{
			pair<int, int> n = q.front();
			q.pop();

			cout << n.first << ' ' << n.second << '\n';            
		}
	}
	
private:
	queue<pair<int, int>> q;


};

void moravecop()
{
	Mat input_image = (Mat_<uchar>(12, 12) << 
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0, 255,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0, 255, 255,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0, 255, 255, 255,   0,   0,   0,   0,   0,   0,
		0,   0,   0, 255, 255, 255, 255,   0,   0,   0,   0,   0,
		0,   0,   0, 255, 255, 255, 255, 255,   0,   0,   0,   0,
		0,	 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,	 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,	 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
		);
	//Mat src = imread("lenna.jpg", IMREAD_GRAYSCALE);
	//Mat mora(src.size(), CV_8U);
	Mat output_image(input_image.size(), CV_8U);
	const int rate = 50;

	Moravec mv;
	mv.moravec(input_image, 126);
	



	resize(input_image, input_image, Size(), rate, rate, INTER_NEAREST);
	imshow("Original", input_image);
	

}

int main(){
	moravecop();
	waitKey();
}