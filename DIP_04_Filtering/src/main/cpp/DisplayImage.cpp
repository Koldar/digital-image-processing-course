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
	int outY;
	int outX;
	Vec3f inputPixel;


	Mat* retVal = new Mat{outputRows, outputCols, input.type()};
	retVal->setTo(Vec3b{0, 0, 0});

	for (int y=0; y<input.rows; y++) {
		for (int x=0; x<input.cols; x++) {

			for (int ky=0; ky<kernel.rows; ky++) {
				for (int kx=0; kx<kernel.cols; kx++) {
					outY = y + ky - halfKernelRows;
					outX = x + kx - halfKernelColumns;

					if ((outY < 0) || (outY >= input.rows)) {
						inputPixel = Vec3b{0, 0, 0};
					} else if ((outX < 0) || (outX >= input.cols)) {
						inputPixel = Vec3b{0, 0, 0};
					} else {
						inputPixel = input.at<Vec3b>(outY, outX);
					}

					//::std::cout << "x,y: " << x << ", " << y <<" outXY: " << outY << ", " << outX << "inputPixel: " << inputPixel << " halves: " << halfKernelRows << ", " << halfKernelColumns << ::std::endl;

					for (int c=0; c<3; c++) {
						retVal->at<Vec3b>(y, x)[c] += (uchar)
													kernel.at<float>(ky, kx) *
													inputPixel[c];
					}
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
	image = imread( argv[1], IMREAD_COLOR);
	//image.convertTo(image, CV_32FC3);

	if (!image.data)
	{
		printf("No image data \n");
		return -1;
	}

	float kernelData[3][3] = {{0./9, 0./9, 0./9}, {0./9, 9./9, 0./9},{0./9, 0./9, 0./9}};
	Mat kernel = Mat{3, 3, CV_32FC1, &kernelData};
	Mat* filteredImage = applyConvolution(image, kernel);

	namedWindow("input", WINDOW_AUTOSIZE );
	namedWindow("filtered", WINDOW_AUTOSIZE );
	imshow("input", image);
	imshow("filtered", *filteredImage);
	waitKey(0);

	destroyAllWindows();

	::std::cout << "filtered images size: " << image.rows << ", " << image.cols << ::std::endl;
	::std::cout << "kernel images size: " << kernel.rows << ", " << kernel.cols << ::std::endl;
	::std::cout << "filtered images size: " << filteredImage->rows << ", " << filteredImage->cols << ::std::endl;

	delete filteredImage;

	return 0;
}
