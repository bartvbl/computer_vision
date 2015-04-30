
#include "stdafx.h"
#include "DiceFilter.h"
#include "DotFinder.h"

using namespace std;
using namespace cv;


int main(int argc, const char** argv)
{
	// Open the camera stream
	cv::VideoCapture capture(0);

	// Debug windows
	namedWindow("frame", CV_WINDOW_AUTOSIZE);
	//namedWindow("original", CV_WINDOW_AUTOSIZE);
	//namedWindow("histogram", CV_WINDOW_AUTOSIZE);

	Mat frame;
	while(true)
	{
		capture >> frame;

		//imshow("original", frame);

		Mat thresholded = segmentOriginalFrame(frame);
		std::vector<int> dieRolls = findDots(thresholded, frame);


		
		char discard; // get rid of the input
		std::cin >> discard;
		for (int i = 0; i < dieRolls.size(); i++) {
			std::cout << dieRolls.at(i) << ", ";
		}
		std::cout << "\n";
		
		waitKey(30);
	}

	waitKey(0);

	destroyWindow("frame");
	//destroyWindow("original");
	//destroyWindow("histogram");

	return 0;
}

