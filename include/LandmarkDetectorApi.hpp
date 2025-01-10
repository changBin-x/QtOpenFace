#ifndef LANDMARK_DETECTOR_HPP
#define LANDMARK_DETECTOR_HPP

#include <FaceAnalyser.h>
#include <Face_utils.h>
#include <LandmarkCoreIncludes.h>
#include <VisualizationUtils.h>

#include <QPointF>
#include <memory>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
/**
 * @brief 面部模型参数类
 * 该类用于封装 LandmarkDetector 的参数，并提供模型配置的实用函数。
 */
class FaceModelParameters {
 public:
  // LandmarkDetector 中的 FaceModelParameters 对象
  std::shared_ptr<LandmarkDetector::FaceModelParameters> m_params;

  /**
   * @brief 构造函数：初始化模型参数
   * @param root 模型文件的根路径
   * @param ceclm 如果为 true，使用 CECLM 模型
   * @param clnf 如果为 true，使用 CLNF 模型
   * @param clm 如果为 true，使用 CLM 模型
   */
  FaceModelParameters(const std::string& root, bool ceclm, bool clnf,
                      bool clm) {
    std::vector<std::string> args;
    args.push_back(root);

    std::string modelLoc =
        "model/main_ceclm_general.txt";  // 默认使用 CECLM 模型
    if (ceclm) {
      modelLoc = "model/main_ceclm_general.txt";
    } else if (clnf) {
      modelLoc = "model/main_clnf_general.txt";
    } else if (clm) {
      modelLoc = "model/main_clm_general.txt";
    }

    args.push_back("-mloc");
    args.push_back(modelLoc);

    m_params = std::make_shared<LandmarkDetector::FaceModelParameters>(args);
  }

  /**
   * @brief 优化模型参数用于视频处理
   */
  void optimizeForVideo() {
    m_params->window_sizes_small = {0, 9, 7, 0};
    m_params->window_sizes_init = {11, 9, 7, 5};
    m_params->window_sizes_current = m_params->window_sizes_init;

    m_params->multi_view = false;
    m_params->num_optimisation_iteration = 5;

    m_params->sigma = 1.5;
    m_params->reg_factor = 25;
    m_params->weight_factor = 0;

    // 如果使用 CE-CLM 模型，进一步调整参数
    if (m_params->curr_landmark_detector ==
        LandmarkDetector::FaceModelParameters::CECLM_DETECTOR) {
      m_params->sigma *= 1.5f;
      m_params->reg_factor *= 0.9f;
    }
  }

  /**
   * @brief 优化模型参数用于图像处理
   */
  void optimizeForImages() {
    m_params->window_sizes_init = {15, 13, 11, 11};
    m_params->multi_view = true;

    m_params->sigma = 1.25;
    m_params->reg_factor = 35;
    m_params->weight_factor = 2.5;
    m_params->num_optimisation_iteration = 10;

    // 如果使用 CE-CLM 模型，进一步调整参数
    if (m_params->curr_landmark_detector ==
        LandmarkDetector::FaceModelParameters::MTCNN_DETECTOR) {
      m_params->sigma *= 1.5f;
      m_params->reg_factor *= 0.9f;
    }
  }

  /**
   * @brief 检查是否使用 CECLM 模型
   * @return 如果使用 CECLM 返回 true，否则返回 false
   */
  bool isCECLM() const {
    return m_params->curr_landmark_detector ==
           LandmarkDetector::FaceModelParameters::CECLM_DETECTOR;
  }

  /**
   * @brief 检查是否使用 CLNF 模型
   * @return 如果使用 CLNF 返回 true，否则返回 false
   */
  bool isCLNF() const {
    return m_params->curr_landmark_detector ==
           LandmarkDetector::FaceModelParameters::CLNF_DETECTOR;
  }

  /**
   * @brief 检查是否使用 CLM 模型
   * @return 如果使用 CLM 返回 true，否则返回 false
   */
  bool isCLM() const {
    return m_params->curr_landmark_detector ==
           LandmarkDetector::FaceModelParameters::CLM_DETECTOR;
  }

  /**
   * @brief 获取 MTCNN 人脸检测器路径
   * @return 返回 MTCNN 检测器路径
   */
  std::string getMTCNNLocation() const {
    return m_params->mtcnn_face_detector_location;
  }

