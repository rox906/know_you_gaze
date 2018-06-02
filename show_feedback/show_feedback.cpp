#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
using namespace cv;
Mat src, src_gray;
Mat dst, detected_edges;
int lowThreshold = 0;
const int max_lowThreshold = 100;
const int ratio = 3;
const int kernel_size = 3;
const char *window_name = "Edge Map";
static void CannyThreshold(int, void *)
{
  blur(src_gray, detected_edges, Size(3, 3));
  Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);
  dst = Scalar::all(0);
  src.copyTo(dst, detected_edges);
  imshow(window_name, dst);
}
int main(int argc, char **argv)
{
  src = imread("view.jpg"); // Load an image
  cv::resize(src, src, cv::Size(960, 540));
  // dst.create(src.size(), src.type());
  cvtColor(src, src_gray, COLOR_BGR2GRAY);
  // namedWindow( window_name, WINDOW_AUTOSIZE );
  // createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );
  // CannyThreshold(0, 0);
  std::vector<std::vector<cv::Point>> my_contours;
  std::vector<cv::Vec4i> hireachy;
  Canny(src_gray, src_gray, 50, 150);
  cv::findContours(src_gray, my_contours, hireachy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
  std::vector<std::pair<double, int>> drawid;
  for (unsigned int i = 0; i < my_contours.size(); ++i)
  {
    drawid.push_back(std::make_pair(cv::arcLength(my_contours[i], 0), i));
  }
  sort(drawid.begin(), drawid.end(), [](std::pair<double, int> a, std::pair<double, int> b) { return a.first > b.first; });
  cv::Mat binary = Mat::zeros(src.size(), CV_8UC3);
  // cv::imshow("raw", binary);
  std::vector<std::pair<double, int>> drawid_area;
  for (unsigned int i = 0; i < 5; ++i)
  {
    drawid_area.push_back(std::make_pair(cv::contourArea(my_contours[drawid[i].second]),drawid[i].second));
  }
  sort(drawid_area.begin(), drawid_area.end(), [](std::pair<double, int> a, std::pair<double, int> b) { return a.first > b.first; });
  for (unsigned int i = 0; i < drawid_area.size(); ++i)
  {
    // cv::drawContours(binary, my_contours, drawid[i].second, cv::Scalar::all(255), 1);
    cv::drawContours(src, my_contours, drawid_area[i].second, cv::Scalar::all(255), 1);
    cv::imshow("drawcontours", src);
    waitKey(0);
  }
  waitKey(0);
  return 0;
}