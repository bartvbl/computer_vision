#include "BoneFilter.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <string>
#include <ctime>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

BoneFilter::BoneFilter(void){}
BoneFilter::~BoneFilter(void){}

using namespace cv;

Mat invert(Mat image);
int counter;
std::string image_name = "";
const int KERNEL_SIZE = 5;

bool WRITE_STEPS = true;

template<class T> inline const T& fastmax(const T& a, const T& b)
{
    return b < a ? a : b;
}

template<class T> inline const T& fastmin(const T& a, const T& b)
{
    return a < b ? a : b;
}


Mat normalize(Mat image) { //16-bit
	//std::cout << "\tNormalizing..\n";
	Mat normalized = image.clone();
	Mat moved = image.clone();
	
	unsigned short minValue = 65535;
	unsigned short maxValue = 0;
	
	for(int row = 0; row < image.rows; row++) {
		for(int column = 0; column < image.cols; column++) {
			unsigned short pixel = image.at<unsigned short>(row, column);
			maxValue = fastmax(pixel, maxValue);
			minValue = fastmin(minValue, pixel);
		}
	}

	double delta = 65535.0 / (double)(maxValue - minValue);

	for(int row = 0; row < image.rows; row++) {
		for(int column = 0; column < image.cols; column++) {
			unsigned short pixel = image.at<unsigned short>(row, column);
			normalized.at<unsigned short>(row, column) = (unsigned short) (delta * (double) (pixel - minValue));
		}
	}

	return normalized;
}

void BoneFilter::write(std::string name, std::string operation, Mat image) {
	if(WRITE_STEPS) {
		std::cout << "\tWriting " << name << ".png..\n";
		imwrite("steps/" + image_name + std::to_string((long long)counter) + "_" + operation + "_" + name + ".png", normalize(invert(image)));
		counter++;
	}
}

Mat generateHugeKernel() {
	cv::Mat kernel = cv::imread("kernels/huge.png", cv::IMREAD_GRAYSCALE);
	kernel.convertTo(kernel, CV_8UC1);
	return kernel;
}

Mat generateLargeKernel() {
	Mat kernel = Mat::ones(KERNEL_SIZE, KERNEL_SIZE, CV_8U);
	kernel.at<unsigned char>(0, 0) = 0;
	kernel.at<unsigned char>(4, 0) = 0;
	kernel.at<unsigned char>(4, 4) = 0;
	kernel.at<unsigned char>(0, 4) = 0;

	//write("kernel", kernel);

	return kernel;
}

Mat generateSmallKernel() {
	Mat kernel = Mat::ones(3, 3, CV_8U);
	kernel.at<unsigned char>(0, 0) = 0;
	kernel.at<unsigned char>(2, 0) = 0;
	kernel.at<unsigned char>(2, 2) = 0;
	kernel.at<unsigned char>(0, 2) = 0;

	//write("kernel", kernel);

	return kernel;
}

Mat blur(Mat image) {//16-bit
	//std::cout << "Blurring..\n";
	Mat blurred = image.clone();
	
	GaussianBlur(image, blurred, Size(5, 5), 0, 0);

	BoneFilter::write("blurred", "boneExtraction", blurred);

	return blurred;
}



Mat dilate(Mat image, Mat kernel) {//16-bit
	//std::cout << "Dilating..\n";
	Mat dilated = image.clone();
	int kernelCenterRow = (int) floor(kernel.rows / 2.0);
	int kernelCenterCol = (int) floor(kernel.cols / 2.0);
	
	cv::dilate(image, dilated, kernel, cv::Point(kernelCenterRow, kernelCenterCol));

	BoneFilter::write("dilated", "boneExtraction", dilated);

	return dilated;
}