  /**
   * @brief 获取 Haar 人脸检测器路径
   * @return 返回 Haar 检测器路径
   */
  std::string getHaarLocation() const {
    return m_params->haar_face_detector_location;
  }

  /**
   * @brief 设置人脸检测器类型
   * @param haar 如果为 true，使用 Haar 检测器
   * @param hog 如果为 true，使用 HOG 检测器
   * @param cnn 如果为 true，使用 CNN 检测器
   */
  void setFaceDetector(bool haar, bool hog, bool cnn) {
    if (cnn) {
      m_params->curr_face_detector = m_params->MTCNN_DETECTOR;
    } else if (hog) {
      m_params->curr_face_detector = m_params->HOG_SVM_DETECTOR;
    } else if (haar) {
      m_params->curr_face_detector = m_params->HAAR_DETECTOR;
    }
  }

  /**
   * @brief 获取底层 LandmarkDetector 的参数
   * @return 返回 LandmarkDetector 的 FaceModelParameters 指针
   */
  LandmarkDetector::FaceModelParameters* getParams() { return m_params.get(); }
};

/**
 * @brief CLNF 模型类
 * 该类提供使用 CLNF 模型进行特征点检测的方法。
 */
class CLNF {
 private:
  LandmarkDetector::CLNF* m_clnf;  // 修正为直接存储 CLNF 对象，而非指针
 public:
  /**
   * @brief 默认构造函数，初始化一个空的 CLNF 模型
   */
  CLNF() : m_clnf(new LandmarkDetector::CLNF()) {}
  ~CLNF() {
    if (m_clnf) {
      delete m_clnf;
      m_clnf = nullptr;
    }
  }
  /**
   * @brief 带参数构造函数，根据指定参数初始化 CLNF 模型
   * @param params FaceModelParameters 对象
   */
  CLNF(FaceModelParameters& params)
      : m_clnf(new LandmarkDetector::CLNF(params.getParams()->model_location)) {
  }
  LandmarkDetector::CLNF* getCLNF() const { return m_clnf; }
  /**
   * @brief 重置 CLNF 模型状态
   */
  void reset() { m_clnf->Reset(); }

  void reset(double x, double y) { m_clnf->Reset(x, y); }
  bool HasEyeModel() { return m_clnf->eye_model; }
  double GetConfidence() { return m_clnf->detection_certainty; }
  /**
   * @brief 检查模型是否加载成功
   * @return 如果加载成功返回 true，否则返回 false
   */
  bool isLoaded() const { return m_clnf->loaded_successfully; }
  bool detectLandmarksInVideo(const cv::Mat& rgbImage,
                              FaceModelParameters& modelParams,
                              cv::Mat& grayImage) {
    return ::LandmarkDetector::DetectLandmarksInVideo(
        rgbImage, *m_clnf, *modelParams.getParams(), grayImage);
  }
  /**
   * @brief 在视频帧中检测面部特征点
   * @param rgbImage RGB 图像
   * @param modelParams 模型参数
   * @return 如果检测成功返回 true，否则返回 false
   */
  bool detectLandmarksInVideo(const cv::Mat& rgbImage,
                              FaceModelParameters& modelParams) {
    if (!m_clnf) return false;
    cv::Mat emptyMat = cv::Mat();
    return LandmarkDetector::DetectLandmarksInVideo(
        rgbImage, *m_clnf, *modelParams.getParams(), emptyMat);
  }
  /**
   * @brief 在图像中检测面部特征点
   * @param rgbImage 输入的 RGB 图像
   * @param modelParams 面部模型参数
   * @param grayImage 输入的灰度图像（可选）
   * @return 如果检测成功，返回 true；否则返回 false
   */
  bool detectFaceLandmarksInImage(
      const cv::Mat& rgbImage,
      LandmarkDetector::FaceModelParameters& modelParams, cv::Mat& grayImage) {
    return LandmarkDetector::DetectLandmarksInImage(rgbImage, *m_clnf,
                                                    modelParams, grayImage);
  }

  /**
   * @brief 在图像中检测面部特征点（不带灰度图像）
   * @param rgbImage 输入的 RGB 图像
   * @param modelParams 面部模型参数
   * @return 如果检测成功，返回 true；否则返回 false
   */
  bool detectFaceLandmarksInImage(
      const cv::Mat& rgbImage,
      LandmarkDetector::FaceModelParameters& modelParams) {
    cv::Mat emptyGray;  // 空的灰度图像
    return LandmarkDetector::DetectLandmarksInImage(rgbImage, *m_clnf,
                                                    modelParams, emptyGray);
  }

