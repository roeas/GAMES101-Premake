//
// Created by LEI XU on 4/11/19.
//

#include "Triangle.hpp"
#include <algorithm>
#include <array>

Triangle::Triangle() {
    v[0] << 0,0,0,1;
    v[1] << 0,0,0,1;
    v[2] << 0,0,0,1;

    color[0] << 0.0, 0.0, 0.0;
    color[1] << 0.0, 0.0, 0.0;
    color[2] << 0.0, 0.0, 0.0;

    tex_coords[0] << 0.0, 0.0;
    tex_coords[1] << 0.0, 0.0;
    tex_coords[2] << 0.0, 0.0;
}

void Triangle::setVertex(int ind, Vector4f ver){
    v[ind] = std::move(ver);
}
void Triangle::setNormal(int ind, Vector3f n){
    normal[ind] = std::move(n);
}
void Triangle::setColor(int ind, float r, float g, float b) {
    if((r<0.0) || (r>255.) ||
       (g<0.0) || (g>255.) ||
       (b<0.0) || (b>255.)) {
        fprintf(stderr, "ERROR! Invalid color values");
        fflush(stderr);
        exit(-1);
    }

    color[ind] = Vector3f((float)r/255.,(float)g/255.,(float)b/255.);
    return;
}
void Triangle::setTexCoord(int ind, Vector2f uv) {
    tex_coords[ind] = std::move(uv);
}

std::array<Vector4f, 3> Triangle::toVector4() const
{
    std::array<Vector4f, 3> res;
    std::transform(std::begin(v), std::end(v), res.begin(), [](auto& vec) { return Vector4f(vec.x(), vec.y(), vec.z(), 1.f); });
    return res;
}
