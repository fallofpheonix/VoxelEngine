If you have done any research into voxel engines previously, or looked into the current voxel games/engines that are developed, you will probably have heard the name '**chunk**' mentioned quite a lot. A chunk is nothing magical or clever really, it is just a simple way for us to think about voxel space arrangements and organisation. In this article we will explore what is meant by a 'chunk' and explain some of the common techniques for making our voxel engine more robust.

## Introduction

It is probably fairly obvious that what we talked about in the [first article](https://sites.google.com/site/letsmakeavoxelengine/home/basic-block-rendering?authuser=0) works fine for a simple example, but has some major problems when we come to scale up our example to work with the sorts of voxel numbers that will be required to make a large, expansive world. In reality we are going to want a voxel engine that can work infinitely in the X, Y and Z planes and just keeps going and going. The previous example would have real difficulty if we try to generate and render a voxel world of 256x256x256 voxels because we are essentially doing a render call for every single voxel that we want to draw, **this is bad!** A good voxel engine should try to minimize the number of draw calls made to the graphics card at all times. If we had some handy way to reduce the number of render calls, while at the same time having a large number of voxels rendered, this would satisfy our goal, this is where chunks come in handy!

A chunk is a way to bundle up a number of blocks together and then only make a single call to the renderer for each chunk, this drastically reduces the rendering overhead or trying to render each block separately. A good place to start is to have each chunk represent 16x16x16 blocks. This means that for a 256x256x256 block sized world, we would only have to render 4096 chunks, rather than 16,777,216 blocks, as we would have previously. The size of a good chunk is dependent on a number of factors and really depends on what sort of things you want to do with your voxel engine, so it is really down to an individual case by case basis. But having a good system to create and render chunks is vital to an efficient voxel engine.

There is a trade off though with using chunks and this comes from the dynamic nature of a voxel engine. Since we want the capacity to alter any individual voxel and turn it on/off as we please, we will need to rebuild the chunk render information each time a voxel contained in a chunk is modified. This is compounded if the modified voxel lies on the boundary between 2 chunks because then we will need to rebuild multiple chunk render data. Later on we will do some number crunching and analysis on the chunk size and performance testing, to try and figure out a good chunk size. But at this stage we can simplify the trade off as: **Larger chunk sizes = less rendering overhead**, but also **larger chunk sizes = slower rebuild times**.

## Chunk Class

Here is some code to show how a chunk class can be defined:

```
class Chunk {
```

```
public:
```

```
  Chunk();
```

```
  ~Chunk();
```

```
  void Update(float dt);
```

```
  void Render(OpenGLRenderer * pRenderer);
```

```
  static const int CHUNK_SIZE = 16;
```

```
private: // The blocks data
```

```
 Block * * * m_pBlocks;
```

```
};
```

  

```
Chunk::Chunk() { // Create the blocks
```

```
 m_pBlocks = new Block **[CHUNK_SIZE];
```

```
 for (int i = 0; i < CHUNK_SIZE; i++) {
```

```
  m_pBlocks[i] = new Block *[CHUNK_SIZE];
```

```
  for (int j = 0; j < CHUNK_SIZE; j++) {
```

```
   m_pBlocks[i][j] = new Block[CHUNK_SIZE];
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
Chunk::~Chunk() { // Delete the blocks
```

```
 for (int i = 0; i < CHUNK_SIZE; ++i) {
```

```
  for (int j = 0; j < CHUNK_SIZE; ++j) {
```

```
   delete[] m_pBlocks[i][j];
```

```
  }
```

```
  delete[] m_pBlocks[i];
```

```
 }
```

```
 delete[] m_pBlocks;
```

```
}
```

All this class does at the moment is create a 3 dimensional array containing the block data for each of our voxels. It isn't vital that the voxel data be stored in a 3d array, but it is useful if we want to easily index the block data. For example being able to quickly say get me the block in the chunk at location (10, 5, 7) and being able to look into the array as **m_pBlocks[10][5][7]** is very useful.

Sitting at a level above the chunk class will be a chunk manager class that contains a list of chunks, I will go into a little more detail about the chunk manager in a future article, but for now it is important to know that a chunk contains multiple blocks and our engine will contain multiple chunks. Now, when we want to render all the voxels in our world, we just call the render function for each chunk. So, how do we get each chunk to only do 1 render call to correspond to ALL of the voxels it contains? That is explained in the next article about [Display Lists and Vertex Buffers](https://sites.google.com/site/letsmakeavoxelengine/home/display-lists-or-vertex-buffers?authuser=0)...