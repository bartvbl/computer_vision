#include "ImageAnalyzer.h"
#include "BoneFilter.h"
#include "Unprojector.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <math.h>
#include "Unprojector.h"
#include <minmax.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

//Image processing code



ImageAnalyzer::ImageAnalyzer(void){}
ImageAnalyzer::~ImageAnalyzer(void){}

cv::Mat skeletonize(cv::Mat image, std::string imageName, bool verbose) {
	if(verbose) {
		std::cout << "Thresholding..\n";
	}
	cv::threshold(image, image, 127, 255, cv::THRESH_BINARY); 
	if(verbose) {
		std::cout << "Thinning..\n";
		std::cout << "Channel count: " << image.channels() << " width depth " << (image.depth() == CV_8U) << "\n";
	}
	for(int row = 0; row < image.rows; row++) {
		for(int column = 0; column < image.cols; column++) {
			bool pixel = image.at<unsigned char>(row, column) == 0;
			if(pixel) {
				//std::cout << "Start pixel at " << row << ", " << column << "\n";
				int startRow = row;
				int endRow = startRow;
				while (pixel && (endRow < image.rows-1)) {//-1 to avoid out of bounds
					image.at<unsigned char>(endRow, column) = 255; // clear pixel
					pixel = image.at<unsigned char>(endRow + 1, column) == 0;
					endRow++;
				}//should also do a horizontal pass, then see which distance is lowest. Right now it's vertical only.
				image.at<unsigned char>(endRow, column) = 255; // clear pixel
				int averageRow = (startRow + endRow) / 2;
				//std::cout << "End pixel at " << endRow << "\n";
				image.at<unsigned char>(averageRow, column) = 0; //make pixel black
			}
		}
	}
	if(verbose) {
		std::cout << "Writing thinned image..\n";
		cv::imwrite("steps/"+ imageName+std::to_string((long long)counter)+"_skeleton.png", image);
	}
	counter++;
	return image;
}

inline bool isBlackPixel(cv::Mat image, int row, int column) {
	return image.at<unsigned char>(row, column) == 0;
}

inline double calculateSlope(std::vector<Unprojector::Point> lineSegment, unsigned int startIndex, unsigned int endIndex) {
	if(lineSegment.size() <= 1) {
		return 0;
	}
	double sumX = 0;
	double sumY = 0;
	double squareSumX = 0;
	double sumProduct = 0;

	double pointCount = endIndex - startIndex;

	for(unsigned int i = startIndex; i < endIndex; i++) {
		sumX += lineSegment[i].x;
		sumY += lineSegment[i].y;
		squareSumX += lineSegment[i].x * lineSegment[i].x;
		sumProduct += lineSegment[i].x * lineSegment[i].y;
	}

	double meanX = sumX / pointCount;
	double meanY = sumY / pointCount;

	double slope = (sumProduct - (sumX * meanY)) / (squareSumX - (sumX * meanX));

	return slope;
}

cv::Mat ImageAnalyzer::renderBones(std::vector<std::vector<Unprojector::Point>> lineSegments, cv::Size size) {
	cv::Mat image = cv::Mat::zeros(size, CV_8U);
	std::cout << "Generating bone output image..\n";
	
	for(unsigned int line = 0; line < lineSegments.size(); line++) {
		int startX = 0;
		std::vector<Unprojector::Point> lineSegment = lineSegments.at(line);
		for(unsigned int point = 0; point < lineSegment.size() - 1; point++) {
			if(startX > lineSegment.at(point).x) {
				std::cout << "Went back at " << lineSegment.at(point).x << ", " << lineSegment.at(point).y << " from " << startX << "\n";
			}
			startX = (int)lineSegment.at(point).x;
			cv::Point point1((int)lineSegment.at(point).x,   (int)lineSegment.at(point).y);
			cv::Point point2((int)lineSegment.at(point+1).x, (int)lineSegment.at(point+1).y);
			cv::Scalar colour(255, 255, 255, 255);

			cv::line(image, point1, point2, colour);
		}
	}
	return image;
}


void writeBones(std::vector<std::vector<Unprojector::Point>> lineSegments, cv::Size size, std::string imageName, std::string identifyer) {
	cv::Mat image = ImageAnalyzer::renderBones(lineSegments, size);
	std::cout << "Writing final image..\n";
	cv::imwrite("steps/"+ imageName + std::to_string((long long)counter)+"_"+identifyer+"_final.png", image);
	counter++;
}

