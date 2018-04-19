#include <cstdio>
#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>

//from https://www.pyimagesearch.com/2014/11/24/detecting-barcodes-images-python-opencv/
int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    cv::Mat image(cv::imread( argv[1], 1));

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    cv::namedWindow("TEST", cv::WINDOW_AUTOSIZE );
    //imshow("Display Image", image);
    //TODO init to empty?


    cv::Mat imageBw;
	
    cv::cvtColor(image, imageBw, cv::COLOR_BGR2GRAY);

    cv::Mat gradX;
    cv::Mat gradY;

    int use_scharr_filter = CV_SCHARR;
    cv::Sobel(imageBw, gradX, CV_32F, 1, 0, use_scharr_filter);
    cv::Sobel(imageBw, gradY, CV_32F, 0, 1, use_scharr_filter);

    auto gradient = abs(gradX - gradY);
    cv::Mat blurred;
    cv::blur(gradient, blurred, cv::Size(9, 9));
    
    cv::Mat thresholded;
    cv::threshold(blurred, thresholded, 255.0, 255.0, cv::THRESH_BINARY);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(21, 7));
    cv::Mat closed;
  
    cv::morphologyEx(thresholded, closed, cv::MORPH_CLOSE, kernel);
 
    
    cv::erode(closed, closed, cv::Mat{}, cv::Point(-1, -1), 4);
    cv::dilate(closed, closed, cv::Mat{}, cv::Point(-1, -1), 4);

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::findContours(closed, contours, hierarchy, cv::RETR_EXTERNAL,
	cv::CHAIN_APPROX_SIMPLE);

    auto sort_contours_op = [](const auto& polygon1, const auto& polygon2)
    {
	const bool not_oriented = false;
	return cv::contourArea(polygon1, not_oriented) < cv::contourArea(polygon2, not_oriented);
    };

    std::sort(contours.begin(), contours.end(), sort_contours_op);

    auto enclosing_rect = cv::minAreaRect(*contours.begin());
	
    cv::Mat enclosing_points;
    cv::boxPoints(enclosing_rect, enclosing_points);

    cv::drawContours(image, enclosing_points, -1, cv::Scalar(0, 255, 0), 3);

    cv::imshow("rectangles", image);
    cv::waitKey(0);

    return 0;
}
