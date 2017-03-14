#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <limits.h>

using namespace cv;

typedef enum {
	///the pixel is always 0
	BP_ZERO,
	///the pixel is the same value of the nearest valid pixel
	BP_PROPAGATION,
	///the pixel is the same value as the image were to be repeated outside the border
	BP_MIRRORING
} BorderPolicy;

Vec3b handleBorderPolicy(const Mat& image, int outY, int outX, BorderPolicy bp);
Mat* applyConvolution(const Mat& input, const Mat& kernel, BorderPolicy borderPolicy);

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
 * @param[in] borderPolicy how you want to deal with the indexes out of the images
 */
Mat* applyConvolution(const Mat& input, const Mat& kernel, BorderPolicy borderPolicy) {
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
						inputPixel = handleBorderPolicy(input, outY, outX, borderPolicy);
					} else if ((outX < 0) || (outX >= input.cols)) {
						inputPixel = handleBorderPolicy(input, outY, outX, borderPolicy);
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
 * Fetch the pixel value we should get when the algorithm goes out of the input image
 *
 * @param[in] image the image where we went outside the border
 * @param[in] outY the row pixel we want to fetch (out of border)
 * @param[in] outX the column pixel we want to fetch (out of border)
 * @param[in] bp the policy we want to exploit to fetch the value of the out-of-border pixel. See ::BorderPolicy
 */
Vec3b handleBorderPolicy(const Mat& image, int outY, int outX, BorderPolicy bp) {
	switch (bp) {
	case BP_ZERO: {
		return Vec3b{0, 0, 0};
	}
	case BP_PROPAGATION: {
		if (outY < 0) {
			outY = 0;
		}
		if (outY >= image.rows) {
			outY = image.rows - 1;
		}

		if (outX < 0) {
			outX = 0;
		}
		if (outX >= image.cols) {
			outX = image.cols - 1;
		}

		return image.at<Vec3b>(outY, outX);
	}
	case BP_MIRRORING: {
		outY = (outY % image.rows);
		outX = (outX % image.cols);
		return image.at<Vec3b>(outY, outX);
	}
	default: {
		::std::cout << "border policy used is " << bp << ::std::endl;
		error(1, "Invalid Border policy!", __func__, __FILE__, __LINE__);
	}
	}
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
	Mat* filteredImage = applyConvolution(image, kernel, BP_ZERO);

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
