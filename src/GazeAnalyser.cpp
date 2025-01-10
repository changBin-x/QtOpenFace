#include "GazeAnalyser.h"

#include "GazeEstimation.h"
GazeAnalyser::GazeAnalyser(const std::string& modelPath) {}

GazeAnalyser::GazeAnalyser()
    : m_gazeDirection0(0.0f, 0.0f, 0.0f),
      m_gazeDirection1(0.0f, 0.0f, 0.0f),
      m_gazeAngle(0.0f, 0.0f),
      m_pupilLeft(0.0f, 0.0f, 0.0f),
      m_pupilRight(0.0f, 0.0f, 0.0f) {}

GazeAnalyser::~GazeAnalyser() {}

/**
 * @brief 更新下一帧的视线估计
 * @param clnf LandmarkDetector 的 CLNF 模型
 * @param success 是否检测成功
 * @param fx 相机的焦距（x）
 * @param fy 相机的焦距（y）
 * @param cx 相机主点（x）
 * @param cy 相机主点（y）
 */
void GazeAnalyser::addNextFrame(LandmarkDetector::CLNF& clnf, bool success,
                                float fx, float fy, float cx, float cy) {
  if (!success) {
    return;
  }

  // 估计视线方向
  GazeAnalysis::EstimateGaze(clnf, m_gazeDirection0, fx, fy, cx, cy, true);
  GazeAnalysis::EstimateGaze(clnf, m_gazeDirection1, fx, fy, cx, cy, false);

  // 获取视线角度
  m_gazeAngle = GazeAnalysis::GetGazeAngle(m_gazeDirection0, m_gazeDirection1);

  // 提取眼睛区域的索引
  int partLeft = -1, partRight = -1;
  for (size_t i = 0; i < clnf.hierarchical_models.size(); ++i) {
    if (clnf.hierarchical_model_names[i] == "left_eye_28") {
      partLeft = i;
    }
    if (clnf.hierarchical_model_names[i] == "right_eye_28") {
      partRight = i;
    }
  }

  // 计算左眼和右眼瞳孔位置
  if (partLeft != -1) {
    cv::Mat_<float> eyeLdmks3dLeft =
        clnf.hierarchical_models[partLeft].GetShape(fx, fy, cx, cy);
    m_pupilLeft = GazeAnalysis::GetPupilPosition(eyeLdmks3dLeft);
  }

  if (partRight != -1) {
    cv::Mat_<float> eyeLdmks3dRight =
        clnf.hierarchical_models[partRight].GetShape(fx, fy, cx, cy);
    m_pupilRight = GazeAnalysis::GetPupilPosition(eyeLdmks3dRight);
  }
}

std::tuple<cv::Point3f, cv::Point3f> GazeAnalyser::getGazeCamera() const {
  return {m_gazeDirection0, m_gazeDirection1};
}

cv::Vec2f GazeAnalyser::getGazeAngle() const { return m_gazeAngle; }

std::vector<std::pair<cv::Point2f, cv::Point2f> >
GazeAnalyser::calculateGazeLines(float fx, float fy, float cx, float cy) const {
  std::vector<cv::Point3f> pointsLeft = {
      m_pupilLeft, m_pupilLeft + m_gazeDirection0 * 40.0f};

  std::vector<cv::Point3f> pointsRight = {
      m_pupilRight, m_pupilRight + m_gazeDirection1 * 40.0f};

  std::vector<cv::Point2f> imagePointsLeft, imagePointsRight;

  // 投影左眼视线到图像平面
  for (const auto& point : pointsLeft) {
    float x = point.x * fx / point.z + cx;
    float y = point.y * fy / point.z + cy;
    imagePointsLeft.emplace_back(x, y);
  }

  // 投影右眼视线到图像平面
  for (const auto& point : pointsRight) {
    float x = point.x * fx / point.z + cx;
    float y = point.y * fy / point.z + cy;
    imagePointsRight.emplace_back(x, y);
  }

  // 返回线段
  return {{imagePointsLeft[0], imagePointsLeft[1]},
          {imagePointsRight[0], imagePointsRight[1]}};
}
