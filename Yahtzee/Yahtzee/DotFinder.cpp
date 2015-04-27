#include "stdafx.h"
#include "DotFinder.h"

using namespace std;
using namespace cv;

#define MIN_PIXELS_PER_DOT 15
#define MAX_DISTANCE_DOT_RATIO 3.5


bool isInBounds(Mat image, int row, int col) {
	if ((row < 0) || (col < 0)) {
		return false;
	}
	if ((row >= image.rows) || (col >= image.cols)) {
		return false;
	}
	return true;
}

bool isPixelWhite(Mat image, int row, int col) {
	if (isInBounds(image, row, col)) {
		return image.at<unsigned char>(row, col) == 255;
	}
	return false;
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

		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
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

std::vector<bool> findDotAreas(std::vector<std::vector<std::pair<int, int>>*>* areas, Mat segmented, Mat output) {
	std::vector<bool> isDotArea;
	for (int i = 0; i < areas->size(); i++) {
		std::vector<std::pair<int, int>>* area = areas->at(i);

		if (area->size() < MIN_PIXELS_PER_DOT) {	// We can jsut discard areas that are too small
			isDotArea.push_back(false);
			continue;
		}

		double sumRows = 0;
		double sumCols = 0;

		std::vector<std::pair<int, int>> contour;

		for (int j = 0; j < area->size(); j++) {
			std::pair<int, int> pixelLocation = area->at(j);

			
			sumRows += pixelLocation.first;	// calculate center of mass
			sumCols += pixelLocation.second;

			bool isBorderPixel =
				!isPixelWhite(segmented, pixelLocation.first - 1, pixelLocation.second) || 
				!isPixelWhite(segmented, pixelLocation.first + 1, pixelLocation.second) || 
				!isPixelWhite(segmented, pixelLocation.first, pixelLocation.second - 1) || 
				!isPixelWhite(segmented, pixelLocation.first, pixelLocation.second + 1);
			if (isBorderPixel) {
				contour.push_back(pixelLocation);
			}
		}

		double centerOfMassRow = sumRows / (double)area->size();
		double centerOfMassCol = sumCols / (double)area->size();

		int row = (int)centerOfMassRow;
		int col = (int)centerOfMassCol;

		double minDistance = 10000;	// Assumes no dot will be larger than 10000 pixels in radius.
		double maxDistance = 0;

		for (int j = 0; j < contour.size(); j++) {
			std::pair<int, int> pixelLocation = contour.at(j);

			double dRow = pixelLocation.first - row;
			double dCol = pixelLocation.second - col;

			double distance = sqrt((dRow * dRow) + (dCol * dCol));
			minDistance = fastmin(minDistance, distance);
			maxDistance = fastmax(maxDistance, distance);
		}

		double distanceRatio = maxDistance / minDistance;

		bool isDot = distanceRatio < MAX_DISTANCE_DOT_RATIO;

		isDotArea.push_back(isDot);
	}
	return isDotArea;
}

Mat findDots(Mat segmented, Mat frame) {
	Mat output = frame.clone();
	
	std::vector<std::vector<std::pair<int, int>>*>* areas = findAreas(segmented);
	std::vector<bool> classifiedAreas = findDotAreas(areas, segmented, output);


	for (int i = 0; i < areas->size(); i++) {
		std::vector<std::pair<int, int>>* area = areas->at(i);
		for (int j = 0; j < area->size(); j++) {
			std::pair<int, int> pixelLocation = area->at(j);
			Vec3b colour = output.at<Vec3b>(pixelLocation.first, pixelLocation.second);
			if (classifiedAreas.at(i)) {
				colour[0] = 0;
				colour[1] = 255;
				colour[2] = 0;
			}
			else {
				colour[0] = 0;
				colour[1] = 0;
				colour[2] = 255;
			}
			output.at<Vec3b>(pixelLocation.first, pixelLocation.second) = colour;
		}
	}

	findDotAreas(areas, segmented, output);

	deallocateFoundAreas(areas);

	return output;
}