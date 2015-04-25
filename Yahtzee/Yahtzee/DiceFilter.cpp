#include "stdafx.h"
#include "DiceFilter.h"
#include <stack>

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

bool isInBounds(Mat image, int row, int col) {
	if ((row < 0) || (col < 0)) {
		return false;
	}
	if ((row >= image.rows) || (col >= image.cols)) {
		return false;
	}
	return true;
}

std::vector<std::pair<int, int>>* floodfill(Mat visited, int row, int col, int label) {
	std::stack<std::pair<int, int>>* pendingCoordinates = new std::stack<std::pair<int, int>>();
	std::vector<std::pair<int, int>>* area = new std::vector<std::pair<int, int>>();
	pendingCoordinates->push(std::make_pair(row, col));

	while (!pendingCoordinates->empty()) {
		std::pair<int, int> currentCoordinate = pendingCoordinates->top();
		pendingCoordinates->pop();
		int currentRow = currentCoordinate.first;
		int currentCol = currentCoordinate.second;

		area->push_back(std::make_pair(currentRow, currentCol));

		for (int i = -1; i < 2; i ++) {
			for (int j = -1; j < 2; j ++) {
				if ((i == 0) && (j == 0)) {
					continue;
				}

				int pixelRow = currentRow + i;
				int pixelCol = currentCol + j;

				if (!isInBounds(visited, pixelRow, pixelCol)) {
					continue;
				}

				unsigned char currentPixel = visited.at<unsigned char>(pixelRow, pixelCol);

				if (currentPixel == 255) {
					visited.at<unsigned char>(pixelRow, pixelCol) = 0;
					pendingCoordinates->push(std::make_pair(pixelRow, pixelCol));
				}
			}
		}

	}
	delete pendingCoordinates;
	return area;
}

// region labelling algorithm from lecture 9, slide 14
std::vector<std::vector<std::pair<int, int>>*>* findAreas(Mat image) {
	std::vector<std::vector<std::pair<int, int>>*>* areas = new std::vector<std::vector<std::pair<int, int>>*>();
	Mat visited = image.clone();
	int label = 256;

	for (int row = 0; row < visited.rows; row++) {
		for (int col = 0; col < visited.cols; col++) {
			unsigned char currentPixel = visited.at<unsigned char>(row, col);

			if (currentPixel == 255) {
				label++; // generate new label
				std::vector<std::pair<int, int>>* area = floodfill(visited, row, col, label);
				areas->push_back(area);
			}
		}
	}

	return areas;
}

void deallocateFoundAreas(std::vector<std::vector<std::pair<int, int>>*>* areas) {
	for (int i = 0; i < areas->size(); i++) {
		std::vector<std::pair<int, int>>* area = areas->at(i);
		delete area;
	}
	delete areas;
}

std::vector<bool> findDotAreas(std::vector<std::vector<std::pair<int, int>>*>* areas, Mat output) {
	std::vector<bool> isDotArea;
	for (int i = 0; i < areas->size(); i++) {
		std::vector<std::pair<int, int>>* area = areas->at(i);
		double sumRows = 0;
		double sumCols = 0;

		for (int j = 0; j < area->size(); j++) {
			std::pair<int, int> pixelLocation = area->at(j);
			sumRows += pixelLocation.first;
			sumCols += pixelLocation.second;
		}

		double centerOfMassRow = sumRows / (double)area->size();
		double centerOfMassCol = sumCols / (double)area->size();

		int row = (int)centerOfMassRow;
		int col = (int)centerOfMassCol;

		Vec3b colour = output.at<Vec3b>(row, col);
		colour[0] = 0;
		colour[1] = 0;
		colour[2] = 255;
		output.at<Vec3b>(row, col) = colour;

		isDotArea.push_back(false);
	}
	return isDotArea;
}

void filter(Mat frame) {
	imshow("original", frame);
	Mat thresholded = segmentOriginalFrame(frame);
	std::vector<std::vector<std::pair<int, int>>*>* areas = findAreas(thresholded);
	
	Mat output = frame.clone();

	for (int i = 0; i < areas->size(); i++) {
		std::vector<std::pair<int, int>>* area = areas->at(i);
		for (int j = 0; j < area->size(); j++) {
			std::pair<int, int> pixelLocation = area->at(j);
			Vec3b colour = output.at<Vec3b>(pixelLocation.first, pixelLocation.second);
			colour[0] = 0;
			colour[1] = 255;
			colour[2] = 0;
			output.at<Vec3b>(pixelLocation.first, pixelLocation.second) = colour;
		}
	}

	std::vector<bool> areDotAreas = findDotAreas(areas, output);

	deallocateFoundAreas(areas);
	imshow("frame", output);
}