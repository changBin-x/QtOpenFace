#include "gazeTracker.h"

#include <QCoreApplication>
#include <QDebug>

#include "GazeEstimation.h"
#include "RecorderOpenFace.h"
#include "RecorderOpenFaceParameters.h"

GazeTracker::GazeTracker(QObject *parent) {
  // TODO: 这个参数表可以用文件来配置
  // 参数参考:
  // https://github.com/TadasBaltrusaitis/OpenFace/wiki/Command-line-arguments/
  m_arguments = {
      ".\\FeatureExtraction.exe",  // 可执行程序的名称或路径。这个参数不重要
      "-device",                   // 参数1：表示设备选项
      "0",                         // 参数1的值：表示设备编号
      "-cam_width",   // 参数2：表示摄像头宽度选项
      "1920",         // 参数2的值：表示摄像头的宽度
      "-cam_height",  // 参数3：表示摄像头高度选项
      "1080"          // 参数3的值：表示摄像头的高度
  };
  m_detctorParameters =
      std::make_shared<LandmarkDetector::FaceModelParameters>(m_arguments);

  if (m_detctorParameters) {
    m_faceModel = std::make_shared<LandmarkDetector::CLNF>(
        m_detctorParameters->model_location);
    if (!m_faceModel->loaded_successfully) {
      qWarning() << "Error: FaceModel failed to load.";
      QCoreApplication::exit(-1);  // 安全退出程序，返回 -1 表示错误状态
      return;                      // 确保退出构造函数
    }
    if (!m_faceModel->eye_model) {
      qWarning() << "WARNING: no eye model found";
    }
  }
  m_faceAnalysisParams =
      std::make_shared<FaceAnalysis::FaceAnalyserParameters>(m_arguments);
  if (m_faceAnalysisParams) {
    m_faceAnalyser =
        std::make_shared<FaceAnalysis::FaceAnalyser>(*m_faceAnalysisParams);
    if (m_faceAnalyser->GetAUClassNames().empty()) {
      qWarning() << "WARNING: no Action Unit models found";
    }
  }
  m_sequenceReader = std::make_shared<Utilities::SequenceCapture>();
}

GazeTracker::~GazeTracker() { m_sequenceReader->Close(); }

cv::Vec2d GazeTracker::getGazeAngle() { return m_gazeAngle; }

cv::Vec6d GazeTracker::getheadPoseEstimate() { return m_headPoseEstimate; }

void GazeTracker::estimateGaze() {
  // reading from a webcam
  while (true) {
    // 序列化读取器根据参数列表打开设备
    if (!m_sequenceReader->Open(m_arguments)) {
      break;
    }
    qWarning() << "Camera Device not open!";
    if (m_sequenceReader->IsWebcam()) {
      qWarning()
          << "WARNING: using a webcam in feature extraction, Action Unit "
             "predictions will not be as accurate in real-time webcam mode";
    }
    cv::Mat capturedImg;
    Utilities::RecorderOpenFaceParameters recording_params(
        m_arguments, true, m_sequenceReader->IsWebcam(), m_sequenceReader->fx,
        m_sequenceReader->fy, m_sequenceReader->cx, m_sequenceReader->cy,
        m_sequenceReader->fps);
    // 眼睛模型没有，则不进行视线追踪
    if (!m_faceModel->eye_model) {
      qWarning() << "WARNING:no eye model found, eye Gaze tracking failed";
      recording_params.setOutputGaze(false);
    }
    Utilities::RecorderOpenFace openFaceRec(m_sequenceReader->name,
                                            recording_params, m_arguments);
    if (recording_params.outputGaze() && !m_faceModel->eye_model) {
      qWarning() << "WARNING: no eye model defined, but outputting gaze";
    }
    capturedImg = m_sequenceReader->GetNextFrame();
    // For reporting progress
    // double reported_completion = 0;
    // 开始视线追踪
    while (!capturedImg.empty()) {
      // 转换为灰度图
      cv::Mat_<uchar> grayScaleImage = m_sequenceReader->GetGrayFrame();
      // 面部标识点检测和追踪
      bool detectionSuccess = LandmarkDetector::DetectLandmarksInVideo(
          capturedImg, *m_faceModel, *m_detctorParameters, grayScaleImage);
      // 眼神追踪, 视线的绝对方向
      cv::Point3f gazeDirection0(0, 0, 0);
      cv::Point3f gazeDirection1(0, 0, 0);
      if (detectionSuccess && m_faceModel->eye_model) {
        GazeAnalysis::EstimateGaze(*m_faceModel, gazeDirection0,
                                   m_sequenceReader->fx, m_sequenceReader->fy,
                                   m_sequenceReader->cx, m_sequenceReader->cy,
                                   true);
        GazeAnalysis::EstimateGaze(*m_faceModel, gazeDirection1,
                                   m_sequenceReader->fx, m_sequenceReader->fy,
                                   m_sequenceReader->cx, m_sequenceReader->cy,
                                   true);
        m_gazeAngle =
            GazeAnalysis::GetGazeAngle(gazeDirection0, gazeDirection1);
        qDebug() << "Gaze Angle:" << m_gazeAngle[0] << "," << m_gazeAngle[1];
      }
      // 脸部对准
      cv::Mat simWarpedImg;
      cv::Mat_<double> hogDescriptor;
      int numHogrows = 0, numHogcols = 0;
      // 执行AU检测和HOG特征提取，耗费计算量大，必要时通过参数开启
      // 对于眼神追踪而言不需要
      if (recording_params.outputAlignedFaces() ||
          recording_params.outputHOG() || recording_params.outputAUs()) {
        m_faceAnalyser->AddNextFrame(
            capturedImg, m_faceModel->detected_landmarks,
            m_faceModel->detection_success, m_sequenceReader->time_stamp,
            m_sequenceReader->IsWebcam());
        m_faceAnalyser->GetLatestAlignedFace(simWarpedImg);
        m_faceAnalyser->GetLatestHOG(hogDescriptor, numHogrows, numHogcols);
      }
      // 计算头部姿态
      m_headPoseEstimate = LandmarkDetector::GetPose(
          *m_faceModel, m_sequenceReader->fx, m_sequenceReader->fy,
          m_sequenceReader->cx, m_sequenceReader->cy);
      // Setting up the recorder output
      // 不需要

      // Grabbing the next frame in the sequence
      capturedImg = m_sequenceReader->GetNextFrame();
    }
    openFaceRec.Close();
    m_sequenceReader->Close();
    m_faceAnalyser->Reset();
    m_faceModel->Reset();
  }
}
