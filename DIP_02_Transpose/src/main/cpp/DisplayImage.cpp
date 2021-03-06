#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <limits.h>

using namespace cv;

typedef enum {
	RE_CLOCKWISE,
	RE_ANTICLOCKWISE
} RotationEnum;

typedef enum {
	FE_HORIZONTAL,
	FE_VERTICAL
} FlipEnum;

static void addSaltAndPepperNoise(Mat& image);
static void addWhiteNoise(Mat& image);
static void addUniformAndGaussianNoise(Mat& image);

static void generateContrastSensitivityFunction(Mat& image);

static void rotate90(Mat& image, RotationEnum re);
static void flipImage(Mat& image, FlipEnum fe);
static void crop(Mat& image, Mat* output, const Rect2i& cropRectangle);
static void negative(Mat& image, Mat* output);
static void setImagePixelsTo(Mat& image, Vec3b color);

/**
 * Generate the constrast sensitivity function
 *
 * <img src="http://ohzawa-lab.bpe.es.osaka-u.ac.jp/ohzawa-lab/izumi/CSF/CSFchart640x480.gif"/>
 *
 * The function is generated with this function:
 *
 * \f[ I(i, j) = 128 · [1 + sin(exp(x)) · (y^3)] \f]
 * \f[ i \in [0; I_{height}] \f]
 * \f[ j \in [0; I_{width}] \f]
 * \f[ x \in [0; 5] \f]
 * \f[ y \in [0; 1] \f]
 *
 *@param[out] the image to use. Set it only width and height images
 */
static void generateContrastSensitivityFunction(Mat& image) {

	for (int y=0; y<image.rows; y++) {
		for (int x=0; x<image.cols; x++) {
			image.at<unsigned char>(y, x) = 128 * (1 + sin(exp(5*((x + 0.)/image.cols))) * (pow((y + 0.)/image.rows, 3)));
		}
	}
}

static void addWhiteNoise(Mat& image) {
	Mat whiteNoise = Mat::zeros(image.rows, image.cols, CV_8UC3);
	/*
	 * The white noise is a signal whose frequency spectrum has all the frequencies equally contributing to the spectrum itself
	 * (constant value in X(f)).
	 *
	 * The image has several spatial frequencies inside  it (depending on the sharpness of the borders).
	 * So if we add a random value (with mean set to 0) per each pixels, we are de facto applying some noise to every
	 * image frequency.
	 */
	randn(whiteNoise, 0, 10);

	image = image + whiteNoise;
}

static void addUniformAndGaussianNoise(Mat& image) {
	Mat uniformAndGaussian = Mat::zeros(image.rows, image.cols, CV_8UC3);
	/*
	 * The gaussian noise is a white noise in frequnecy X(f) = k but is gaussian in probability:
	 * so we need to attack every frequency in the image but the amount of noise depends on mean/variance of the gaussian itself.
	 *
	 * How can we create a gaussian noise. By definition with randu
	 */
	randu(uniformAndGaussian, 0, 100);

	image += uniformAndGaussian;
}

static void addSaltAndPepperNoise(Mat& image) {

	//static method: create a matrix all with zeros
	Mat saltpepper_noise = Mat::zeros(image.rows, image.cols, CV_8U);

	//now we fill saltpepper_noise matrix with values uniformly picked amont [0;255]
	randu(saltpepper_noise,0,255);

	/* ok, now this is interesting. <,>,<= and >= are overloaded. if you do "Mat < num" you will
	 * obtain a new matrix always of pixel type 8U where each pixel is 0xFF if it is less than num, 0x00 otherwise.
	 * See http://stackoverflow.com/a/9783817/1887602
	 *
	 * This allow us to automatically set binarize according to a condition our image! pretty cool!
	 */
	Mat black = saltpepper_noise < 30;
	Mat white = saltpepper_noise > 225;

	/*
	 * SetTo seems pretty cool.
	 * The second argument is a mask (aka a 8U matrix type).
	 * We will set the pixel in "image" whose coordinates in "white" are "0xFF" to the first parameter (255 in this case).
	 */
	image.setTo(255, white);
	image.setTo(0, black);
}

/**
 * Set the all the pixel of the image to a certain color
 *
 * \attention
 * 	\c image will be changed!
 *
 * @param[inout] image the image to alter
 * @param[in] color the color each pixel will have
 */
static void setImagePixelsTo(Mat& image, Vec3b color) {
	for (int y=0; y<image.rows; y++) {
		for (int x=0; x<image.cols; x++) {
			image.at<Vec3b>(y, x) = color;
		}
	}
}

/**
 * compute the negative of an image
 *
 *
 * <img src="https://3.bp.blogspot.com/-VGsbdrM3LTM/UOOSsvUkZoI/AAAAAAAAAek/i8AQnCmc_FY/s1600/imagenegative.png"/>
 *
 * @param[in] image the image to compute the negative on
 * @param[out] output a non initialized ::Mat pointer
 */
static void negative(Mat& image, Mat* output) {
	*output = Mat{image.rows, image.cols,  CV_8UC3};

	for (int y=0; y<image.rows; y++) {
		for (int x=0; x<image.cols; x++) {
			for (int c=0; c<3; c++) {
				output->at<Vec3b>(y, x).val[c] = UCHAR_MAX - image.at<Vec3b>(y,x).val[c];
			}

		}
	}
}

/**
 * Crop an image
 *
 * <img src="http://www.pyimagesearch.com/wp-content/uploads/2015/01/mouse-event-crop-tim.jpg"/>
 *
 * @param[in] image the image to crop on
 * @param[out] a pointer to an empty ::Mat that will contain the cropped image
 * @param[in] cropRectangle a rectangle that will be used to define the area cropped
 */
static void crop(const Mat& image, Mat* output, const Rect2i& cropRectangle) {
	/*
	 * CV_8UC3 indicates how we represents a single channel AND how many channel we will have.
	 * 8U is how we represents a single channel:
	 *  - 8U=8 bit unsigned char
	 *  - 32F = 32 bit of floating-point
	 *
	 * The second sector of the constant represents  the number of channels. It is in the form of CX where X is a number:
	 * - C3 are 3 channels;
	 * - C4 are 4 channels;
	 *
	 * CV_8UC3 means we want an image with 3 channels each with 8 bit unsigned char
	 */
	*output = Mat{cropRectangle.width, cropRectangle.height,  CV_8UC3};

	for (int y=0; y<cropRectangle.height; y++) {
		for (int x=0; x<cropRectangle.width; x++) {
			output->at<Vec3b>(y, x) = image.at<Vec3b>(y + cropRectangle.y, x + cropRectangle.x);
		}
	}

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
			/*
			 * To set or get a pixel of an image, you can use at method.
			 * However, since an image may have several channels and they can be encoded in several ways, we need
			 * a template to let C++ understand what exactly we want to get the pixels.
			 * Very useful indeed
			 */
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
	//flipImage(image, FE_VERTICAL);

	Mat output;
	//crop(image, &output, Rect2i{100,100, 70, 50});
	//negative(image, &output);
	//setImagePixelsTo(image, Vec3b{120,0,0});

	//addWhiteNoise(image);
	addUniformAndGaussianNoise(image);
	//addSaltAndPepperNoise(image);

	//Mat csf = Mat{700, 700, CV_8U};
	//generateContrastSensitivityFunction(csf);

	namedWindow("Display Image", WINDOW_AUTOSIZE );
	imshow("Display Image", image);
	waitKey(0);

	return 0;
}
