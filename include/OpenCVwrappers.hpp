/******************************************************************************
 * @file       OpenCVwrappers.hpp
 * @brief 将QImage转换为opencv的操作
 * @detail
 * @author     ChangBin(bin_chang@qq.com)
 * @date       2025/01/09
 * @version    0.1
 * *Copyright (C) 2024 DonghaiLab Ocean Underwater Robot Tech Team. All Rights
 Reserved. Contact: https://www.donghailab.cn/
 *****************************************************************************/
#ifndef OPENCVWRAPPERS_HPP
#define OPENCVWRAPPERS_HPP
#include <QImage>
#include <opencv2/opencv.hpp>
class RawImage {
 private:
  cv::Mat m_cvMat;

 public:
  /**
   * @brief 构造函数：从 OpenCV 的 Mat 创建
   * @param image OpenCV 的 Mat
   */
  RawImage(const cv::Mat& image) : m_cvMat(image.clone()) {}

  /**
   * @brief 构造函数：从 QImage 创建
   * @param image 从 QImage 创建
   */
  RawImage(const QImage& image) {
    if (image.format() == QImage::Format_RGB888) {
      m_cvMat = cv::Mat(image.height(), image.width(), CV_8UC3,
                        const_cast<uchar*>(image.bits()), image.bytesPerLine())
                    .clone();
      cv::cvtColor(m_cvMat, m_cvMat, cv::COLOR_RGB2BGR);  // 转换为 BGR 格式
    } else if (image.format() == QImage::Format_Grayscale8) {
      m_cvMat = cv::Mat(image.height(), image.width(), CV_8UC1,
                        const_cast<uchar*>(image.bits()), image.bytesPerLine())
                    .clone();
    } else {
      throw std::runtime_error("Unsupported QImage format");
    }
  }

  ~RawImage() {
    m_cvMat.release();  // 释放 Mat 内存
  }

  /**
   * @brief 镜像图像（水平翻转）
   */
  void mirror() {
    cv::flip(m_cvMat, m_cvMat, 1);  // 水平翻转
  }

  /**
   * @brief 转换为 QImage
   * @return
   */
  QImage toQImage() const {
    if (m_cvMat.empty()) {
      return QImage();
    }

    QImage image;
    if (m_cvMat.type() == CV_8UC3) {
      cv::Mat rgbMat;
      cv::cvtColor(m_cvMat, rgbMat, cv::COLOR_BGR2RGB);  // 转换为 RGB 格式
      image = QImage(rgbMat.data, rgbMat.cols, rgbMat.rows, rgbMat.step,
                     QImage::Format_RGB888);
    } else if (m_cvMat.type() == CV_8UC1) {
      image = QImage(m_cvMat.data, m_cvMat.cols, m_cvMat.rows, m_cvMat.step,
                     QImage::Format_Grayscale8);
    } else {
      throw std::runtime_error("Unsupported image format");
    }

    return image.copy();  // 确保 QImage 的数据独立于原始 Mat
  }

  /**
   * @brief 获取图像宽度
   * @return 图像宽度
   */
  int width() const { return m_cvMat.cols; }

  /**
   * @brief 获取图像高度
   * @return 图像高度
   */
  int height() const { return m_cvMat.rows; }

  /**
   * @brief 获取每行的字节数
   * @return 每行的字节数
   */
  int stride() const { return m_cvMat.step; }

  bool isEmpty() const { return m_cvMat.empty(); }

  const cv::Mat& getMat() const { return m_cvMat; }
};

class VideoWriter {
 private:
  cv::VideoWriter m_writer;  // OpenCV 视频写入器
 public:
  VideoWriter(const std::string& filePath, int width, int height, double fps,
              bool isColor = true) {
    m_writer.open(filePath, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), fps,
                  cv::Size(width, height), isColor);
    if (!m_writer.isOpened()) {
      throw std::runtime_error("Failed to open video writer");
    }
  }

  ~VideoWriter() { close(); }

  /**
   * @brief 写入一帧图像
   * @param 一帧图像
   * @return
   */
  bool write(const RawImage& image) {
    if (!m_writer.isOpened() || image.isEmpty()) {
      return false;
    }

    m_writer.write(image.getMat());
    return true;
  }

  /**
   * @brief 检查写入器是否已打开
   * @return 写入器是否已打开
   */
  bool isOpen() const { return m_writer.isOpened(); }
  /**
   * @brief 关闭视频写入
   */
  void close() {
    if (m_writer.isOpened()) m_writer.release();
  }
};
#endif  // OPENCVWRAPPERS_HPP
