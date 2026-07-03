#include <iostream>

// Mocking the Vector3d structure
struct Vector3d {
    float x, y, z;
    Vector3d(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

// Mocking the Renderer interface based on the tutorial's custom library
enum RenderMode { RM_SOLID };
enum ImmediateMode { IM_QUADS };

class Renderer {
public:
    void PushMatrix() { std::cout << "Pushing Matrix\n"; }
    void PopMatrix() { std::cout << "Popping Matrix\n"; }
    void TranslateWorldMatrix(float x, float y, float z) { 
        std::cout << "Translating to: " << x << ", " << y << ", " << z << "\n"; 
    }
    void SetRenderMode(RenderMode mode) { std::cout << "Set Render Mode: SOLID\n"; }
    void EnableImmediateMode(ImmediateMode mode) { std::cout << "Enabled Immediate Mode: QUADS\n"; }
    void DisableImmediateMode() { std::cout << "Disabled Immediate Mode\n"; }
    void ImmediateColourAlpha(float r, float g, float b, float a) {}
    void ImmediateNormal(float x, float y, float z) {}
    void ImmediateVertex(float x, float y, float z) {
        std::cout << "Vertex: " << x << ", " << y << ", " << z << "\n";
    }
};

void RenderSingleCube(Renderer* pRenderer) {
    float l_length = 1.0f;
    float l_height = 1.0f;
    float l_width = 1.0f;
    Vector3d blockPosition(0.0f, 0.0f, 0.0f);

    pRenderer->PushMatrix();
    pRenderer->TranslateWorldMatrix(blockPosition.x, blockPosition.y, blockPosition.z);
    pRenderer->SetRenderMode(RM_SOLID);
    pRenderer->EnableImmediateMode(IM_QUADS);
    pRenderer->ImmediateColourAlpha(1.0f, 1.0f, 1.0f, 1.0f);

    // Front Face
    pRenderer->ImmediateNormal(0.0f, 0.0f, -1.0f);
    pRenderer->ImmediateVertex(l_length, -l_height, -l_width);
    pRenderer->ImmediateVertex(-l_length, -l_height, -l_width);
    pRenderer->ImmediateVertex(-l_length, l_height, -l_width);
    pRenderer->ImmediateVertex(l_length, l_height, -l_width);

    // Back Face
    pRenderer->ImmediateNormal(0.0f, 0.0f, 1.0f);
    pRenderer->ImmediateVertex(-l_length, -l_height, l_width);
    pRenderer->ImmediateVertex(l_length, -l_height, l_width);
    pRenderer->ImmediateVertex(l_length, l_height, l_width);
    pRenderer->ImmediateVertex(-l_length, l_height, l_width);

    // Right Face
    pRenderer->ImmediateNormal(1.0f, 0.0f, 0.0f);
    pRenderer->ImmediateVertex(l_length, -l_height, l_width);
    pRenderer->ImmediateVertex(l_length, -l_height, -l_width);
    pRenderer->ImmediateVertex(l_length, l_height, -l_width);
    pRenderer->ImmediateVertex(l_length, l_height, l_width);

    // Left Face
    pRenderer->ImmediateNormal(-1.0f, 0.0f, 0.0f);
    pRenderer->ImmediateVertex(-l_length, -l_height, -l_width);
    pRenderer->ImmediateVertex(-l_length, -l_height, l_width);
    pRenderer->ImmediateVertex(-l_length, l_height, l_width);
    pRenderer->ImmediateVertex(-l_length, l_height, -l_width);

    // Bottom Face
    pRenderer->ImmediateNormal(0.0f, -1.0f, 0.0f);
    pRenderer->ImmediateVertex(-l_length, -l_height, -l_width);
    pRenderer->ImmediateVertex(l_length, -l_height, -l_width);
    pRenderer->ImmediateVertex(l_length, -l_height, l_width);
    pRenderer->ImmediateVertex(-l_length, -l_height, l_width);

    // Top Face
    pRenderer->ImmediateNormal(0.0f, 1.0f, 0.0f);
    pRenderer->ImmediateVertex(l_length, l_height, -l_width);
    pRenderer->ImmediateVertex(-l_length, l_height, -l_width);
    pRenderer->ImmediateVertex(-l_length, l_height, l_width);
    pRenderer->ImmediateVertex(l_length, l_height, l_width);

    pRenderer->DisableImmediateMode();
    pRenderer->PopMatrix();
}

int main() {
    Renderer renderer;
    RenderSingleCube(&renderer);
    return 0;
}
