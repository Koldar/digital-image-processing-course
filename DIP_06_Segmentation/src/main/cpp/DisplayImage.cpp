#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <limits.h>
#include <iostream>
#include <queue>
#include <stdexcept>

using namespace cv;
using namespace std;

// OTSU ALGORITHM

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


static void computeHistogram(Mat& image, Mat& output) {
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

static void populateWeights(Mat& image, int threshold, OtsuThresholdInfo& output) {
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

static void populateMeans(Mat& image, Mat& histogram, int threshold, OtsuThresholdInfo& output) {

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

static void populateVariance(Mat& image, Mat& histogram, float threshold, float meanBackground, float meanForeground, OtsuThresholdInfo& output) {

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

static float populateClassVariance(OtsuThresholdInfo& oti) {
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

//REGION GROWING


class Region {
public:
	int id;
	float mean;
	int size;
	static Mat* pixelMapping;
};

static Point2i pickPixelWithoutRegion(vector<Region>& regions) {
	for (int y=0; y<Region::pixelMapping->rows; y++) {
		for (int x=0; x<Region::pixelMapping->cols; x++) {
			//cout << "trying to pick up "<< x << ", " << y << " of region " << (int)(regions->at<uchar>(y, x)) << endl;
			if (((int)Region::pixelMapping->at<uchar>(y, x)) == 0) {
				return Point2i(x, y);
			}
		}
	}
	throw logic_error("no pixel without region");
}

bool shouldPointBeAddedInQueue(Point2i p, vector<Region>& regions) {
	return (p.x >= 0) &&
			(p.y >= 0) &&
			(p.x < Region::pixelMapping->cols) &&
			(p.y < Region::pixelMapping->rows) &&
			(Region::pixelMapping->at<uchar>(p) == 0);
}

vector<Point2i> getNonVisitedNeighbours(vector<Region>& regions, Point2i p) {
	vector<Point2i> retVal;

	Point2i neighbour;
	for (int y=-1; y<=+1; y++) {
		for (int x=-1; x<=+1; x++) {
			if (x==0 && y==0) {
				continue;
			}
			neighbour = Point{p.x + x, p.y + y};
			if (shouldPointBeAddedInQueue(neighbour, regions)) {
				retVal.push_back(neighbour);
			}
		}
	}
	return retVal;
}

bool isPixelBelogingToRegion(Point2i p, uchar r, Mat& image, vector<Region>& regions) {
	cout << image.at<uchar>(p) << " == " << (int)r << endl;
	Region region;
	for (vector<Region>::iterator it=regions.begin(); it!=regions.end(); ++it) {
		if (it->id == (int)r) {
			region = *it;
		}
	}
	return abs(image.at<uchar>(p) - region.mean) < 50;
}

void growRegion(deque<Point2i>* openPixels, uchar currentRegionValue, Mat& image, vector<Region>& regions) {
	Point2i p;
	vector<Point2i> neighbourhood;

	while (!openPixels->empty()) {
		p = openPixels->front();
		cout << "queue size " << openPixels->size() << " p: " << p << endl;
		openPixels->pop_front();

		if (isPixelBelogingToRegion(p, currentRegionValue, image, regions)) {
			Region::pixelMapping->at<uchar>(p) = currentRegionValue;

			//update region data


			neighbourhood = getNonVisitedNeighbours(regions, p);
			cout << "neighbourhood "<< neighbourhood << endl;
			for (std::vector<Point2i>::iterator it=neighbourhood.begin(); it!=neighbourhood.end(); ++it) {
				openPixels->push_back(*it);
			}
		}
	}
}

Mat* computeSegmentationViaRegionGrowing(Mat& image) {
	deque<Point2i>* openPixels = new deque<Point2i>;
	Point2i p;
	uchar currentRegionValue = 0;
	vector<Point2i> neighbourhood;
	vector<Region> regions;
	Region::pixelMapping = new Mat{image.rows, image.cols, CV_8UC1};

	Region::pixelMapping->setTo(0);

	while (true) {
		//pick a new pixel
		try {
			if (currentRegionValue == 255) {
				throw logic_error("we used all the available regions!");
			}
			p = pickPixelWithoutRegion(regions);
			cout << "picking up "<< p << " of region " << (int)(Region::pixelMapping->at<uchar>(p)) << endl;
		} catch (logic_error e) {
			cout << "out!" << endl;
			//no pixel can be fetched
			goto exit;
		}
		//the new pixel is a new region
		currentRegionValue += 1;
		Region::pixelMapping->at<uchar>(p) = currentRegionValue;
		cout << "point "<< p << " is now of region " << (int)(Region::pixelMapping->at<uchar>(p)) << endl;
		openPixels->push_back(p);
		growRegion(openPixels, currentRegionValue, image, regions);
	}

	exit:
	cout << "regions are " << (int)(currentRegionValue) << endl;
	delete openPixels;
	return regions;
}


// MAIN

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

	//OTSU ALGORITHM
	//	int threshold = computeOtsuThreshold(image);
	//	Mat binarized(image.rows, image.cols, CV_8UC1);
	//	applyOtsuThreshold(image, binarized, threshold);
	/// Display
	//		namedWindow("Otsu", CV_WINDOW_AUTOSIZE );
	//		imshow("Otsu", binarized );
	// waitKey(0);

	//REGION GROWING
	Mat* regions = computeSegmentationViaRegionGrowing(image);
	namedWindow("Regions", CV_WINDOW_AUTOSIZE );
	imshow("Regions", *regions);
	waitKey(0);
	delete regions;

	return 0;
}
