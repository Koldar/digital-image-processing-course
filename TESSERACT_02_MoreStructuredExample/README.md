Building
========

	```
	mkdir -p build/Debug
	cd build/Debug
	cmake ../..
	make
	./Tesseract Hello_world.png
	```

Expected output
===============

	```
	starting program...
	fetching image name from Hello_world.png...
	name is Hello_world.png
	tesseract ocr version: 4.00.00alpha
	leptonica version: leptonica-1.74.1
	I have read the following text: "Hello World!

	"
	```
	
