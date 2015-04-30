#include "stdafx.h"
#include "DotFinder.h"

using namespace std;
using namespace cv;

#define MIN_PIXELS_PER_DOT 15
#define MAX_DISTANCE_DOT_RATIO 3
#define MAX_IMAGE_SIZE 10000
#define MAX_DISTANCE_BETWEEN_DOTS 30


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

std::vector<std::pair<int, int>> findDots(std::vector<std::vector<std::pair<int, int>>*>* areas, Mat segmented, Mat output, std::vector<bool>* acceptedAreas) {
	std::vector<std::pair<int, int>> dotCenters;
	for (int i = 0; i < areas->size(); i++) {
		std::vector<std::pair<int, int>>* area = areas->at(i);

		if (area->size() < MIN_PIXELS_PER_DOT) {	// We can just discard areas that are too small
			acceptedAreas->push_back(false);
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

		double minDistance = MAX_IMAGE_SIZE;	// Assumes no dot will be larger than MAX_IMAGE_SIZE pixels in radius.
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

		if (isDot) {
			dotCenters.push_back(std::make_pair(row, col));
		}

		acceptedAreas->push_back(isDot);
	}
	return dotCenters;
}

bool containsDotCenter(std::vector<std::pair<int, int>> face, std::pair<int, int> dotCenter) {
	for (int i = 0; i < face.size(); i++) {
		std::pair<int, int> dot = face.at(i);
		if ((dot.first == dotCenter.first) && (dot.second == dotCenter.second)) {
			return true;
		}
	}
	return false;
}

void registerDieFace(std::vector<std::vector<std::pair<int, int>>>* dieFacesFound, std::pair<int, int> dotCenter, std::pair<int, int> otherDotCenter) {
	//std::cout << "Checking (" << dotCenter.first << ", " << dotCenter.second << ") and (" << otherDotCenter.first << ", " << otherDotCenter.second << "): ";
	for (int i = 0; i < dieFacesFound->size(); i++) {
		std::vector<std::pair<int, int>> face = dieFacesFound->at(i);
		if (containsDotCenter(face, dotCenter) && containsDotCenter(face, otherDotCenter)) {
			return; // Both already exist -> nothing to do here
		}
		if (containsDotCenter(face, dotCenter)) { // Either one is missing at this point -> add the other one
			face.push_back(otherDotCenter);
			dieFacesFound->at(i) = face;
			return;
		}
		if (containsDotCenter(face, otherDotCenter)) {
			face.push_back(dotCenter);
			dieFacesFound->at(i) = face;
			return;
		}
	}
	// No existing face to link up with -> create a new face
	std::vector<std::pair<int, int>> newFace;
	newFace.push_back(dotCenter);
	newFace.push_back(otherDotCenter);
	dieFacesFound->push_back(newFace);
}

std::vector<std::vector<std::pair<int, int>>> findDieRolls(std::vector<std::pair<int, int>> dotCenters, Mat output) {
	std::vector<std::vector<std::pair<int, int>>> dieFacesFound;

	for (int dot = 0; dot < dotCenters.size(); dot++) {
		std::pair<int, int> dotCenter = dotCenters.at(dot);
		bool foundConnection = false;

		for (int otherDot = 0; otherDot < dotCenters.size(); otherDot++) {
			if (dot == otherDot) {
				continue;
			}

			std::pair<int, int> otherDotCenter = dotCenters.at(otherDot);

			double dRow = dotCenter.first - otherDotCenter.first;
			double dCol = dotCenter.second - otherDotCenter.second;

			double distance = sqrt((dRow * dRow) + (dCol * dCol));

			if (distance < MAX_DISTANCE_BETWEEN_DOTS) {
				registerDieFace(&dieFacesFound, dotCenter, otherDotCenter);
				foundConnection = true;
				cv::line(output, Point(dotCenter.second, dotCenter.first), Point(otherDotCenter.second, otherDotCenter.first), Scalar(255, 0, 0));
			}
		}

		if (!foundConnection) {
			std::vector<std::pair<int, int>> newFace;
			newFace.push_back(dotCenter);
			dieFacesFound.push_back(newFace);
		}
	}
	return dieFacesFound;
}

std::vector<int> findDots(Mat segmented, Mat frame) {
	Mat output = frame.clone();

	std::vector<bool> acceptedAreas;
	
	std::vector<std::vector<std::pair<int, int>>*>* areas = findAreas(segmented);
	std::vector<std::pair<int, int>> dotCenters = findDots(areas, segmented, output, &acceptedAreas);


	for (int i = 0; i < areas->size(); i++) {
		std::vector<std::pair<int, int>>* area = areas->at(i);
		for (int j = 0; j < area->size(); j++) {
			std::pair<int, int> pixelLocation = area->at(j);
			Vec3b colour = output.at<Vec3b>(pixelLocation.first, pixelLocation.second);
			if (acceptedAreas.at(i)) {
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

	std::vector<std::vector<std::pair<int, int>>> foundFaces = findDieRolls(dotCenters, output);
	std::vector<int> dieRolls;

	for (int i = 0; i < foundFaces.size(); i++) {
		dieRolls.push_back(foundFaces.at(i).size());
	}

	imshow("frame", output);

	return dieRolls;
}