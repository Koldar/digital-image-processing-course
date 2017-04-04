#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <limits.h>
#include <iostream>
#include <queue>
#include <stdexcept>

using namespace cv;
using namespace std;

//// OTSU ALGORITHM
//
//struct OtsuThresholdInfo {
//	float backgroundWeight;
//	float backgroundMean;
//	float backgroundVariance;
//	float foregroundWeight;
//	float foregroundMean;
//	float foregroundVariance;
//	float classVariance;
//};
//
//ostream& operator<<(ostream& os, const OtsuThresholdInfo& dt)
//{
//	os << "w=" << dt.backgroundWeight << " m=" << dt.backgroundMean << " v=" << dt.backgroundVariance
//			<< " "
//			<< "w=" << dt.foregroundWeight << " m=" << dt.foregroundMean << " v=" << dt.foregroundVariance;
//	return os;
//}
//
//
//static void computeHistogram(Mat& image, Mat& output) {
//	//COMPUTE HISTOGRAM
//	/// Establish the number of bins
//	int histSize = 256;
//	/// Set the ranges ( for B,G,R) )
//	float range[] = { 0, 256 } ;
//	const float* histRange = { range };
//
//	bool uniform = true; bool accumulate = false;
//	/// Compute the histograms:
//	calcHist( &image, 1, 0, Mat(), output, 1, &histSize, &histRange, uniform, accumulate );
//}
//
//void applyOtsuThreshold(Mat& image, Mat& output, float threshold) {
//	//put everything to 0
//	output.setTo(0);
//	for (int y=0; y<image.rows; y++) {
//		for (int x=0; x<image.cols; x++) {
//			output.at<uchar>(y, x) = (image.at<uchar>(y, x) >= threshold) ? 0 : 255;
//		}
//	}
//}
//
//static void populateWeights(Mat& image, int threshold, OtsuThresholdInfo& output) {
//	double pixelNumber = image.rows * image.cols;
//
//	for (int y=0; y<image.rows; y++) {
//		for (int x=0; x<image.cols; x++) {
//			if (image.at<uchar>(y, x) > threshold) {
//				output.foregroundWeight++;
//			} else {
//				output.backgroundWeight++;
//			}
//		}
//	}
//
//	output.foregroundWeight /= pixelNumber;
//	output.backgroundWeight /= pixelNumber;
//}
//
//static void populateMeans(Mat& image, Mat& histogram, int threshold, OtsuThresholdInfo& output) {
//
//	long sumForeground = 0;
//	long sumBackground = 0;
//	for (int color=0; color<255; color++) {
//		if (color > threshold) {
//			output.foregroundMean += color * histogram.at<float>(color);
//			sumForeground += histogram.at<float>(color);
//		} else {
//			output.backgroundMean += color * histogram.at<float>(color);
//			sumBackground += histogram.at<float>(color);
//		}
//	}
//	output.foregroundMean /= sumForeground;
//	output.backgroundMean /= sumBackground;
//}
//
//static void populateVariance(Mat& image, Mat& histogram, float threshold, float meanBackground, float meanForeground, OtsuThresholdInfo& output) {
//
//	long sumForeground = 0;
//	long sumBackground = 0;
//	for (int color=0; color<255; color++) {
//		if (color > threshold) {
//			output.foregroundVariance += pow(meanForeground - color, 2) * histogram.at<float>(color);
//			sumForeground += histogram.at<float>(color);
//		} else {
//			output.backgroundVariance += pow(meanBackground - color, 2) * histogram.at<float>(color);
//			sumBackground += histogram.at<float>(color);
//		}
//	}
//	output.foregroundVariance /= sumForeground;
//	output.backgroundVariance /= sumBackground;
//}
//
//static float populateClassVariance(OtsuThresholdInfo& oti) {
//	oti.classVariance = oti.backgroundWeight * oti.backgroundVariance + oti.foregroundWeight * oti.foregroundVariance;
//	return oti.classVariance;
//}
//
//int computeOtsuThreshold(Mat& image) {
//	Mat histogram;
//	OtsuThresholdInfo oti;
//
//	computeHistogram(image, histogram);
//	int minThreshold = -1;
//	float minClassVariance = ULONG_MAX;
//
//	for (int threshold=1; threshold<255; threshold++) {
//		oti = OtsuThresholdInfo{0, 0, 0, 0, 0, 0, 0};
//		populateWeights(image, threshold-1, oti);
//		if (oti.backgroundWeight == 0 || oti.foregroundWeight == 0) {
//			//if the weights are non existent we ignore the threshold
//			continue;
//		}
//		populateMeans(image, histogram, threshold-1, oti);
//		populateVariance(image, histogram, threshold-1, oti.backgroundMean, oti.foregroundMean, oti);
//		populateClassVariance(oti);
//		if (oti.classVariance < minClassVariance) {
//			//cout << "old class variance was " << minClassVariance <<", now it is " << oti << endl;
//			minThreshold = threshold;
//			minClassVariance = oti.classVariance;
//		}
//	}
//	cout << "min class variance is " << minClassVariance << endl;
//	cout << "threshold is " << minThreshold << endl;
//	return minThreshold;
//}
//
////REGION GROWING
//
//typedef enum PickPixelPolicy {
//	SEQUENTIAL,
//	RANDOM
//} PickPixelPolicy;
//
//float ENHANCECOEFFICIENT = 256/50;
//int MAX_REGION_NUMBER = 70;
//int REGION_THRESHOLD = 50;
//PickPixelPolicy PICK_PIXEL_POLICY = RANDOM;
//
//
//class MyRegion {
//public:
//	int id;
//	float mean;
//	int size;
//	static Mat* pixelMapping;
//
//	MyRegion();
//	MyRegion(int id);
//
//	void add(uchar pixel);
//
//	/**
//	 * Check of a point belongs to a region
//	 * @param[in] p the point to check
//	 */
//	static bool isPixelInsideARegion(Point2i& p);
//};
//
//Mat* MyRegion::pixelMapping = NULL;
//
//MyRegion::MyRegion(int id) : id(id), mean(0), size(0) {
//}
//
//MyRegion::MyRegion() : id(0), mean(0), size(0){
//}
//
//bool MyRegion::isPixelInsideARegion(Point2i& p) {
//	//cout << __func__ << " " << (int)MyRegion::pixelMapping->at<uchar>(p) << endl;
//	return MyRegion::pixelMapping->at<uchar>(p) > 0;
//}
//
//void MyRegion::add(uchar pixel) {
//	this->mean = this->mean + (pixel - this->mean)/(this->size + 1);
//	this->size += 1;
//}
//
//void computeColorsFromRegions(Mat& image, vector<MyRegion>& regions);
//
//static Point2i pickPixelWithoutRegion(PickPixelPolicy ppp, vector<MyRegion>& regions) {
//
//	switch (ppp) {
//	case SEQUENTIAL:
//		for (int y=0; y<MyRegion::pixelMapping->rows; y++) {
//			for (int x=0; x<MyRegion::pixelMapping->cols; x++) {
//				//cout << "trying to pick up "<< x << ", " << y << " of region " << (int)(regions->at<uchar>(y, x)) << endl;
//				if (((int)MyRegion::pixelMapping->at<uchar>(y, x)) == 0) {
//					return Point2i{x, y};
//				}
//			}
//		}
//		throw logic_error("no pixel without region");
//	case RANDOM:
//		int limit = 0;
//		while(limit < 1e5) {
//			int y = rand() % MyRegion::pixelMapping->rows;
//			int x = rand() % MyRegion::pixelMapping->cols;
//			if (((int)MyRegion::pixelMapping->at<uchar>(y, x)) == 0) {
//				return Point2i{x, y};
//			}
//			limit++;
//		}
//		throw logic_error("can't find a random pixel bcause we have supassed limit!");
//	}
//
//
//
//}
//
//bool shouldPointBeAddedInQueue(Point2i p) {
//	return (p.x >= 0) &&
//			(p.y >= 0) &&
//			(p.x < MyRegion::pixelMapping->cols) &&
//			(p.y < MyRegion::pixelMapping->rows) &&
//			(MyRegion::isPixelInsideARegion(p) == false);
//}
//
//vector<Point2i> getNonVisitedNeighbours(Point2i p) {
//	vector<Point2i> retVal;
//
//	Point2i neighbour;
//	for (int y=-1; y<=+1; y++) {
//		for (int x=-1; x<=+1; x++) {
//			if (x==0 && y==0) {
//				continue;
//			}
//			neighbour = Point{p.x + x, p.y + y};
//			if (shouldPointBeAddedInQueue(neighbour)) {
//				//cout << " adding point "<< neighbour << " into the neighbouhood!" << endl;
//				retVal.push_back(neighbour);
//			}
//		}
//	}
//	return retVal;
//}
//
//bool shouldPixelBeInRegion(Point2i p, MyRegion region, Mat& image) {
//	//cout << __func__ << " " << (int)image.at<uchar>(p) << " VS " << region.mean << " = " << (abs(image.at<uchar>(p) - region.mean) < 50) << endl;
//	return abs(image.at<uchar>(p) - region.mean) < REGION_THRESHOLD;
//}
//
//void growRegion(deque<Point2i>* openPixels, MyRegion& currentRegion, Mat& image) {
//	Point2i p;
//	vector<Point2i> neighbourhood;
//
//	while (!openPixels->empty()) {
//		p = openPixels->front();
//		//cout << __func__ << " queue size " << openPixels->size() << " p: " << p << endl;
//		openPixels->pop_front();
//
//		if ((currentRegion.size > 1) && (MyRegion::isPixelInsideARegion(p) == true)) {
//			continue;
//		}
//
//		if (shouldPixelBeInRegion(p, currentRegion, image)) {
//			MyRegion::pixelMapping->at<uchar>(p) = currentRegion.id;
//
//			//update region data
//			//online alkgorithm to compute average has been inspired from https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Online_algorithm
//			currentRegion.add(image.at<uchar>(p));
//
//			neighbourhood = getNonVisitedNeighbours(p);
//			//cout << __func__ << " neighbourhood "<< neighbourhood << endl;
//			for (std::vector<Point2i>::iterator it=neighbourhood.begin(); it!=neighbourhood.end(); ++it) {
//				//cout << "before adding neighourhood " << openPixels->size() << endl;
//				openPixels->push_back(*it);
//				//cout << "after adding neighourhood " << openPixels->size() << endl;
//			}
//		}
//	}
//}
//
//Mat* computeSegmentationViaRegionGrowing(Mat& image) {
//	deque<Point2i>* openPixels = new deque<Point2i>;
//	Point2i p;
//	uchar currentRegionValue = 0;
//	vector<Point2i> neighbourhood;
//	vector<MyRegion> regions;
//	//the new regione created (and we need to build it)
//	MyRegion rgn;
//	MyRegion::pixelMapping = new Mat{image.rows, image.cols, CV_8UC1};
//
//	MyRegion::pixelMapping->setTo(0);
//
//	while (true) {
//		//pick a new pixel
//		try {
//			if (currentRegionValue == MAX_REGION_NUMBER) {
//				throw logic_error("we used all the available regions!");
//			}
//			p = pickPixelWithoutRegion(PICK_PIXEL_POLICY, regions);
//			//cout << __func__ << " picking up "<< p << " of region " << (int)(MyRegion::pixelMapping->at<uchar>(p)) << endl;
//		} catch (logic_error e) {
//			cout << "out!" << endl;
//			//no pixel can be fetched
//			goto exit;
//		}
//		//the new pixel is a new region
//		currentRegionValue += 1;
//		rgn = MyRegion{currentRegionValue};
//
//		rgn.add(image.at<uchar>(p));
//		MyRegion::pixelMapping->at<uchar>(p) = currentRegionValue;
//
//
//		//cout << __func__ << "point "<< p << " is now of region " << (int)(MyRegion::pixelMapping->at<uchar>(p)) << endl;
//		openPixels->push_back(p);
//		growRegion(openPixels, rgn, image);
//		regions.push_back(rgn);
//	}
//
//	exit:
//	for (vector<MyRegion>::iterator it=regions.begin(); it!=regions.end(); ++it) {
//		cout << "region #" << it->id << " has " << it->size << " elements with an average of " << it->mean << endl;
//	}
//	cout << "regions are " << (int)(currentRegionValue) << endl;
//	int unRegionedPixel = 0;
//	for (int y=0; y<MyRegion::pixelMapping->rows; y++) {
//		for (int x=0; x<MyRegion::pixelMapping->rows; x++) {
//			p = Point2i{x,y};
//			if (MyRegion::isPixelInsideARegion(p) == false) {
//				unRegionedPixel++;
//			}
//		}
//	}
//	cout << "unregioned pixels are " << unRegionedPixel << "(percentage = " << (100*unRegionedPixel)/(1.0 * image.rows * image.cols) <<endl;
//
//
//	delete openPixels;
//	computeColorsFromRegions(*MyRegion::pixelMapping, regions);
//	return MyRegion::pixelMapping;
//}
//
//void computeColorsFromRegions(Mat& image, vector<MyRegion>& regions) {
//	for (int y=0; y<image.rows; y++) {
//			for (int x=0; x<image.cols; x++) {
//				for (vector<MyRegion>::iterator it=regions.begin(); it!=regions.end(); ++it) {
//					if (it->id == image.at<uchar>(y,x)) {
//						//region we were looking for
//						image.at<uchar>(y,x) = (uchar)it->mean;
//					}
//
//				}
//
//			}
//		}
//}
//
//void enhanceColors(Mat& image, float enhanceCoefficient) {
//	for (int y=0; y<image.rows; y++) {
//		for (int x=0; x<image.cols; x++) {
//			image.at<uchar>(y,x) = (uchar)(image.at<uchar>(y,x) * enhanceCoefficient);
//		}
//	}
//}

