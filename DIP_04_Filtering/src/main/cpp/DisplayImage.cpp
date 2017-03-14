#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <limits.h>

using namespace cv;

/**
 * Apply the convolution between 2 signals, aka:
 *
 * \f[
 * 	z = x * y
 * 	z(t) = \int_{\tau=-\infty}^{\tau=+\infty}{x(t-\tau)y(\tau)d\tau}
 * \f]
 *
 * \attention
 * The function wil create a new ::Mat in the heap. You need to manually remove it from the memory afterwards.
 *
 * @param[in] input1 the first image to convolve
 * @param[in] input2 the second image to convolve
 */
Mat* applyConvolution(const Mat& input, const Mat& kernel) {
	int outputRows =  input.rows + kernel.rows - 1;
	int outputCols = input.cols + kernel.cols - 1;
	int halfKernelRows = kernel.rows/2;
	int halfKernelColumns = kernel.cols/2;

	Mat* retVal = new Mat{outputRows, outputCols, input.type()};
	retVal->setTo(0);

	for (int y=0; y<input.rows; y++) {
		for (int x=0; x<input.cols; x++) {

			for (int ky=0; ky<kernel.rows; ky++) {
				for (int kx=0; kx<kernel.cols; kx++) {
					retVal->at<uchar>(y+ky-halfKernelRows, x+kx-halfKernelColumns) += kernel.at<uchar>(ky, kx) * input.at<uchar>(y-halfKernelRows, x-halfKernelColumns);
				}
			}


		}
	}

	return retVal;
}


/**
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
	//image = imread( argv[1], IMREAD_GRAYSCALE);
	image = imread( argv[1], IMREAD_GRAYSCALE);

	if (!image.data)
	{
		printf("No image data \n");
		return -1;
	}

	float kernelData[][] = {{1./9, 1./9, 1./9}, {1./9, 1./9, 1./9},{1./9, 1./9, 1./9}};
	Mat kernel = Mat{2, 5, CV_32FC1, &kernelData};
	Mat* filteredImage = applyConvolution(image, kernel);

	namedWindow("input", WINDOW_AUTOSIZE );
	namedWindow("filtered", WINDOW_AUTOSIZE );
	imshow("input", image);
	imshow("filtered", *filteredImage);
	waitKey(0);

	destroyAllWindows();

	return 0;
}
