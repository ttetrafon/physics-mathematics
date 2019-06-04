#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/fast_math.hpp>


/*
Function for grayscale image (0-255) computes the convolution with given kernel with size 3x3 of float (CV_32FC1).
Border image values keep 0.
Implement on your own by iteration over the image, for each pixels' neighbourhood do a convolution with the kernel.
Normalize the result value before storing to output image.

For implementation, the following OpenVC functions/methods are only allowed:
Mat:: rows, cols, step(), size(), at<>(), zeros(), ones(), eye()
*/
void convolution(cv::Mat& gray, const cv::Mat& kernel, cv::Mat& dst)
{
	dst = cv::Mat::zeros(gray.size(), CV_32FC1);

	if (kernel.rows != 3 || kernel.cols != 3)
		return;

	/*  Working area - begin */

	cv::Mat temp = cv::Mat::zeros(gray.size(), CV_32FC1); // Set a temporary Matrix to hold the image, to be used for normalization.

	float kerSum = 0; // Calculate the kernel's sum.
	for (int i = 0; i < kernel.rows; i++) {
		for (int j = 0; j < kernel.cols; j++) {
			kerSum += abs(kernel.at<float>(i, j));
		}
	}

	float oldMax = 0, oldMin = 0; // Variables to store min & max value of the calculation, to be used for normalization.
	float newMax = 1, newMin = -1; // Set the limits of pixel values in the output image.

	for (int r = 1; r < gray.rows - 1; r++) {
		for (int c = 1; c < gray.cols - 1; c++) {
			float pixel = ( // Calculate the convoluted pixel value.
				kernel.at<float>(0, 0)*(float)gray.at<uchar>(r - 1, c - 1) + 
				kernel.at<float>(0, 1)*(float)gray.at<uchar>(r - 1, c) +
				kernel.at<float>(0, 2)*(float)gray.at<uchar>(r - 1, c + 1) +
				kernel.at<float>(1, 0)*(float)gray.at<uchar>(r, c - 1) +
				kernel.at<float>(1, 1)*(float)gray.at<uchar>(r, c) +
				kernel.at<float>(1, 2)*(float)gray.at<uchar>(r, c + 1) +
				kernel.at<float>(2, 0)*(float)gray.at<uchar>(r + 1, c - 1) +
				kernel.at<float>(2, 1)*(float)gray.at<uchar>(r + 1, c) +
				kernel.at<float>(2, 2)*(float)gray.at<uchar>(r + 1, c + 1)
				) / kerSum;
			if (pixel > oldMax) {
				oldMax = pixel;
			}
			if (pixel < oldMin) {
				oldMin = pixel;
			}
			temp.at<float>(r, c) = pixel; // Assign the calculated value of the pixel.
		}
	}

	float oldRange = oldMax - oldMin;
	float newRange = newMax - newMin;

	for (int r = 1; r < gray.rows - 1; r++) {
		for (int c = 1; c < gray.cols - 1; c++) {
			// Perform linear normalization.
			float scale = (temp.at<float>(r, c) - oldMin) / oldRange;
			dst.at<float>(r, c) = (newRange * scale) + newMin;
		}
	}
	
	// Display input-output images and wait (for testing).
	cv::imshow("conv input", gray);
	cv::imshow("cov output", temp);
	cv::imshow("conv output (normalized)", dst);
	cv::waitKey(0);
	/*  Working area - end */

	return;
}

