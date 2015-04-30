#include "MarkedUnprojector.h"
#include "ImageAnalyzer.h"
#include <math.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>


const bool showBoneLinkSteps = false;

template<class T> inline const T& fastmax(const T& a, const T& b)
{
	return b < a ? a : b;
}

template<class T> inline const T& fastmin(const T& a, const T& b)
{
	return a < b ? a : b;
}
//Unprojection calculations

inline double calculateBonePieceX(MarkedUnprojector::Coordinate image1Coordinate, MarkedUnprojector::Coordinate image2Coordinate, MarkedUnprojector::Settings settings) {
	double part1 = settings.emitter.x - settings.detector2x;
	double part2 = image1Coordinate.x - settings.detector1x;
	double part3 = settings.emitter.x - settings.detector1x;
	double part4 = image2Coordinate.x - settings.detector2x;
	double deltaD = settings.detector2x - settings.detector1x;

	return (((part1 * part2) - (part3 * part4)) / deltaD);

	/*double part1 = settings.emitter.x - settings.detector2x;
	double part2 = settings.detector1x - image1Coordinate.x;
	double part3 = settings.emitter.x - settings.detector1x;
	double part4 = settings.detector2x - image2Coordinate.x;
	double deltaD = settings.detector1x - settings.detector2x;

	std::cout << "Emitter: (" << settings.emitter.x << ", " << settings.emitter.y << ", " << settings.emitter.z << ")\n";
	std::cout << "Image 1: (" << image1Coordinate.x << ", " << image1Coordinate.y << ")\n";
	std::cout << "Image 2: (" << image2Coordinate.x << ", " << image2Coordinate.y << ")\n";
	std::cout << "Detectors: (" << settings.detector1x << ", " << settings.detector2x << ")\n";

	return (((part1 * part2) - (part3 * part4)) / deltaD);*/
}

inline double calculateBonePieceY(MarkedUnprojector::Coordinate image1Coordinate, MarkedUnprojector::Settings settings, double pieceZ) {
	double slope = (settings.emitter.y - image1Coordinate.y) / settings.emitter.z;
	double coordinate = pieceZ + settings.fishOrigin.z;
	return (slope * coordinate) + image1Coordinate.y - settings.fishOrigin.y;
}

inline double calculateBonePieceZ(MarkedUnprojector::Coordinate image1Coordinate, MarkedUnprojector::Settings settings, double pieceX) {
	double slope = settings.emitter.z / (settings.emitter.x - settings.detector1x);
	double coordinate = pieceX - image1Coordinate.x + settings.detector1x; //image1Coordinate.x + pieceX -settings.detector1x;
	return (slope * coordinate) - settings.fishOrigin.z;
}

inline struct MarkedUnprojector::Point calculateBonePieceLocation(MarkedUnprojector::Coordinate image1Coordinate, MarkedUnprojector::Coordinate image2Coordinate, MarkedUnprojector::Settings settings) {
	MarkedUnprojector::Point location;
	location.x = calculateBonePieceX(image1Coordinate, image2Coordinate, settings);
	location.x *= -1;
	location.z = calculateBonePieceZ(image1Coordinate, settings, location.x);
	location.y = calculateBonePieceY(image1Coordinate, settings, location.z);
	//std::cout << "Unprojected (" << image1Coordinate.x << ", " << image1Coordinate.y << ") and (" << image2Coordinate.x << ", " << image2Coordinate.y << ") to (" << location.x << ", " << location.y << ", " << location.z << ")\n\n";
	return location;
}

/*int getDeltaI(MarkedUnprojector::Settings settings, double zCoordinate) {
double deltaD = settings.detector2x - settings.detector1x;
double ratio = ((zCoordinate + settings.fishOrigin.z) * deltaD) / settings.emitter.z;
ratio -= deltaD;
return (int) ceil(settings.detectorResolution * ratio);
}

int minBoneDisplacementPixels(MarkedUnprojector::Settings settings) {
return getDeltaI(settings, settings.fishZMin);
}

int maxBoneDisplacementPixels(MarkedUnprojector::Settings settings) {
return getDeltaI(settings, settings.fishZMax);
}*/

