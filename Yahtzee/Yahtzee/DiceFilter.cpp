#include "stdafx.h"
#include "DiceFilter.h"
#include "DotFinder.h"

using namespace cv;

Mat blur(Mat image, int kernelSize) {//16-bit
	Mat blurred = image.clone();

	GaussianBlur(image, blurred, Size(kernelSize, kernelSize), 0, 0);

	return blurred;
}

Mat dilate(Mat image, Mat kernel) {
	Mat dilated = image.clone();
	int kernelCenterRow = (int)floor(kernel.rows / 2.0);
	int kernelCenterCol = (int)floor(kernel.cols / 2.0);

	cv::dilate(image, dilated, kernel, cv::Point(kernelCenterRow, kernelCenterCol));

	return dilated;
}

Mat erode(Mat image, Mat kernel) {
	Mat eroded = image.clone();
	int kernelCenterRow = (int)floor(kernel.rows / 2.0);
	int kernelCenterCol = (int)floor(kernel.cols / 2.0);

	cv::erode(image, eroded, kernel, cv::Point(kernelCenterRow, kernelCenterCol));

	return eroded;
}

Mat imgDifference(Mat image, Mat secondImage) {
	Mat differenceResult = image.clone();
	cv::absdiff(image, secondImage, differenceResult);

	return differenceResult;
}

Mat threshold(Mat image) {
	Mat thresholded = image.clone();

	if (image.channels() == 3) {
		cv::cvtColor(image, image, CV_BGR2GRAY);
	}

	cv::threshold(image, thresholded, 0, 255, THRESH_BINARY + THRESH_OTSU);

	return thresholded;
}

Mat sobel(Mat image) {
	Mat derivative = image.clone();
	cv::Sobel(image, derivative, image.depth(), 1, 1);
	return derivative;
}

Mat equalise(Mat image) {
	cv::cvtColor(image, image, CV_BGR2GRAY);
	Mat equalised = image.clone();
	equalizeHist(image, equalised);
	return equalised;
}

Mat multiplyWithThreshold(Mat frame, Mat thresholded) {
	Mat multiplied = frame.clone();
	for (int row = 0; row < frame.rows; row++) {
		for (int col = 0; col < frame.cols; col++) {
			uchar pixel = thresholded.at<uchar>(row, col);
			Vec3b framePixel = frame.at<Vec3b>(row, col);
			if (pixel == 255) {
				multiplied.at<Vec3b>(row, col) = framePixel;
			}
			else {
				multiplied.at<Vec3b>(row, col)[0] = 0;
				multiplied.at<Vec3b>(row, col)[1] = 0;
				multiplied.at<Vec3b>(row, col)[2] = 0;
			}
		}
	}
	return multiplied;
}

Mat segmentOriginalFrame(Mat frame) {
	Mat kernel_large = cv::Mat::ones(11, 11, CV_8U);

	Mat blurred = blur(frame, 9);
	Mat dilated = dilate(blurred, kernel_large);
	Mat eroded = erode(dilated, kernel_large);
	Mat difference = imgDifference(frame, eroded);
	Mat thresholded = threshold(difference);

	return thresholded;
}
