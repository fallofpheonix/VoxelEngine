Now that we have got a lot of the theory out of the way, we can start to have some fun with our voxel engine. One of the best ways to try out your new voxel engine functionality is to get it to load a world that will actually look like a proper landscape or whatever configuration you desire. I will explain how to achieve this in this article.

## 

Chunk Setup

By now we have a pretty good understanding of the way a chunk is setup and the loading/setup code. Once we have the data loaded to hold all our blocks, all we have to do during setup is turn some voxels on/off (active/inactive)... simple! Depending on which blocks we turn on or off during setup will change how the chunk is displayed when we are rendering our scene.

We have already seen how to setup a completely solid cube arrangement, this is simple... we turn turn every single block on during setup. To create a more complex example, such as a voxel sphere, we can use the following setup code:

```
void Chunk::Setup_Sphere() {
```

```
  for (int z = 0; z < CHUNK_SIZE; z++) {
```

```
    for (int y = 0; y < CHUNK_SIZE; y++) {
```

```
      for (int x = 0; x < CHUNK_SIZE; x++) {
```

```
        if (sqrt((float)(x - CHUNK_SIZE / 2) * (x - CHUNK_SIZE / 2) + (y - CHUNK_SIZE / 2) * (y - CHUNK_SIZE / 2) + (z - CHUNK_SIZE / 2) * (z - CHUNK_SIZE / 2)) <= CHUNK_SIZE / 2) {
```

```
          m_blocks[x][y][z].SetActive(true);
```

