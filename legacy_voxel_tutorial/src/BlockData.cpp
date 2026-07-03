#include <iostream>
#include <vector>

// Define the available block types
enum BlockType {
    BlockType_Default = 0,
    BlockType_Grass,
    BlockType_Dirt,
    BlockType_Water,
    BlockType_Stone,
    BlockType_Wood,
    BlockType_Sand,
    BlockType_NumTypes,
};

class Block {
public:
    Block() : m_active(true), m_blockType(BlockType_Default) {}
    ~Block() {}

    bool IsActive() { return m_active; }
    void SetActive(bool active) { m_active = active; }

    BlockType GetType() { return m_blockType; }
    void SetType(BlockType type) { m_blockType = type; }

private:
    bool m_active;
    BlockType m_blockType;
};

// Mock Renderer to simulate procedural rendering based on block type
class Renderer {
public:
    void SetColorForType(BlockType type) {
        switch (type) {
            case BlockType_Grass: std::cout << "[Color: Green] "; break;
            case BlockType_Dirt:  std::cout << "[Color: Brown] "; break;
            case BlockType_Stone: std::cout << "[Color: Gray]  "; break;
            case BlockType_Water: std::cout << "[Color: Blue]  "; break;
            case BlockType_Wood:  std::cout << "[Color: DarkBrown] "; break;
            case BlockType_Sand:  std::cout << "[Color: Yellow] "; break;
            default:             std::cout << "[Color: White] "; break;
        }
    }
    
    void RenderCubeAt(int x, int y, int z) {
        std::cout << "Rendering Block at (" << x << "," << y << "," << z << ")\n";
    }
};

int main() {
    // 32x32x32 world
    static Block* world[32][32][32];

    // Initialize world with different types
    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 32; y++) {
            for (int z = 0; z < 32; z++) {
                if (y == 0) {
                    world[x][y][z] = new Block();
                    world[x][y][z]->SetType(BlockType_Stone); // Bedrock/Stone floor
                } else if (y < 3 && x < 10 && z < 10) {
                    world[x][y][z] = new Block();
                    world[x][y][z]->SetType(BlockType_Dirt);
                } else if (y == 3 && x < 10 && z < 10) {
                    world[x][y][z] = new Block();
                    world[x][y][z]->SetType(BlockType_Grass);
                } else {
                    world[x][y][z] = nullptr; // Air (inactive)
                }
            }
        }
    }

    Renderer renderer;
    std::cout << "Simulating Voxel World Rendering:\n";

    // Render loop
    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 32; y++) {
            for (int z = 0; z < 32; z++) {
                if (world[x][y][z] && world[x][y][z]->IsActive()) {
                    renderer.SetColorForType(world[x][y][z]->GetType());
                    renderer.RenderCubeAt(x, y, z);
                }
            }
        }
    }

    // Cleanup
    for (int x = 0; x < 32; x++)
        for (int y = 0; y < 32; y++)
            for (int z = 0; z < 32; z++)
                delete world[x][y][z];

    return 0;
}