  /**
   * @brief 在给定的区域中检测面部特征点
   * @param rgbImage 输入的 RGB 图像
   * @param boundingBox 面部区域的边界框（cv::Rect）
   * @param modelParams 面部模型参数
   * @param grayImage 输入的灰度图像（可选）
   * @return 如果检测成功，返回 true；否则返回 false
   */
  bool detectFaceLandmarksInImage(
      const cv::Mat& rgbImage, const cv::Rect_<double>& boundingBox,
      LandmarkDetector::FaceModelParameters& modelParams, cv::Mat& grayImage) {
    return LandmarkDetector::DetectLandmarksInImage(
        rgbImage, boundingBox, *m_clnf, modelParams, grayImage);
  }

  /**
   * @brief 在给定的区域中检测面部特征点（不带灰度图像）
   * @param rgbImage 输入的 RGB 图像
   * @param boundingBox 面部区域的边界框（cv::Rect）
   * @param modelParams 面部模型参数
   * @return 如果检测成功，返回 true；否则返回 false
   */
  bool detectFaceLandmarksInImage(
      const cv::Mat& rgbImage, const cv::Rect_<double>& boundingBox,
      LandmarkDetector::FaceModelParameters& modelParams) {
    cv::Mat emptyGray;  // 空的灰度图像
    return LandmarkDetector::DetectLandmarksInImage(
        rgbImage, boundingBox, *m_clnf, modelParams, emptyGray);
  }

