Before we go any further it is probably best to define the **block** class.

## Definition

A block is a voxel and is a 3d cube. This is what makes up the bulk of a voxel engine and can be though of as the base data class that our engine will contain. When we load the voxel data for our world we are going to be loading and creating lots of blocks, when we save we will need some way to save information for each block in the world. Since each and every voxel is unique and can be dynamically altered by the player, each block needs to store its own data. Later on we will find that there are some very clever techniques for packing block data into files to help with storage issues. But for the moment if we keep things simple we know we will need to store block data for each and every voxel in our world.

Even at this stage is is important to try and minimize the variables that are stored in the block class, since any wasted overhead at this level will drastically bloat the storage of our voxel data. As I mentioned in the [previous article](https://sites.google.com/site/letsmakeavoxelengine/home/basic-block-rendering?authuser=0), we dont need to store the position for each and every block, since we can infer the position based on the block indices within our storage array.

One thing that is important to store about each block, is whether it is active or not. This basically defined if the block is **on** or **off**. When we are rendering our voxels, any blocks that are inactive (off) we dont render. This is what makes our voxel world dynamic, because we can turn a voxel on or off at runtime when we are manipulating the voxel world.

## Block Class

Here is the block class in code:

```
class Block {
```

```
  public: Block();
```

```
  ~Block();
```

```
  bool IsActive();
```

```
  void SetActive(bool active);
```

```
  private: bool m_active;
```

```
  BlockType m_blockType;
```

```
};
```

## Block Type

Another important piece of data that we should store about each block is the type of block it is. Later on we are going to want to have lots of different types of blocks (Wood, Grass, Dirt, Stone, etc...) and this will need to be stored along with the active flag for the block.

```
enum BlockType {
```

```
 BlockType_Default = 0,
```

```
 BlockType_Grass,
```

```
 BlockType_Dirt,
```

```
 BlockType_Water,
```

```
 BlockType_Stone,
```

```
 BlockType_Wood,
```

```
 BlockType_Sand,
```

```
 BlockType_NumTypes,
```

```
};
```

Now, if we have a file system that can be used to write out the active flag and the block type for each voxel, and we are also able to read it back in. We have a simple way to save and load our voxel information

## Procedural Values

What is important to think about at this early stage is that we want to write out as little information about a block as possible when storing our data, since we are going to be storing millions of voxels to our files. So one thing to get used to at this stage is procedurally generating anything that we can in relation to a blocks data. For example we don't ever need to store the normal vector information about the block sides, since we can easily work this out for each side of the voxel. Also texture coordinates or colour information for a block doesnt need to be stored. If we are using block types, we know that all grass blocks will use the grass texture or be coloured green.

This is actually an important concept to get used to, and is quite different to what you might be used to when making a traditional 3D application. For example usually when you think about a 3d model, you assume that most (if not all) of your data is going to be stored on file and loaded at runtime. A traditional 3D mesh file will contain vertex information, indices, texture coordinates, normals, material values, etc... We want to move away from this idea of mesh storage and only concentrate on storing what is vital for our voxel data and enough to allow us to re-generate the same world next time we load the data.

In the [next article](https://sites.google.com/site/letsmakeavoxelengine/home/chunks?authuser=0), I will explain another vital structure for a good voxel engine, the chunk class.