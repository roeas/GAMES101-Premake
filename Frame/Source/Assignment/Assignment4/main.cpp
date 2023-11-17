#include <chrono>
#include <iostream>

#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/Dense>
#include <opencv2/opencv.hpp>

constexpr size_t POITN_COUNT = 4;

std::vector<cv::Point2f> control_points;

void mouse_handler(int event, int x, int y, int flags, void *userdata) 
{
    if (event == cv::EVENT_LBUTTONDOWN && control_points.size() < POITN_COUNT)
    {
        std::cout << "Mouse clicked: (" << x << ", " << y << ")" << std::endl;
        control_points.emplace_back(x, y);
    }     
}

void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window) 
{
    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001) 
    {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                 3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

inline cv::Point2f LerpPoint2f(const cv::Point2f &a, const cv::Point2f &b, float t)
{
    return a + t * (b - a);
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f> &control_points, float t) 
{
    if (control_points.size() == 2)
    {
        return LerpPoint2f(control_points[0], control_points[1], t);
    }

    std::vector<cv::Point2f> temp;
    for (size_t i = 0; i < control_points.size() - 1; ++i)
    {
        temp.emplace_back(LerpPoint2f(control_points[i], control_points[i + 1], t));
    }

    return recursive_bezier(temp, t);
}

void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window) 
{
    float filter[3][3] = {
        0.2f, 0.5f, 0.2f,
        0.5f, 1.0f, 0.5f,
        0.2f, 0.5f, 0.2f
    };

    Eigen::Vector2i offset[3][3] = {
        Eigen::Vector2i{-1, 1}, Eigen::Vector2i{0, 1}, Eigen::Vector2i{1, 1},
        Eigen::Vector2i{-1, 0}, Eigen::Vector2i{0, 0}, Eigen::Vector2i{1, 0},
        Eigen::Vector2i{-1, -1}, Eigen::Vector2i{0, -1}, Eigen::Vector2i{1, -1},
    };

    for (float t = 0.0; t <= 1.0; t += 0.001)
    {
        cv::Point2f p = recursive_bezier(control_points, t);
        Eigen::Vector2i point{ p.x, p.y };

        for (size_t i = 0; i < 3; ++i)
        {
            for (size_t j = 0; j < 3; ++j)
            {
                int x_index = point.x() + offset[i][j].x();
                int y_index = point.y() + offset[i][j].y();

                // OpenVC 会使一个像素点的值 mod 255
                uchar color = std::max(static_cast<uchar>(filter[i][j] * 255), window.at<cv::Vec3b>(y_index, x_index)[1]);
                window.at<cv::Vec3b>(y_index, x_index)[1] = std::min(static_cast<uchar>(255), color);
            }
        }
    }
}

int main() 
{
    cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    int key = -1;
    while (key != 27) 
    {
        for (auto &point : control_points) 
        {
            cv::circle(window, point, 3, { 255, 255, 255 }, 3);
        }

        if (control_points.size() == POITN_COUNT)
        {
            // naive_bezier(control_points, window);
            bezier(control_points, window);
        }

        cv::imshow("Bezier Curve", window);

        key = cv::waitKey(10);
    }

    return 0;
}
