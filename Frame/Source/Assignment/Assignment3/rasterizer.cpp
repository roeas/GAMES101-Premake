//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>

namespace
{

inline static Eigen::Vector4f to_vec4(const Eigen::Vector3f &v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}

inline static bool insideTriangle(float x, float y, const Vector4f *_v)
{
    Vector3f v[3];
    for (int i = 0; i < 3; i++)
        v[i] = { _v[i].x(),_v[i].y(), 1.0 };
    Vector3f f0, f1, f2;
    f0 = v[1].cross(v[0]);
    f1 = v[2].cross(v[1]);
    f2 = v[0].cross(v[2]);
    Vector3f p(x, y, 1.);
    if ((p.dot(f0) * f0.dot(v[2]) > 0) && (p.dot(f1) * f1.dot(v[0]) > 0) && (p.dot(f2) * f2.dot(v[1]) > 0))
        return true;
    return false;
}

inline static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector4f *v)
{
    float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
    float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
    float c3 = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());
    return { c1,c2,c3 };
}

inline static Eigen::Vector3f interpolate(float alpha, float beta, float gamma, const Eigen::Vector3f &vert1, const Eigen::Vector3f &vert2, const Eigen::Vector3f &vert3, float weightReciprocal)
{
    return (vert1 * alpha + vert2 * beta + vert3 * gamma) * weightReciprocal;
}
inline static Eigen::Vector2f interpolate(float alpha, float beta, float gamma, const Eigen::Vector2f &vert1, const Eigen::Vector2f &vert2, const Eigen::Vector2f &vert3, float weightReciprocal)
{
    return (vert1 * alpha + vert2 * beta + vert3 * gamma) * weightReciprocal;
}
inline static float interpolate(float alpha, float beta, float gamma, float vert1, float vert2, float vert3, float weightReciprocal)
{
    return (vert1 * alpha + vert2 * beta + vert3 * gamma) * weightReciprocal;
}

}

void rst::rasterizer::draw(std::vector<Triangle *> &TriangleList)
{
    constexpr float f1 = (50 - 0.1) / 2.0;
    constexpr float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (const auto& t : TriangleList)
    {
        Triangle newtri = *t;

        std::array<Eigen::Vector4f, 3> mm {
                (view * model * t->v[0]),
                (view * model * t->v[1]),
                (view * model * t->v[2])
        };

        std::array<Eigen::Vector3f, 3> viewspace_pos;

        std::transform(mm.begin(), mm.end(), viewspace_pos.begin(), [](auto& v) {
            return v.template head<3>();
        });

        Eigen::Vector4f v[] = {
                mvp * t->v[0],
                mvp * t->v[1],
                mvp * t->v[2]
        };
        //Homogeneous division
        for (auto& vec : v) {
            vec.x()/=vec.w();
            vec.y()/=vec.w();
            vec.z()/=vec.w();
        }

        Eigen::Matrix4f inv_trans = (view * model).inverse().transpose();
        Eigen::Vector4f n[] = {
                inv_trans * to_vec4(t->normal[0], 0.0f),
                inv_trans * to_vec4(t->normal[1], 0.0f),
                inv_trans * to_vec4(t->normal[2], 0.0f)
        };

        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            //screen space coordinates
            newtri.setVertex(i, v[i]);
        }

        for (int i = 0; i < 3; ++i)
        {
            //view space normal
            newtri.setNormal(i, n[i].head<3>());
        }

        newtri.setColor(0, 92.0f, 138.0, 186.0f);
        newtri.setColor(1, 92.0f, 138.0, 186.0f);
        newtri.setColor(2, 92.0f, 138.0, 186.0f);

        // Also pass view space vertice position
        rasterize_triangle(newtri, viewspace_pos);
    }
}

// Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t, const std::array<Eigen::Vector3f, 3>& view_pos) 
{
    // 为了获取顶点的 w 分量，这里不能使用 Triangle::toVector4
    const Eigen::Vector4f *v = t.v;

    // AABB 应当由整数的像素下标索引定义
    uint16_t min_x = std::floor(std::min(v[0].x(), std::min(v[1].x(), v[2].x())));
    uint16_t max_x = std::ceil(std::max(v[0].x(), std::max(v[1].x(), v[2].x())));
    uint16_t min_y = std::floor(std::min(v[0].y(), std::min(v[1].y(), v[2].y())));
    uint16_t max_y = std::ceil(std::max(v[0].y(), std::max(v[1].y(), v[2].y())));

    for (uint16_t pos_x = min_x; pos_x <= max_x; ++pos_x)
    {
        for (uint16_t pos_y = min_y; pos_y <= max_y; ++pos_y)
        {
            if (!insideTriangle((float)pos_x + 0.5f, static_cast<float>(pos_y) + 0.5f, t.v))
            {
                continue;
            }

            auto [alpha, beta, gamma] = computeBarycentric2D((float)pos_x, static_cast<float>(pos_y), t.v);
            float alpha_w = alpha / v[0].w();
            float beta_w = beta / v[1].w();
            float gamma_w = gamma / v[2].w();
            float final_z = 1.0f / (alpha_w + beta_w + gamma_w);

            size_t index = static_cast<size_t>(get_index((int)pos_x, (int)pos_y));
            float &depth = depth_buf[index];

            // 修复框架 bug
            if (final_z > depth)
            {
                depth = final_z;

                Eigen::Vector3f color = interpolate(alpha_w, beta_w, gamma_w, t.color[0], t.color[1], t.color[2], final_z);
                Eigen::Vector3f normal = interpolate(alpha_w, beta_w, gamma_w, t.normal[0], t.normal[1], t.normal[2], final_z);
                Eigen::Vector2f texCoords = interpolate(alpha_w, beta_w, gamma_w, t.tex_coords[0], t.tex_coords[1], t.tex_coords[2], final_z);
                Eigen::Vector3f point = interpolate(alpha_w, beta_w, gamma_w, view_pos[0], view_pos[1], view_pos[2], final_z);

                fragment_shader_payload payload(
                    std::move(point),
                    std::move(color),
                    std::move(normal),
                    std::move(texCoords),
                    texture.has_value() ? &(texture.value()) : nullptr);

                set_pixel({ (int)pos_x, (int)pos_y }, fragment_shader(std::move(payload)));
            }
        }
    }
}

void rst::rasterizer::set_model(Eigen::Matrix4f m)
{
    model = std::move(m);
}

void rst::rasterizer::set_view(Eigen::Matrix4f v)
{
    view = std::move(v);
}

void rst::rasterizer::set_projection(Eigen::Matrix4f p)
{
    projection = std::move(p);
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{ 0.0f, 0.0f, 0.0f });
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

    texture = std::nullopt;
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height - y) * width + x;
}

int rst::rasterizer::get_index(const Vector2i& xy)
{
    return (height - xy.y()) * width + xy.x();
}

void rst::rasterizer::set_pixel(const Vector2i &point, Eigen::Vector3f color)
{
    frame_buf[get_index(point)] = std::move(color);
}

void rst::rasterizer::set_vertex_shader(std::function<Eigen::Vector3f(vertex_shader_payload)> vert_shader)
{
    vertex_shader = std::move(vert_shader);
}

void rst::rasterizer::set_fragment_shader(std::function<Eigen::Vector3f(fragment_shader_payload)> frag_shader)
{
    fragment_shader = std::move(frag_shader);
}
