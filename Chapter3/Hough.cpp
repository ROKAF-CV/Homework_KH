#define _USE_MATH_DEFINES

#include <iostream>
#include <queue>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <cmath>
using namespace std;
using namespace cv;

//https://webnautes.tistory.com/1041 ����

class Hough
{
public:
	void HoughT(Mat &src,Mat &edge_img)
	{
		int centerX = edge_img.cols / 2;	//����� ǥ���ϱ� ���� ���Ͱ�
		int centerY = edge_img.rows / 2;

		int diag = Dcalc(edge_img);		//�������� �ݴ��� �������� �밢�� ����
		int angle = 180;	
		int hough_height = 2 * diag;	//����� ǥ���ϱ� ����

		int accumulator_size = angle * hough_height;

		int *accumulator = (int*)malloc(sizeof(int)*accumulator_size); //������ 2���� �迭�� �����ؾ� ������, ������ �迭���� ������ ���� ���
		memset(accumulator, 0, sizeof(int)*accumulator_size);	//0���� �ʱ�ȭ

		double table_sin[180], table_cos[180];	//sin, cos���� �������� �ٲ۰�
		double d2r = CV_PI / 180;

		for (int i = 0; i < 180; i++)
		{
			table_sin[i] = sin(i*d2r);
			table_cos[i] = cos(i*d2r);
		}

		//���� �迭�� ����
		for (int x = 0; x < edge_img.cols; x++)
		{
			for (int y = 0; y < edge_img.rows; y++)
			{
				if (edge_img.at<uchar>(y, x) > 0){
					for (int a = 0; a < angle; a++)
					{
						int r = (x - centerX)*table_cos[a] + (y - centerY)*table_sin[a]; // �� ���͸� ������??

						r = r + diag;

						accumulator[r*angle + a] +=1 ;
						
					}
				}
			}
		}

		//�����迭�� �ִ� ���
		int max = -1;
		for (int j = 0; j < hough_height; j++)
		{
			for (int i = 0; i < angle; i++)
			{
				if (max < accumulator[j*angle + i])
					max = accumulator[j*angle + i];
			//	cout << accumulator[j*angle + i] << "\n";
			}
		}

		Mat img_accumulator = Mat(hough_height, angle, CV_8UC3);	//���� �̹����� ���ϱ� ���� Mat

		for (int j = 0; j < hough_height; j++)
		{
			for (int i = 0; i < angle; i++)
			{
				int value = 255 * ((double)accumulator[j*angle + i]) / max;				//�� �𸣴� �κ�

				img_accumulator.at<Vec3b>(j, i)[0] = 255 - value;
				img_accumulator.at<Vec3b>(j, i)[1] = 255 - value;
				img_accumulator.at<Vec3b>(j, i)[2] = 255 - value;
			}
		}

		int count = 0;
		for (int r = 0; r < hough_height; r++)
		{
			for (int a = 0; a < angle; a++)
			{
				if (accumulator[r*angle + a] > 50)
				{
					int max = accumulator[r*angle + a];
					for (int j = -5; j <= 5; j++)		//�ֺ������߿��� maximum�� �˻�
					{
						for (int i = -5; i <= 5; i++)
						{
							int new_r = r + j;
							int new_a = a + i;

							if (new_a < 0)
							{
								new_a = 180 + new_a;
							}
							else if (new_a >= 180)
							{
								new_a = new_a - 180;
							}

							if (new_r >= 0 && new_r < hough_height)
							{
								if (accumulator[new_r * angle + new_a] > max)
								{
									max = accumulator[new_r * angle + new_a];
									break;  
								}
							}

						}
					}

					if (max > accumulator[r*angle + a])
						continue;

					int x1, x2, y1, y2;
					

					if (a >= 45 && a <= 135) //�����̶�� �ϴµ� �� ���� ���� ����
					{
						x1 = 0;
						x2 = edge_img.cols;
						y1 = ((r - diag) - (x1 - centerX)*table_cos[a]) / table_sin[a] + centerY;
						y2 = ((r - diag) - (x2 - centerX)*table_cos[a]) / table_sin[a] + centerY;
					}
					else  //�������̶�� �ϴµ� �� ���� ���� ����
					{
						y1 = 0;
						y2 = edge_img.rows;
						x1 = ((r - diag) - (y1 - centerY)*table_sin[a]) / table_cos[a] + centerX;
						x2 = ((r - diag) - (y2 - centerY)*table_sin[a]) / table_cos[a] + centerX;
					}

					circle(img_accumulator, cvPoint(a, r), 3, Scalar(255, 0, 0));
					line(src, cvPoint(x1, y1), cvPoint(x2, y2), Scalar(255, 0, 0));
					count++;

						cout << count << " (" << x1 << "," << y1 << ") (" << x2 << "," << y2 << ")  angle=" << a << " r=" << r << " accumulator��=" << accumulator[r * angle + a] << endl;

				}
			}

			
		}
		imshow("img_result", src);
		
		imshow("img_edge", edge_img);
		imshow("img_accumulator", img_accumulator);

		
	
	}


	void Hop()
	{
		Mat src, gray, edge_img;
		src= imread("houghtest.jpg", IMREAD_COLOR);
		if (src.empty())
		{
			cout << "Could not open or find the image" << std::endl;
			return;
		}

		cvtColor(src, gray, COLOR_BGR2GRAY);

		GaussianBlur(gray, edge_img, cvSize(5, 5), 0, 0);	//����þ� ����
		Canny(edge_img, edge_img, 50, 150, 3);	//ĳ�Ͽ��� ����

		HoughT(src, edge_img);
		///imshow("src", src);
		//imshow("edge_img", edge_img);
	}

private:
	double Dcalc(Mat &src)
	{
		return sqrt(pow(src.cols, 2) + pow(src.rows, 2));
	}


};

int main(void)
{
	Hough h;
	h.Hop();

	waitKey(0);
}