void printLineSegment(std::vector<Unprojector::Point> lineSegment) {
	int x = 0;
	for(int i = 0; i < (int)lineSegment.size(); i++) {
		std::cout << "(" << lineSegment.at(i).x << ", " << lineSegment.at(i).y << ")";
		if(x > lineSegment.at(i).x) {
			std::cout << " (smaller)";
		}
		x = (int)lineSegment.at(i).x;
		std::cout << "\n";
	}
}

std::vector<std::vector<Unprojector::Point>> combineLines(std::vector<std::vector<Unprojector::Point>> lineSegments, cv::Size size, std::string imageName, bool verbose, Unprojector::Settings settings) {
	std::cout << "Reconstructing bones..\n";
	bool changed = true;

	while(changed) { //while lines are still being merged together
		changed = false;
		for(int line = 0; line < (int)lineSegments.size(); line++) {
			if(lineSegments.at(line).size() == 1) { //don't attempt to link points
				continue;
			}
			double slope_start = calculateSlope(lineSegments.at(line), 0, min((int)lineSegments.at(line).size(), settings.reconstruction_slope_coordinate_count));
			double slope_end = calculateSlope(lineSegments.at(line), max(0, (int)lineSegments.at(line).size() - settings.reconstruction_slope_coordinate_count), lineSegments.at(line).size());

			for(int otherLine = 0; otherLine < (int)lineSegments.size(); otherLine++) {
				if(line == otherLine) { //if same line segment -> skip
					continue;
				}
				std::vector<Unprojector::Point> lineSegment = lineSegments.at(line); //update pointer
				std::vector<Unprojector::Point> otherLineSegment = lineSegments.at(otherLine);
				
				double otherLineStartX = otherLineSegment.at(0).x;
				double otherLineStartY = otherLineSegment.at(0).y;
				double otherLineEndX = otherLineSegment.back().x;
				double otherLineEndY = otherLineSegment.back().y;

				double lineStartX = lineSegment.at(0).x;
				double lineStartY = lineSegment.at(0).y;
				double lineEndX = lineSegment.back().x;
				double lineEndY = lineSegment.back().y;

				bool shouldCombine = false;
				
				if(lineEndX < otherLineStartX) { //otherLinSegment starts after lineSegment ends
					double travelDistance = otherLineStartX - lineEndX;
					
					if((travelDistance > settings.reconstruction_max_travel_x) || (travelDistance <= 0)) { //don't allow lines that are too far apart
						continue;
					}
					
					double deltaY = travelDistance * slope_end;
					double yCoordAtLineStart = deltaY + lineEndY;
					double distanceY = abs(otherLineStartY - yCoordAtLineStart);

					if(distanceY < settings.reconstruction_max_travel_y) {
						shouldCombine = true;
					}
				} else if(otherLineEndX < lineStartX) {
					double travelDistance = lineStartX - otherLineEndX;
					
					if((travelDistance > settings.reconstruction_max_travel_x) || (travelDistance <= 0)) { //don't allow lines that are too far apart
						continue;
					}
					
					double deltaY = travelDistance * slope_start;
					double yCoordAtLineEnd = lineStartY - deltaY; //going in reverse direction -> subtract delta
					double distanceY = abs(otherLineEndY - yCoordAtLineEnd);

					if(distanceY < settings.reconstruction_max_travel_y) {
						shouldCombine = true;
					}
				}
				if(shouldCombine) {
					//connect two bones
					lineSegment.insert(lineSegment.end(), otherLineSegment.begin(), otherLineSegment.end());
					lineSegments[line] = lineSegment; //update pointer
					
					changed = true;
					if(verbose) {
						std::cout << "connected " << lineEndX << ", " << lineEndY << " to " << otherLineStartX << ", " << otherLineStartY << " with size " << lineSegment.size() << " (" << lineSegments.size() << " segments remaining)\n";
					}

					//sort the points in the line segment
					std::sort(lineSegment.begin(), lineSegment.end());
					lineSegments[line] = lineSegment; //update pointer

					//delete other bone from line segments
					lineSegments.erase(lineSegments.begin() + otherLine);

					//update indices as appropriate
					if(line > otherLine) {
						line--;
					}
					otherLine--;

					//reclaculate slopes
					slope_start = calculateSlope(lineSegments.at(line), 0, min((int)lineSegments.at(line).size(), settings.reconstruction_slope_coordinate_count));
					slope_end = calculateSlope(lineSegments.at(line), max(0, (int)lineSegments.at(line).size() - settings.reconstruction_slope_coordinate_count), lineSegments.at(line).size());
				}
			}
		}
	}
	if(verbose) {
		std::cout << "Post-merging: " << lineSegments.size() << " line segments.\n";
		writeBones(lineSegments, size, imageName, "pre_point");
	}

	int removedCount = 0;
	
	for(unsigned int line = 0; line < lineSegments.size(); line++) { //remove all single point line segments
		std::vector<Unprojector::Point> lineSegment = lineSegments.at(line);
		if((int)lineSegment.size() <= settings.reconstruction_min_bone_size) {
			lineSegments.erase(lineSegments.begin() + line);
			line--;
			removedCount++;
		}
	}
	if(verbose) {
		std::cout << "Removed " << removedCount << " line segments that are considered noise.\n";
		std::cout << "Single points: " << lineSegments.size() << " line segments.\n";
	}
	return lineSegments;
}