/*
The function performs geometric image transformation using interpolation with the nearest neighbor.

The input transformation describes the transformation of the pixels from the input image to the output.
We use the inverse process (see lecture), i.e. calculate inverse transformation and
for each point of the output matrix we will look for its position in the input image and use the
interpolation with the nearest neighbor to find the output pixel value.
*/
void geometricalTransform(const cv::Mat& src, cv::Mat& dst, const cv::Mat& transformation)
{
	// inverse transform
	cv::Mat T = transformation.inv();

	// output matrix/image
	dst = cv::Mat::zeros(src.size(), CV_8UC1);

	/*
	For each pixel of the output image do
		1. map its position to the source image using inverse transform
			viz http://docs.opencv.org/modules/imgproc/doc/geometric_transformations.html?highlight=warpaffine#warpaffine
		2. check the coordinates to be within the image
		3. apply nearest neighbor interpolation

	For implementation, the following OpenVC functions/methods are only allowed:
		Mat:: rows, cols, step(), size(), at<>(), zeros(), ones(), eye(), cvRound()
	*/

	/*  Working area - begin */

	for (int r = 0; r < src.rows; r++) {
		for (int c = 0; c < src.cols; c++) {
			// Tranformation: dst(x,y) = src(M11*x + M12*y + M13, M12*x + M21*y + M23)
			float rOld = transformation.at<float>(0, 0) * r + transformation.at<float>(0, 1) * c + transformation.at<float>(0, 2);
			rOld = (rOld < src.rows && rOld > 0) ? rOld : -1;
			float cOld = transformation.at<float>(1, 0) * r + transformation.at<float>(1, 1) * c + transformation.at<float>(1, 2);
			cOld = (cOld < src.cols && cOld > 0) ? rOld : -1;

			double value = (rOld < 0 || cOld < 0) ? -1 : src.at<uchar>(rOld, cOld); // Set all values that are not defined at negative, to be ignored while interpolating.
			//if (r == c) { std::cout << (int)src.at<uchar>(r,c) << " -> " << value << std::endl; }
			dst.at<uchar>(r, c) = (uchar)value; // Assign the calculated value.
		}
	}

	// Display input-output images and wait (for testing).
	cv::imshow("input", src);
	cv::imshow("output", dst);
	cv::waitKey(0);

	/*  Working area - end */

	return;
}




//---------------------------------------------------------------------------
void checkDifferences(const cv::Mat test, const cv::Mat ref, std::string tag, bool save = false);
//---------------------------------------------------------------------------

//
// Examples of input parameters
//
// mt-02 image_path [rotation in degrees] [scale]