  /**
   * @brief 获取相对于摄像头的面部姿态
   * @param fx 焦距（x）
   * @param fy 焦距（y）
   * @param cx 主点（x）
   * @param cy 主点（y）
   * @return 包含面部姿态参数（缩放、旋转和位移）的向量
   */
  std::vector<float> getPose(float fx, float fy, float cx, float cy) const {
    cv::Vec6f pose = LandmarkDetector::GetPose(*m_clnf, fx, fy, cx, cy);

    std::vector<float> poseVec{0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    return std::vector<float>{pose[0], pose[1], pose[2],
                              pose[3], pose[4], pose[5]};
  }

  /**
   * @brief 获取相对于摄像头的面部姿态（包括旋转角度）
   * @param fx 摄像头的焦距 x
   * @param fy 摄像头的焦距 y
   * @param cx 摄像头的主点 x
   * @param cy 摄像头的主点 y
   * @return 一个包含面部姿态参数的 `std::vector<float>`，格式为 [Tx, Ty, Tz,
   * Eul_x, Eul_y, Eul_z]
   */
  std::vector<float> getPoseWRTCamera(float fx, float fy, float cx,
                                      float cy) const {
    // 调用 LandmarkDetector 的 GetPoseWRTCamera 方法
    cv::Vec6f pose =
        LandmarkDetector::GetPoseWRTCamera(*m_clnf, fx, fy, cx, cy);

    // 将 cv::Vec6f 转换为 std::vector<float>
    return std::vector<float>{pose[0], pose[1], pose[2],
                              pose[3], pose[4], pose[5]};
  }
  /**
   * @brief 获取特征点的可见性（是否被遮挡）
   * @return 一个包含可见性信息的
   * `std::vector<bool>`，其中每个元素表示一个特征点是否可见
   */
  std::vector<bool> getVisibilities() const {
    // 获取特征点可见性矩阵（0 表示不可见，1 表示可见）
    cv::Mat_<int> visibilities = m_clnf->GetVisibilities();

    // 将 OpenCV 矩阵转换为标准 C++ 的 vector<bool>
    std::vector<bool> visibility_vector;
    visibility_vector.reserve(visibilities.rows);

    for (int i = 0; i < visibilities.rows; ++i) {
      visibility_vector.push_back(visibilities(i, 0) != 0);  // 非零值表示可见
    }

    return visibility_vector;
  }
  /**
   * @brief 计算可见的特征点
   * @return 一个 `std::vector<std::pair<float,
   * float>>`，每个元素表示一个特征点的 (x, y) 坐标
   */
  std::vector<std::pair<float, float>> calculateVisibleLandmarks() const {
    // 使用 LandmarkDetector 提供的函数计算可见特征点
    std::vector<cv::Point2f> vecLandmarks =
        LandmarkDetector::CalculateVisibleLandmarks(*m_clnf);

    // 将 OpenCV 的 Point2f 转换为 std::pair<float, float>
    std::vector<std::pair<float, float>> landmarks;
    landmarks.reserve(vecLandmarks.size());  // 提前分配内存，提高性能

    for (const auto& point : vecLandmarks) {
      landmarks.emplace_back(point.x, point.y);
    }

    return landmarks;
  }
  /**
   * @brief 计算所有的面部特征点
   * @return 一个 `std::vector<std::pair<float,
   * float>>`，每个元素表示一个特征点的 (x, y) 坐标
   */
  std::vector<std::pair<float, float>> calculateAllLandmarks() const {
    // 调用 LandmarkDetector 的函数计算所有特征点
    std::vector<cv::Point2f> vecLandmarks =
        LandmarkDetector::CalculateAllLandmarks(*m_clnf);

    // 将 OpenCV 的 Point2f 转换为 std::pair<float, float>
    std::vector<std::pair<float, float>> landmarks;
    landmarks.reserve(vecLandmarks.size());  // 提前分配内存，提高性能

    for (const auto& point : vecLandmarks) {
      landmarks.emplace_back(point.x, point.y);
    }

    return landmarks;
  }

  /**
   * @brief 计算所有眼睛的特征点
   * @return 一个 `std::vector<std::pair<float,
   * float>>`，每个元素表示一个眼睛特征点的 (x, y) 坐标
   */
  std::vector<std::pair<float, float>> calculateAllEyeLandmarks() const {
    // 调用 LandmarkDetector 的函数计算所有眼睛的特征点
    std::vector<cv::Point2f> vecLandmarks =
        LandmarkDetector::CalculateAllEyeLandmarks(*m_clnf);

    // 将 OpenCV 的 Point2f 转换为 std::pair<float, float>
    std::vector<std::pair<float, float>> eyeLandmarks;
    eyeLandmarks.reserve(vecLandmarks.size());  // 提前分配内存，提高性能

    for (const auto& point : vecLandmarks) {
      eyeLandmarks.emplace_back(point.x, point.y);
    }

    return eyeLandmarks;
  }
  /**
   * @brief 计算所有眼睛的 3D 特征点
   * @param fx 焦距 (x)
   * @param fy 焦距 (y)
   * @param cx 主点坐标 (x)
   * @param cy 主点坐标 (y)
   * @return 一个包含所有眼睛特征点 3D 坐标的 `std::vector<std::tuple<float,
   * float, float>>`
   */
  std::vector<std::tuple<float, float, float>> calculateAllEyeLandmarks3D(
      float fx, float fy, float cx, float cy) const {
    // 调用 LandmarkDetector 的函数计算所有眼睛的 3D 特征点
    std::vector<cv::Point3f> vecLandmarks =
        LandmarkDetector::Calculate3DEyeLandmarks(*m_clnf, fx, fy, cx, cy);

    // 转换为 std::tuple<float, float, float>
    std::vector<std::tuple<float, float, float>> eyeLandmarks3D;
    eyeLandmarks3D.reserve(vecLandmarks.size());  // 提前分配内存，提高性能

    for (const auto& point : vecLandmarks) {
      eyeLandmarks3D.emplace_back(point.x, point.y, point.z);
    }

    return eyeLandmarks3D;
  }

  /**
   * @brief 计算可见的眼部特征点
   * @return 一个包含所有可见眼部特征点 2D 坐标的 `std::vector<std::pair<float,
   * float>>`
   */
  std::vector<std::pair<float, float>> calculateVisibleEyeLandmarks() const {
    // 调用 LandmarkDetector 的函数计算所有可见眼部特征点
    std::vector<cv::Point2f> vecLandmarks =
        LandmarkDetector::CalculateVisibleEyeLandmarks(*m_clnf);

    // 转换为 std::pair<float, float>
    std::vector<std::pair<float, float>> visibleEyeLandmarks;
    visibleEyeLandmarks.reserve(vecLandmarks.size());  // 提前分配内存以优化性能

    for (const auto& point : vecLandmarks) {
      visibleEyeLandmarks.emplace_back(point.x, point.y);
    }

    return visibleEyeLandmarks;
  }
  /**
   * @brief 计算 3D 特征点
   * @param fx 摄像机焦距 (x 方向)
   * @param fy 摄像机焦距 (y 方向)
   * @param cx 摄像机光心坐标 (x 方向)
   * @param cy 摄像机光心坐标 (y 方向)
   * @return 一个包含所有特征点 3D 坐标的 `std::vector<std::tuple<float, float,
   * float>>`
   */
  std::vector<std::tuple<float, float, float>> calculate3DLandmarks(
      float fx, float fy, float cx, float cy) const {
    // 获取 3D 特征点矩阵
    cv::Mat_<float> shape3D = m_clnf->GetShape(fx, fy, cx, cy);

    // 转换为 std::vector<std::tuple<float, float, float>>
    std::vector<std::tuple<float, float, float>> landmarks_3D;
    landmarks_3D.reserve(shape3D.cols);  // 提前分配内存以优化性能

    for (int i = 0; i < shape3D.cols; ++i) {
      landmarks_3D.emplace_back(shape3D.at<float>(0, i),  // x 坐标
                                shape3D.at<float>(1, i),  // y 坐标
                                shape3D.at<float>(2, i)   // z 坐标
      );
    }

    return landmarks_3D;
  }
  /**
   * @brief 计算 2D 人脸包围盒
   * @param fx 摄像机焦距 (x 方向)
   * @param fy 摄像机焦距 (y 方向)
   * @param cx 摄像机光心坐标 (x 方向)
   * @param cy 摄像机光心坐标 (y 方向)
   * @return 包含线段起点和终点的 `std::vector<std::pair<QPointF, QPointF>>`
   */
  std::vector<std::pair<QPointF, QPointF>> calculateBox(float fx, float fy,
                                                        float cx,
                                                        float cy) const {
    // 获取人脸的 6 个姿态参数 (Tx, Ty, Tz, Roll, Pitch, Yaw)
    cv::Vec6f pose = LandmarkDetector::GetPose(*m_clnf, fx, fy, cx, cy);

    // 计算包围盒线段
    std::vector<std::pair<cv::Point2f, cv::Point2f>> vecLines =
        Utilities::CalculateBox(pose, fx, fy, cx, cy);

    // 将 OpenCV 的 cv::Point2f 转换为 Qt 的 QPointF
    std::vector<std::pair<QPointF, QPointF>> lines;
    lines.reserve(vecLines.size());  // 提前分配内存以优化性能

    for (const auto& line : vecLines) {
      lines.emplace_back(QPointF(line.first.x, line.first.y),   // 起点
                         QPointF(line.second.x, line.second.y)  // 终点
      );
    }

    return lines;
  }

  /**
   * @brief 获取检测到的特征点数量
   * @return 特征点数量
   */
  int getNumPoints() const { return m_clnf->pdm.NumberOfPoints(); }

  /**
   * @brief 获取形状模型的模式数量
   * @return 模式数量
   */
  int getNumModes() const { return m_clnf->pdm.NumberOfModes(); }

  /**
   * @brief 获取刚性形状参数（描述面部的缩放、旋转和位移）
   * @return 返回刚性参数（scale, rotx, roty, rotz, tx, ty）的浮点数向量
   */
  std::vector<float> getRigidParams() const {
    std::vector<float> rigidParams;

    // CLNF 模型中刚性参数的数量固定为 6
    for (size_t i = 0; i < 6; ++i) {
      rigidParams.push_back(m_clnf->params_global[i]);
    }

    return rigidParams;
  }

  /**
   * @brief 获取非刚性形状参数（描述面部表情）
   * @return 返回非刚性参数的浮点数向量
   */
  std::vector<float> getNonRigidParams() const {
    std::vector<float> nonRigidParams;

    // 遍历 CLNF 模型中的局部形状参数矩阵
    for (int i = 0; i < m_clnf->params_local.rows; ++i) {
      nonRigidParams.push_back(m_clnf->params_local.at<float>(i));
    }

    return nonRigidParams;
  }

  /**
   * @brief 获取刚性和非刚性参数的组合
   * @return 返回所有参数的浮点数向量
   */
  std::vector<float> getParams() const {
    // 获取刚性参数
    std::vector<float> allParams = getRigidParams();

    // 获取非刚性参数并追加到刚性参数的末尾
    std::vector<float> nonRigidParams = getNonRigidParams();
    allParams.insert(allParams.end(), nonRigidParams.begin(),
                     nonRigidParams.end());

    return allParams;
  }
};

#endif  // LANDMARK_DETECTOR_HPP
