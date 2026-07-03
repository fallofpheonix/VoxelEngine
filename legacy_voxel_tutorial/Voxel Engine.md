## Introduction

These tutorials and guides are what I wrote as I made my own voxel engine which later changed into the game [Vox](http://www.google.com/url?q=http%3A%2F%2Fwww.vox-game.com&sa=D&sntz=1&usg=AOvVaw183pdWYneKpIq1B3qiXY3l). If you are interested in learning about voxel engines, or want to make your own voxel game then these articles and tutorials should help you achieve this. Please be sure to checkout Vox, the game I am making that is based on the code contained on this site.

This series of guides and tutorials is going to be on the topic of making a voxel engine. If you don’t know what a voxel engine is you should probably research that first. ([Voxel information](http://www.google.com/url?q=http%3A%2F%2Fen.wikipedia.org%2Fwiki%2FVoxel&sa=D&sntz=1&usg=AOvVaw2EQcMh8AwUo4jm4Yz2YQK5)) The main purpose of these guides is to introduce you to voxel engine concepts and ideas which help you produce an engine that satisfies a number of goals. Making a good voxel engine takes time and effort, but luckily the process fits nicely into an iterative development cycle. It is easy to start with something very basic and slowly build up to a more complex and thorough voxel engine with lots of little steps. The more complex tasks such as chunk management or collision detection can be thought of as singular problems and tackled individually as sub-tasks.

These guides are going to assume you are familiar with 3D graphics and 3D math and have already done some programming in a 3D environment, such as OpenGL or Direct3D.

Most of the rendering code for these guides will be written using my own personal rendering library, which is essentially a wrapper around OpenGL, where I abstract away the OpenGL calls or group together common OpenGL functionality into a single renderer function, to make the game code easier to manage. Anyone who understands OpenGL should have no problem translating my native renderer code back into OpenGL, and in some instances I will provide the OpenGL calls in these guides.

I will also be uploading videos to my [YouTube channel](http://www.youtube.com/alwaysgeeky) so if you like to see visual progess you can subscribe to my channel for updates.

I am currently using the voxel engine I have created to make a brand new voxel based adventure, RPG and creation game called **Vox**, information about this can be found at [IndieDB](http://www.google.com/url?q=http%3A%2F%2Fwww.indiedb.com%2Fgames%2Fvox&sa=D&sntz=1&usg=AOvVaw1l-3BlEok4M6KKDTjFnmmC).

The main goals and ambitions of a good voxel engine are as follows:

- **Efficient** – Able to render a large number of voxels on screen at once.
- **Dynamic** – The voxel engine should be able to modify ANY voxel within the world at ANY time.
- **Expansive** – The overall scale of the world should be large and not limited by arbitary constraints.

There are many different tricks you can do with voxel data that allows you to achieve these goals, some are efficient programming tricks, others are rendering optimizations. Once you get used to visualizing the simple voxel data there are many fascinating things you can achieve, and hopefully these guides will show you the mysteries behind a good voxel engine.

Below are the rough steps that I have followed when making my own personal voxel engine: