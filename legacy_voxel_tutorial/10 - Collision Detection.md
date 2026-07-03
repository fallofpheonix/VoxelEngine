## Introduction

The way that I have implemented collision detection in my voxel engine is to try and make it run efficiently as possible.

Generally some games require a very hihgly detailed collision detection system, and some games only require rudimental collision detection. My voxel engine uses a combination of both. There are some situations where I am only required to do simple testing, since accuracy isn't all that important (particle effects) and there are some situations where the collision detection needs to be more complex (players, monsters).

With a fully dynamic world (as are most voxel engines) it isn't possible to use any kind of pre-baked or offline collision detection solution. You must make sure you are using the latest world data whenever you do collision detection, since the world can change at any moment.

Another goal of my collision detection code was to avoid any computationally long iteration over all the world data. So instead of looping over every voxel to test for collision detection, which would not work, I only check collision data that is relevant for the object that I am testing.

So when I have an object, say the player and I want to test if he is colliding with the world or not, I do the following:

1. Take the players position vector and transform it into 'voxel space' coordinates.
2. Check if the voxel that corresponds to the player's voxel position is currently active or not.
3. If the voxel IS active, it means the player is colliding with the world. since an active voxel is part of the world and should be collided with.
    1. Work out a collision response based on what the player is doing. for example if we are falling downwards (gravity) then just push the player upwards to the voxels top boundary, so he is not intersecting with the active voxel, and DON'T apply gravity this frame.
    2. If the player is walking, sidestepping, jumping, etc... then we need to work out a slide vector so players can slide along walls.
4. If the voxel IS NOT active it means the player is in open space and is free to move and update his physics properties as normal

The important thing to notice here is that the players position gets 'transformed' into voxel space co-ordinates. This basically eliminates any kind of iteration or looping over the voxel data, since I only have to do collision and physics based tests on **1** voxel.

The following video demonstrates collision detection using a player controlled object:

As you can see the player is able to fully move around the voxel would and behaves as you would expect; not falling through active voxels, sliding against walls, etc...

## 

Point Objects or Bounding Boxes

Depending on what object you are simulating it sometimes makes sense to identify the object as a point and sometimes by using a bouncing box. All my particle effects and block particles are treated as point objects. Since there are generally many block particles on screen at once and all need simulating and detecting collision, it makes more sense and reduces overhead to just assume that these particles have a point position. This avoids any horrible cube-to-cube collision detection for primitive objects such as particles.

Usually players or other sufficiently large objects within the game (monsters, NPCs, items, etc) will need to have a bounding box and this should be checked for collision detection properly. This will allow for much better accuracy when simulating larger objects.

## 

Missed Collisions

One of the problems that occur in a collision detection system, is that sometimes you might miss a collision entirely! For example if an object is moving very fast towards a very thin wall (one cube thick). If the object is on one side of the wall during one frame and then after it's velocity and position have been integrated it has passed all the way to the other side of the wall, a collision will never be detected.

One way to solve this problem is to do multiple collision checks a single pass. For example, do an intermediary check that represents the object during motion. This can be very effective at solving this problem. If you know the minimum distance that an object can travel to fully pass through your collision data (the size of a voxel in our voxel engine) then you can make sure you check the object's position enough times so that it can never skip fully over a voxel during a single update.

Here is some example code to highlight this:

// TODO