#include "stdafx.h"
#include "DiceFilter.h"

using namespace cv;

// ------ IMAGE SEGMENTATION -------

void histShow(Mat image) { // adapted from opencv wiki
	if (!image.data || image.rows == 0 || image.cols == 0) {
		return;
	}
	std::cout << "" << image.channels() << "\n";
	vector<Mat> planes(3);
	int binCount = 256;
	cv::split(image, planes);
	float range[] = {0, 256};
	const float* histRange = {range};
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

template<class T> inline const T& fastmax(const T& a, const T& b)
{
	return b < a ? a : b;
}

template<class T> inline const T& fastmin(const T& a, const T& b)
{
	return a < b ? a : b;
}

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

// region labelling algorithm from lecture 9, slide 14
Mat labelAreas(Mat image) {
	Mat labelled = image.clone();
	labelled.convertTo(labelled, CV_32S);
	int label = 256;

	unsigned char currentPixel = labelled.at<unsigned int>(0, 0);
	unsigned char topLeftPixel = 0;		// all black
	unsigned char topPixel = 0;
	unsigned char topRightPixel = 0;
	unsigned char leftPixel = 0;

	for (int row = 0; row < labelled.rows; row++) {
		if (row > 0) {
			topLeftPixel = 0;			// These pixels always start out of bounds. They thus get a black pixel.
			leftPixel = 0;
			topPixel = labelled.at<unsigned int>(row - 1, 0);
			topRightPixel = labelled.at<unsigned int>(row - 1, 1);
		}
		
		for (int col = 0; col < labelled.cols; col++) {
			currentPixel = labelled.at<unsigned int>(row, col);

			int labelledPixelCount = 0;
			labelledPixelCount = (topLeftPixel != 0)	? labelledPixelCount + 1 : labelledPixelCount;
			labelledPixelCount = (topPixel != 0)		? labelledPixelCount + 1 : labelledPixelCount;
			labelledPixelCount = (topRightPixel != 0)	? labelledPixelCount + 1 : labelledPixelCount;
			labelledPixelCount = (leftPixel != 0)		? labelledPixelCount + 1 : labelledPixelCount;

			// if all labels are black and current pixel is white -> new label
			if ((topLeftPixel == 0) && (topPixel == 0) && (topRightPixel == 0) &&
				(leftPixel == 0)	&& (currentPixel == 255)) {
				labelled.at<unsigned int>(row, col) = label;
				currentPixel = label;
				label++;
			} // If existing labelled pixel was found -> label 
			else if ((labelledPixelCount == 1) && (currentPixel == 255)) {
				int existingLabel = fastmax(fastmax(topLeftPixel, topPixel), fastmax(topRightPixel, leftPixel));
				labelled.at<unsigned int>(row, col) = existingLabel;
				currentPixel = existingLabel;
			}
			else if ((labelledPixelCount > 1) && (currentPixel == 255)) {
				int existingLabel = fastmax(fastmax(topLeftPixel, topPixel), fastmax(topRightPixel, leftPixel));
				labelled.at<unsigned int>(row, col) = existingLabel;
				currentPixel = existingLabel;
			}
			
			// shift values forward
			topLeftPixel = topPixel;
			topPixel = topRightPixel;
			leftPixel = currentPixel;

			if ((col < image.cols - 1) && (row > 0)) {		// Pad the top and bottom row with black pixels
				topRightPixel = image.at<unsigned char>(row - 1, col + 1);
			}
			else {
				topRightPixel = 0;
			}
		}
	}
	std::cout << "Final label: " << label << "\n";

	return labelled;
}

std::vector<std::vector<struct PixelCoordinate>> findConnectedAreas(Mat segments) {
	std::vector<std::vector<struct PixelCoordinate>> foundAreas;
	return foundAreas;
}

void filter(Mat frame) {
	imshow("original", frame);
	Mat thresholded = segmentOriginalFrame(frame);
	Mat labelled = labelAreas(thresholded);
	std::vector<std::vector<struct PixelCoordinate>> areas = findConnectedAreas(labelled);
	Mat output = thresholded.clone();
	cv::normalize(labelled, output, 0, 255, NORM_MINMAX, CV_8UC1);
	imshow("frame", output);
}