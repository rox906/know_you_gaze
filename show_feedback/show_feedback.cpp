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

void find_corners(std::vector<cv::Point> &corners, std::vector<cv::Point> &contour)
{
  auto i = contour.begin();
  for (int j = 0; j < 4; ++j)
    corners.push_back(*i);
  ++i;
  for (; i != contour.end(); ++i)
  {
    if (-corners[0].x + corners[0].y < -i->x + i->y)
      corners[0] = *i;
    if (corners[1].x + corners[1].y < i->x + i->y)
      corners[1] = *i;
    if (-corners[2].x + corners[2].y > -i->x + i->y)
      corners[2] = *i;
    if (corners[3].x + corners[3].y > i->x + i->y)
      corners[3] = *i;
  }
}

void screen_point(cv::Point frontpoint, cv::Mat &src, std::vector<cv::Point> &corners)
{
  //caculate transform
  std::vector<cv::Point2f> srccorners(4);
  srccorners[0] = cv::Point2f(0, src.rows-1);
  srccorners[1] = cv::Point2f(src.cols-1, src.rows-1);
  srccorners[2] = cv::Point2f(src.cols-1, 0);
  srccorners[3] = cv::Point2f(0, 0);
  std::vector<cv::Point2f> corners2f;
  for (int i = 0; i < 4; ++i) corners2f.push_back(corners[i]);
  cv::Mat transform = cv::getPerspectiveTransform(corners2f, srccorners);
  //calculate screenpoint and draw
  std::vector<cv::Point2f> raw;
  std::vector<cv::Point2f> trans;
  raw.push_back(frontpoint);
  cv::perspectiveTransform(raw, trans, transform);
  cv::circle(src, trans[0], 10, cv::Scalar::all(0), -1);
  return;
}

void detect_and_show(cv::Mat src, cv::Mat &myfigure, std::string showfigure)
{
  //init
  cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
  std::vector<std::vector<cv::Point>> my_contours;
  std::vector<cv::Vec4i> hireachy;
  double mythreshold = 100;
  //find biggest contour and draw
  cv::Canny(src_gray, src_gray, mythreshold, 3 * mythreshold);
  cv::findContours(src_gray, my_contours, hireachy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
  std::vector<std::pair<double, int>> drawid;
  for (unsigned int i = 0; i < my_contours.size(); ++i)
  {
    drawid.push_back(std::make_pair(cv::arcLength(my_contours[i], 0), i));
  }
  sort(drawid.begin(), drawid.end(), [](std::pair<double, int> a, std::pair<double, int> b) { return a.first > b.first; });
  std::vector<std::pair<double, int>> drawid_area;
  for (unsigned int i = 0; i < 5; ++i)
  {
    drawid_area.push_back(std::make_pair(cv::contourArea(my_contours[drawid[i].second]), drawid[i].second));
  }
  sort(drawid_area.begin(), drawid_area.end(), [](std::pair<double, int> a, std::pair<double, int> b) { return a.first > b.first; });
  cv::drawContours(src, my_contours, drawid_area[0].second, cv::Scalar::all(255), 1);
  //find corners and draw
  myfigure = cv::Scalar::all(255);
  std::vector<cv::Point> corners;
  find_corners(corners, my_contours[drawid_area[0].second]);
  screen_point(cv::Point(320, 200), myfigure, corners);
  //show result
  cv::imshow("drawcontour", src);
  cv::imshow(showfigure, myfigure);
  return;
}

int main(int argc, char **argv)
{
  //init
  cv::Mat frame, camera_matrix, distortion_coefficients, tmp;
  cv::VideoCapture capture(0);
  //show figure
  std::string showfigure = "showfigure";
  cv::namedWindow(showfigure, CV_WINDOW_NORMAL);
  cv::setWindowProperty(showfigure, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
  cv::Mat myfigure = cv::Mat::zeros(cv::Size(1920, 1080), CV_8UC3);
  myfigure = cv::Scalar::all(255);
  cv::imshow(showfigure, myfigure);
  //read matrix
  cv::FileStorage file_storage("out_camera_data.xml", cv::FileStorage::READ);
  file_storage["camera_matrix"] >> camera_matrix;
  file_storage["distortion_coefficients"] >> distortion_coefficients;
  file_storage.release();
  //main loop
  if (capture.isOpened())
  {
    while (true)
    {
      capture.read(frame);
      cv::resize(frame, frame, cv::Size(640, 400));
      if (!frame.empty())
      {
        tmp = frame.clone();
        cv::undistort(frame, tmp, camera_matrix, distortion_coefficients);
        detect_and_show(tmp, myfigure, showfigure);
      }
      int c = cv::waitKey(10);
      if ((char)c == 'q')
        break;
    }
  }
  return 0;
}