std::vector<std::vector<Unprojector::Point>> extractLines(cv::Mat image, std::string imageName, bool verbose, Unprojector::Settings settings) {
	std::vector<std::vector<Unprojector::Point>> lineSegments;
	
	for(int column = 0; column < image.cols - 1; column++) { //ignore last row			Loop order matters.
		for(int row = 1; row < image.rows - 1; row++) { //ignore first and last row
			bool pixel = isBlackPixel(image, row, column);
			if(pixel) {
				std::vector<Unprojector::Point> currentLineSegment;
				int linePointRow = row;
				int linePointColumn = column;
				bool endOfLine = false;
				while(!endOfLine) {
					Unprojector::Point currentPoint;
					currentPoint.x = linePointColumn;
					currentPoint.y = linePointRow;
					currentLineSegment.push_back(currentPoint);
					image.at<unsigned char>(linePointRow, linePointColumn) = 255; //remove pixel from image

					//out of bounds checks
					bool canMoveRowUp = linePointRow < image.rows - 1;
					bool canMoveRowDown = linePointRow > 0;
					bool canMoveColumnRight = linePointColumn < image.cols - 1;

					//try to move in an 8-connected direction, not moving back
					if(canMoveRowDown && isBlackPixel(image, linePointRow - 1, linePointColumn)) { //pixel above
						linePointRow--;
					} else if(canMoveRowDown && canMoveColumnRight && isBlackPixel(image, linePointRow - 1, linePointColumn + 1)) { //top right pixel
						linePointRow--;
						linePointColumn++;
					} else if(canMoveColumnRight && isBlackPixel(image, linePointRow, linePointColumn + 1)) { //pixel to the right
						linePointColumn++;
					} else if(canMoveRowUp && canMoveColumnRight && isBlackPixel(image, linePointRow + 1, linePointColumn + 1)) { //bottom right pixel
						linePointRow++;
						linePointColumn++;
					} else if(canMoveRowUp && isBlackPixel(image, linePointRow + 1, linePointColumn)) { //pixel above
						linePointRow++;
					} else {
						endOfLine = true;
					}
				}
				lineSegments.push_back(currentLineSegment);
			}
		}
	}

	lineSegments = combineLines(lineSegments, image.size(), imageName, verbose, settings);

	if(verbose) {
		writeBones(lineSegments, image.size(), imageName, "final");
	}

	return lineSegments;
}

std::vector<std::vector<Unprojector::Point>> ImageAnalyzer::process(cv::Mat image, std::string name, Unprojector::Settings settings, bool verbose) {
	if(image.depth() != CV_8UC1) {
		std::cout << "Warning: input image is 16-bit. Converting to 8 bit..\n";
		image.convertTo(image, CV_8UC1);
	}

	std::cout << "Skeletonising image..\n";

	image = skeletonize(image, name, verbose);
	
	std::cout << "Finding connected points..\n";

	std::vector<std::vector<Unprojector::Point>> bones = extractLines(image, name, verbose, settings);
	//cv::Mat bonesImage = renderBones(bones, image.size());

	std::cout << "Found " << bones.size() << " bones.\n";

	std::cout << "Bones have been extracted from image. Press ENTER to continue.\n";

	std::cin.ignore();

	return bones;
}