/**
 * @file compress.cpp
 * @author Alexander Novotny (anovotny@nevada.unr.edu)
 * @brief A program which takes in several images and applies compression to the saturation
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

void compressImage(const std::string& image, unsigned level, const std::string& inputDirectory,
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
		std::cout << "Usage: compress <input directory> <output directory> [-v]" << std::endl
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

	// Compress all test images at every level
	for (int i = 0; i < 5; i++) {
		for (unsigned l = 2; l <= MAX_COMPRESS_LEVEL; l *= 2) {
			compressImage(IMAGES[i], l, inputDirectory, outputDirectory, displayImageToScreen);
			cv::waitKey(0);
		}
	}

	// Then close all windows. Important - will segmentation fault if not included.
	cv::destroyAllWindows();
	return 0;
}

void compressImage(const std::string& image, unsigned level, const std::string& inputDirectory,
                   const std::string& outputDirectory, bool displayToScreen) {
	cv::Mat oldImage, compressedImage, hsv[3];
	unsigned bucket;

	// Load the starting image
	oldImage = cv::imread(inputDirectory + image, cv::IMREAD_COLOR);
	oldImage.copyTo(compressedImage);
	cv::cvtColor(compressedImage, compressedImage, cv::COLOR_BGR2HSV);
	cv::split(compressedImage, hsv);

	// Only compress the saturation value. Break into buckets and choose the minimum value for that
	// bucket. I would prefer to do an average like toon.cpp, but this doesn't give results which
	// look like the assignment results.
	unsigned c = 1;  // Saturation
	for (unsigned i = 0; i < oldImage.cols; i++) {
		for (unsigned j = 0; j < oldImage.rows; j++) {
			uint8_t& pixel = hsv[c].at<uint8_t>(j, i);
			bucket         = pixel * level / 256;

			// Minimum saturation level for that bucket
			pixel = bucket * 255 / level;
		}
	}

	// Merge back into image and convert back to RGB
	cv::merge(hsv, 3, compressedImage);
	cv::cvtColor(compressedImage, compressedImage, cv::COLOR_HSV2BGR);

	// Output compressed image
	cv::imwrite(outputDirectory + image.substr(0, image.find_last_of('.')) + "-comp-" +
	                std::to_string(level) + image.substr(image.find_last_of('.')),
	            compressedImage);

	if (displayToScreen) {
		cv::imshow("Display Image", compressedImage);
		cv::imshow("Display Image Original", oldImage);
	}
}
