#include <stdio.h>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

#include "ImageAnalyzer.h"
#include "BoneFilter.h"

#include "MarkedUnprojector.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

cv::Mat loadImage(std::string src) {
	std::cout << "Loading image at " << src << "\n";
	cv::Mat originalImage = cv::imread(src, cv::IMREAD_UNCHANGED);

	if(originalImage.depth() != CV_16U) {
		std::cout << "Warning: input image is 8-bit only. Converting to 16 bit..\n";
		originalImage.convertTo(originalImage, CV_16U);
	}
	if(originalImage.channels() != 1) {
		std::cout << "Warning: input image has more than 1 channel. Converting back to 1..\n";
		std::vector<cv::Mat> channels(3);
		cv::split(originalImage, channels);
		originalImage = channels[0];
	}
	std::cout << "\n";
	return originalImage;
}

/*std::vector<std::vector<Unprojector::Point>> process(std::string src, std::string name, Unprojector::Settings settings, bool verbose) {
	cv::Mat image = loadImage(src);
	image = BoneFilter::filter(image, name, verbose);
	std::vector<std::vector<Unprojector::Point>> bones = ImageAnalyzer::process(image, name, settings, verbose);
	return bones;
}*/

bool startswith(std::string string, std::string prefix) {
	return string.substr(0, prefix.size()) == prefix;
}

int getValue(std::string line) {
	return atoi(line.substr(line.find_first_of('=')+1, line.size()).c_str());
}

MarkedUnprojector::Settings readSettings(bool verbose) {
	std::cout << "Reading settings..\n";
	std::ifstream infile("setup.cfg");

	MarkedUnprojector::Point emitterLocation;
	MarkedUnprojector::Point fishOrigin;
	MarkedUnprojector::Settings settings;

	emitterLocation.x = 0;
	emitterLocation.y = 0;
	emitterLocation.z = 0;

	fishOrigin.x = 0;
	fishOrigin.y = 0;
	fishOrigin.z = 0;

	settings.detector1x = 0;
	settings.detector2x = 0;

	settings.imageWidth = 0;
	settings.imageHeight = 0;
	
	std::string line;
	while (std::getline(infile, line))
	{
		//remove whitespace
		line.erase(remove(line.begin(), line.end(), ' '), line.end());

		//emitter position
		if(startswith(line, "emitter.x")) {
			emitterLocation.x = getValue(line);
		}
		if(startswith(line, "emitter.y")) {
			emitterLocation.y = getValue(line);
		}
		if(startswith(line, "emitter.z")) {
			emitterLocation.z = getValue(line);
		}

		//fish origin
		if(startswith(line, "fish-origin.x")) {
			fishOrigin.x = getValue(line);
		}
		if(startswith(line, "fish-origin.y")) {
			fishOrigin.y = getValue(line);
		}
		if(startswith(line, "fish-origin.z")) {
			fishOrigin.z = getValue(line);
		}

		//detector positions
		if(startswith(line, "detector1.x")) {
			settings.detector1x = getValue(line);
		}
		if(startswith(line, "detector2.x")) {
			settings.detector2x = getValue(line);
		}

		//input image size
		if(startswith(line, "image.width")) {
			settings.imageWidth = getValue(line);
		}
		if(startswith(line, "image.height")) {
			settings.imageHeight = getValue(line);
		}

		//bone reconstruction
		if(startswith(line, "bone_reconstruction.slope_used_coordinate_count")) {
			settings.reconstruction_slope_coordinate_count = getValue(line);
		}
		if(startswith(line, "bone_reconstruction.max_travel_x")) {
			settings.reconstruction_max_travel_x = getValue(line);
		}
		if(startswith(line, "bone_reconstruction.max_travel_y")) {
			settings.reconstruction_max_travel_y = getValue(line);
		}
		if(startswith(line, "bone_reconstruction.min_bone_size")) {
			settings.reconstruction_min_bone_size = getValue(line);
		}
	}
	
	settings.emitter = emitterLocation;
	settings.fishOrigin = fishOrigin;

	if(verbose) {
		std::cout << "Read settings: \n\n";

		std::cout << "emitter.x = " << settings.emitter.x << "\n";
		std::cout << "emitter.y = " << settings.emitter.y << "\n";
		std::cout << "emitter.z = " << settings.emitter.z << "\n\n";

		std::cout << "fish-origin.x = " << settings.fishOrigin.x << "\n";
		std::cout << "fish-origin.y = " << settings.fishOrigin.y << "\n";
		std::cout << "fish-origin.z = " << settings.fishOrigin.z << "\n\n";

		std::cout << "detector1.x = " << settings.detector1x << "\n";
		std::cout << "detector2.x = " << settings.detector2x << "\n\n";

		std::cout << "image.width = " << settings.imageWidth << "\n";
		std::cout << "image.height = " << settings.imageHeight << "\n\n";

		std::cout << "bone_reconstruction.max_travel_x = " << settings.reconstruction_max_travel_x << "\n";
		std::cout << "bone_reconstruction.max_travel_y = " << settings.reconstruction_max_travel_y << "\n";
		std::cout << "bone_reconstruction.min_bone_size = " << settings.reconstruction_min_bone_size << "\n";
		std::cout << "bone_reconstruction.slope_used_coordinate_count = " << settings.reconstruction_slope_coordinate_count << "\n\n";
	}

	return settings;
}

int main(int argc, char** argv)
{
	if(argc < 3) {
		std::cout << "Missing arguments. Usage: image1.png \"image_name\" [-verbose]\n";
		return 0;
	}
	
	bool verbose = false;
	if(argc >= 4) {
		std::string param = argv[3];
		verbose = param == "-verbose";
	}

	bool useMarkedImages = false;
	if (argc >= 5) {
		std::string param = argv[4];
		useMarkedImages = param == "-usemarked";
	}
	
	MarkedUnprojector::Settings settings = readSettings(verbose);
	
	std::cout << "\n-- Extracting bones from images --\n\n";

	if (!useMarkedImages) {
		//std::vector<std::vector<Unprojector::Point>> left = process(argv[1], argv[2], settings, verbose);
		//std::vector<std::vector<Unprojector::Point>> right = process(argv[2], "simulator_right", settings, verbose);

		//std::cout << "\n-- Performing unprojection --\n\n";

		//Unprojector::unproject(left, right, settings, "simulated", verbose);
	}
	else {
		cv::Mat leftMarked = cv::imread(argv[1], cv::IMREAD_UNCHANGED);
		cv::Mat rightMarked = cv::imread(argv[2], cv::IMREAD_UNCHANGED);
		
		MarkedUnprojector::unproject(leftMarked, rightMarked, settings, "simulated", verbose);
	}

	std::cout << "Complete.";
    return 0;
}