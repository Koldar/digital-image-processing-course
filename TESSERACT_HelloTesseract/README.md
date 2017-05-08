Building
========

after running the script tesseract.bash (installing everything you need to have in order to have tesseract up and running), perform the following
inside the "helloTesseract" folder:

	```
	g++ -std=c++11 helloWorld.cpp -o helloWorld -llept -ltesseract
	./helloWorld Hello_world.png
	```


The expected output should be:

	```
	starting program...
fetching image name from Hello_world.png...
name is Hello_world.png
I have read the following text: "Hello World!

"
	```
