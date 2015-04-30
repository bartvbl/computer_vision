#pragma once

#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Unprojector.h"

class ImageAnalyzer
{
public:
	ImageAnalyzer(void);
	~ImageAnalyzer(void);
	static std::vector<std::vector<Unprojector::Point>> process(cv::Mat image, std::string name, Unprojector::Settings settings, bool verbose);
	static cv::Mat renderBones(std::vector<std::vector<Unprojector::Point>> lineSegments, cv::Size size);
private:
	struct Point;
	struct Coordinate;
	struct Settings;

};

