#include <stdio.h>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define ROOT_PATH std::string("C:/Users/Bart/git/xfish/xfish_c/NoiseEstimation/Debug/images")

using namespace cv;

void calculateHistogram(std::string src) {
	Mat image = imread(src, CV_LOAD_IMAGE_UNCHANGED);
	
	int elements[256] = {};
	
	for(int x = 0; x < image.cols; x++) {
		for(int y = 0; y < image.rows; y++) {
			if(image.at<Vec4b>(y,x)[3] != 0 && image.at<Vec3b>(y,x)[0] != 0) {
				elements[image.at<Vec3b>(y,x)[0]]++;
			}
		}
	}

	//writing output file
	std::ofstream outputFile;
	outputFile.open(ROOT_PATH + "/output.txt");


	for(int i = 0; i < 256; i++) {
		outputFile << elements[i] << '\n';
	}

	outputFile.close();
}

int getPixel(Mat image, int x, int y) {
	//Out of bounds policy: use closest pixel that's in bounds
	if(x < 0) {
		x = 0;
	}
	if(x >= image.cols) {
		x = image.cols - 1;
	}
	if(y < 0) {
		y = 0;
	}
	if(y >= image.rows) {
		y = image.rows - 1;
	}

	return image.at<Vec3b>(y,x)[0];
}

double calculateMean(Mat image, int x, int y, int radius) {
	double sum = 0;
	int count = 0;

	for(int kernelX = -radius; kernelX < radius; kernelX++) {
		for(int kernelY = -radius; kernelY < radius; kernelY++) {
			if(sqrt((double) (kernelX*kernelX + kernelY*kernelY)) <= radius) {
				sum += getPixel(image, x + kernelX, y + kernelY);
				count++;
			}
		}
	}
	return sum / (double) count;
}

double calculateVariance(Mat image, int x, int y, int radius) {
	double mean = calculateMean(image, x, y, radius);
	
	double varianceSum = 0;
	int count = 0;

	for(int kernelX = -radius; kernelX < radius; kernelX++) {
		for(int kernelY = -radius; kernelY < radius; kernelY++) {
			if(sqrt((double) (kernelX*kernelX + kernelY*kernelY)) <= radius) {
				double deltaMean = ((double) getPixel(image, x + kernelX, y + kernelY)) - mean;
				varianceSum += deltaMean * deltaMean;
				count++;
			}
		}
	}

	return varianceSum / (double) count;
}

void runGlobal(std::string src) {
	std::cout << src;
	Mat image = imread(src, CV_LOAD_IMAGE_UNCHANGED);
	
	double sum = 0;
	double count = 0;
	
	for(int x = 0; x < image.cols; x++) {
		for(int y = 0; y < image.rows; y++) {
			if(image.at<Vec4b>(y,x)[3] != 0) {
				sum += image.at<Vec3b>(y,x)[0];
				count++;
			}
		}
	}

	double mean = sum / count;

	printf("Global mean: %f\n", mean);
	
	double varianceSum = 0;
	count = 0;

	for(int x = 0; x < image.cols; x++) {
		for(int y = 0; y < image.rows; y++) {
			if(image.at<Vec4b>(y,x)[3] != 0) {
				double deltaMean = ((double) getPixel(image, x, y)) - mean;
				varianceSum += deltaMean * deltaMean;
				count++;
			}
		}
	}

	double variance = varianceSum / count;

	printf("Global variance: %f\n", variance);

}

void runPerPixel() {
	double const KERNEL_RADIUS = 30;

    Mat image = imread(ROOT_PATH + "/redfish.png", 1);
	Mat varianceImage = Mat::zeros(image.size(), image.type());

	int prevProgress = 0;
	std::vector<double> varianceMatrix(image.cols * image.rows);

	double maxVariance = 0;

	for(int x = 0; x < image.cols; x++) {
		//showing progress
		int progress = (int) floor(((double) x / (double) image.cols) * 100.0);
		prevProgress = progress;
		//printf("%i\n", progress);
		//printf("%e\n", maxVariance);
		

		for(int y = 0; y < image.rows; y++) {
			varianceMatrix[(x * image.rows) + y] = calculateVariance(image, x, y, KERNEL_RADIUS);
			maxVariance = max(maxVariance, varianceMatrix[(x * image.rows) + y]);
		}
	}

	for(int x = 0; x < image.cols; x++) {
		for(int y = 0; y < image.rows; y++) {
			varianceImage.at<Vec3b>(y,x)[0] = (int) floor((varianceMatrix[(x * image.rows) + y] / maxVariance) * 255.0);
		}
	}

    if (!image.data)
    {
        printf("No image data \n");
        return;
    }

	printf("Results of calculations:");

	imwrite(ROOT_PATH + "/output.png", varianceImage);
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", varianceImage);
	

    waitKey(0);
}

int main(int argc, char** argv)
{
	printf("Constructing histogram..");
	calculateHistogram(ROOT_PATH + "/fish.png");
	printf("Estimating background noise\n");
    runGlobal(ROOT_PATH + "/backgroundNoise.png");
	printf("Estimating noise in fish\n");
    runGlobal(ROOT_PATH + "/fish.png");
	runPerPixel();
    return 0;
}