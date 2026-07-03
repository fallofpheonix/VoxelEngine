Previously I have mentioned that a good voxel engine will only do 1 render call for each chunk. This is vital if we want to achieve the sort of performance that is going to allow us to load and render millions of voxels in our voxel engine. This article will explain how we can achieve this.

## 

Introduction

If you have done any 3D programming previously, you will have heard of **vertex buffers** and **display lists**. They are used when we don't want to keep doing the same render functions over and over again each frame and also reducing the traffic sent from the CPU to the GPU for graphics processing.

I'm not going to go into massive depths about the technical details of what vertex buffers and display lists are (there is lots of information on the web about this already), but it's important to just know that they are used when we want to reduce the rendering overhead of our 3d application. I will just give you a quick rundown below of the 3 types of rendering modes we can use in our 3d application.

- **Immediate mode** is where we send render calls 1 by 1 to the graphics card every frame and the
- **Vertex buffers** allow us to group up our data into a big array and then send it all to the graphics card together (each frame).
- **Display lists** are special lists of render calls that are stored on the GPU itself and rendered each frame when the list is rendered, but we dont have to send this data to the GPU every frame.

OpenGL has some handy functions that allow us to render vertex buffers easily, glDrawElements(...)and glDrawArrays(...). These OpenGL calls are what will allow us to group together all our voxel information for a chunk and render it in a single GPU call. This will give us a vast improvement over rendering our voxel data in immediate mode.

## 

Mesh Creation

For the creation of a chunk using only 1 render call to draw all the blocks that we contain, you first have to generate the vertex buffer for the chunks. This is the code used to generate the vertex buffer:

```
void Chunk::CreateMesh() {
```

```
 m_pRenderer->CreateMesh(&m_meshID, OGLMeshType_Colour);
```

```
 for (int x = 0; x < CHUNK_SIZE; x++) {
```

```
  for (int y = 0; y < CHUNK_SIZE; y++) {
```

```
   for (int z = 0; z < CHUNK_SIZE; z++) {
```

```
    if (m_pBlocks[x][y][z].IsActive() == false) { // Don't create triangle data for inactive blocks
```

```
     continue;
```

```
    }
```

```
    CreateCube();
```

```
   }
```

```
  }
```

```
 }
```

```
 m_pRenderer->FinishMesh(m_meshID, -1, m_pChunkManager->GetMaterialID());
```

```
}
```

  

```
void Chunk::CreateCube() {
```

```
 Vector3d p1(x - Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
```

```
 Vector3d p2(x + Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
```

```
 Vector3d p3(x + Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
```

```
 Vector3d p4(x - Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
```

```
 Vector3d p5(x + Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
```

```
 Vector3d p6(x - Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
```

```
 Vector3d p7(x - Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
```

```
 Vector3d p8(x + Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
```

```
 Vector3d n1;
```

```
 unsigned int v1;
```

```
 unsigned int v2;
```

```
 unsigned int v3;
```

```
 unsigned int v4;
```

```
 unsigned int v5;
```

```
 unsigned int v6;
```

```
 unsigned int v7;
```

```
 unsigned int v8;
```

```
 float r = 1.0 f;
```

```
 float g = 1.0 f;
```

```
 float b = 1.0 f;
```

```
 float a = 1.0 f; // Front     
```

```
 n1 = Vector3d(0.0 f, 0.0 f, 1.0 f);
```

```
 v1 = m_pRenderer->AddVertexToMesh(m_meshID, p1, n1, r, g, b, a);
```

```
 v2 = m_pRenderer->AddVertexToMesh(m_meshID, p2, n1, r, g, b, a);
```

```
 v3 = m_pRenderer->AddVertexToMesh(m_meshID, p3, n1, r, g, b, a);
```

```
 v4 = m_pRenderer->AddVertexToMesh(m_meshID, p4, n1, r, g, b, a);
```

```
 m_pRenderer->AddTriangleToMesh(m_meshID, v1, v2, v3);
```

```
 m_pRenderer->AddTriangleToMesh(m_meshID, v1, v3, v4); // Back     
```

```
 n1 = Vector3d(0.0 f, 0.0 f, -1.0 f);
```

```
 v5 = m_pRenderer->AddVertexToMesh(m_meshID, p5, n1, r, g, b, a);
```

```
 v6 = m_pRenderer->AddVertexToMesh(m_meshID, p6, n1, r, g, b, a);
```

```
 v7 = m_pRenderer->AddVertexToMesh(m_meshID, p7, n1, r, g, b, a);
```

```
 v8 = m_pRenderer->AddVertexToMesh(m_meshID, p8, n1, r, g, b, a);
```

```
 m_pRenderer->AddTriangleToMesh(m_meshID, v5, v6, v7);
```

```
 m_pRenderer->AddTriangleToMesh(m_meshID, v5, v7, v8); // Right     
```

```
 n1 = Vector3d(1.0 f, 0.0 f, 0.0 f);
```

