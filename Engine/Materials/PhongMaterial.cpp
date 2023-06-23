#include <PhongMaterial.hpp>

PhongMaterial::PhongMaterial()
    : ambient(1.0f, 1.0f, 1.0f, 1.0f),
    diffuse(1.0f, 1.0f, 1.0f, 1.0f),
    specular(1.0f, 1.0f, 1.0f, 1.0f),
    shininess(3.0f)
{ }