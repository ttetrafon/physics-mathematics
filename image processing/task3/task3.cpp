#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


#define LOW_PASS_FILTER 1
#define HIGH_PASS_FILTER 2


/* Misc support function prepared to use */
/* Function rearrange the spectrum to have the origin in the middle of the spectrum image. */
void rearrangeSpectrum(cv::Mat& s);
/* Function computes spectrum magnitude - absolute values out of complex numbers in pixeles. */
cv::Mat spectrumMagnitude(cv::Mat & specCplx);

/* Evaluation of the results based on reference results. */
void checkDifferences(const cv::Mat test, const cv::Mat ref, std::string tag, bool save = false);



/* Functions to work on and finish their functionality  ... */
/*  Filtration functions - high, resp. low pass. */
void passFilter(const cv::Mat& src, cv::Mat& dst, int limit_frequency, int flag, cv::Mat * spektrum = NULL)
{
	// find optimal size of the spectrum for effective DFT computation, cv::getOptimalDFTSize() used
	cv::Size dftSize;
	dftSize.width = cv::getOptimalDFTSize(src.cols);
	dftSize.height = cv::getOptimalDFTSize(src.rows);

	// create new spectrum image of optimal size and reset to zeros
	cv::Mat srcPadded;
	srcPadded = cv::Mat::zeros(dftSize, CV_32FC1);
	src.convertTo(srcPadded(cv::Rect(0, 0, src.cols, src.rows)), srcPadded.type());

	// use OpenCV function to compute an image spectrum 
	cv::Mat spectrumCplx;
	cv::dft(srcPadded, spectrumCplx, cv::DFT_COMPLEX_OUTPUT, src.rows);

	// re-arrange the spectrum - zero frequencies in the middle of the spectrum image
	rearrangeSpectrum(spectrumCplx);

	/*
		Do you own implementation of low and high pass by setting right areas of to spectrum to zeros,
		e.g. for low pass by setting the rectangular area between zero and limit_frequency to zeros
	*/

	/*  Working area - begin */

	src.copyTo(dst);

	if (flag == HIGH_PASS_FILTER) {
		// keep only spectrum frequencies bellow the limit_frequency
		for (int r = 0; r < src.rows; r++) {
			for (int c = 0; c < src.cols; c++) {
				int pixel = (int)src.at<uchar>(r, c);
				if (pixel < limit_frequency) {
					dst.at<uchar>(r, c) = 0;
				}
			}
		}
	}
	else if (flag == LOW_PASS_FILTER) {
		// keep only spectrum frequencies above the limit_frequency
		for (int r = 0; r < src.rows; r++) {
			for (int c = 0; c < src.cols; c++) {
				int pixel = (int)src.at<uchar>(r, c);
				if (pixel > limit_frequency) {
					dst.at<uchar>(r, c) = 0;
				}
			}
		}
	}

	// Setup a simple kernel to filter the result of the frequency pass.
	int s = 3;
	cv::Mat kernel = cv::Mat::ones(s, s, CV_32F)*0.8;
	cv::Scalar kTot = cv::sum(kernel);
	for (int i = 0; i < s; i++) {
		for (int j = 0; j < s; j++) {
			kernel.at<float>(i, j) /= (float)kTot[0];
		}
	}
	cv::filter2D(dst, dst, -1, kernel); // Filter the output image with the kernel.

	// Show images (for testing).
	cv::imshow("input", src);
	cv::imshow("output", dst);
	cv::waitKey(0);

	/*  Working area - end */

	if (spektrum != NULL) *spektrum = spectrumCplx.clone();

	// re-arrange the spectrum back
	rearrangeSpectrum(spectrumCplx);

	// use OpenCV function to transform a spectrum back to an image 
	cv::dft(spectrumCplx, srcPadded, cv::DFT_REAL_OUTPUT + cv::DFT_INVERSE + cv::DFT_SCALE, src.rows);

	// normalize and convert the output values to origin format of the input image
	cv::normalize(srcPadded, srcPadded, 0, 1, cv::NORM_MINMAX); // CV_MINMAX -> NORM_MINMAX
	srcPadded(cv::Rect(0, 0, src.cols, src.rows)).convertTo(dst, src.type(), 255, 0);

	/*  // only for debuggin purposes, when submitting the source-code, it MUST be back in comment!!!
		cv::imshow("Input image", src );
		cv::imshow("Size of the spectrum", spectrumMagnitude(spectrumCplx) );
		cv::imshow("Output filtered image", dst );
		cv::waitKey();
	*/
	return;
}





