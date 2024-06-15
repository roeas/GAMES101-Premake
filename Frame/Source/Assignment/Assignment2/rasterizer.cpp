// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>


rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}


static bool insideTriangle(float x, float y, const Vector3f* v)
{   
    // 此时位于屏幕空间，保证 z 分量一致即可
    Eigen::Vector3f point = { x, y, 1.0f };
    Eigen::Vector3f vertex_a = { v[0].x(), v[0].y(), 1.0f };
    Eigen::Vector3f vertex_b = { v[1].x(), v[1].y(), 1.0f };
    Eigen::Vector3f vertex_c = { v[2].x(), v[2].y(), 1.0f };

    Eigen::Vector3f vector_ab = vertex_b - vertex_a;
    Eigen::Vector3f vector_bc = vertex_c - vertex_b;
    Eigen::Vector3f vector_ca = vertex_a - vertex_c;
    Eigen::Vector3f vector_ap = point - vertex_a;
    Eigen::Vector3f vector_bp = point - vertex_b;
    Eigen::Vector3f vector_cp = point - vertex_c;

    float crossz1 = vector_ab.cross(vector_ap).z();
    float crossz2 = vector_bc.cross(vector_bp).z();
    float crossz3 = vector_ca.cross(vector_cp).z();

    return (crossz1 <= 0.0f && crossz2 <= 0.0f && crossz3 <= 0.0f) ||
        (crossz1 >= 0.0f && crossz2 >= 0.0f && crossz3 >= 0.0f);
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f* v)
{
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        Triangle t;
        Eigen::Vector4f v[] = {
                mvp * to_vec4(buf[i[0]], 1.0f),
                mvp * to_vec4(buf[i[1]], 1.0f),
                mvp * to_vec4(buf[i[2]], 1.0f)
        };
        //Homogeneous division
        for (auto& vec : v) {
            vec /= vec.w();
        }
        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        rasterize_triangle(t);
    }
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t) {
    auto v = t.toVector4();
    
    // AABB 应当由整数的像素下标索引定义
    uint16_t min_x = std::floor(std::min(v[0].x(), std::min(v[1].x(), v[2].x())));
    uint16_t max_x = std::ceil(std::max(v[0].x(), std::max(v[1].x(), v[2].x())));
    uint16_t min_y = std::floor(std::min(v[0].y(), std::min(v[1].y(), v[2].y())));
    uint16_t max_y = std::ceil(std::max(v[0].y(), std::max(v[1].y(), v[2].y())));

    for (uint16_t pos_x = min_x; pos_x <= max_x; ++pos_x)
    {
        for (uint16_t pos_y = min_y; pos_y <= max_y; ++pos_y)
        {
            size_t index = static_cast<size_t>(get_index(static_cast<int>(pos_x), static_cast<int>(pos_y)));
            float &depth = depth_buf[index];

            // SSAA 所采样的四个子像素相对于像素左下角坐标的偏移量
            static const std::vector<Eigen::Vector2f> s_offsets = { {0.25f, 0.25f}, {0.75f, 0.25f}, {0.25f, 0.75f}, {0.75f, 0.75f} };
            constexpr uint8_t SubPixelCount = 4;
            assert(s_offsets.size() == SubPixelCount);

            // 用于计算子像素贡献值
            uint8_t activeColor = 0;
            uint8_t activeDepth = 0;
            Eigen::Vector3f finalColor = { 0.0f, 0.0f , 0.0f };
            float finalDepth = 0.0f;

            for (const auto& offset : s_offsets)
            {
                float subPos_x = static_cast<float>(pos_x) + offset.x();
                float subPos_y = static_cast<float>(pos_y) + offset.y();

                // 获取子像素的深度插值
                auto [alpha, beta, gamma] = computeBarycentric2D(subPos_x, subPos_y, t.v);
                float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                z_interpolated *= w_reciprocal;

                if (insideTriangle(subPos_x, subPos_y, t.v))
                {
                    // 修复框架 bug
                    if (z_interpolated > depth)
                    {
                        // 只有同时通过了 inside 测试与深度测试才会对颜色有贡献
                        ++activeColor;
                        finalColor += t.getColor();
                    }
                    // 只要通过 inside 测试就会对深度有贡献
                    ++activeDepth;
                    finalDepth += z_interpolated;
                }
            }

            finalColor /= static_cast<float>(activeColor);
            finalDepth /= static_cast<float>(activeDepth);

            // 没有通过测试的子像素依旧应当对颜色与深度产生贡献，
            // 按理来说其值应从 frame_buf_ssaax4 与 depth_buf_ssaax4 中获取。
            // 鉴于框架本身较为简陋，这里意思到位即可。
            // finalColor += static_cast<float>(SubPixelCount - activeColor) / static_cast<float>(SubPixelCount) * frame_buf_ssaax2[subIndex];
            // finalDepth += static_cast<float>(SubPixelCount - activeDepth) / static_cast<float>(SubPixelCount) * depth_buf_ssaax2[subIndex];

            // 修复框架 bug
            if (finalDepth > depth)
            {
                depth = finalDepth;
                set_pixel(Eigen::Vector3f{ static_cast<float>(pos_x), static_cast<float>(pos_y), finalDepth }, finalColor);
            }
        }
    }
}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        // 修复框架 bug
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::lowest());
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-1-y)*width + x;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (height-1-point.y())*width + point.x();
    frame_buf[ind] = color;

}

// clang-format on