## 

Introduction

In this article I am going to explain some of the chunk optimizations that can be applied during the load, update and render cycles of the voxel engine. Each optimizations that is implemented for a voxel engine is designed to fulfil a specific purpose, such as:

- Increase render time and frame rate performance.
- Make the chunk rebuild/re-create algorithm faster.
- Decrease the storage space required to store voxel information.
- Reduce memory footprint of voxel data in RAM.

The optimization goal that I am going to focus on here is the first one, to increase the render performance of our rendering. At the moment, we have made a vast improvement by using vertex buffers to render out voxels, rather than immediate mode. This means we have greatly reduced the overall render calls made to the renderer for large voxel world, but we are still rendering lots of redundant triangles for voxels that will never be seen by the player.

## 

Don’t Render Unseen Triangles

One easy optimization we can try is to stop creating triangles for voxels that are occluded by a neighbouring active voxel. This works at the vertex buffer load/create stage and is easy to implement. Before we call ‘CreateCube()’ for an active block, we check if the neighbouring blocks are also active. For any neighbour block that is active we don’t render the triangles that would be next to that neighbour. The neighbours are x-1, x+1, y-1, y+1, z-1 and z+1. The code that contains this optimization is as follows:

```
bool lDefault = true;
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
      if (m_pBlocks[x][y][z].IsActive() == false) {
```

```
        continue;
```

```
      }
```

```
      bool lXNegative = lDefault;
```

```
      if (x > 0) lXNegative = m_pBlocks[x - 1][y][z].IsActive();
```

```
      bool lXPositive = lDefault;
```

```
      if (x < CHUNK_SIZE - 1) lXPositive = m_pBlocks[x + 1][y][z].IsActive();
```

```
      bool lYNegative = lDefault;
```

```
      if (y > 0) lYNegative = m_pBlocks[x][y - 1][z].IsActive();
```

```
      bool lYPositive = lDefault;
```

```
      if (y < CHUNK_SIZE - 1) lYPositive = m_pBlocks[x][y + 1][z].IsActive();
```

```
      bool lZNegative = lDefault;
```

```
      if (z > 0) lZNegative = m_pBlocks[x][y][z - 1].IsActive();
```

```
      bool lZPositive = lDefault;
```

```
      if (z < CHUNK_SIZE - 1) lZPositive = m_pBlocks[x][y][z + 1].IsActive();
```

```
      CreateCube(lXNegative, lXPositive, lYNegative, lYPositive, lZNegative, lZPositive);
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

## 

Don’t Render Empty Chunks

Another simple chunk rendering optimization is to completely skip over the rendering of chunks that contain no active blocks. This is easy to accomplish and if you have a flag to represent an empty chunk then you can skip over it when looping through the chunk managers render list.

Afer we have loaded the block information for a chunk, we can call a function to update our render flags:

```
void Chunk::UpdateShouldRenderFlags() { // Figure out if we are a completely empty chunk     
```

```
  int numVerts;
```

```
  int numTriangles;
```

```
  m_pRenderer -> GetMeshInformation(m_meshID, & numVerts, & numTriangles);
```

```
  if (numVerts == 0 && numTriangles == 0) {
```

```
    m_emptyChunk = true;
```

```
  }
```

```
}
```

Then during the chunk managers render function:

```
void ChunkManager::Render(OpenGLRenderer * pRenderer, bool translateTexture) { // Render any chunks in the render list
```

```
  ChunkList::iterator iterator;
```

```
  for (iterator = m_vpChunkRenderList.begin(); iterator != m_vpChunkRenderList.end(); ++iterator) {
```

```
    Chunk * pChunk = ( * iterator);
```

```
    if (pChunk -> IsLoaded() && pChunk -> IsSetup()) {
```

```
      if (pChunk -> ShouldRender()) // Early flags check so we don't always have to do the frustum check...             
```

```
      {
```

```
        pChunk -> Render(pRenderer, translateTexture);
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
}
```

The ShouldRender() function for a chunk just returns whether or not a chunk should be rendered, depending on its properties. For the above situation, ShouldRender() will return false when we are a completely emtpy chunk. i.e. m_emptyChunk == true.

## 

Don’t Render Completely Surrounded Chunks

- **// TODO : Write me...**

## 

Triangle Face Merging

Another rendering optimization that you can do is merge triangle faces for neighbour blocks that share the same block type or block colour. This optimization is a little more involved because you have to come up with a looping algorithm that will check each block neighbour in sequence and merge any faces which share common properties. This can become quite complex depending on the type of algorithm you use. And in some cases you might not get the most optimum face merge output. This is an area for further investigation.

I will explain in psuedo code how I implemented **Triangle Face Merging** in my voxel engine:

- **// TODO : Write me...**

## 

Demonstration Video

Once you have implemented a few optimizations you should be able to increase your voxel world size to something fairly high to see how your performance is. Here is a video demonstrating some of the optimizations in my voxel engine.

There are probably many other optimizations and tricks you can come up with to improve the performance of your voxel engine. I have just suggested a few here that I use in my own engine and the ones described here work for me. What is important is to really understand how your voxel world is being created, rendered and updated and then you will find that you naturally see optimizations and come up with ways to improve your engine as you are working on it.

In the [next article](https://sites.google.com/site/letsmakeavoxelengine/home/landcape-creation?authuser=0) I will explain some of the fun ways we can create different voxel configurations to make actual worlds and use a landscape generation algorithm to make auto-generated terrain.