//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//
// Examples of input parameters
//
// mt03 image_path spatial_frequency_limit [path to reference results]


int main(int argc, char* argv[])
{
	std::string img_path = "";
	std::string ref_path = "";
	int limit_frequency = 100;

	// check input parameters
	if (argc > 1) img_path = std::string(argv[1]);
	if (argc > 2) limit_frequency = atoi(argv[2]);
	if (argc > 3) ref_path = std::string(argv[3]);

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

	cv::Mat low_pass, high_pass;
	cv::Mat low_spec_cplx, high_spec_cplx;
	cv::Mat low_spec, high_spec;
	passFilter(src_gray, low_pass, limit_frequency, LOW_PASS_FILTER, &low_spec_cplx);
	passFilter(src_gray, high_pass, limit_frequency, HIGH_PASS_FILTER, &high_spec_cplx);
	spectrumMagnitude(low_spec_cplx).convertTo(low_spec, CV_8UC1, 255);
	spectrumMagnitude(high_spec_cplx).convertTo(high_spec, CV_8UC1, 255);

	cv::Mat low_pass_ref, high_pass_ref;
	cv::Mat low_spec_ref, high_spec_ref;
	low_pass_ref = cv::imread(ref_path + "low_pass_ref.png", cv::IMREAD_GRAYSCALE);
	high_pass_ref = cv::imread(ref_path + "high_pass_ref.png", cv::IMREAD_GRAYSCALE);
	low_spec_ref = cv::imread(ref_path + "low_spec_ref.png", cv::IMREAD_GRAYSCALE);
	high_spec_ref = cv::imread(ref_path + "high_spec_ref.png", cv::IMREAD_GRAYSCALE);
	if (low_pass_ref.empty() || low_spec_ref.empty() || high_pass_ref.empty() || high_spec_ref.empty())
		std::cout << "WARNING: references data failed to load." << std::endl;

	// evaluation
	checkDifferences(low_pass, low_pass_ref, "03_low_pass", true);
	checkDifferences(low_spec, low_spec_ref, "03_low_spec", true);
	checkDifferences(high_pass, high_pass_ref, "03_high_pass", true);
	checkDifferences(high_spec, high_spec_ref, "03_high_spec", true);
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



void rearrangeSpectrum(cv::Mat& s)
{
	int cx = s.cols / 2;
	int cy = s.rows / 2;

	cv::Mat q0(s, cv::Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	cv::Mat q1(s, cv::Rect(cx, 0, cx, cy));  // Top-Right
	cv::Mat q2(s, cv::Rect(0, cy, cx, cy));  // Bottom-Left
	cv::Mat q3(s, cv::Rect(cx, cy, cx, cy)); // Bottom-Right

	cv::Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
	q2.copyTo(q1);
	tmp.copyTo(q2);
}

cv::Mat spectrumMagnitude(cv::Mat & specCplx)
{
	cv::Mat specMag, planes[2];
	cv::split(specCplx, planes);
	cv::magnitude(planes[0], planes[1], planes[0]);
	cv::log((planes[0] + cv::Scalar::all(1)), specMag);
	cv::normalize(specMag, specMag, 0, 1, cv::NORM_MINMAX); // CV_MINMAX -> NORM_MINMAX
	return specMag;
}
