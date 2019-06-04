#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>



//  Function to generate a salt-and-pepper noise.
//  Parameter "cover" controls the percentage of pixels influenced by a noise and is in the range of 0.0-1.0.
void saltAndPepper(const cv::Mat& src, cv::Mat& dst, float cover)
{
	dst = src.clone();
	// Generate a noise to random pixels .
	// The amount of noisy pixels must corelate to cover parameter. 

	/*  Working area - begin */
	
	cv::Mat sp_noise = cv::Mat::zeros(src.size(), CV_8UC1); // Create a black and white matrix of zeroes.
	cv::randu(sp_noise, 0, 255); // Randomise the matrix with values between 0 (black) and 255 (white).

	float limit = 255 * cover / 2; // Calculate a limit to be used when cutting the noise matrix.
	cv::Mat black = sp_noise < limit; // Get the lower values (near black) based on the limit.
	cv::Mat white = sp_noise > 255 - limit; // Get the higher values (near white) based on the limit.

	// Use the 'black' and 'white' matrices created above as masks for the source image.
	dst.setTo(255, white);
	dst.setTo(0, black);

	// Showing images (for testing).
	//cv::imshow("input", src);
	//cv::imshow("noise", sp_noise);
	//cv::imshow("output", dst);
	//cv::waitKey(0);

	/*  Working area - end */
}


//  Function to filter the image using median filter.
//     - filter size in pixels


void median(const cv::Mat& src, cv::Mat& dst, int size)
{
	// size - we need an odd number
	int center = size / 2;
	center = MAX(1, center);
	size = 2 * center + 1;

	// enlarge the image and copy the border values
	cv::Mat srcBorder;
	copyMakeBorder(src, srcBorder, center, center, center, center, cv::BORDER_REPLICATE);

	// prepare the output image
	dst = cv::Mat(src.size(), src.type());

	// manually implement the median filter and apply it to the input (src) image and store it to the output (dst) image
	// to sort the values use std::sort()

	/*  Working area - begin */
	
	int offset = center;
	float kerSize = size * size;

	for (int r = offset; r <= src.rows - offset; r++) {
		for (int c = offset; c <= src.cols - offset; c++) {
			cv::Mat submatrix = src.colRange(c - offset, c + offset).rowRange(r - offset, r + offset); // Extract the sub-kernel around the element at hand.
			cv::Scalar value = cv::sum(submatrix); // Calculate the total of the pixel values in the sub-kernel.
			uchar pixel = (uchar)((float)value[0] / kerSize); // Normalize the value.
			dst.at<uchar>(r, c) = pixel; // Assign the value to the output image.
		}
	}

	// Show images (for testing).
	//cv::imshow("input", src);
	//cv::imshow("border", srcBorder);
	//cv::imshow("output", dst);
	//cv::waitKey(0);

	/*  Working area - end */

	return;
}



/* Evaluation function. */
void checkDifferences(const cv::Mat test, const cv::Mat ref, std::string tag, bool save = false);


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//
// Examples of input parameters
// ./mt-05 ../../data/garden.png 0.05 7

int main(int argc, char* argv[])
{
	std::string img_path = "";
	float noise_amount = 0.05;
	int filter_size = 7;

	// check input parameters
	if (argc > 1) img_path = std::string(argv[1]);
	if (argc > 2) noise_amount = atof(argv[2]);
	if (argc > 3) filter_size = atoi(argv[3]);

	// load testing images
	cv::Mat src_rgb = cv::imread(img_path);

	// check testing images
	if (src_rgb.empty()) {
		std::cout << "Failed to load image: " << img_path << std::endl;
		return -1;
	}

	cv::Mat src_gray;
	cv::cvtColor(src_rgb, src_gray, cv::COLOR_BGR2GRAY); // CV_BGR2GRAY -> COLOR_BGR2GRAY

	//---------------------------------------------------------------------------

	cv::Mat zasum, medi, medi_ref;

	saltAndPepper(src_gray, zasum, noise_amount);

	median(zasum, medi, filter_size);
	cv::medianBlur(zasum, medi_ref, filter_size);

	// evaluation
	checkDifferences(zasum, src_gray, "05_noise", true);
	checkDifferences(medi, medi_ref, "05_median", true);
	std::cout << std::endl;

	return 0;
}
//---------------------------------------------------------------------------




void checkDifferences(const cv::Mat test, const cv::Mat ref, std::string tag, bool save)
{
	double mav = 255., err = 255., nonzeros = 255.;
	cv::Mat diff;

	if (!test.empty() && !ref.empty()) {
		cv::absdiff(test, ref, diff);
		cv::minMaxLoc(diff, NULL, &mav);
		err = (cv::sum(diff).val[0] / (diff.rows*diff.cols));
		nonzeros = 1. * cv::countNonZero(diff) / (diff.rows*diff.cols);
	}

	if (save) {
		if (!test.empty()) { cv::imwrite((tag + ".png").c_str(), test); }
		if (!diff.empty()) { diff *= 255;	cv::imwrite((tag + "_err.png").c_str(), diff); }
	}

	printf("%s %.2f %.2f %.2f ", tag.c_str(), err, nonzeros, mav);

	return;
}

