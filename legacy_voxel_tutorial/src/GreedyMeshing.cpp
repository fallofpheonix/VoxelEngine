#include <iostream>
#include <vector>

// ============================================================================
// GREEDY MESHING EXPLANATION
// ============================================================================
/*
  Greedy Meshing is an optimization that combines multiple adjacent block faces
  into a single large "quad". 
  
  In our previous implementation:
  A 16x16 floor of grass = 256 quads (1,024 vertices).
  
  With Greedy Meshing:
  A 16x16 floor of grass = 1 quad (4 vertices).
  
  This reduces the workload on the GPU by orders of magnitude.
*/

struct Block {
    bool active;
    int type;
};

const int CHUNK_SIZE = 16;

struct Quad {
    int x, y, z;
    int w, h;
    int direction; // 0=X, 1=Y, 2=Z
};

void GenerateGreedyMesh(Block blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE]) {
    int originalQuads = 0;
    int greedyQuads = 0;

    // We'll simulate the reduction for one face (e.g., the Top face / Y+)
    bool mask[CHUNK_SIZE][CHUNK_SIZE];
    
    // For each Y slice
    for (int y = 0; y < CHUNK_SIZE; y++) {
        // 1. Create a mask of active faces for this slice
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                // If block is active and has no block above it, it's a visible top face
                bool topVisible = blocks[x][y][z].active && 
                                 (y == CHUNK_SIZE - 1 || !blocks[x][y+1][z].active);
                mask[x][z] = topVisible;
                if (topVisible) originalQuads++;
            }
        }

        // 2. Greedy Mesh the mask
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                if (mask[x][z]) {
                    // We found a face, now expand it as much as possible
                    int width, height;

                    // Expand Width (X)
                    for (width = 1; x + width < CHUNK_SIZE && mask[x + width][z]; width++);

                    // Expand Height (Z)
                    bool done = false;
                    for (height = 1; z + height < CHUNK_SIZE; height++) {
                        for (int k = 0; k < width; k++) {
                            if (!mask[x + k][z + height]) {
                                done = true;
                                break;
                            }
                        }
                        if (done) break;
                    }

                    // We have a quad of [width x height]
                    greedyQuads++;

                    // Mark these faces as visited in the mask
                    for (int h = 0; h < height; h++) {
                        for (int w = 0; w < width; w++) {
                            mask[x + w][z + h] = false;
                        }
                    }
                }
            }
        }
    }

    std::cout << "--- GREEDY MESHING RESULTS (Top Faces Only) ---" << std::endl;
    std::cout << "Original Face Count: " << originalQuads << std::endl;
    std::cout << "Greedy Quad Count:   " << greedyQuads << std::endl;
    float reduction = (1.0f - (float)greedyQuads / originalQuads) * 100.0f;
    std::cout << "Efficiency Gain:     " << reduction << "% reduction in geometry!" << std::endl;
}

int main() {
    Block blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

    // Create a flat 16x16 grass floor
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                blocks[x][y][z].active = (y == 0); // Only bottom layer is active
                blocks[x][y][z].type = 1;
            }
        }
    }

    GenerateGreedyMesh(blocks);

    // Create a more complex 5x5 pillar
    std::cout << "\nTesting complex shape (5x5 pillar)..." << std::endl;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                blocks[x][y][z].active = (x < 5 && z < 5); 
            }
        }
    }
    GenerateGreedyMesh(blocks);

    return 0;
}
