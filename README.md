# Computer Vision Programming Assignment 1 - Image Normaliser

My first programming assignment for CS 485 - Computer Vision at UNR.

## How to build
Create a new directory `build/` and navigate to it. Then run cmake to generate appropriate makefiles:
```
	cmake ..
```
If you are on Windows and/or do not use standard makefiles, you may need to configure cmake to generate appropriate makefiles. An example of this is shown below for MinGW:
```
	cmake .. -G "MinGW Makefiles"
```
Then run the generated makefiles:
```
	make
```
(or for MinGW users)
```
	mingw32-make
```

## Command Usage
After building, run the program as follows:
```
	./normalise <input directory> <output directory> [-v]
```
Failure to meet the above syntax will result in a help menu with more detailed information. With the included input/output directories (and in the `build/` directory), the command can be run as follows:
```
	./normalise ../Images/ ../Out/
```
