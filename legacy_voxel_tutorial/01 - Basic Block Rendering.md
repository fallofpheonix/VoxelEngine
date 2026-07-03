The very first step that you should aim for when creating a voxel engine is to create the framework for block rendering. This might seem very basic for an advanced 3D programmer, but it is important to get into the voxel mindset and start with something basic. Once we have laid the foundations for a voxel engine, then we will move onto more advanced topics and improve upon what we initially write.

## A Single Cube

Let's first render a singular cube:

```
float l_length = 1.0 f;
```

```
float l_height = 1.0 f;
```

```
float l_width = 1.0 f;
```

```
Vector3d blockPosition(0.0 f, 0.0 f, 0.0 f);
```

```
pRenderer->PushMatrix();
```

```
 pRenderer->TranslateWorldMatrix(blockPosition.x, blockPosition.y, blockPosition.z);
```

```
 pRenderer->SetRenderMode(RM_SOLID);
```

```
 pRenderer->EnableImmediateMode(IM_QUADS);
```

```
 pRenderer->ImmediateColourAlpha(1.0 f, 1.0 f, 1.0 f, 1.0 f);
```

```
 pRenderer->ImmediateNormal(0.0 f, 0.0 f, -1.0 f);
```

```
 pRenderer->ImmediateVertex(l_length, -l_height, -l_width);
```

```
 pRenderer->ImmediateVertex(-l_length, -l_height, -l_width);
```

```
 pRenderer->ImmediateVertex(-l_length, l_height, -l_width);
```

```
 pRenderer->ImmediateVertex(l_length, l_height, -l_width);
```

```
 pRenderer->ImmediateNormal(0.0 f, 0.0 f, 1.0 f);
```

```
 pRenderer->ImmediateVertex(-l_length, -l_height, l_width);
```

```
 pRenderer->ImmediateVertex(l_length, -l_height, l_width);
```

```
 pRenderer->ImmediateVertex(l_length, l_height, l_width);
```

```
 pRenderer->ImmediateVertex(-l_length, l_height, l_width);
```

```
 pRenderer->ImmediateNormal(1.0 f, 0.0 f, 0.0 f);
```

```
 pRenderer->ImmediateVertex(l_length, -l_height, l_width);
```

```
 pRenderer->ImmediateVertex(l_length, -l_height, -l_width);
```

```
 pRenderer->ImmediateVertex(l_length, l_height, -l_width);
```

```
 pRenderer->ImmediateVertex(l_length, l_height, l_width);
```

```
 pRenderer->ImmediateNormal(-1.0 f, 0.0 f, 0.0 f);
```

```
 pRenderer->ImmediateVertex(-l_length, -l_height, -l_width);
```

```
 pRenderer->ImmediateVertex(-l_length, -l_height, l_width);
```

```
 pRenderer->ImmediateVertex(-l_length, l_height, l_width);
```

```
 pRenderer->ImmediateVertex(-l_length, l_height, -l_width);
```

```
 pRenderer->ImmediateNormal(0.0 f, -1.0 f, 0.0 f);
```

```
 pRenderer->ImmediateVertex(-l_length, -l_height, -l_width);
```

```
 pRenderer->ImmediateVertex(l_length, -l_height, -l_width);
```

```
 pRenderer->ImmediateVertex(l_length, -l_height, l_width);
```

```
 pRenderer->ImmediateVertex(-l_length, -l_height, l_width);
```

```
 pRenderer->ImmediateNormal(0.0 f, 1.0 f, 0.0 f);
```

```
 pRenderer->ImmediateVertex(l_length, l_height, -l_width);
```

```
 pRenderer->ImmediateVertex(-l_length, l_height, -l_width);
```

```
 pRenderer->ImmediateVertex(-l_length, l_height, l_width);
```

```
 pRenderer->ImmediateVertex(l_length, l_height, l_width);
```

```
 pRenderer->DisableImmediateMode();
```

```
pRenderer->PopMatrix();
```

  

![](https://lh3.googleusercontent.com/sitesv/AA5AbUA39MRjhBvjqWfPXJljCxtXQ8fKIzCqhS5Cleh7WTMJANVYxABJzfL_L_s5MbtDzhJscN3do98J3eeWJHX8p3MFE_B3vKc72D50S6Dq4Ivk4iL9bh6h-Fpyd8vkVMoqgcX_EoPw96xxjHLRC9L2HoOvkpfsSF8uYrJ-tAlUBZgmKArQMX0-Bi5ZsFV5w3taBTl3MhB4Ga8Il18NK00=w1280)

Now as you can see, this is a simple QUAD cube rendered using immediate mode, later on we will change to using vertex buffers and also triangulate the cube, but for the moment this seems like a nice easy start.

Putting this into your 3D application and running it should just simple display a single cube, what makes a voxel engine truely amazing is that it will be displaying millions of these cubes in the final engine and our whole world will be made up of singlular cubed voxels.

## Multiple Cubes

Now we can move onto creating multiple cubes for our renderer. Essentially a voxel engine is just a really big list of cubes that get rendered to the screen each frame. Depending on which cubes are 'on' or 'off' is what makes our voxel engine look like an expansive world and allows for detailed looking structures.

If you wanted you could just have a block structure with a position component in it and a simple array that holds all the blocks of your world. Then when you come to the rendering loop you just iterate over all your blocks and render each one at it's position. I don't like to store the position data within a block though, since this will later become very cumbersome when dealing with the storage of block information (we want to store as little information for each block as possible). So what I propose is that we store the blocks in a multidimensional array and infer the block position from its index within the array.

For example we could have the blocks stored as:

```
Block* m_pBlocks[32][32][32]
```

This would then allow us to index each block using an x, y, z coordinate and working out where to draw each block as follows:

```
for (int x = 0; x < 32; x++) {
```

```
  for (int y = 0; y < 32; y++) {
```

```
  for (int z = 0; z < 32; z++) {
```

```
   pRenderer->TranslateWorldMatrix(x, y, z); // Render m_pBlocks[x][y][z]
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

Using this approach we can render a 1x1x1 sized block at each position as we iterate through each of the loops for x, y, and z. Also if we want to have a modifiable block size we can do this and then just work out the modified position, like:

```
TranslateWorldMatrix(x*Block::BLOCK_RENDER_SIZE, y*Block::BLOCK_RENDER_SIZE, z*Block::BLOCK_RENDER_SIZE);
```

Now you should have something that renders like the following:

![](https://lh3.googleusercontent.com/sitesv/AA5AbUCMVdejExb7H0aLJtvb_EU2VIuQ_AgrKZFJhprNz29Qgu2F3yNZtQ5DB4VDY5zfEu8ZQZHpe6jB_L5J0Nv7Twjzy6XNrjjzt7appmTszbrxMkeoNw2p1U3QmlN96uVzLK0ucCY4TvqMAzwGdERE5ilZxbh-j9f3YQyE5-H0sB9griCs6ukwFJnbQlYgCgbG7pmQ6RPbZs8MZSiDrs8=w1280)

As you can see, the rendering of voxels within our engine is very simple to start with and using this approach we are quickly able to render a number of voxels onto the screen. In the later guides I will explain about the management of voxel information and describe some of the **Do's and Don'ts** with regards to how we store our block information and manage large number of blocks concurrently.

The [next article](https://sites.google.com/site/letsmakeavoxelengine/home/block-data-structure?authuser=0) will describe how we should store our block information and what data we need to save/load for each voxel.