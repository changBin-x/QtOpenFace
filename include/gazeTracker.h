#ifndef GAZETRACKER_H
#define GAZETRACKER_H

#include <QImage>
#include <QMutex>
#include <QObject>
#include <QTimer>
#include <memory>
#include <opencv2/opencv.hpp>

#include "FaceAnalyser.h"
#include "FaceAnalyserParameters.h"
#include "LandmarkDetectorFunc.h"
#include "LandmarkDetectorModel.h"
#include "LandmarkDetectorParameters.h"
#include "SequenceCapture.h"
class GazeTracker : public QObject {
  Q_OBJECT

 public:
  explicit GazeTracker(QObject* parent = nullptr);
  ~GazeTracker();
  void estimateGaze();  ///< 估计视线方向
  cv::Vec2d getGazeAngle();
  cv::Vec6d getheadPoseEstimate();
 signals:
 private:
  // 调用openFace API的参数表
  std::vector<std::string> m_arguments;
  ///< 面部模型参数
  std::shared_ptr<LandmarkDetector::FaceModelParameters> m_detctorParameters;
  std::shared_ptr<LandmarkDetector::CLNF> m_faceModel;  ///< CLNF 面部模型

  // 加载面部特征检测器参数
  std::shared_ptr<FaceAnalysis::FaceAnalyserParameters> m_faceAnalysisParams;
  // 面部分析器
  std::shared_ptr<FaceAnalysis::FaceAnalyser> m_faceAnalyser;

  std::shared_ptr<Utilities::SequenceCapture> m_sequenceReader;

  cv::Vec2d m_gazeAngle;         // 视线角度
  cv::Vec6d m_headPoseEstimate;  // 头部的姿态

 private:
  void convertFrameToQImage(const cv::Mat& mat, QImage& image);
};

#endif  // GAZETRACKER_H
