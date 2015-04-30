#include "Unprojector.h"
#include "ImageAnalyzer.h"
#include <math.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

const bool showBoneLinkSteps = false;

Unprojector::Unprojector(void)
{
}


Unprojector::~Unprojector(void)
{
}

//Unprojection calculations

double calculateBonePieceX(Unprojector::Coordinate image1Coordinate, Unprojector::Coordinate image2Coordinate, Unprojector::Settings settings) {
	double part1 = settings.emitter.x - settings.detector2x;
	double part2 = image1Coordinate.x - settings.detector1x;
	double part3 = settings.emitter.x - settings.detector1x;
	double part4 = image2Coordinate.x - settings.detector2x;
	double deltaD = settings.detector2x - settings.detector1x;

	return ((part1 * part2) - (part3 * part4)) / deltaD;
}

double calculateBonePieceY(Unprojector::Coordinate image1Coordinate, Unprojector::Settings settings, double pieceZ) {
	double slope = (settings.emitter.y - image1Coordinate.y) / settings.emitter.z;
	double coordinate = pieceZ + settings.fishOrigin.z;
	return (slope * coordinate) + image1Coordinate.y - settings.fishOrigin.y;
}

double calculateBonePieceZ(Unprojector::Coordinate image1Coordinate, Unprojector::Settings settings, double pieceX) {
	double slope = settings.emitter.z / (settings.emitter.x - settings.detector1x);
	double coordinate = image1Coordinate.x + pieceX - settings.detector1x;
	return (slope * coordinate) - settings.fishOrigin.z;
}

struct Unprojector::Point calculateBonePieceLocation(Unprojector::Coordinate image1Coordinate, Unprojector::Coordinate image2Coordinate, Unprojector::Settings settings) {
	Unprojector::Point location;
	location.x = calculateBonePieceX(image1Coordinate, image2Coordinate, settings);
	location.z = calculateBonePieceZ(image1Coordinate, settings, location.x);
	location.y = calculateBonePieceY(image1Coordinate, settings, location.z);
	return location;
}

/*int getDeltaI(Unprojector::Settings settings, double zCoordinate) {
	double deltaD = settings.detector2x - settings.detector1x;
	double ratio = ((zCoordinate + settings.fishOrigin.z) * deltaD) / settings.emitter.z;
	ratio -= deltaD;
	return (int) ceil(settings.detectorResolution * ratio);
}

int minBoneDisplacementPixels(Unprojector::Settings settings) {
	return getDeltaI(settings, settings.fishZMin);
}

int maxBoneDisplacementPixels(Unprojector::Settings settings) {
	return getDeltaI(settings, settings.fishZMax);
}*/

//Helper functions
bool compareLineSegment(std::vector<Unprojector::Point> segment, std::vector<Unprojector::Point> otherSegment) {
	return segment[0].x < otherSegment[0].x;
}

std::vector<std::vector<Unprojector::Point>> sortLineSegments(std::vector<std::vector<Unprojector::Point>> lineSegments) {
	std::sort(lineSegments.begin(), lineSegments.end(), compareLineSegment);
	return lineSegments;
}

double distance(Unprojector::Point point1, Unprojector::Point point2) {
	double dx = point2.x - point1.x;
	double dy = point2.y - point1.y;
	return sqrt(dx*dx + dy*dy);
}

