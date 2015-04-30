#include "stdafx.h"
#include "Util.h"

using namespace std;
using namespace cv;

void histShow(Mat image) { // adapted from opencv wiki
	if (!image.data || image.rows == 0 || image.cols == 0) {
		return;
	}
	std::cout << "" << image.channels() << "\n";
	vector<Mat> planes(3);
	int binCount = 256;
	cv::split(image, planes);
	float range[] = { 0, 256 };
	const float* histRange = { range };
	bool uniform = true;
	bool accumulate = false;
	Scalar colours[] = {
		Scalar(255, 0, 0),
		Scalar(0, 255, 0),
		Scalar(0, 0, 255)
	};

	int histWidth = 512;
	int histHeight = 400;
	int binWidth = cvRound((double)histWidth / binCount);
	Mat histImage(histHeight, histWidth, CV_8UC3, Scalar(0, 0, 0));
	for (int channel = 0; channel < image.channels(); channel++) {
		Mat channelHist;
		cv::calcHist(&planes[channel], 1, 0, Mat(), channelHist, 1, &binCount, &histRange, uniform, accumulate);
		cv::normalize(channelHist, channelHist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
		for (int i = 0; i < binCount; i++) {
			Scalar colour = colours[channel];
			cv::line(histImage, Point(binWidth*(i - 1), histHeight - cvRound(channelHist.at<float>(i - 1))),
				Point(binWidth*i, histHeight - cvRound(channelHist.at<float>(i))),
				colour, 2, 8, 0);
		}
	}
	imshow("histogram", histImage);


}

void histShow3(Mat &image) {
	int bins = 256; // number of bins
	int nc = image.channels(); // number of channels
	vector<Mat> hist(nc); // histogram arrays
	// Initalize histogram arrays
	for (int i = 0; i < hist.size(); i++)
		hist[i] = Mat::zeros(1, bins, CV_32SC1);
	// Calculate the histogram of the image
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			for (int k = 0; k < nc; k++)
			{
				uchar val = nc == 1 ? image.at<uchar>(i, j) : image.at<Vec3b>(i, j)[k];
				hist[k].at<int>(val) += 1;
			}
		}
	}
	// For each histogram arrays, obtain the maximum (peak) value
	// Needed to normalize the display later
	int hmax[3] = { 0, 0, 0 };
	for (int i = 0; i < nc; i++)
	{
		for (int j = 0; j < bins - 1; j++)
			hmax[i] = hist[i].at<int>(j) > hmax[i] ? hist[i].at<int>(j) : hmax[i];
	}
	const char* wname[3] = { "blue", "green", "red" };
	Scalar colors[3] = { Scalar(255, 0, 0), Scalar(0, 255, 0), Scalar(0, 0, 255) };
	vector<Mat> canvas(nc);
	// Display each histogram in a canvas
	for (int i = 0; i < nc; i++)
	{
		canvas[i] = Mat::ones(125, bins, CV_8UC3);
		for (int j = 0, rows = canvas[i].rows; j < bins - 1; j++)
		{
			line(
				canvas[i],
				Point(j, rows),
				Point(j, rows - (hist[i].at<int>(j) * rows / hmax[i])),
				nc == 1 ? Scalar(200, 200, 200) : colors[i],
				1, 8, 0
				);
		}
		imshow(nc == 1 ? "value" : wname[i], canvas[i]);
	}
}
