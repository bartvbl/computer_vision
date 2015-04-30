#pragma once

#include <string>
#include <opencv2/opencv.hpp>

extern int counter;

class BoneFilter
{
public:
	BoneFilter(void);
	~BoneFilter(void);

	static cv::Mat filter(cv::Mat image, std::string name, bool verbose);
	static void write(std::string name, std::string operation, cv::Mat image);
};