#define CHILDREN_NUMBER 4

class QuadNode {
public:
	QuadNode* children[CHILDREN_NUMBER];
	Rect2i region;

public:
	QuadNode(Rect2i& region);
	/**
	 * Explode the QuadNode into 4 children.
	 * The children are added automatically inside the QuadNode object.
	 * No operation is performed id the QuadNode has already have its children
	 */
	void explode();

	bool hasChildren();
	bool isHomogenous(Mat& image);
	bool isEmpty();


	virtual ~QuadNode();
};

QuadNode::QuadNode(Rect2i& region) {
	this->region = region;
	for (int i=0; i<CHILDREN_NUMBER; i++) {
		this->children[i] = NULL;
	}
}

bool QuadNode::hasChildren() {
	return this->children[0] != NULL;
}

void QuadNode::explode() {
	if (this->hasChildren()) {
		return;
	}

	Point2i center;

	center.x = this->region.x + region.width/2;
	center.y = this->region.y + region.height/2;

	Rect2i r;

	//TOP LEFT
	r.x = this->region.x;
	r.y = this->region.y;
	r.width = abs(center.x - r.x);
	r.height = abs(center.y - r.y);
	QuadNode* top_left = new QuadNode{r};

	//TOP RIGHT
	r.x = this->region.x + center.x;
	r.y = this->region.y;
	r.width = abs(this->region.width - top_left->region.width);
	r.height = top_left->region.height;
	QuadNode* top_right = new QuadNode{r};

	//BUTTOM LEFT
	r.x = this->region.x;
	r.y = this->region.y + center.y;
	r.width = top_left->region.width;
	r.height = abs(this->region.height - top_left->region.height);
	QuadNode* bottom_left = new QuadNode{r};

	//BUTTOM RIGHT
	r.x = this->region.x + center.x;
	r.y = this->region.y + center.y;
	r.width = top_right->region.width;
	r.height = bottom_left->region.height;
	QuadNode* bottom_right = new QuadNode{r};

	cout << "root" << this->region << endl;
	cout << "top left " << top_left->region << endl;
	cout << "top right " << top_right->region << endl;
	cout << "bottom left " << bottom_left->region << endl;
	cout << "bottom right " << bottom_right->region << endl;

	this->children[0] = top_left;
	this->children[1] = top_right;
	this->children[2] = bottom_left;
	this->children[3] = bottom_right;
}

