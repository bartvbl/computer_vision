#pragma once

#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>

class MarkedUnprojector
{
public:
	MarkedUnprojector(void);
	~MarkedUnprojector(void);

	struct Coordinate {
		double x;
		double y;
	};

	struct Point {
		double x;
		double y;
		double z;

		bool operator < (const MarkedUnprojector::Point& other) {
			return x < other.x;
		}
	};

	struct Settings {														//	Names as used in report
		double						detector1x;								//	D1_x
		double						detector2x;								//	D2_x
		struct MarkedUnprojector::Point	emitter;								//	E
		struct MarkedUnprojector::Point	fishOrigin;								//	F
		int							imageWidth;								//  [not mentioned in report, used by program]
		int							imageHeight;							//  [not mentioned in report, used by program]
		int							reconstruction_slope_coordinate_count;	//  [used by program]
		int							reconstruction_max_travel_x;			//  [used by program]
		int							reconstruction_max_travel_y;			//  [used by program]
		int							reconstruction_min_bone_size;			//  [used by program]
	};

	struct LineSegment {
		struct MarkedUnprojector::Point start;
		struct MarkedUnprojector::Point end;
	};

	static void unproject(cv::Mat leftBones, cv::Mat rightBones, MarkedUnprojector::Settings settings, std::string name, bool verbose);
};