```
 v2 = m_pRenderer->AddVertexToMesh(m_meshID, p2, n1, r, g, b, a);
```

```
 v5 = m_pRenderer->AddVertexToMesh(m_meshID, p5, n1, r, g, b, a);
```

```
 v8 = m_pRenderer->AddVertexToMesh(m_meshID, p8, n1, r, g, b, a);
```

```
 v3 = m_pRenderer->AddVertexToMesh(m_meshID, p3, n1, r, g, b, a);
```

```
 m_pRenderer->AddTriangleToMesh(m_meshID, v2, v5, v8);
```

```
 m_pRenderer->AddTriangleToMesh(m_meshID, v2, v8, v3); // left     
```

```
 n1 = Vector3d(-1.0 f, 0.0 f, 0.0 f);
```

```
 v6 = m_pRenderer->AddVertexToMesh(m_meshID, p6, n1, r, g, b, a);
```

```
 v1 = m_pRenderer->AddVertexToMesh(m_meshID, p1, n1, r, g, b, a);
```

```
 v4 = m_pRenderer->AddVertexToMesh(m_meshID, p4, n1, r, g, b, a);
```

```
 v7 = m_pRenderer->AddVertexToMesh(m_meshID, p7, n1, r, g, b, a);
```

```
 m_pRenderer->AddTriangleToMesh(m_meshID, v6, v1, v4);
```

```
 m_pRenderer->AddTriangleToMesh(m_meshID, v6, v4, v7); // Top     
```

```
 n1 = Vector3d(0.0 f, 1.0 f, 0.0 f);
```

```
 v4 = m_pRenderer->AddVertexToMesh(m_meshID, p4, n1, r, g, b, a);
```

```
 v3 = m_pRenderer->AddVertexToMesh(m_meshID, p3, n1, r, g, b, a);
```

```
 v8 = m_pRenderer->AddVertexToMesh(m_meshID, p8, n1, r, g, b, a);
```

```
 v7 = m_pRenderer->AddVertexToMesh(m_meshID, p7, n1, r, g, b, a);
```

```
 m_pRenderer->AddTriangleToMesh(m_meshID, v4, v3, v8);
```

```
 m_pRenderer->AddTriangleToMesh(m_meshID, v4, v8, v7); // Bottom     
```

```
 n1 = Vector3d(0.0 f, -1.0 f, 0.0 f);
```

```
 v6 = m_pRenderer->AddVertexToMesh(m_meshID, p6, n1, r, g, b, a);
```

```
 v5 = m_pRenderer->AddVertexToMesh(m_meshID, p5, n1, r, g, b, a);
```

```
 v2 = m_pRenderer->AddVertexToMesh(m_meshID, p2, n1, r, g, b, a);
```

```
 v1 = m_pRenderer->AddVertexToMesh(m_meshID, p1, n1, r, g, b, a);
```

```
 m_pRenderer->AddTriangleToMesh(m_meshID, v6, v5, v2);
```

```
 m_pRenderer->AddTriangleToMesh(m_meshID, v6, v2, v1);
```

```
}
```

What the above code does: During chunk setup/load, a vertex buffer object is created in the renderer and then the chunk’s blocks are looped over and if the current block is active, triangle information for the block is added to the vertex buffer object. This means that when we do a call to render this vertex buffer object, what will be rendered to the screen are just the currently active blocks for the chunk:

## 

Mesh Rendering

```
void Chunk::Render(OpenGLRenderer * pRenderer, bool translateTexture)
```

```
{
```

```
  pRenderer -> PushMatrix();
```

```
  pRenderer -> ImmediateColourAlpha(1.0 f, 1.0 f, 1.0 f, 1.0 f);
```

```
  pRenderer -> SetRenderMode(RM_TEXTURED_LIGHTING);
```

```
  float x = m_position.x;
```

```
  float y = m_position.y;
```

```
  float z = m_position.z;
```

```
  pRenderer -> TranslateWorldMatrix(x, y, z);
```

```
  if (m_meshID != -1) {
```

```
    pRenderer -> RenderMesh(m_meshID);
```

```
  }
```

```
  pRenderer -> PopMatrix();
```

```
}
```

If you have been following along up to this point then you will probably be aware that every time we modify a block within this chunk. (turn off/on a voxel) then we will need to re-create the vertex buffer object, since the triangle information that we need to render will have changed. This is the reason why we need to choose our block size within a chunk sensibly. A chunk size too large and this rebuilding will take too long. This is the big trade-off between chunk rendering speed and rebuild time.

The above code is the very simplest form of creating a vertex buffer to display our voxel information, there are MANY different optimizations we can make to remove redundant triangle information and speed up the rendering of the chunk. But essentially this is the root of the chunk rendering routine.

In future articles I will explain some of the optimizations and tricks that can be applied to get further speed and performance increases from the chunk rendering. The [next article](https://sites.google.com/site/letsmakeavoxelengine/home/chunk-management?authuser=0) is about Chunk management...