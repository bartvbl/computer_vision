#pragma once

#include "stdafx.h"

using namespace cv;

bool isInBounds(Mat image, int row, int col);
std::vector<std::pair<int, int>>* floodfill(Mat visited, int row, int col, int label);
std::vector<std::vector<std::pair<int, int>>*>* findAreas(Mat image);
void deallocateFoundAreas(std::vector<std::vector<std::pair<int, int>>*>* areas);
std::vector<bool> findDotAreas(std::vector<std::vector<std::pair<int, int>>*>* areas, Mat output);