#include <iostream>

// Re-using the Block structure from before
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
    bool IsActive() { return m_active; }
    void SetActive(bool active) { m_active = active; }
    BlockType GetType() { return m_blockType; }
    void SetType(BlockType type) { m_blockType = type; }
private:
    bool m_active;
    BlockType m_blockType;
};

// The Chunk Class
class Chunk {
public:
    static const int CHUNK_SIZE = 16;

    Chunk() {
        // Allocate the 3D array of blocks
        m_pBlocks = new Block**[CHUNK_SIZE];
        for (int i = 0; i < CHUNK_SIZE; i++) {
            m_pBlocks[i] = new Block*[CHUNK_SIZE];
            for (int j = 0; j < CHUNK_SIZE; j++) {
                m_pBlocks[i][j] = new Block[CHUNK_SIZE];
            }
        }
        std::cout << "Chunk created: " << CHUNK_SIZE << "^3 blocks initialized.\n";
    }

    ~Chunk() {
        // Clean up the 3D array
        for (int i = 0; i < CHUNK_SIZE; ++i) {
            for (int j = 0; j < CHUNK_SIZE; ++j) {
                delete[] m_pBlocks[i][j];
            }
            delete[] m_pBlocks[i];
        }
        delete[] m_pBlocks;
        std::cout << "Chunk destroyed and memory freed.\n";
    }

    void Update(float dt) {
        // Logic for block updates (gravity, water flow, etc.)
    }

    void Render() {
        // In the next tutorial, this will use Vertex Buffers
        // For now, it represents a single unit of rendering
        std::cout << "Rendering Chunk as a single batch...\n";
    }

private:
    Block*** m_pBlocks;
};

int main() {
    std::cout << "Initializing Voxel Engine with Chunks...\n";
    
    // Create a single chunk for demonstration
    Chunk* myChunk = new Chunk();
    
    myChunk->Render();
    
    delete myChunk;
    
    return 0;
}