void Unprojector::unproject(std::vector<std::vector<Unprojector::Point>> leftBones, std::vector<std::vector<Unprojector::Point>> rightBones, Unprojector::Settings settings, std::string name, bool verbose) {
	std::vector<Unprojector::Point> unprojectedPairs;
	
	while((leftBones.size() > 0) && (rightBones.size() > 0)) {
		if(verbose) {
			std::cout << leftBones.size() << "." << leftBones[0].size() << " bones remaining.\n";
		}

		// 1. Get leftmost point on left image
		
		leftBones = sortLineSegments(leftBones);
		rightBones = sortLineSegments(rightBones);
		
		std::vector<Unprojector::Point> leftMostSegment = leftBones.at(0);

		Unprojector::Point point = leftBones.at(0).at(0);	//take leftmost point


		// 2. Find closest point on other image

		int nearestSegmentIndex = -1;
		int nearestPointIndex = -1;
		Unprojector::Point nearestPoint = rightBones.at(0).at(0);
		double minDistance = distance(point, nearestPoint); 

		for(int line = 0; line < (int)rightBones.size(); line++) {
			std::vector<Unprojector::Point> otherSegment = rightBones.at(line);
			for(int otherPointIndex = 0; otherPointIndex < (int)otherSegment.size(); otherPointIndex++) {
				Unprojector::Point otherPoint = otherSegment.at(otherPointIndex);
				double deltaRow = abs(otherPoint.y - point.y);
				double pointDistance = distance(point, otherPoint);
				if((pointDistance < minDistance) && (deltaRow <= 0) && (otherPoint.x > point.x)) {
					minDistance = pointDistance;
					nearestSegmentIndex = line;
					nearestPointIndex = otherPointIndex;
					nearestPoint = otherPoint;
				}
			}
		}

		
		// 3. Perform unprojection

		if(nearestSegmentIndex != -1) { //only if a point was found in step 2
			Unprojector::Coordinate leftCoord;
			leftCoord.x = point.x; //point.y is column -> x coordinate in coordinate system
			leftCoord.y = settings.imageHeight - 1 - point.y; //point.x is column -> invert with respect to image height to get y coordinate in system

			Unprojector::Coordinate rightCoord;
			rightCoord.x = nearestPoint.x + settings.detector2x - settings.detector1x; //getting the X coordinates in the same coordinate space
			rightCoord.y = settings.imageHeight - 1 - nearestPoint.y;

			Unprojector::Point unprojected = calculateBonePieceLocation(leftCoord, rightCoord, settings);
			unprojectedPairs.push_back(unprojected);
		}

		if(showBoneLinkSteps) {
			cv::Mat out = cv::Mat(settings.imageHeight, settings.imageWidth, CV_8UC3);
			cv::Mat left = ImageAnalyzer::renderBones(leftBones, out.size());
			cv::Mat right = ImageAnalyzer::renderBones(rightBones, out.size());
			cv::Mat connection = cv::Mat(out.size(), CV_8UC1);

			cv::line(connection, cv::Point((int)point.x, (int)point.y), cv::Point((int)nearestPoint.x, (int)nearestPoint.y), cv::Scalar(1, 1, 1, 1));

			std::vector<cv::Mat> channels;
			channels.push_back(left);
			channels.push_back(connection);
			channels.push_back(right);
		
			cv::Mat out32 = cv::Mat(out.size(), CV_8UC3);

			cv::merge(channels, out32);

			cv::imshow("out", out32);
			cv::waitKey();
		}


		//4. Get rid of left point

		leftMostSegment.erase(leftMostSegment.begin()); //remove the point
		leftBones.at(0) = leftMostSegment; //update pointer

		if(leftMostSegment.size() == 0) { //throw away the list if it's been processed completely
			leftBones.erase(leftBones.begin());
		}


		//5. get rid of right point

		if(nearestSegmentIndex != -1) { //only if a point was found in step 2
			std::vector<Unprojector::Point> nearestSegment = rightBones.at(nearestSegmentIndex);

			nearestSegment.erase(nearestSegment.begin() + nearestPointIndex);
			rightBones.at(nearestSegmentIndex) = nearestSegment; //update pointer

			if(nearestSegment.size() == 0) { //throw away the list if it's been processed completely
				rightBones.erase(rightBones.begin() + nearestSegmentIndex);
			}
		}

		
	}

	//Write out results

	std::cout << "Writing results..\n";

	std::ofstream outFile;
	outFile.open ("output/" + name + ".txt");
	
	for(unsigned int i = 0; i < unprojectedPairs.size(); i++) {
		Unprojector::Point point = unprojectedPairs.at(i);
		outFile << point.x << ", " << point.y << ", " << point.z << "\n";
	}

	outFile.close();
}
