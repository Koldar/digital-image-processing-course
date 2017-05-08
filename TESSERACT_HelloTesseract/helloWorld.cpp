/**
 * reference: https://github.com/tesseract-ocr/tesseract/wiki/APIExample
 */


#include <iostream>
#include <string.h>
//contains some base API of tesseract
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>



using namespace std;
//be sure to add the namespace as well
using namespace tesseract;

int main(const int argc, const char** args) {
	cout << "starting program..." << endl;
	if (argc != 2) {
		cout << "Need to have exactly one arguments representing the name of the image to load." << endl;
		cout << "Accepted images: tif, png" << endl;
		cout << "language: english" << endl;
		
		return 1;
	}
	cout << "fetching image name from " << args[1] << "..." << endl;
	char* imageName;
	imageName = strdup(args[1]);	
	cout << "name is " << imageName << endl;

	TessBaseAPI* api = new TessBaseAPI{};
	if (api->Init(NULL, "eng")) {
		cout << "Can't initialize tesseract" << endl;
		return 2;
	}

	// Open input image with leptonica library
	Pix* pix = pixRead(imageName);
	api->SetImage(pix);
	//get OCR result
	char* text = api->GetUTF8Text();
	cout << "I have read the following text: \"" << text << "\"" << endl;

	api->End();
	delete [] text;
	pixDestroy(&pix);
	delete imageName;
	return 0;
}
