#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <limits.h>
#include <iostream>
#include <queue>
#include <stdexcept>

using namespace cv;
using namespace std;

//REGION GROWING

typedef enum PickPixelPolicy {
	SEQUENTIAL,
	RANDOM
} PickPixelPolicy;

float ENHANCECOEFFICIENT = 256/50;
int MAX_REGION_NUMBER = 70;
int REGION_THRESHOLD = 50;
PickPixelPolicy PICK_PIXEL_POLICY = RANDOM;


class MyRegion {
public:
	int id;
	float mean;
	int size;
	static Mat* pixelMapping;

	MyRegion();
	MyRegion(int id);

	void add(uchar pixel);

	/**
	 * Check of a point belongs to a region
	 * @param[in] p the point to check
	 */
	static bool isPixelInsideARegion(Point2i& p);
};

Mat* MyRegion::pixelMapping = NULL;

MyRegion::MyRegion(int id) : id(id), mean(0), size(0) {
}

MyRegion::MyRegion() : id(0), mean(0), size(0){
}

bool MyRegion::isPixelInsideARegion(Point2i& p) {
	//cout << __func__ << " " << (int)MyRegion::pixelMapping->at<uchar>(p) << endl;
	return MyRegion::pixelMapping->at<uchar>(p) > 0;
}

void MyRegion::add(uchar pixel) {
	this->mean = this->mean + (pixel - this->mean)/(this->size + 1);
	this->size += 1;
}

void computeColorsFromRegions(Mat& image, vector<MyRegion>& regions);

static Point2i pickPixelWithoutRegion(PickPixelPolicy ppp, vector<MyRegion>& regions) {

	switch (ppp) {
	case SEQUENTIAL:
		for (int y=0; y<MyRegion::pixelMapping->rows; y++) {
			for (int x=0; x<MyRegion::pixelMapping->cols; x++) {
				//cout << "trying to pick up "<< x << ", " << y << " of region " << (int)(regions->at<uchar>(y, x)) << endl;
				if (((int)MyRegion::pixelMapping->at<uchar>(y, x)) == 0) {
					return Point2i{x, y};
				}
			}
		}
		throw logic_error("no pixel without region");
	case RANDOM:
		int limit = 0;
		while(limit < 1e5) {
			int y = rand() % MyRegion::pixelMapping->rows;
			int x = rand() % MyRegion::pixelMapping->cols;
			if (((int)MyRegion::pixelMapping->at<uchar>(y, x)) == 0) {
				return Point2i{x, y};
			}
			limit++;
		}
		throw logic_error("can't find a random pixel bcause we have supassed limit!");
	}



}

bool shouldPointBeAddedInQueue(Point2i p) {
	return (p.x >= 0) &&
			(p.y >= 0) &&
			(p.x < MyRegion::pixelMapping->cols) &&
			(p.y < MyRegion::pixelMapping->rows) &&
			(MyRegion::isPixelInsideARegion(p) == false);
}

vector<Point2i> getNonVisitedNeighbours(Point2i p) {
	vector<Point2i> retVal;

	Point2i neighbour;
	for (int y=-1; y<=+1; y++) {
		for (int x=-1; x<=+1; x++) {
			if (x==0 && y==0) {
				continue;
			}
			neighbour = Point{p.x + x, p.y + y};
			if (shouldPointBeAddedInQueue(neighbour)) {
				//cout << " adding point "<< neighbour << " into the neighbouhood!" << endl;
				retVal.push_back(neighbour);
			}
		}
	}
	return retVal;
}

bool shouldPixelBeInRegion(Point2i p, MyRegion region, Mat& image) {
	//cout << __func__ << " " << (int)image.at<uchar>(p) << " VS " << region.mean << " = " << (abs(image.at<uchar>(p) - region.mean) < 50) << endl;
	return abs(image.at<uchar>(p) - region.mean) < REGION_THRESHOLD;
}

