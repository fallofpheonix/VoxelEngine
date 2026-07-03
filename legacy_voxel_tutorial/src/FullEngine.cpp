#include <iostream>
#include <vector>
#include <cmath>
#include <map>

// ============================================================================
// STEP 02 & 08: BLOCK DATA & TYPES
// ============================================================================
enum BlockType {
    BlockType_Air = 0,
    BlockType_Grass,
    BlockType_Dirt,
    BlockType_Stone,
    BlockType_Wood,
    BlockType_Sand,
    BlockType_NumTypes
};

class Block {
public:
    Block() : m_active(false), m_type(BlockType_Air) {}
    bool IsActive() const { return m_active; }
    void SetActive(bool active) { m_active = active; }
    BlockType GetType() const { return m_type; }
    void SetType(BlockType type) { m_type = type; }
private:
    bool m_active;
    BlockType m_type;
};

// Simple Vector3 helper
struct Vector3 {
    float x, y, z;
    Vector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
    Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
    Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }
    Vector3 operator*(float s) const { return Vector3(x * s, y * s, z * s); }
};

// ============================================================================
// STEP 09: FRUSTUM CULLING (MOCK)
// ============================================================================
class Frustum {
public:
    bool CubeInFrustum(Vector3 center, float size) {
        // Mock check: in a real engine, this checks planes
        return true; 
    }
};

// ============================================================================
// STEP 03 & 04 & 06: CHUNK & MESH OPTIMIZATIONS
// ============================================================================
class Chunk {
public:
    static const int CHUNK_SIZE = 16;
    
    Chunk(int x, int y, int z) : m_pos(x, y, z), m_meshID(-1), m_empty(true) {
        // Step 07: Landscape/Sphere Generation
        SetupLandscape();
    }

    void SetupLandscape() {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                // Procedural Height
                int worldX = x + m_pos.x * CHUNK_SIZE;
                int worldZ = z + m_pos.z * CHUNK_SIZE;
                float height = 5.0f + 3.0f * sin(worldX * 0.1f) * cos(worldZ * 0.1f);
                
                for (int y = 0; y < CHUNK_SIZE; y++) {
                    int worldY = y + m_pos.y * CHUNK_SIZE;
                    if (worldY < height) {
                        m_blocks[x][y][z].SetActive(true);
                        m_blocks[x][y][z].SetType(worldY > height - 1 ? BlockType_Grass : BlockType_Dirt);
                        m_empty = false;
                    }
                }
            }
        }
    }

    // Step 06: Optimization - Only create faces that are visible
    void CreateMesh() {
        if (m_empty) return;

        int facesCreated = 0;
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    if (!m_blocks[x][y][z].IsActive()) continue;

                    // Check neighbors (Occlusion Culling)
                    bool xNeg = (x > 0) ? m_blocks[x-1][y][z].IsActive() : false;
                    bool xPos = (x < CHUNK_SIZE-1) ? m_blocks[x+1][y][z].IsActive() : false;
                    bool yNeg = (y > 0) ? m_blocks[x][y-1][z].IsActive() : false;
                    bool yPos = (y < CHUNK_SIZE-1) ? m_blocks[x][y+1][z].IsActive() : false;
                    bool zNeg = (z > 0) ? m_blocks[x][y][z-1].IsActive() : false;
                    bool zPos = (z < CHUNK_SIZE-1) ? m_blocks[x][y][z+1].IsActive() : false;

                    if (!xNeg) facesCreated++;
                    if (!xPos) facesCreated++;
                    if (!yNeg) facesCreated++;
                    if (!yPos) facesCreated++;
                    if (!zNeg) facesCreated++;
                    if (!zPos) facesCreated++;
                }
            }
        }
        m_meshID = 1; // Simulated ID
        // std::cout << "Chunk [" << m_pos.x << "," << m_pos.y << "," << m_pos.z << "] Mesh Created: " << facesCreated << " faces.\n";
    }

    bool IsEmpty() const { return m_empty; }
    Vector3 GetPosition() const { return m_pos; }
    Block* GetBlock(int x, int y, int z) { 
        if(x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) return nullptr;
        return &m_blocks[x][y][z]; 
    }

