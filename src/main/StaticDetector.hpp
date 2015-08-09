#ifndef __BTB_STATIC_DETECTOR__
#define __BTB_STATIC_DETECTOR__

#include <string>
#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv2/nonfree/features2d.hpp"

namespace BTB
{
  class ProcessedImage
  {
  public:
    ProcessedImage(cv::SURF algo, cv::Mat image);
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
  };

  class StaticDetector
  {
  public:
    StaticDetector();
    static StaticDetector CreateFromTrainFolder(std::string path);
    void addTrainImage(cv::Mat image);
    cv::Point2f detectIn(cv::Mat image);
    cv::Size getTrainImageSize();

  private:
    cv::SURF algo;
    std::vector<ProcessedImage> processedTrainImages;
    cv::Size trainImageSize;
    cv::BFMatcher matcher;
  };
}

#endif