QuadNode::~QuadNode() {
	for (int i=0; i<CHILDREN_NUMBER; i++) {
		if (this->children[i] != NULL) {
			delete this->children[i];
		}
	}
	//???
}

bool QuadNode::isEmpty() {
	return (this->region.width == 0) ||
			(this->region.height == 0);
}

bool QuadNode::isHomogenous(Mat& image) {
	uchar pixel;
	bool first;

	if (this->isEmpty()) {
		return true;
	}

	first = true;
	for (int y=0; y<this->region.height; y++) {
		for (int x=0; x<this->region.width; x++) {
			if (first) {
				pixel = image.at<uchar>(y, x);
				first = !first;
			} else {
				if (pixel != image.at<uchar>(y, x)) {
					return false;
				}
			}

		}
	}
	return true;
}

void computeHomogeneousTree(Mat& image, QuadNode& node) {

	if (node.isHomogenous(image)) {
		cout << " homogeneous " << node.region << endl;
		return;
	}

	node.explode();
	for (int i=0; i<CHILDREN_NUMBER; i++) {
		computeHomogeneousTree(image, *(node.children[i]));
	}
}

Mat* computeSegmentationViaQuadTrees(Mat& image) {
	Rect2i wholeImage = Rect2i{0, 0, image.cols, image.rows};
	QuadNode* tree = new QuadNode{wholeImage};

	computeHomogeneousTree(image, *tree);
	cout << "DONE" << endl;
}

// MAIN

/**
 * Segmentation algorithm
 *
 * \li Quadtrees;
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
	//Mat* segmentedImage = computeSegmentationViaQuadTrees(image);
	computeSegmentationViaQuadTrees(image);
//
//	Mat* regions = computeSegmentationViaRegionGrowing(image);
//	enhanceColors(*regions, ENHANCECOEFFICIENT);

	//namedWindow("Regions", CV_WINDOW_AUTOSIZE );
	//imshow("Regions", *segmentedImage);
	//waitKey(0);
	//delete segmentedImage;

	return 0;
}
