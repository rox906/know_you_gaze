#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <sstream>

cv::Mat src, src_gray;
cv::Mat dst, detected_edges;
int lowThreshold = 0;
const int max_lowThreshold = 100;
const int ratio = 3;
const int kernel_size = 3;
const char *window_name = "Edge Map";

void detect_and_show(cv::Mat src)
{
  // cv::resize(src, src, cv::Size(960, 540));
  // dst.create(src.size(), src.type());
  cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
  // namedWindow( window_name, WINDOW_AUTOSIZE );
  // createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );
  // CannyThreshold(0, 0);
  std::vector<std::vector<cv::Point>> my_contours;
  std::vector<cv::Vec4i> hireachy;
  double mythreshold = 100;
  cv::Canny(src_gray, src_gray, mythreshold, 3 * mythreshold);
  cv::findContours(src_gray, my_contours, hireachy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
  std::vector<std::pair<double, int>> drawid;
  for (unsigned int i = 0; i < my_contours.size(); ++i)
  {
    drawid.push_back(std::make_pair(cv::arcLength(my_contours[i], 0), i));
  }
  sort(drawid.begin(), drawid.end(), [](std::pair<double, int> a, std::pair<double, int> b) { return a.first > b.first; });
  // cv::Mat binary = Mat::zeros(src.size(), CV_8UC3);
  // cv::imshow("raw", binary);
  std::vector<std::pair<double, int>> drawid_area;
  for (unsigned int i = 0; i < 5; ++i)
  {
    drawid_area.push_back(std::make_pair(cv::contourArea(my_contours[drawid[i].second]), drawid[i].second));
  }
  sort(drawid_area.begin(), drawid_area.end(), [](std::pair<double, int> a, std::pair<double, int> b) { return a.first > b.first; });
  cv::drawContours(src, my_contours, drawid_area[0].second, cv::Scalar::all(255), 1);
  cv::imshow("drawcontours", src);
  // for (unsigned int i = 0; i < drawid_area.size(); ++i)
  // {
  //   // cv::drawContours(binary, my_contours, drawid[i].second, cv::Scalar::all(255), 1);
  //   cv::drawContours(src, my_contours, drawid_area[i].second, cv::Scalar::all(255), 1);
  //   cv::imshow("drawcontours", src);
  // }
  // cv::waitKey(0);
}

int main(int argc, char **argv)
{
  cv::Mat frame, camera_matrix, distortion_coefficients, tmp;
  std::string showfigure = "showfigure";
  cv::namedWindow(showfigure, CV_WINDOW_NORMAL);
  cv::setWindowProperty(showfigure, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
  cv::Mat myfigure = cv::Mat::zeros(cv::Size(1920, 1080), CV_8UC3);
  myfigure = cv::Scalar::all(255);
  cv::imshow(showfigure, myfigure);
  cv::VideoCapture capture(0);
  cv::FileStorage file_storage("out_camera_data.xml", cv::FileStorage::READ);
  file_storage["camera_matrix"] >> camera_matrix;
  file_storage["distortion_coefficients"] >> distortion_coefficients;
  file_storage.release();

  if (capture.isOpened())
  {
    while (true)
    {
      capture.read(frame);
      cv::resize(frame,frame,cv::Size(640,400));
      if (!frame.empty())
      {
        tmp = frame.clone();
        cv::undistort(frame, tmp, camera_matrix, distortion_coefficients);
        detect_and_show(tmp);
      }
      int c = cv::waitKey(10);
      if ((char)c == 'q')
        break;
    }
  }
  return 0;
}