int main(int argc, char* argv[])
{
	std::string img_path = "";
	float s = 1.f;
	float r = 0.f;

	// check input parameters
	if (argc > 1) img_path = std::string(argv[1]);
	if (argc > 2) r = atof(argv[2]);
	if (argc > 3) s = atof(argv[3]);


	// load testing images
	cv::Mat src_rgb = cv::imread(img_path);

	// check testing images
	if (src_rgb.empty()) {
		std::cout << "Failed to load image: " << img_path << std::endl;
		return -1;
	}

	// convert to grayscale image
	cv::Mat src_gray;
	cvtColor(src_rgb, src_gray, cv::COLOR_BGR2GRAY); // CHANGED 'CV_BGR2GRAY' TO 'COLOR_BGR2GRAY'!

	//---------------------------------------------------------------------------

	// convolution kernel
	float ker[9] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
	cv::Mat kernel(3, 3, CV_32FC1, ker);
	cv::Mat conv_res, conv_ref;

	// your own convolution function
	convolution(src_gray, kernel, conv_res);

	// reference solution
	cv::flip(kernel, kernel, -1);
	cv::filter2D(src_gray, conv_ref, CV_32F, kernel);
	conv_ref *= 1.f / (cv::sum(abs(kernel)).val[0] + 0.000000001);
	// reset border values of reference output (our function does not work well on image border)
	cv::rectangle(conv_ref, cv::Point(0, 0), cv::Point(conv_ref.cols - 1, conv_ref.rows - 1), cv::Scalar::all(0), 1);

	//---------------------------------------------------------------------------

	// geometry transformation

	// image center
	cv::Point2f c = cv::Point2f(0.5f*src_gray.cols, 0.5f*src_gray.rows);

	// translation, rotation, scale and final matrices
	cv::Mat T = cv::Mat::eye(3, 3, CV_32FC1);
	cv::Mat R = cv::Mat::eye(3, 3, CV_32FC1);
	cv::Mat S = cv::Mat::eye(3, 3, CV_32FC1);
	cv::Mat M = cv::Mat::eye(3, 3, CV_32FC1);

	/*
	Set the translation, rotation, and scaling matrix coefficients
	and multiply the transformation matrix in the correct order,
	so that the resulting image is rotated and rescaled around its center.
	Values are in variables 'c', 'r' and 's'.
	Notes:
		- the angle is in degrees (not in radians),
		- we want to rotate counterclockwise (the rotation matrix must be inverted before use),
		- multiplication of matrices A * B * C is in c++ from left, brackets (A * (B * C)) must be used to multiply from right

	For implementation, the following OpenVC functions/methods are only allowed:
		Mat:: rows, cols, at<>(), zeros(), ones(), eye(), inv(), cos, sin, CV_PI
	*/

	/*  Working area - begin */

	// Transformation matrix:
	// [	a	b	(1-a)*centerX-b*centerY	]
	// [	-b	a	b*centerX+(1-a)*centerY	]
	// a = scale*cos(angle)
	// b = scale*sin(angle)

	std::cout << "r: " << r << std::endl;
	std::cout << "s: " << s << std::endl;

	float a = s * cos(r * CV_PI / 180);
	float b = s * sin(r * CV_PI / 180);
	std::cout << "a: " << a << std::endl;
	std::cout << "b: " << b << std::endl;

	M.at<float>(0, 0) = a;
	M.at<float>(0, 1) = b;
	M.at<float>(0, 2) = (1 - a) * src_gray.rows / 2 - b * src_gray.cols / 2;
	M.at<float>(1, 0) = -b;
	M.at<float>(1, 1) = a;
	M.at<float>(1, 2) = b * src_gray.rows / 2 + (1 - a) * src_gray.cols / 2;

	std::cout << M << std::endl;

	/*  Working area - end */

	//std::cout << M << std::endl;

	// final image
	cv::Mat tran_res, tran_ref;

	// your own solution
	geometricalTransform(src_gray, tran_res, M);
	cv::imwrite("tran_res.png", tran_res);

	// reference solution
	cv::Mat Mref = cv::getRotationMatrix2D(c, r, s);
	// warpAffine internally inverts the transformation matrix and interpolates the pixel output values from the input image	
	cv::warpAffine(src_gray, tran_ref, Mref, src_gray.size(), cv::INTER_NEAREST, cv::BORDER_CONSTANT, cv::Scalar::all(0));
	cv::imwrite("tran_ref.png", tran_ref);
	//std::cout << Mref << std::endl;

	//---------------------------------------------------------------------------


	// evaluation
	checkDifferences(conv_res, conv_ref, "convolution", true);
	checkDifferences(tran_res, tran_ref, "geometry", true);
	std::cout << std::endl;

	return 0;
}
//---------------------------------------------------------------------------




void checkDifferences(const cv::Mat test, const cv::Mat ref, std::string tag, bool save)
{
	double mav = 255., err = 255., nonzeros = 1000.;

	if (!test.empty()) {
		cv::Mat diff;
		cv::absdiff(test, ref, diff);
		cv::minMaxLoc(diff, NULL, &mav);
		nonzeros = 1. * cv::countNonZero(diff); // / (diff.rows*diff.cols);
		err = (nonzeros > 0 ? (cv::sum(diff).val[0] / nonzeros) : 0);

		if (save) {
			diff *= 255;
			cv::imwrite((tag + ".png").c_str(), test);
			cv::imwrite((tag + "_err.png").c_str(), diff);
		}
	}

	printf("%s %.2f %.2f %.2f ", tag.c_str(), err, nonzeros, mav);

	return;
}