void growRegion(deque<Point2i>* openPixels, MyRegion& currentRegion, Mat& image) {
	Point2i p;
	vector<Point2i> neighbourhood;

	while (!openPixels->empty()) {
		p = openPixels->front();
		//cout << __func__ << " queue size " << openPixels->size() << " p: " << p << endl;
		openPixels->pop_front();

		if ((currentRegion.size > 1) && (MyRegion::isPixelInsideARegion(p) == true)) {
			continue;
		}

		if (shouldPixelBeInRegion(p, currentRegion, image)) {
			MyRegion::pixelMapping->at<uchar>(p) = currentRegion.id;

			//update region data
			//online alkgorithm to compute average has been inspired from https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Online_algorithm
			currentRegion.add(image.at<uchar>(p));

			neighbourhood = getNonVisitedNeighbours(p);
			//cout << __func__ << " neighbourhood "<< neighbourhood << endl;
			for (std::vector<Point2i>::iterator it=neighbourhood.begin(); it!=neighbourhood.end(); ++it) {
				//cout << "before adding neighourhood " << openPixels->size() << endl;
				openPixels->push_back(*it);
				//cout << "after adding neighourhood " << openPixels->size() << endl;
			}
		}
	}
}

Mat* computeSegmentationViaRegionGrowing(Mat& image) {
	deque<Point2i>* openPixels = new deque<Point2i>;
	Point2i p;
	uchar currentRegionValue = 0;
	vector<Point2i> neighbourhood;
	vector<MyRegion> regions;
	//the new regione created (and we need to build it)
	MyRegion rgn;
	MyRegion::pixelMapping = new Mat{image.rows, image.cols, CV_8UC1};

	MyRegion::pixelMapping->setTo(0);

	while (true) {
		//pick a new pixel
		try {
			if (currentRegionValue == MAX_REGION_NUMBER) {
				throw logic_error("we used all the available regions!");
			}
			p = pickPixelWithoutRegion(PICK_PIXEL_POLICY, regions);
			//cout << __func__ << " picking up "<< p << " of region " << (int)(MyRegion::pixelMapping->at<uchar>(p)) << endl;
		} catch (logic_error e) {
			cout << "out!" << endl;
			//no pixel can be fetched
			goto exit;
		}
		//the new pixel is a new region
		currentRegionValue += 1;
		rgn = MyRegion{currentRegionValue};

		rgn.add(image.at<uchar>(p));
		MyRegion::pixelMapping->at<uchar>(p) = currentRegionValue;


		//cout << __func__ << "point "<< p << " is now of region " << (int)(MyRegion::pixelMapping->at<uchar>(p)) << endl;
		openPixels->push_back(p);
		growRegion(openPixels, rgn, image);
		regions.push_back(rgn);
	}

	exit:
	for (vector<MyRegion>::iterator it=regions.begin(); it!=regions.end(); ++it) {
		cout << "region #" << it->id << " has " << it->size << " elements with an average of " << it->mean << endl;
	}
	cout << "regions are " << (int)(currentRegionValue) << endl;
	int unRegionedPixel = 0;
	for (int y=0; y<MyRegion::pixelMapping->rows; y++) {
		for (int x=0; x<MyRegion::pixelMapping->rows; x++) {
			p = Point2i{x,y};
			if (MyRegion::isPixelInsideARegion(p) == false) {
				unRegionedPixel++;
			}
		}
	}
	cout << "unregioned pixels are " << unRegionedPixel << "(percentage = " << (100*unRegionedPixel)/(1.0 * image.rows * image.cols) <<endl;


	delete openPixels;
	computeColorsFromRegions(*MyRegion::pixelMapping, regions);
	return MyRegion::pixelMapping;
}

void computeColorsFromRegions(Mat& image, vector<MyRegion>& regions) {
	for (int y=0; y<image.rows; y++) {
			for (int x=0; x<image.cols; x++) {
				for (vector<MyRegion>::iterator it=regions.begin(); it!=regions.end(); ++it) {
					if (it->id == image.at<uchar>(y,x)) {
						//region we were looking for
						image.at<uchar>(y,x) = (uchar)it->mean;
					}

				}

			}
		}
}

void enhanceColors(Mat& image, float enhanceCoefficient) {
	for (int y=0; y<image.rows; y++) {
		for (int x=0; x<image.cols; x++) {
			image.at<uchar>(y,x) = (uchar)(image.at<uchar>(y,x) * enhanceCoefficient);
		}
	}
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

	srand(time(NULL));
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
	enhanceColors(*regions, ENHANCECOEFFICIENT);

	namedWindow("Regions", CV_WINDOW_AUTOSIZE );
	imshow("Regions", *regions);
	waitKey(0);
	delete regions;

	return 0;
}
