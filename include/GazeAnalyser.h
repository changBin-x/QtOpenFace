#ifndef GazeAnalyser_H
#define GazeAnalyser_H
#include <opencv2/opencv.hpp>

#include "LandmarkDetectorApi.hpp"
class GazeAnalyser {
 private:
  // Variable storing gaze for recording

  // Absolute gaze direction
  cv::Point3f m_gazeDirection0;
  cv::Point3f m_gazeDirection1;
  cv::Vec2f m_gazeAngle;

  cv::Point3f m_pupilLeft;
  cv::Point3f m_pupilRight;

 public:
  GazeAnalyser(const std::string& modelPath);
  GazeAnalyser();
  ~GazeAnalyser();

  /**
   * @brief 更新下一帧的视线估计
   * @param clnf LandmarkDetector 的 CLNF 模型
   * @param success 是否检测成功
   * @param fx 相机的焦距（x）
   * @param fy 相机的焦距（y）
   * @param cx 相机主点（x）
   * @param cy 相机主点（y）
   */
  void addNextFrame(LandmarkDetector::CLNF& clnf, bool success, float fx,
                    float fy, float cx, float cy);

  /**
   * @brief 获取相机坐标系下的视线方向
   * @return 返回左右眼视线方向的三维坐标
   */
  std::tuple<cv::Point3f, cv::Point3f> getGazeCamera() const;

  /**
   * @brief 获取视线角度
   * @return 返回视线角度（yaw, pitch）
   */
  cv::Vec2f getGazeAngle() const;

  /**
   * @brief 计算视线在图像中的投影线
   * @param fx 焦距（x）
   * @param fy 焦距（y）
   * @param cx 主点（x）
   * @param cy 主点（y）
   * @return 返回左右眼视线的投影线段
   */
  std::vector<std::pair<cv::Point2f, cv::Point2f>> calculateGazeLines(
      float fx, float fy, float cx, float cy) const;
};

#endif  // GazeAnalyser_H
