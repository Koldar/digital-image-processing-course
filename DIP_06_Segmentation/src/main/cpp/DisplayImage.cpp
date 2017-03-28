#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <limits.h>
#include <iostream>

using namespace cv;
using namespace std;

struct OtsuThresholdInfo {
	float backgroundWeight;
	float backgroundMean;
	float backgroundVariance;
	float foregroundWeight;
	float foregroundMean;
	float foregroundVariance;
	float classVariance;
};

ostream& operator<<(ostream& os, const OtsuThresholdInfo& dt)
	{
		os << "w=" << dt.backgroundWeight << " m=" << dt.backgroundMean << " v=" << dt.backgroundVariance
				<< " "
				<< "w=" << dt.foregroundWeight << " m=" << dt.foregroundMean << " v=" << dt.foregroundVariance;
	    return os;
	}


void computeHistogram(Mat& image, Mat& output) {
	//COMPUTE HISTOGRAM
	/// Establish the number of bins
	int histSize = 256;
	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 } ;
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;
	/// Compute the histograms:
	calcHist( &image, 1, 0, Mat(), output, 1, &histSize, &histRange, uniform, accumulate );
}

void applyOtsuThreshold(Mat& image, Mat& output, float threshold) {
	//put everything to 0
	output.setTo(0);
	for (int y=0; y<image.rows; y++) {
		for (int x=0; x<image.cols; x++) {
			output.at<uchar>(y, x) = (image.at<uchar>(y, x) >= threshold) ? 0 : 255;
		}
	}
}

void populateWeights(Mat& image, int threshold, OtsuThresholdInfo& output) {
	double pixelNumber = image.rows * image.cols;

	for (int y=0; y<image.rows; y++) {
		for (int x=0; x<image.cols; x++) {
			if (image.at<uchar>(y, x) > threshold) {
				output.foregroundWeight++;
			} else {
				output.backgroundWeight++;
			}
		}
	}

	output.foregroundWeight /= pixelNumber;
	output.backgroundWeight /= pixelNumber;
}

void populateMeans(Mat& image, Mat& histogram, int threshold, OtsuThresholdInfo& output) {

	long sumForeground = 0;
	long sumBackground = 0;
	for (int color=0; color<255; color++) {
		if (color > threshold) {
			output.foregroundMean += color * histogram.at<float>(color);
			sumForeground += histogram.at<float>(color);
		} else {
			output.backgroundMean += color * histogram.at<float>(color);
			sumBackground += histogram.at<float>(color);
		}
	}
	output.foregroundMean /= sumForeground;
	output.backgroundMean /= sumBackground;
}

OtsuThresholdInfo populateVariance(Mat& image, Mat& histogram, float threshold, float meanBackground, float meanForeground, OtsuThresholdInfo& output) {

	long sumForeground = 0;
	long sumBackground = 0;
	for (int color=0; color<255; color++) {
		if (color > threshold) {
			output.foregroundVariance += pow(meanForeground - color, 2) * histogram.at<float>(color);
			sumForeground += histogram.at<float>(color);
		} else {
			output.backgroundVariance += pow(meanBackground - color, 2) * histogram.at<float>(color);
			sumBackground += histogram.at<float>(color);
		}
	}
	output.foregroundVariance /= sumForeground;
	output.backgroundVariance /= sumBackground;
}

float populateClassVariance(OtsuThresholdInfo& oti) {
	oti.classVariance = oti.backgroundWeight * oti.backgroundVariance + oti.foregroundWeight * oti.foregroundVariance;
	return oti.classVariance;
}

int computeOtsuThreshold(Mat& image) {
	Mat histogram;
	OtsuThresholdInfo oti;

	computeHistogram(image, histogram);
	int minThreshold = -1;
	float minClassVariance = ULONG_MAX;

	for (int threshold=1; threshold<255; threshold++) {
		oti = OtsuThresholdInfo{0, 0, 0, 0, 0, 0, 0};
		populateWeights(image, threshold-1, oti);
		if (oti.backgroundWeight == 0 || oti.foregroundWeight == 0) {
			//if the weights are non existent we ignore the threshold
			continue;
		}
		populateMeans(image, histogram, threshold-1, oti);
		populateVariance(image, histogram, threshold-1, oti.backgroundMean, oti.foregroundMean, oti);
		populateClassVariance(oti);
		if (oti.classVariance < minClassVariance) {
			//cout << "old class variance was " << minClassVariance <<", now it is " << oti << endl;
			minThreshold = threshold;
			minClassVariance = oti.classVariance;
		}
	}
	cout << "min class variance is " << minClassVariance << endl;
	cout << "threshold is " << minThreshold << endl;
	return minThreshold;
}

/**
 * Segmentation algorithm
 *
 * \li Otsu algorithm: http://www.labbookpages.co.uk/software/imgProc/otsuThreshold.html
 *
 * Try to apply filtering on an image
 *
 *
 */
int main(int argc, char** argv )
{

	if ( argc != 2 )
	{
		printf("usage: DisplayImage.out <Image_Path>\n");
		return -1;
	}

	Mat image;
	image = imread( argv[1], IMREAD_GRAYSCALE);

	if (!image.data)
	{
		printf("No image data \n");
		return -1;
	}

	int threshold = computeOtsuThreshold(image);
	Mat binarized(image.rows, image.cols, CV_8UC1);
	applyOtsuThreshold(image, binarized, threshold);

	/// Display
	namedWindow("Otsu", CV_WINDOW_AUTOSIZE );
	imshow("Otsu", binarized );

	waitKey(0);

	return 0;
}
