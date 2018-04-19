#include <cstdio>
#include <vector>
#include <array>
#include <algorithm>
#include <exception>
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

    cv::Mat imageBw;
	
    cv::cvtColor(image, imageBw, cv::COLOR_BGR2GRAY);

    cv::Mat gradX;
    cv::Mat gradY;

    int use_scharr_filter = CV_SCHARR;
    cv::Sobel(imageBw, gradX, CV_32F, 1, 0, use_scharr_filter);
    cv::Sobel(imageBw, gradY, CV_32F, 0, 1, use_scharr_filter);

    cv::Mat gradient;
    auto diff = gradX - gradY;
    cv::convertScaleAbs(diff, gradient);
    cv::Mat blurred;
    cv::blur(gradient, blurred, cv::Size(9, 9));
    
    cv::Mat thresholded;
    cv::threshold(blurred, thresholded, 225.0, 255.0, cv::THRESH_BINARY);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(21, 7));
    cv::Mat closed;
  
    cv::morphologyEx(thresholded, closed, cv::MORPH_CLOSE, kernel);
 
    cv::erode(closed, closed, cv::Mat{}, cv::Point(-1, -1), 4);
    cv::dilate(closed, closed, cv::Mat{}, cv::Point(-1, -1), 4);

    std::vector<std::vector<cv::Point>> contours;
    
    cv::findContours(closed, contours, cv::RETR_EXTERNAL,
	cv::CHAIN_APPROX_SIMPLE);

    if(contours.empty())
       throw std::logic_error("I must have at least 1 contour");

    auto sort_contours_op = [](const auto& polygon1, const auto& polygon2)
    {
	const bool not_oriented = false;
	return cv::contourArea(polygon1, not_oriented) > cv::contourArea(polygon2, not_oriented);
    };

    std::sort(contours.begin(), contours.end(), sort_contours_op);

    auto enclosing_rect = cv::minAreaRect(*contours.begin());
    const int thickness = 3;
    std::array<cv::Point2f, 4> rect_points; enclosing_rect.points(rect_points.begin());
       for( int j = 0; j < rect_points.size(); j++ )
          cv::line(image, rect_points[j], rect_points[(j + 1) %  rect_points.size()], cv::Scalar(0, 255, 0), thickness);

    cv::imshow("rectangles", image);
    cv::waitKey(0);

    return 0;
}
