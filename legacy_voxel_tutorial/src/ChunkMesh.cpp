#include <iostream>
#include <vector>

// Mock Vector3d
struct Vector3d {
    float x, y, z;
    Vector3d() : x(0), y(0), z(0) {}
    Vector3d(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

// Mock Block
class Block {
public:
    static const int BLOCK_RENDER_SIZE = 1;
    Block() : m_active(true) {}
    bool IsActive() { return m_active; }
    void SetActive(bool active) { m_active = active; }
private:
    bool m_active;
};

// Mock Renderer with Mesh Support
class Renderer {
public:
    int CreateMesh() {
        static int idCounter = 0;
        std::cout << "GPU: Created new Vertex Buffer Object (ID: " << ++idCounter << ")\n";
        return idCounter;
    }

    unsigned int AddVertexToMesh(int meshID, Vector3d p, Vector3d n, float r, float g, float b, float a) {
        // In reality, this appends data to a CPU-side array
        return 0; // Returning mock vertex index
    }

    void AddTriangleToMesh(int meshID, unsigned int v1, unsigned int v2, unsigned int v3) {
        // In reality, this appends indices to an index buffer
    }

    void FinishMesh(int meshID) {
        std::cout << "GPU: Uploading Mesh ID " << meshID << " to VRAM.\n";
    }

    void RenderMesh(int meshID) {
        std::cout << "GPU: glDrawElements(MeshID: " << meshID << ") - One draw call for the whole chunk!\n";
    }

    void PushMatrix() {}
    void PopMatrix() {}
    void TranslateWorldMatrix(float x, float y, float z) {}
};

class Chunk {
public:
    static const int CHUNK_SIZE = 16;

    Chunk(Renderer* renderer, Vector3d pos) : m_pRenderer(renderer), m_position(pos), m_meshID(-1) {
        m_pBlocks = new Block**[CHUNK_SIZE];
        for (int i = 0; i < CHUNK_SIZE; i++) {
            m_pBlocks[i] = new Block*[CHUNK_SIZE];
            for (int j = 0; j < CHUNK_SIZE; j++) {
                m_pBlocks[i][j] = new Block[CHUNK_SIZE];
            }
        }
    }

    ~Chunk() {
        for (int i = 0; i < CHUNK_SIZE; ++i) {
            for (int j = 0; j < CHUNK_SIZE; ++j) {
                delete[] m_pBlocks[i][j];
            }
            delete[] m_pBlocks[i];
        }
        delete[] m_pBlocks;
    }

    void CreateMesh() {
        m_meshID = m_pRenderer->CreateMesh();
        
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    if (m_pBlocks[x][y][z].IsActive()) {
                        CreateCubeMesh(x, y, z);
                    }
                }
            }
        }
        m_pRenderer->FinishMesh(m_meshID);
    }

    void CreateCubeMesh(int x, int y, int z) {
        float size = Block::BLOCK_RENDER_SIZE;
        // Vertices (simplified for demonstration)
        Vector3d p1(x - size, y - size, z + size);
        Vector3d p2(x + size, y - size, z + size);
        // ... (etc for all 8 points)
        
        // Normals, Colors, etc.
        Vector3d n1(0, 0, 1);
        
        // In a real implementation, we'd add all 24 vertices and 12 triangles per cube
        unsigned int v1 = m_pRenderer->AddVertexToMesh(m_meshID, p1, n1, 1, 1, 1, 1);
        unsigned int v2 = m_pRenderer->AddVertexToMesh(m_meshID, p2, n1, 1, 1, 1, 1);
        // ...
        m_pRenderer->AddTriangleToMesh(m_meshID, v1, v2, 0); 
    }

    void Render() {
        m_pRenderer->PushMatrix();
        m_pRenderer->TranslateWorldMatrix(m_position.x, m_position.y, m_position.z);
        if (m_meshID != -1) {
            m_pRenderer->RenderMesh(m_meshID);
        }
        m_pRenderer->PopMatrix();
    }

private:
    Block*** m_pBlocks;
    Renderer* m_pRenderer;
    Vector3d m_position;
    int m_meshID;
};

int main() {
    Renderer renderer;
    Chunk chunk(&renderer, Vector3d(0, 0, 0));

    std::cout << "--- Generating Chunk Mesh ---\n";
    chunk.CreateMesh();

    std::cout << "\n--- Rendering Frame ---\n";
    chunk.Render();

    return 0;
}
