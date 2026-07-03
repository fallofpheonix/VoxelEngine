import matplotlib.pyplot as plt
import numpy as np

# Create a 32x32x16 grid for visualization
dim = 16
x, y, z = np.indices((dim, dim, dim))

# Define procedural terrain (y < 5 is filled)
voxels = (z < 5)

# Set colors based on type (Grass)
colors = np.empty(voxels.shape, dtype=object)
colors[voxels] = 'green'

fig = plt.figure(figsize=(10, 8))
ax = fig.add_subplot(111, projection='3d')

# Render the voxels
ax.voxels(voxels, facecolors=colors, edgecolors='k', linewidth=0.1)

ax.set_title('Voxel Engine GUI Snapshot (Native Render)')
ax.set_axis_off()

# Save as PNG
plt.savefig('Voxel Engine/gui_snapshot.png')
print("GUI Snapshot saved to Voxel Engine/gui_snapshot.png")
