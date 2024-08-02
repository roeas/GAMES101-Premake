//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>

namespace
{

static inline Eigen::Vector3f Vec3bToVector3f(const cv::Vec3b& vec)
{
    return Eigen::Vector3f(vec[0], vec[1], vec[2]);
}

static inline Eigen::Vector3f LerpVec3(const Eigen::Vector3f& veca, const Eigen::Vector3f &vecb, float t)
{
    return veca + std::clamp(t, 0.0f, 1.0f) * (vecb - veca);
}

}

class Texture
{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        u = std::clamp(u, 0.0f, 1.0f);
        v = std::clamp(v, 0.0f, 1.0f);

        int u_index = static_cast<int>(u * static_cast<float>(width));
        int v_index = static_cast<int>((1.0f - v) * static_cast<float>(height));
        u_index = std::clamp(u_index, 0, width - 1);
        v_index = std::clamp(v_index, 0, height - 1);

        return Vec3bToVector3f(image_data.at<cv::Vec3b>(v_index, u_index));
    }

    Eigen::Vector3f getColorBilinear(float u, float v)
    {
        u = std::clamp(u, 0.0f, 1.0f);
        v = std::clamp(v, 0.0f, 1.0f);

        float widthf = static_cast<float>(width);
        float heightf = static_cast<float>(height);

        float u_img = u * widthf;
        float v_img = (1.0f - v) * heightf;
        Eigen::Vector2f center{ std::round(u_img), std::round(v_img) };

        // 边界情况，当采样点位于边界时进行 clamp，使得两对次采样点值相等，即此时双线性插值退化为单次线性插值
        Eigen::Vector2f u00{ std::max(0.0f, center.x() - 0.5f), std::max(0.0f, center.y() - 0.5f) };
        Eigen::Vector2f u01{ std::max(0.0f, center.x() - 0.5f), std::min(heightf, center.y() + 0.5f)};
        Eigen::Vector2f u10{ std::min(widthf, center.x() + 0.5f), std::max(0.0f, center.y() - 0.5f) };
        Eigen::Vector2f u11{ std::min(widthf, center.x() + 0.5f), std::min(heightf, center.y() + 0.5f) };

        // 边界情况，假设次采样点坐标为 (width, height)，它应当采样的下标则为 (width - 1, height - 1)
        Eigen::Vector2i u00_index{ std::clamp(static_cast<int>(u00.x()), 0, width - 1), std::clamp(static_cast<int>(u00.y()), 0, height - 1) };
        Eigen::Vector2i u01_index{ std::clamp(static_cast<int>(u01.x()), 0, width - 1), std::clamp(static_cast<int>(u01.y()), 0, height - 1) };
        Eigen::Vector2i u10_index{ std::clamp(static_cast<int>(u10.x()), 0, width - 1), std::clamp(static_cast<int>(u10.y()), 0, height - 1) };
        Eigen::Vector2i u11_index{ std::clamp(static_cast<int>(u11.x()), 0, width - 1), std::clamp(static_cast<int>(u11.y()), 0, height - 1) };

        Eigen::Vector3f u00_value = Vec3bToVector3f(image_data.at<cv::Vec3b>(u00_index.y(), u00_index.x()));
        Eigen::Vector3f u01_value = Vec3bToVector3f(image_data.at<cv::Vec3b>(u01_index.y(), u01_index.x()));
        Eigen::Vector3f u10_value = Vec3bToVector3f(image_data.at<cv::Vec3b>(u10_index.y(), u10_index.x()));
        Eigen::Vector3f u11_value = Vec3bToVector3f(image_data.at<cv::Vec3b>(u11_index.y(), u11_index.x()));

        float s = (u_img - u00.x()) / (u10.x() - u00.x());
        float t = (v_img - u00.y()) / (u01.y() - u00.y());

        Eigen::Vector3f u0 = LerpVec3(u00_value, u10_value, s);
        Eigen::Vector3f u1 = LerpVec3(u01_value, u11_value, s);
        Eigen::Vector3f color = LerpVec3(u0, u1, t);

        return color;
    }
};
#endif //RASTERIZER_TEXTURE_H
