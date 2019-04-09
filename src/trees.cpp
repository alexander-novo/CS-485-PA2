/**
 * @file trees.cpp
 * @author Alexander Novotny (anovotny@nevada.unr.edu)
 * @brief A program which takes in an image and attempts to find tree trunks
 * @version 0.1
 * @date 2019-04-06
 *
 * @copyright Copyright (c) 2019
 *
 */
#include <chrono>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

void treeifyImage(const std::string& imagePath, bool displayToScreen);

/**
 * @brief Main function
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char** argv) {
	if (argc < 2 || !strcmp("-h", argv[1]) || !strcmp("--help", argv[1])) {
		std::cout << "Usage: trees <input picture> [-v]" << std::endl
		          << std::endl
		          << "    <input directory> - Which picture to detect trees in.\n"
		          << std::endl
		          << "    -v - Flag to enable visual output. Cycle through by pressing any key"
		          << std::endl;

		return 1;
	}

	// Pull command line arguments
	std::string imagePath     = argv[1];
	bool displayImageToScreen = (argc >= 3 && !strcmp("-v", argv[2]));

	// Acknowledge choice to display images and open windows
	if (displayImageToScreen) {
		std::cout << "Displaying comparison images to screen." << std::endl
		          << "Focus on window and press any key to cycle." << std::endl;
		cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
	}

	treeifyImage(imagePath, displayImageToScreen);

	cv::waitKey(0);

	// Then close all windows. Important - will segmentation fault if not included.
	cv::destroyAllWindows();
	return 0;
}

void treeifyImage(const std::string& imagePath, bool displayToScreen) {
	const static unsigned MIN_TREE_RADIUS  = 4;
	const static unsigned MAX_TREE_RADIUS  = 41;
	const static unsigned TREE_RADIUS_STEP = 4;
	cv::Mat image;
	std::vector<cv::Vec3f> circles, actualCircles;

	// Load the starting image
	image = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);

	using std::chrono::high_resolution_clock;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	for (unsigned r = MIN_TREE_RADIUS; r < MAX_TREE_RADIUS; r += TREE_RADIUS_STEP) {
		cv::HoughCircles(image, circles, cv::HOUGH_GRADIENT, 1, 2 * r, 100.0,
		                 11 * r / sqrt(r * r - 1), r, r + TREE_RADIUS_STEP + 1);

		for (const cv::Vec3f& suspect : circles) {
			cv::Point suspectCenter(suspect[0], suspect[1]);
			unsigned suspectRadius = suspect[2];

			bool conflict = false;
			for (const cv::Vec3f& circle : actualCircles) {
				cv::Point center(circle[0], circle[1]);
				unsigned radius = circle[2];

				if (cv::norm(suspectCenter - center) < suspectRadius + radius) {
					conflict = true;
					break;
				}
			}

			if (!conflict) { actualCircles.push_back(suspect); }
		}

		circles.clear();
	}
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	std::chrono::duration<double> time_span =
	    std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	std::cout << "Run time: " << time_span.count() << " seconds." << std::endl;

	cv::cvtColor(image, image, cv::COLOR_GRAY2BGR);

	for (unsigned i = 0; i < actualCircles.size(); i++) {
		cv::Point center(actualCircles[i][0], actualCircles[i][1]);
		unsigned radius = actualCircles[i][2];

		cv::circle(image, center, radius, cv::Scalar(0, 0, 255), 1);
	}

	// Output Circle Image
	cv::imwrite(imagePath.substr(0, imagePath.find_last_of('.')) + "-circled" +
	                imagePath.substr(imagePath.find_last_of('.')),
	            image);

	if (displayToScreen) { cv::imshow("Display Image", image); }
}
