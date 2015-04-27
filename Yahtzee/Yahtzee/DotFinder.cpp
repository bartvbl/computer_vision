#include "stdafx.h"
#include "DotFinder.h"

using namespace std;
using namespace cv;


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