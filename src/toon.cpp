/**
 * @file toon.cpp
 * @author Alexander Novotny (anovotny@nevada.unr.edu)
 * @brief A program which takes in several images and applies toon-shading based compression
 * @version 0.1
 * @date 2019-04-06
 *
 * @copyright Copyright (c) 2019
 *
 */
#include <opencv2/opencv.hpp>
#include <string>

/**
 * @brief Array of test images
 *
 */
const std::string IMAGES[]        = {"ElCapitan.jpg", "Lilly.jpg", "Orchids.jpg", "OrchidsY.jpg",
                              "Parrot.jpg"};
const unsigned MAX_COMPRESS_LEVEL = 16;

void toonifyImage(const std::string& image, unsigned level, const std::string& inputDirectory,
                  const std::string& outputDirectory, bool displayToScreen);

/**
 * @brief Main function
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char** argv) {
	if (argc < 3 || !strcmp("-h", argv[2]) || !strcmp("--help", argv[2])) {
		std::cout << "Usage: toon <input directory> <output directory> [-v]" << std::endl
		          << std::endl
		          << "    <input directory> - Where the images can be found. Must end in '/' (i.e. "
		             "'Images/P-1/').\n"
		          << "    <output directory> - Where the output images should be stored. Must end "
		             "in '/' (i.e. 'Out/'). Must exist before running."
		          << std::endl
		          << "    -v - Flag to enable visual output. Cycle through by pressing any key"
		          << std::endl;

		return 1;
	}

	// Pull command line arguments
	std::string inputDirectory = argv[1], outputDirectory = argv[2];
	bool displayImageToScreen = (argc >= 4 && !strcmp("-v", argv[3]));

	// Acknowledge choice to display images and open windows
	if (displayImageToScreen) {
		std::cout << "Displaying comparison images to screen." << std::endl
		          << "Focus on window and press any key to cycle." << std::endl;
		cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
		cv::namedWindow("Display Image Original", cv::WINDOW_AUTOSIZE);
	}

	// toonify all test images at every level
	for (int i = 0; i < 5; i++) {
		for (unsigned l = 2; l <= MAX_COMPRESS_LEVEL; l *= 2) {
			toonifyImage(IMAGES[i], l, inputDirectory, outputDirectory, displayImageToScreen);
			cv::waitKey(0);
		}
	}

	// Then close all windows. Important - will segmentation fault if not included.
	cv::destroyAllWindows();
	return 0;
}

void toonifyImage(const std::string& image, unsigned level, const std::string& inputDirectory,
                  const std::string& outputDirectory, bool displayToScreen) {
	// Images
	cv::Mat oldImage, toonImage, bgr[3];
	double bucketValues[3][MAX_COMPRESS_LEVEL]    = {0};
	unsigned bucketEntries[3][MAX_COMPRESS_LEVEL] = {0};
	double entries;
	unsigned bucket;

	// Load the starting image in grayscale
	// If the grayscale otion is not included, will not work (learned the hard way)
	oldImage = cv::imread(inputDirectory + image, cv::IMREAD_COLOR);
	cv::split(oldImage, bgr);

	// Break each pixel down into its corresponding bucket, and keep track of what color each
	// bucket should be (a moving average)
	for (unsigned c = 0; c < 3; c++) {
		for (unsigned i = 0; i < oldImage.cols; i++) {
			for (unsigned j = 0; j < oldImage.rows; j++) {
				uint8_t& pixel = bgr[c].at<uint8_t>(j, i);
				bucket         = pixel * level / 256;
				entries        = bucketEntries[c][bucket];
				bucketEntries[c][bucket]++;

				// Moving average
				// (S + a) / (n + 1) = S/(n+1) + a/(n+1) = S/n * n/(n+1) + a/(n+1)
				bucketValues[c][bucket] =
				    bucketValues[c][bucket] * (entries / (entries + 1)) + pixel / (entries + 1);
				pixel = bucket;
			}
		}
	}

	// Then re-assign the colors for each bucket
	for (unsigned c = 0; c < 3; c++) {
		for (unsigned i = 0; i < oldImage.cols; i++) {
			for (unsigned j = 0; j < oldImage.rows; j++) {
				uint8_t& pixel = bgr[c].at<uint8_t>(j, i);
				pixel          = bucketValues[c][pixel];
			}
		}
	}

	// Merge back into a normal image
	cv::merge(bgr, 3, toonImage);

	// Save resulting image
	cv::imwrite(outputDirectory + image.substr(0, image.find_last_of('.')) + "-toon-" +
	                std::to_string(level) + image.substr(image.find_last_of('.')),
	            toonImage);

	if (displayToScreen) {
		cv::imshow("Display Image", toonImage);
		cv::imshow("Display Image Original", oldImage);
	}
}