private:
    Block m_blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
    Vector3 m_pos;
    int m_meshID;
    bool m_empty;
};

// ============================================================================
// STEP 05: CHUNK MANAGEMENT
// ============================================================================
class ChunkManager {
public:
    void AddChunk(int x, int y, int z) {
        m_chunks[getIdx(x,y,z)] = new Chunk(x, y, z);
    }

    void Update(Vector3 playerPos, Frustum& frustum) {
        m_renderList.clear();
        for (auto const& [key, chunk] : m_chunks) {
            if (chunk->IsEmpty()) continue;
            
            // Step 09: Frustum Culling
            if (frustum.CubeInFrustum(chunk->GetPosition() * 16.0f, 16.0f)) {
                m_renderList.push_back(chunk);
            }
        }
    }

    void Render() {
        for (auto chunk : m_renderList) {
            // chunk->Render();
        }
        std::cout << "Rendering " << m_renderList.size() << " chunks.\n";
    }

    Block* GetBlockAtWorld(int x, int y, int z) {
        int cx = x / 16; int cy = y / 16; int cz = z / 16;
        int bx = x % 16; int by = y % 16; int bz = z % 16;
        if (m_chunks.count(getIdx(cx, cy, cz))) {
            return m_chunks[getIdx(cx, cy, cz)]->GetBlock(bx, by, bz);
        }
        return nullptr;
    }

private:
    std::map<long long, Chunk*> m_chunks;
    std::vector<Chunk*> m_renderList;

    long long getIdx(int x, int y, int z) {
        return ((long long)x << 40) | ((long long)y << 20) | (long long)z;
    }
};

// ============================================================================
// STEP 10: COLLISION DETECTION
// ============================================================================
class Player {
public:
    Vector3 pos;
    Player() : pos(16, 20, 16) {}

    void Update(ChunkManager& cm) {
        // Simple Gravity & Collision
        Vector3 nextPos = pos - Vector3(0, 0.5f, 0); // Falling
        
        // Transform to voxel space
        int vx = (int)nextPos.x;
        int vy = (int)nextPos.y;
        int vz = (int)nextPos.z;

        Block* b = cm.GetBlockAtWorld(vx, vy, vz);
        if (b && b->IsActive()) {
            // Collision! Stop falling
            // std::cout << "Player Collided at (" << vx << "," << vy << "," << vz << ")\n";
        } else {
            pos = nextPos;
        }
    }
};

// ============================================================================
// MAIN LOOP (INTEGRATION)
// ============================================================================
int main() {
    std::cout << "--- VOXEL ENGINE: STEPS 01-10 ---" << std::endl;

    ChunkManager chunkManager;
    Frustum frustum;
    Player player;

    // Step 05: Setup Initial Chunks
    std::cout << "Initializing 4x4 Chunk Grid..." << std::endl;
    for(int x=0; x<4; x++) {
        for(int z=0; z<4; z++) {
            chunkManager.AddChunk(x, 0, z);
        }
    }

    // Step 04: Build Meshes (Batching)
    // In a real engine this would be async
    std::cout << "Building Meshes..." << std::endl;

    // Simulation Loop
    for (int frame = 0; frame < 5; frame++) {
        std::cout << "\n[Frame " << frame << "]" << std::endl;
        
        // Step 05 & 09: Update Management & Culling
        chunkManager.Update(player.pos, frustum);
        
        // Step 10: Collision Detection
        player.Update(chunkManager);
        std::cout << "Player Position: (" << player.pos.x << ", " << player.pos.y << ", " << player.pos.z << ")" << std::endl;

        // Render
        chunkManager.Render();
    }

    std::cout << "\nEngine Complete up to Step 10." << std::endl;
    return 0;
}
