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

	double sum = 0;
	for (int i=0; i<h.size(); i++) {
		sum += h[i];
	}

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
 * @param[in] pdf the pdf function to use
 * @return the cumulative density probability distribution
 */
::std::vector<double> getCumulativeDensityProbabilityDistribution(const ::std::vector<double>& pdf) {
	::std::vector<double> f = ::std::vector<double>{256, 0};
	double previousSum = 0;

	for (int i=0; i<pdf.size(); i++) {
		f[i] = f[i] + previousSum;
		previousSum = f[i];
	}

	return f;
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
	::std::vector<double> H = getCumulativeDensityProbabilityDistribution(h);

	FILE* f = fopen(fileToCreate, "w");
	if (f != NULL) {
		for (int i=0; i<h.size(); i++) {
			fprintf(f, "%d %f\n", i, h[i]);
		}
		fclose(f);
	}
}


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

	createHistogramOf(image, "histogram.dat");
	::std::cout << "DONE. Please open gnuplot (sudo apt-get install gnuplot) and inside its terminal perform the action" << ::std::endl;
	::std::cout << "   plot \"histogram.dat\" using 1:2 with lines" << ::std::endl;

	namedWindow("Display Image", WINDOW_AUTOSIZE );
	imshow("Display Image", image);
	waitKey(0);

	destroyWindow("Display Image");

	return 0;
}
