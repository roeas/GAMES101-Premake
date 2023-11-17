#include "Scene.hpp"
#include "Sphere.hpp"
#include "Triangle.hpp"
#include "Light.hpp"
#include "Renderer.hpp"

// In the main function of the program, we create the scene (create objects and lights)
// as well as set the options for the render (image width and height, maximum recursion
// depth, field-of-view, etc.). We then call the render function().
int main()
{
    Scene scene(1280, 960);

    auto sph1 = std::make_unique<Sphere>(Vector3f(-1.0f, 0.0f, -12.0f), 2.0f);
    sph1->materialType = DIFFUSE_AND_GLOSSY;
    sph1->diffuseColor = Vector3f(0.6f, 0.7f, 0.8f);

    auto sph2 = std::make_unique<Sphere>(Vector3f(0.5f, -0.5f, -8.0f), 1.5f);
    sph2->ior = 1.5f;
    sph2->materialType = REFLECTION_AND_REFRACTION;

    scene.Add(std::move(sph1));
    scene.Add(std::move(sph2));

    Vector3f verts[4] = { {-5.0f,-3.0f,-6.0f}, {5.0f,-3.0f,-6.0f}, {5.0f,-3.0f,-16.0f}, {-5.0f,-3.0f,-16.0f} };
    uint32_t vertIndex[6] = { 0, 1, 3, 1, 2, 3 };
    Vector2f st[4] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };
    auto mesh = std::make_unique<MeshTriangle>(verts, vertIndex, 2, st);
    mesh->materialType = DIFFUSE_AND_GLOSSY;

    scene.Add(std::move(mesh));
    scene.Add(std::make_unique<Light>(Vector3f(-20.0f, 70.0f, 20.0f), 0.5f));
    scene.Add(std::make_unique<Light>(Vector3f(30.0f, 50.0f, -12.0f), 0.5f));

    Renderer r;
    r.Render(scene);

    return 0;
}