```
          m_blocks[x][y][z].SetBlockType(BlockType_Grass);
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

```
}
```

This will give us a spherical chunk, that is rendered something like this:

![](https://lh3.googleusercontent.com/sitesv/AA5AbUAIXFJjkXJzSRiPvU2xsvb8pWsYKoYfErVKQ4571ZjgjooB2c4q-cnc3mCFQx3iV3bQM2l5hwLnJPzU5XHQVFGdoLuZFLUFxoTWG7R2tsAMs2LbeBTrVddCMLGnIPDiZlnf7fr085ttWOrhp4xlyUOkKk6vJJAXd2nYdD0dCohn47XmkHb-8bVT-nt4G2maaZhrGKSlXc_qAXNUFEI=w1280)

## 

Landscape

Now that we have got our hands dirty with chunk setup, we have opened up a whole new possibility to the configurations we can create, just by swithcing blocks on or off. The next step would be to use some kind of terrain algorithm or height map to create a nice looking terrain. Personally for this I use the noise library **libnoise**. This can be found at: [http://libnoise.sourceforge.net/](http://www.google.com/url?q=http%3A%2F%2Flibnoise.sourceforge.net%2F&sa=D&sntz=1&usg=AOvVaw3POE_I_A1q4fvRc_ZnaSPR) There are some amazing tutorials and guides on that site explaining how to use noise to generate terrain configurations and also lots of other resources about noise generation. I am not going to go into all the small details about noise, but for the moment let us assume that we have a way of returning a height value for a specific x,y co-ordinate. This could quite easily come from a height map texture where the grayscale of the texture represents the height.

We can use the following code to create a landscape setup:

```
void Chunk::Setup_Landscape() {
```

```
  Texture * heightTexture = m_pRenderer -> GetTexture(m_pChunkManager -> GetHeightMapTexture());
```

```
  for (int x = 0; x < CHUNK_SIZE; x++) {
```

```
    for (int z = 0; z < CHUNK_SIZE; z++) { // Use the noise library to get the height value of x, z             
```

```
      //float height = m_pChunkManager->GetNoiseValue(x, z);              
```

```
      // Use the height map texture to get the height value of x, z  
```

```
      float height = (heightTexture -> GetHeightMap(x, z) * (CHUNK_SIZE - 1) * 1.0 f) * 1.0 f;
```

```
      for (int y = 0; y < height; y++) {
```

```
        m_pBlocks[x][y][z].SetActive(true);
```

```
        m_pBlocks[x][y][z].SetBlockType(BlockType_Grass);
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

As you can see I have provided two different ways of obtaining a height value, just to highlight that it doesn't matter how we obtain a height value, we still do the same afterwards.

What is important to note is for this setup algorithm, we only iterate across the x and z planes of the chunk, once we obtain a height value for each x, z co-ordinate we just loop up from the ground and turn every voxel up until the height value on. This means we will have generated a solid terrain that represents the height map or noise function.

Here is a screenshot of the output rendered scene:

![](https://lh3.googleusercontent.com/sitesv/AA5AbUAucRTeGEevg3ThqO6K4X3QfLxXLoQluC5HoLALJXMwwfgYTsW8AGcLTQwqOnxxw_tH7x7Fyz_2TeZf-tNtdNd8HuuZOBA5098oBklulK8TJq30sKmboMFwBroceoxzVM8iES7qAW1SlDQvgdGTqkCKJUkwqm_9qiP3ePjUQTt1UO-aHeOlr8ARCmHUEl--cZSuIGvZPmiQWuOD9Rw=w1280)

You can now see how our voxel engine is starting to look really nice, and it we wanted to we could code some sort of camera/player controls and play around in the engine. This would already be a fun little engine to run around as a player and do some interesting voxel manipulations.

## 

Multi-Chunk Setup

The next stage in our chunk setup is to investigate multi-chunk setup. So far we have only just been concerned with how an individual chunk setups up it's own voxel configuraiont. This is fine if your engine only contains 1 chunk, or if you have a voxel requirement that each chunk is setup individually, but when doing landscape generation you really want chunks to seamlessly link to one another. By that I mean the landscape generation should care that there are seperate chunks and boundaries between them. To the world generation alrogithm it should just take a chunk and voxel position as input and output a correct configuration for the landscape.

This is where the noise library has one major advantage over using a texture height map.... If we were to use texture height maps to generate our landscape for ALL the chunks in the voxel engine, we would have to have a different texture for each chunk, and even worse than that, we would have to ensure that the boundaries between the textures lined up, else our terrain would look wrong. For example, as a visual demonstration, if we just setup each chunk in our voxel engine to use the same texture height map, we would end up with something like this:

![](https://lh3.googleusercontent.com/sitesv/AA5AbUDmEeWWRSJkywR7TZOS5fuAZkZkGFBW_ZPHlmpdSYqJw8lUWn2WHR5urwF9XqxdQjiNJviFCj80-aF_3yWKOBekMlAXahMmkhlLjoqQE0QTk2PyycgkmKj_r8iKEAlb5u2H-pdGxc6jPWqUWhyLqcqSs5psAc_Yln1RYnNTgpUXtB42yXxAqaVwL6snoMskZjJBga2g5ibCwTHs=w1280)

As you can see, the chunk boundaries do not line up and the terrain looks very strange. This would still happen if we used different texture height maps for each chunk *unless* we ensured that the textures lined up (We dont want to do this!) Luckily for us, if we use the noise library there are handy ways to generate seamless terrain data given a discrete range, and then when we want to move onto a chunk neighbour, we can just move the range and be sure that the boundaries will line up seamlessly. (For reference see this article: [http://libnoise.sourceforge.net/tutorials/tutorial3.html](http://www.google.com/url?q=http%3A%2F%2Flibnoise.sourceforge.net%2Ftutorials%2Ftutorial3.html&sa=D&sntz=1&usg=AOvVaw0NOQQc6XM7prh7tpaaCpio))

Here is a progress video that I posted on YouTube when I reached this milestone, it shows off the landscape generation feature:

The [next article](https://sites.google.com/site/letsmakeavoxelengine/home/block-type-textures-and-colours?authuser=0) is going to focus on an important question with regards to rendering a voxel engine... 'Should I use textures or not?'