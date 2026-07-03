## 

Introduction

One very important question you should ask yourself and answer fairly early on during the development of your voxel engine, is; **Should my voxel engine support textures?** This is actually a very critical question to answer since it will not only define the look and feel of your voxel engine, but also drastically change the underlying technical voxel representation and the computation you need to do when generating your voxel world and chunk data.

As you can see from my personal voxel engine, I chose NOT to use texture rendering on my voxels and instead opt for a more stylized look during rendering. Removing textures (and importantly texture co-ordinate generation) from the voxel data is a bit of a cheat and offers a solution to a big headache, but at the same time I made this decision not just for the technical reasons, but also because I was confident I could make my voxel engine look good just by using colour and material properties.

Early on in development I did support textures on my voxels (as you can see from previous article's screenshots) and during this time I did figure out a few important points to consider if you do want to have textures in your voxel engine and I will share these with you.

## 

Block Type

Ultimately the main reason why you might want voxels to have different textures or colours is because you want the concept of different 'block types'. I touched upon this concept in my article on [Block Data Structures](https://sites.google.com/site/letsmakeavoxelengine/home/block-data-structure?authuser=0) but I will explain in a little more detail here.

A typical voxel engine isn't really going to look very appealing or interesting to a user/player if all blocks are rendering in exactly the same way. The world will lack clarity and depth and I really cant see much practical use for rendering 'every' voxel in your engine the same way. Therefore having different block types will allow you to add depth to your engine and make it much more appealing. You can then chose to render the different block types in a different way (such as different textures, or colours) so that you voxel world looks much more vibrant.

Also, from a gameplay point of view, (assuming you are making your voxel engine for games) having different block types will easily allow you to apply different properties for the voxels depending on the block type. For example a 'sand' block in your world might react totally differently to a 'dirt' block.

## 

Textures

Two of the biggest things I noticed during my initial development supporting texture in my voxel engine were:

1. Don't store texture co-ordinates for a voxel, generate them at runtime.
2. Use a texture atlas!

If you are unsure of what a texture atlas is, I would suggest you do some [Googling](http://www.google.com/url?q=http%3A%2F%2Flmgtfy.com%2F%3Fq%3Dtexture%2Batlas&sa=D&sntz=1&usg=AOvVaw0ZJc97uhaYe6FPcsZR_6jx) and reading up, but essentially a texture atlas allows you to store all your textures in one file rather than separate texture files. Doing so provides one very subtle, but **VITAL**, difference when rendering your voxels and chunk; you only need to bind a texture **ONCE** during chunk rendering. Texture binding is an expensive operation for any rendering API, so if each of your block types had a separate texture file, you would need to bind a texture many many times during a chunk render... this would kill performance and something you want to avoid.

The other point I made above is that you should work out your texture co-ordinates at runtime when you are generating your chunks. DO NOT store the texture co-ordinates in your chunk/voxel files, since this is wasted storage space since the texture co-ordinates for each voxel are essentially the same.

When using a texture atlas for each of the different voxel textures, you simply need to work out the texture co-ordinates based on what block type the voxel is. For example if your voxel engine only have 4 block types and the 4 textures for this were stored in a texture atlas. the texture range for each block type would be as follows: (assuming you start in the top left corner as your texture origin)

- Block type 1: UV range [X: 0.0 -> 0.5, Y: 0.0 -> 0.5]
- Block type 2: UV range [X: 0.5 -> 1.0, Y: 0.0 -> 0.5]
- Block type 3: UV range [X: 0.0 -> 0.5, Y: 0.5 -> 1.0]
- Block type 4: UV range [X: 0.5 -> 1.0, Y: 0.5 -> 1.0]
- etc...

This method can easily be extended for a texture atlas containing any number of block type textures.

## 

Colouring

Using simple colouring to allow your voxel engine to look nice takes a few things...