Mat erode(Mat image, Mat kernel) {//16-bit
	//std::cout << "Eroding..\n";
	Mat eroded = image.clone();
	int kernelCenterRow = (int) floor(kernel.rows / 2.0);
	int kernelCenterCol = (int) floor(kernel.cols / 2.0);

	cv::erode(image, eroded, kernel, cv::Point(kernelCenterRow, kernelCenterCol));

	BoneFilter::write("eroded", "boneExtraction", eroded);

	return eroded;
}

Mat imgDifference(Mat image, Mat secondImage) {//16-bit
	//std::cout << "Differentiating..\n";
	Mat differenceResult = image.clone();
	for(int row = 0; row < image.rows; row++) {
		for(int column = 0; column < image.cols; column++) {
			unsigned short imagePixel = image.at<unsigned short>(row, column);
			unsigned short subPixel = secondImage.at<unsigned short>(row, column);
			differenceResult.at<unsigned short>(row, column) = fastmax(imagePixel, subPixel) - fastmin(imagePixel, subPixel);
		}
	}

	BoneFilter::write("difference", "boneExtraction", differenceResult);

	return differenceResult;
}

Mat invert(Mat image) {//16-bit
	//std::cout << "\tInverting..\n";
	Mat inverted = image.clone();
	for(int row = 0; row < image.rows; row++) {
		for(int column = 0; column < image.cols; column++) {
			unsigned short pixel = (unsigned short) 65535 - image.at<unsigned short>(row, column);
			inverted.at<unsigned short>(row, column) = pixel;
		}
	}
	return inverted;
}

Mat threshold(Mat image) {//8-bit
	//std::cout << "Thresholding..\n";
	image.convertTo(image, CV_8U);
	Mat thresholded = image.clone();
	
	cv::threshold(image,thresholded,0,255,THRESH_BINARY+THRESH_OTSU);
	thresholded.convertTo(thresholded, CV_16U);

	BoneFilter::write("thresholded", "boneExtraction", thresholded);
	return thresholded;
}

Mat sharpen(Mat image) {//16-bit
	//std::cout << "Sharpening..\n";

	Mat sharpened = image.clone();
	cv::GaussianBlur(image, sharpened, cv::Size(0, 0), 3);
	cv::addWeighted(image, 1.5, sharpened, -0.5, 0, sharpened);

	BoneFilter::write("sharpened", "boneExtraction", sharpened);
	return sharpened;
}

Mat median(Mat image) {//8-bit (unused)
	//std::cout << "Medianating..\n";

	image.convertTo(image, CV_8U);
	Mat median = image.clone();
	medianBlur(image, median, 3);
	median.convertTo(median, CV_16U);

	BoneFilter::write("median", "boneExtraction", median);
	return median;
}

cv::Mat BoneFilter::filter(cv::Mat originalImage, std::string name, bool verbose) {
	double const KERNEL_RADIUS = 30;
	counter = 0;
	image_name = name;
	WRITE_STEPS = verbose;

	BoneFilter::write("original", "boneExtraction", originalImage);
	std::clock_t begin = clock();

	Mat smallKernel = generateSmallKernel();
	Mat largeKernel = generateLargeKernel();
	Mat hugeKernel = generateHugeKernel();
	
	int runcount = 10000;
	Mat image;

	for(int i = 0; i < runcount; i++) {

	//Processing
	image = blur(originalImage);					//16.bit
	image = dilate(image, hugeKernel);				//16.bit
	image = erode(image, hugeKernel);				//16.bit
	image = imgDifference(originalImage, image);	//16.bit
	image = sharpen(image);							//16.bit
	image = erode(image, smallKernel);				//16.bit
	image = blur(image);							//16.bit
	image = threshold(image);						//8-bit

	}

	//End processing

	BoneFilter::write("output_"+name, "boneExtraction", image);

	image = invert(image);
	image = normalize(image);
    
	std::clock_t end = clock();
	double elapsedSeconds = double(end - begin) / (CLOCKS_PER_SEC * runcount);
	std::cout << "Processing took " << elapsedSeconds << " s.\n";

	return image;
}