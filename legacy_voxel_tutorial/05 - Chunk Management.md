```
void ChunkManager::Update(float dt, Vector3d cameraPosition, Vector3d cameraView) {
```

```
  UpdateAsyncChunker();
```

```
  UpdateLoadList();
```

```
  UpdateSetupList();
```

```
  UpdateRebuildList();
```

```
  UpdateFlagsList();
```

```
  UpdateUnloadList();
```

```
  UpdateVisibilityList(cameraPosition);
```

```
  if (m_cameraPosition != cameraPosition || m_cameraView != cameraView) {
```

```
    UpdateRenderList();
```

```
  }
```

```
  m_cameraPosition = cameraPosition;
```

```
  m_cameraView = cameraView;
```

```
}
```

## 

Chunk Loading

This phase of the chunk manager update iterates over the **LoadList** container and calls load for the chunks within this container.. there inst anything particular special to note about this phase, except that it has some additional code to only allow a certain number of chunks to be loaded each frame, so we get a sort of asynchronous loading system. This container is cleared every frame, and gets re-updated in the visibility update phase.

```
void ChunkManager::UpdateLoadList() {
```

```
  int lNumOfChunksLoaded = 0;
```

```
  ChunkList::iterator iterator;
```

```
  for (iterator = m_vpChunkLoadList.begin(); iterator != m_vpChunkLoadList.end() && (lNumOfChunksLoaded != ASYNC_NUM_CHUNKS_PER_FRAME); ++iterator) {
```

```
    Chunk * pChunk = ( * iterator);
```

```
    if (pChunk -> IsLoaded() == false) {
```

```
      if (lNumOfChunksLoaded != ASYNC_NUM_CHUNKS_PER_FRAME)) {
```

```
      pChunk -> Load(); // Increase the chunks loaded count
```

```
      lNumOfChunksLoaded++;
```

```
      m_forceVisibilityUpdate = true;
```

```
    }
```

```
  }
```

```
} // Clear the load list (every frame)     
```

```
m_vpChunkLoadList.clear();
```

```
}
```

## 

Chunk Setup

The setup phase for a chunk is very similar to the load phase. We iterate over the **SetupList** container and setup any chunks within that list:

```
void ChunkManager::UpdateSetupList() { // Setup any chunks that have not already been setup     
```

```
  ChunkList::iterator iterator;
```

```
  for (iterator = m_vpChunkSetupList.begin(); iterator != m_vpChunkSetupList.end(); ++iterator) {
```

```
    Chunk * pChunk = ( * iterator);
```

```
    if (pChunk -> IsLoaded() && pChunk -> IsSetup() == false) {
```

```
      pChunk -> Setup();
```

```
      if (pChunk -> IsSetup()) { // Only force the visibility update if we actually setup the chunk, some chunks wait in the pre-setup stage... 
```

```
        m_forceVisibilityUpdate = true;
```

```
      }
```

```
    }
```

```
  } // Clear the setup list (every frame)    
```

```
  m_vpChunkSetupList.clear();
```

```
}
```

## 

Chunk Rebuilding

Chunk rebuilding is the act of re-creating a chunks render buffer object, since some voxel information within the chunk has been modified (for example a voxel being turned on or off) This is fairly simple, but in my voxel engine I also do asynchronous chunk rebuilding, in a very similar way to how I do chunk loading. Basically I only want a certain number of chunks to be rebuilt each frame, to maintain a constant and steady frame rate.

Also another complication to chunk rebuilding occurs, since we do some chunk rendering optimizations with regards to chunk neighbors, so when we rebuild a chunk we also need to do some logic to check if the neighbor chunks also need to be updated.

```
void ChunkManager::UpdateRebuildList() {
```

```
  // Rebuild any chunks that are in the rebuild chunk list     
```

```
  ChunkList::iterator iterator;
```

```
  int lNumRebuiltChunkThisFrame = 0;
```

```
  for (iterator = m_vpChunkRebuildList.begin(); iterator != m_vpChunkRebuildList.end() && (lNumRebuiltChunkThisFrame != ASYNC_NUM_CHUNKS_PER_FRAME); ++iterator) {
```

```
    Chunk * pChunk = ( * iterator);
```

```
    if (pChunk -> IsLoaded() && pChunk -> IsSetup()) {
```

```
      if (lNumRebuiltChunkThisFrame != ASYNC_NUM_CHUNKS_PER_FRAME) {
```

```
        pChunk -> RebuildMesh(); // If we rebuild a chunk, add it to the list of chunks that need their render flags updated                 
```

```
        // since we might now be empty or surrounded   
```

```
        m_vpChunkUpdateFlagsList.push_back(pChunk); // Also add our neighbours since they might now be surrounded too (If we have neighbours)                
```

```
        Chunk * pChunkXMinus = GetChunk(pChunk -> GetX() - 1, pChunk -> GetY(), pChunk -> GetZ());
```

