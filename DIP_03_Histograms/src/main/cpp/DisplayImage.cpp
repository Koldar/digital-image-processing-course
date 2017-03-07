#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <limits.h>

using namespace cv;



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
	//addUniformAndGaussianNoise(image);
	//addSaltAndPepperNoise(image);

	//Mat csf = Mat{700, 700, CV_8U};
	//generateContrastSensitivityFunction(csf);

	namedWindow("Display Image", WINDOW_AUTOSIZE );
	imshow("Display Image", image);
	waitKey(0);

	return 0;
}
