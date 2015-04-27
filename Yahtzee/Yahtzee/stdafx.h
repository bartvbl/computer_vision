// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <stack>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

template<class T> inline const T& fastmax(const T& a, const T& b)
{
	return b < a ? a : b;
}

template<class T> inline const T& fastmin(const T& a, const T& b)
{
	return a < b ? a : b;
}