```
        Chunk * pChunkXPlus = GetChunk(pChunk -> GetX() + 1, pChunk -> GetY(), pChunk -> GetZ());
```

```
        Chunk * pChunkYMinus = GetChunk(pChunk -> GetX(), pChunk -> GetY() - 1, pChunk -> GetZ());
```

```
        Chunk * pChunkYPlus = GetChunk(pChunk -> GetX(), pChunk -> GetY() + 1, pChunk -> GetZ());
```

```
        Chunk * pChunkZMinus = GetChunk(pChunk -> GetX(), pChunk -> GetY(), pChunk -> GetZ() - 1);
```

```
        Chunk * pChunkZPlus = GetChunk(pChunk -> GetX(), pChunk -> GetY(), pChunk -> GetZ() + 1);
```

```
        if (pChunkXMinus != NULL) m_vpChunkUpdateFlagsList.push_back(pChunkXMinus);
```

```
        if (pChunkXPlus != NULL) m_vpChunkUpdateFlagsList.push_back(pChunkXPlus);
```

```
        if (pChunkYMinus != NULL) m_vpChunkUpdateFlagsList.push_back(pChunkYMinus);
```

```
        if (pChunkYPlus != NULL) m_vpChunkUpdateFlagsList.push_back(pChunkYPlus);
```

```
        if (pChunkZMinus != NULL) m_vpChunkUpdateFlagsList.push_back(pChunkZMinus);
```

```
        if (pChunkZPlus != NULL) m_vpChunkUpdateFlagsList.push_back(pChunkZPlus); // Only rebuild a certain number of chunks per frame   
```

```
        lNumRebuiltChunkThisFrame++;
```

```
        m_forceVisibilityUpdate = true;
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
  // Clear the rebuild list     
```

```
  m_vpChunkRebuildList.clear();
```

```
}
```

## 

Chunk Unloading

Chunk unloading is the inverse of a chunk load, this is where all the block data gets deleted and the memory is freed back to the pool. Again there is a separate **UnloadList** that is maintained for this purpose.

```
void ChunkManager::UpdateUnloadList() { // Unload any chunks
```

```
  ChunkList::iterator iterator;
```

```
  for (iterator = m_vpChunkUnloadList.begin(); iterator != m_vpChunkUnloadList.end(); ++iterator) {
```

```
    Chunk * pChunk = ( * iterator);
```

```
    if (pChunk -> IsLoaded()) {
```

```
      pChunk -> Unload();
```

```
      m_forceVisibilityUpdate = true;
```

```
    }
```

```
  } // Clear the unload list (every frame)    
```

```
  m_vpChunkUnloadList.clear();
```

```
}
```

## 

Chunk Visibility

This is the update phase that brings most of the ChunkManager concepts to life. The visibility update phase is used to update the **VilibilityList** with all the potential blocks that might be visible to the current camera. Also the visibility list is used to update all the other list, for only chunks that are visible can be set to load,setup,rebuild and render.

- // TODO : include code

## 

Chunk Render

The render update phase is a simple culling process, in my voxel engine I do frustum culling and some other rendering optimizations but the basic logic is the same. We have a **VisibilityList** that we calculated in the visibility phase and just cull away any chunks that dont need to be rendered. For example completely empty chunks, chunks that are completely surrounded by their neighbors, occluded chunks, etc...

```
void ChunkManager::UpdateRenderList() {
```

```
  // Clear the render list each frame BEFORE we do our tests to see what chunks should be rendered     
```

```
  m_vpChunkRenderList.clear();
```

```
  ChunkList::iterator iterator;
```

```
  for (iterator = m_vpChunkVisibilityList.begin(); iterator != m_vpChunkVisibilityList.end(); ++iterator) {
```

```
    Chunk * pChunk = ( * iterator);
```

```
    if (pChunk != NULL) {
```

```
      if (pChunk -> IsLoaded() && pChunk -> IsSetup()) {
```

```
        if (pChunk -> ShouldRender()) // Early flags check so we don't always have to do the frustum check... 
```

```
        {// Check if this chunk is inside the camera frustum                    
```

```
          float c_offset = (Chunk::CHUNK_SIZE * Block::BLOCK_RENDER_SIZE) - Block::BLOCK_RENDER_SIZE;
```

```
          Vector3d chunkCenter = pChunk -> GetPosition() + Vector3d(c_offset, c_offset, c_offset);
```

```
          float c_size = Chunk::CHUNK_SIZE * Block::BLOCK_RENDER_SIZE;
```

```
          if (m_pRenderer -> CubeInFrustum(m_pRenderer -> GetActiveViewPort(), chunkCenter, c_size, c_size, c_size)) {
```

```
            m_vpChunkRenderList.push_back(pChunk);
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

```
}
```