#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

typedef enum {
	RE_CLOCKWISE,
	RE_ANTICLOCKWISE
} RotationEnum;

typedef enum {
	FE_HORIZONTAL,
	FE_VERTICAL
} FlipEnum;

static void rotate90(Mat& image, RotationEnum re);
static void flipImage(Mat& image, FlipEnum fe);
static void crop(Mat& image, Rect2i cropRectangle);

static void crop(Mat& image, Rect2i cropRectangle) {

}

/**
 * Flip an image
 *
 * \post
 * 	\li the \c image changes
 *
 * @param[inout] image the image to change
 * @param[in] fe how you want to flip the image
 */
static void flipImage(Mat& image, FlipEnum fe) {
	Mat tmp = image.clone();
	unsigned int newX;
	unsigned int newY;
	unsigned int channelSize = image.channels();

	for (unsigned int y=0; y<image.rows; y++) {
		for (unsigned int x=0; x<image.cols; x++) {
			switch (fe) {
			case FE_HORIZONTAL: {
				newX = x;
				newY = (int)(image.rows - y);
				break;
			}
			case FE_VERTICAL: {
				newX = (int)(image.cols - x);
				newY = y;
				break;
			}
			}
			image.at<Vec3b>(newY, newX) = tmp.at<Vec3b>(y, x);
		}
	}
}

/**
 * Rotate the image either clockwise or anticlockwise
 *
 * @param[inout] image the image to alter
 * @param[in] re if yu want to rotate the image 90° clockwiseor anticlockwise
 */
static void rotate90(Mat& image, RotationEnum re) {
	Mat tmp = image.clone();
	unsigned int newx;
	unsigned int newy;
	unsigned int channelsSize = image.channels();

	for (unsigned int y=0; y<image.rows; y++) {
		//get the y-th row
		for (unsigned int x=0; x<image.cols; x++) {

			switch (re) {
			case RE_CLOCKWISE: {
				newx = image.rows-y; //xcos(kpi/2) - ysin(kpi/2)
				newy = +x; //xsin(kpi/2) + ycos(kpi/2)
				image.at<Vec3b>(newy, newx) = tmp.at<Vec3b>(y, x);
				break;
			}
			case RE_ANTICLOCKWISE: {
				newx = +y; //xcos(kpi/2) - ysin(kpi/2)
				newy = image.cols-x; //xsin(kpi/2) + ycos(kpi/2)
				image.at<Vec3b>(newy, newx) = tmp.at<Vec3b>(y, x);
				break;
			}
			}

		}
	}
}

/**
 * Exercise to show how to rotate an image
 *
 * OpenCV structures the image into a matrix (Mat).
 * The matrix contains the pixels (y and x). In order to get a single pixel you should do ```image.at<TYPE>(y,x)```.
 *
 * `TYPE` is the type that represents a single pixel. Normally the type allows you to get information about the channels of a pixel
 * (RGB, BRG, RGBA and so on). So, in order to rotate our image, we need to clone the image, and then, for each pixel we rotate them.
 * We need to put as `TYPE` Vect3b because that is the length of a single pixel. Failing to do so will make us jump in piece of memory
 * that are between different pixels (and that's nasty)
 */
int main(int argc, char** argv )
{
	if ( argc != 2 )
	{
		printf("usage: DisplayImage.out <Image_Path>\n");
		return -1;
	}

	Mat image;
	image = imread( argv[1], IMREAD_COLOR);

	if (!image.data)
	{
		printf("No image data \n");
		return -1;
	}

	//rotate90(image, RE_CLOCKWISE);
	//rotate90(image, RE_ANTICLOCKWISE);

	//flipImage(image, FE_HORIZONTAL);
	flipImage(image, FE_VERTICAL);

	namedWindow("Display Image", WINDOW_AUTOSIZE );
	imshow("Display Image", image);
	waitKey(0);

	return 0;
}