//Helper functions
bool compareLineSegment(std::vector<MarkedUnprojector::Point> segment, std::vector<MarkedUnprojector::Point> otherSegment) {
	return segment[0].x < otherSegment[0].x;
}

std::vector<std::vector<MarkedUnprojector::Point>> sortLineSegments(std::vector<std::vector<MarkedUnprojector::Point>> lineSegments) {
	std::sort(lineSegments.begin(), lineSegments.end(), compareLineSegment);
	return lineSegments;
}

double distance(MarkedUnprojector::Point point1, MarkedUnprojector::Point point2) {
	double dx = point2.x - point1.x;
	double dy = point2.y - point1.y;
	return sqrt(dx*dx + dy*dy);
}

inline MarkedUnprojector::Coordinate findMatchingPoint(MarkedUnprojector::Coordinate coordinate, cv::Vec4b pixel, cv::Mat image) {
	MarkedUnprojector::Coordinate matchingCoordinate;
	matchingCoordinate.x = -1;
	matchingCoordinate.y = -1;
	
	for (int row = fastmax<int>(coordinate.y - 1, 0); row <= fastmin<int>(coordinate.y + 1, image.rows-1); row++) {
		for (int col = 0; col < image.cols; col++) {
			cv::Vec4b otherPixel = image.at<cv::Vec4b>(row, col);
			if ((pixel[0] == otherPixel[0]) && (pixel[1] == otherPixel[1]) && (pixel[2] == otherPixel[2]) && (pixel[3] == otherPixel[3])) {
				matchingCoordinate.x = col;
				matchingCoordinate.y = row;
				return matchingCoordinate;
			}
		}
	}
	return matchingCoordinate;
}

void MarkedUnprojector::unproject(
	cv::Mat leftBones, 
	cv::Mat rightBones,
	MarkedUnprojector::Settings settings, 
	std::string name, 
	bool verbose
	) {

	

	std::vector<MarkedUnprojector::Point> unprojectedPairs;
	std::vector<unsigned int> colours;

	for (int row = 0; row < leftBones.rows; row++) {
		int pixelsTried = 0;
		int pixelsFound = 0;
		for (int col = 0; col < leftBones.cols; col++) {
			cv::Vec4b bgrPixel = leftBones.at<cv::Vec4b>(row, col);
			if (((int)bgrPixel[0] != 0) || ((int)bgrPixel[1] != 0) || ((int)bgrPixel[2] != 0)) {
				
				pixelsTried++;
				
				MarkedUnprojector::Coordinate leftPoint;
				leftPoint.x = (double) col;
				leftPoint.y = (double) row;

				MarkedUnprojector::Coordinate rightPoint = findMatchingPoint(leftPoint, bgrPixel, rightBones);

				if (rightPoint.x != -1 || rightPoint.y != -1) { // Ensure matching point was found
					pixelsFound++;
					leftPoint.x += settings.detector1x;
					rightPoint.x += (settings.detector2x); // correct offsets between detectors
					leftPoint.y = settings.imageHeight - leftPoint.y;
					rightPoint.y = settings.imageHeight - rightPoint.y;
					MarkedUnprojector::Point boneLocation = calculateBonePieceLocation(leftPoint, rightPoint, settings);
					unprojectedPairs.push_back(boneLocation);
					unsigned int b = bgrPixel[0];
					unsigned int g = bgrPixel[1];
					unsigned int r = bgrPixel[2];
					unsigned int pixel = (b << 24) | (g << 16) | (b << 8) | 0xFF;
					colours.push_back(pixel);
				}
			}
		}
		std::cout << "Row " << row << ": found " << pixelsFound << "/" << pixelsTried << "\n";
	}

	//Write out results

	std::cout << "Writing results..\n";

	std::ofstream outFile;
	outFile.open("output/coordinates.txt");

	for (unsigned int i = 0; i < unprojectedPairs.size(); i++) {
		MarkedUnprojector::Point point = unprojectedPairs.at(i);
		outFile << "0x" << std::hex << colours.at(i);
		outFile << ": " << point.x << ", " << point.y << ", " << point.z << "\n";
	}

	outFile.close();

	std::cout << "Write complete. \n";
}

