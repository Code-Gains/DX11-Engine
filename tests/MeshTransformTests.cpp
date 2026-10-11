#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>

// Mirror the Slang constructor's row ordering, independently of GLM's columns.
glm::mat4 ShaderTransform(glm::vec3 position, glm::quat q, glm::vec3 scale)
{
    const float x2=q.x*q.x, y2=q.y*q.y, z2=q.z*q.z;
    const float xy=q.x*q.y, xz=q.x*q.z, yz=q.y*q.z;
    const float wx=q.w*q.x, wy=q.w*q.y, wz=q.w*q.z;
    const std::array<glm::vec4, 4> rows{{
        {scale.x*(1-2*(y2+z2)), scale.x*2*(xy+wz), scale.x*2*(xz-wy), 0},
        {scale.y*2*(xy-wz), scale.y*(1-2*(x2+z2)), scale.y*2*(yz+wx), 0},
        {scale.z*2*(xz+wy), scale.z*2*(yz-wx), scale.z*(1-2*(x2+y2)), 0},
        {position, 1}
    }};
    glm::mat4 matrix(0.0f);
    for (int row=0; row<4; ++row) {
        for (int column=0; column<4; ++column) {
            matrix[column][row] = rows[row][column];
        }
    }
    return glm::transpose(matrix);
}

glm::mat3 ShaderNormalMatrix(glm::mat3 matrix)
{
    const glm::vec3 row0(matrix[0][0], matrix[1][0], matrix[2][0]);
    const glm::vec3 row1(matrix[0][1], matrix[1][1], matrix[2][1]);
    const glm::vec3 row2(matrix[0][2], matrix[1][2], matrix[2][2]);
    const auto c0 = glm::cross(row1, row2);
    const auto c1 = glm::cross(row2, row0);
    const auto c2 = glm::cross(row0, row1);
    const float inverseDeterminant = 1.0f / glm::dot(row0, c0);
    return glm::transpose(glm::mat3(c0, c1, c2)) * inverseDeterminant;
}

void Check(bool passed, const char* message)
{
    if (!passed) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

int main()
{
    const std::array rotations{
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 0, 1)),
        glm::angleAxis(glm::radians(-61.0f), glm::normalize(glm::vec3(1, 2, -3)))
    };
    const std::array scales{
        glm::vec3(1), glm::vec3(500), glm::vec3(2.4f, 10.0f, 2.4f),
        glm::vec3(0.1f, 3.0f, 7.0f)
    };
    const glm::vec3 position(17, -23, 9);
    const glm::vec3 a(-1, 0.1f, 0), b(2, 0, 1), c(0, 3, -2);
    const auto normal = glm::normalize(glm::cross(b-a, c-a));
    for (auto rotation : rotations) {
        for (auto scale : scales) {
            const auto matrix = ShaderTransform(position, rotation, scale);
            const auto expected = glm::translate(glm::mat4(1), position) *
                glm::mat4_cast(rotation) * glm::scale(glm::mat4(1), scale);
            for (int i=0; i<4; ++i) {
                Check(glm::length(matrix[i]-expected[i]) < 0.001f, "Transform disagrees with GLM T*R*S.");
            }
            const auto transformedNormal = glm::normalize(ShaderNormalMatrix(glm::mat3(matrix)) * normal);
            const auto edge1 = rotation * ((b-a)*scale);
            const auto edge2 = rotation * ((c-a)*scale);
            const auto geometricNormal = glm::normalize(glm::cross(edge1, edge2));
            Check(glm::dot(transformedNormal, geometricNormal) > 0.99999f,
                "Lighting normal disagrees with rotated/scaled triangle geometry.");
            const auto tangent = glm::mat3(matrix) * (b-a);
            Check(std::abs(glm::dot(glm::normalize(tangent), transformedNormal)) < 0.00001f,
                "Transformed tangent is not perpendicular to the normal.");
        }
    }
    const auto zRotation = rotations[1];
    const auto transformed = glm::mat3(ShaderTransform({}, zRotation, glm::vec3(1))) * glm::vec3(1, 0, 0);
    Check(glm::length(transformed-glm::vec3(0, 1, 0)) < 0.00001f, "90-degree regression: +X must rotate toward +Y.");
    std::cout << "Mesh transform tests passed (12 rotation/scale combinations).\n";
}
