#pragma once

cv::Mat segmentOriginalFrame(cv::Mat frame);

struct PixelCoordinate {
	int x;
	int y;
};