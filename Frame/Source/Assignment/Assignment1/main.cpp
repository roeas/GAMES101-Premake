#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.141592634;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eyePos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f translate;
    translate <<
        1.0f, 0.0f, 0.0f, -eyePos[0],
        0.0f, 1.0f, 0.0f, -eyePos[1],
        0.0f, 0.0f, 1.0f, -eyePos[2],
        0.0f, 0.0f, 0.0f, 1.0f;
    view = translate * view;
    return view;
}

Eigen::Matrix4f get_model_matrix(float angle)
{
    Eigen::Matrix4f model;
    model <<
        std::cos(angle), -std::sin(angle), 0.0f, 0.0f,
        std::sin(angle), std::cos(angle), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f;
    return model;
}

Eigen::Matrix4f get_projection_matrix(float fov, float aspect, float near, float far)
{
    float top = std::tan(fov * 0.5f * MY_PI / 180.0f) * std::abs(near);
    float right = aspect * top;
    float bottom = -top;
    float left = -right;

    Eigen::Matrix4f Mp2o;
    Mp2o <<
        near, 0.0f, 0.0f, 0.0f,
        0.0f, near, 0.0f, 0.0f,
        0.0f, 0.0f, near + far, -near * far,
        0.0f, 0.0f, 1.0f, 0.0f;

    Eigen::Matrix4f Mtrans;
    Mtrans <<
        1.0f, 0.0f, 0.0f, (right + left) * -0.5f,
        0.0f, 1.0f, 0.0f, (top + bottom) * -0.5f,
        0.0f, 0.0f, 1.0f, (near + far) * -0.5f,
        0.0f, 0.0f, 0.0f, 1.0f;

    Eigen::Matrix4f Mrotate;
    Mrotate <<
        2.0f / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f / (near - far), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f;

    Eigen::Matrix4f Morthographic = Mrotate * Mtrans;

    return Morthographic * Mp2o;
}

int main(int argc, const char **argv)
{
    float angle = 0;
    rst::rasterizer r(700, 700);
    Eigen::Vector3f eyePos = { 0, 0, 5 };

    std::vector<Eigen::Vector3f> pos{ {2, 0, -2}, {0, 2, -2}, {-2, 0, -2} };
    std::vector<Eigen::Vector3i> ind{ {0, 1, 2} };

    auto posHandle = r.load_positions(pos);
    auto indHandle = r.load_indices(ind);

    int key = 0;
    int frameCount = 0;

    while (key != 27)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eyePos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(posHandle, indHandle, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(1);

        std::cout << "frame count: " << frameCount++ << '\n';

        if (key == 'a')
        {
            angle += 0.2f;
        }
        else if (key == 'd')
        {
            angle -= 0.2f;
        }
    }

    return 0;
}
