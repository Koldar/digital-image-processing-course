#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <limits.h>

using namespace cv;

/**
 * Generate the density of probability distribution.
 *
 * Such function is defined as:
 *
 * <b>The function h[k] is an estimation of the probability
 * distribution (density) of the pixel intensity<br>
 * Note: P h[k] = 1
 * </b>
 *
 * @param[in] input input image
 * @return a vector. each cell represents the probability (indexed by color);
 */
::std::vector<double> getDensityProbabilityDistribution(const Mat& input) {
	::std::vector<double> h = ::std::vector<double>{256, 0};
	ulong pixelsNumber;

	pixelsNumber = (ulong)input.rows * (ulong)input.cols;

	for (int y=0; y<input.rows; y++) {
		for (int x=0; x<input.cols; x++) {
			h[input.at<uchar>(y, x)] += 1;
		}
	}

	for (int i=0; i<h.size(); i++) {
		h[i] = h[i]/pixelsNumber;
	}

	//teh result should be 1
	double sum = 0;
	for (int i=0; i<h.size(); i++) {
		sum += h[i];
	}
	::std::cout << "The sum of all the values of the pdf should be 1. It is = " << sum << ::std::endl;

	return h;
}

/**
 * Generate the cumulative density of probability given the pdf function
 *
 * By definition, the cumulative density probability disitribution is:
 *
 * \f[
 * 		H[k] = \sum_{i=0}^{k}{h[i]}
 * \f]
 * In order to generate the pdf function use ::getDensityProbabilityDistribution function
 *
 * @param[in] input the image to compute the cumulative density probability distribution
 * @return the cumulative density probability distribution
 */
::std::vector<double> getCumulativeDensityProbabilityDistribution(const Mat& input) {
	::std::vector<double> H = ::std::vector<double>{256, 0};
	ulong pixelsNumber;

	pixelsNumber = (ulong)input.rows * (ulong)input.cols;

	for (int y=0; y<input.rows; y++) {
		for (int x=0; x<input.cols; x++) {
			H[input.at<uchar>(y, x)] += 1;
		}
	}

	for (int i=0; i<H.size(); i++) {
		H[i] = H[i]/pixelsNumber;
	}

	double previousSum = 0;
	for (int i=0; i<H.size(); i++) {
		H[i] = previousSum + H[i];
		previousSum = H[i];
	}

	::std::cout << "the last element of the cumulative function has to be 1. It is " << H[H.size()-1] << ::std::endl;

	return H;
}

/**
 * Testing function used to generate plot file of a function
 *
 * \post
 * 	\li this will create in the CWD the file called \c fileToCreate
 *
 * @param[in] function the function to plot
 * @param[in] fileToCreate name of the file to create
 */
void createHistogramOf(const ::std::vector<double> function, const char* fileToCreate) {
	FILE* f = fopen(fileToCreate, "w");
	if (f != NULL) {
		for (int i=0; i<function.size(); i++) {
			fprintf(f, "%d %f\n", i, function[i]);
		}
		fclose(f);
	}
}

/**
 * Create an image representing the histogram of \c input
 *
 * @param[in] input the image in input
 * @parma[in] fileToCreate the name of the file containing histogram data
 * @return a matrix containing the histogram. Be sure to manually free it!
 */
void createHistogramOf(const Mat& input, const char* fileToCreate) {
	::std::vector<double> h = getDensityProbabilityDistribution(input);
	createHistogramOf(h, fileToCreate);
}

/**
 * Generate an equalized image given \c input
 *
 * Equalization is defined as:
 * \$[
 * 	I_{eq}(i,j) = T(I(i,j)) = H(I(i,j))
 * \f]
 *
 * Where \c H is the cumulative density probability distribution.
 *
 * @param[in] input the image to compute the equalization on
 * @return the equalized image. Be sure to \b free the return value after using it!
 */
Mat* generateEqualizedImage(const Mat& input) {
	::std::vector<double> h = getCumulativeDensityProbabilityDistribution(input);
	Mat* retVal = new Mat{input.rows, input.cols, CV_8UC1};

	for (int y=0; y<input.rows; y++) {
		for (int x=0; x<input.cols; x++) {
			retVal->at<uchar>(y,x) = 255. * h[input.at<uchar>(y, x)];
		}
	}

	return retVal;
}


/**
 * It generate the histogram of the given image
 *
 * At the end of the program output, you will be prompted to open gnuplot.
 * On a terminal, do:
 *
 * <code>
 * 	$ gnuplot
 * 	$ plot "histogram-before-equalization.dat" using 1:2 with lines
 * </code>
 *
 * This will allow you to show the histogram of the image grayscale.
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

	createHistogramOf(getCumulativeDensityProbabilityDistribution(image), "H.dat");
	createHistogramOf(getDensityProbabilityDistribution(image), "h.dat");

	createHistogramOf(image, "histogram-before-equalization.dat");
	Mat* equalizedImage = generateEqualizedImage(image);
	createHistogramOf(*equalizedImage, "histogram-after-equalization.dat");

	::std::cout << "DONE. Please open gnuplot (sudo apt-get install gnuplot) and inside its terminal perform the action" << ::std::endl;
	::std::cout << "   plot \"histogram-before-equalization.dat\" using 1:2 with lines" << ::std::endl;
	::std::cout << "   plot \"histogram-after-equalization.dat\" using 1:2 with lines" << ::std::endl;

	namedWindow("input", WINDOW_AUTOSIZE );
	namedWindow("equalized", WINDOW_AUTOSIZE );
	imshow("input", image);
	imshow("equalized", *equalizedImage);
	waitKey(0);

	destroyAllWindows();

	return 0;
}
