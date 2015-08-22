#include "DynamicDetector.hpp"

#include <gtest/gtest.h>
#include <time.h>

TEST(dynamicDetector, nonregression)
{
  const int expectedFrameCount = 825;
  const int expectedWidth = 640;
  const int expectedHeight = 512;

  const int tolerance = 10;
  const int assertEveryNDetectedFrame = 20;

  const int expectedMissDetectFrame = 759;

  cv::VideoCapture cap("../../../database/videos/webcam_1.avi");
  ASSERT_TRUE(cap.isOpened()) << "Unable to open video!";
  int frames = cap.get(CV_CAP_PROP_FRAME_COUNT);
  ASSERT_EQ(expectedFrameCount, frames) << "Unexpected number of frames";
  ASSERT_EQ(expectedWidth, cap.get(CV_CAP_PROP_FRAME_WIDTH)) << "Unexpected width";
  ASSERT_EQ(expectedHeight, cap.get(CV_CAP_PROP_FRAME_HEIGHT)) << "Unexpected height";

  BTB::StaticDetector staticDetector = BTB::StaticDetector::CreateFromTrainFolder("../../../database/training/");
  BTB::DynamicDetector detector(staticDetector);

  int assertCounter = 0;
  for (int i = 0; i < frames; i++)
  {
    cv::Mat frame;
    cap >> frame;
    ASSERT_TRUE(frame.data) << "Could not load frame";

    cv::Point2f v;
    if (detector.detectIn(frame, v))
    {
      cv::Size trainImageSize = staticDetector.getTrainImageSize();
      cv::rectangle(frame, cv::Point2f(0, 0) + v, cv::Point2f(trainImageSize.width, trainImageSize.height) + v, cv::Scalar(0, 0, 255), 4);

      assertCounter++;
      if (assertCounter % assertEveryNDetectedFrame == 0)
      {
        cv::Point2f v2;
        if (staticDetector.detectIn(frame, v2))
        {
          double distance = cv::norm(v2 - v);
          EXPECT_TRUE(distance < tolerance || i == expectedMissDetectFrame) << "Static vs Dynamic did not detect the same object on frame " << i << ", distance = " << distance;
        }
        else
        {
          std::cout << "Could detect dynamically but not statically on frame " << i << std::endl;
        }
      }
    }
    else
    {
      EXPECT_FALSE(staticDetector.detectIn(frame, v)) << "Could detect statically but not dynamically on frame " << i;
    }

    cv::imshow("live", frame);
    cv::waitKey(1);
  }

  cv::destroyWindow("live");
}

TEST(dynamicDetector, performance)
{
  cv::VideoCapture cap("../../../database/videos/webcam_1.avi");
  ASSERT_TRUE(cap.isOpened()) << "Unable to open video!";
  int frames = cap.get(CV_CAP_PROP_FRAME_COUNT);

  BTB::StaticDetector staticDetector = BTB::StaticDetector::CreateFromTrainFolder("../../../database/training/");
  BTB::DynamicDetector detector(staticDetector);

  time_t start;
  time(&start);

  for (int i = 0; i < frames; i++)
  {
    cv::Mat frame;
    cap >> frame;
    ASSERT_TRUE(frame.data) << "Could not load frame";

    cv::Point2f v;
    if (!detector.detectIn(frame, v))
    {
      continue;
    }
  }

  time_t end;
  time(&end);
  double duration = end - start;
  double fps = frames / duration;

  std::cout << "Performances: Tracked " << frames << " frames in " << duration << " seconds, " << fps << " FPS" << std::endl;
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
