## 

Video Demonstration

Here is a video demonstrating frustum culling working in my voxel engine:

As you can see from this video I am only applying frustum culling during an event (key press), and not every frame. This is just to demonstrate that the frustum culling is working and allows a way for me to move the camera after I have applied the frustum culling and see if it has made any difference to the rendered geometry.

You can also see that I am rendering a debug octree visualization to further test the frustum culling. When I apply the frustum cull event, I render all octree nodes that are visible with a red outline and the other nodes with a white outline. When I move the camera after I have applied the frustum cull you can see that the octree nodes that were inside the frustum when I applied the cull are red, and the nodes outside the frustum get rendered white.

## 

Frustum Object

In my Juice engine I have a class object that represents a frustum. Essentially a frustum is a 3d geometric object, a cone with the top and bottom sliced off, so it makes sense to treat it as a seperate entity that have functions and properties like any other object. The way that I represent my frustum is by using planes. Each side of the frustum is a plane, and I also maintain a few other 3D vectors for corner points and float values for distance and clip plane values.

Here is my frustum class:

```
class Frustum {
```

```
  public: Frustum();
```

```
  ~Frustum();
```

```
  void SetFrustum(float angle, float ratio, float nearD, float farD);
```

```
  void SetCamera(const Vector3d & pos,
```

```
    const Vector3d & target,
```

```
      const Vector3d & up);
```

```
  int PointInFrustum(const Vector3d & point);
```

```
  int SphereInFrustum(const Vector3d & point, float radius);
```

```
  int CubeInFrustum(const Vector3d & center, float x, float y, float z);
```

```
  public: enum {
```

```
    FRUSTUM_TOP = 0, FRUSTUM_BOTTOM, FRUSTUM_LEFT, FRUSTUM_RIGHT, FRUSTUM_NEAR, FRUSTUM_FAR,
```

```
  };
```

```
  static enum {
```

```
    FRUSTUM_OUTSIDE = 0, FRUSTUM_INTERSECT, FRUSTUM_INSIDE,
```

```
  };
```

```
  Plane3D planes[6];
```

```
  Vector3d nearTopLeft, nearTopRight, nearBottomLeft, nearBottomRight;
```

```
  Vector3d farTopLeft, farTopRight, farBottomLeft, farBottomRight;
```

```
  float nearDistance, farDistance;
```

```
  float nearWidth, nearHeight;
```

```
  float farWidth, farHeight;
```

```
  float ratio, angle, tang;
```

```
};
```

As you can see, the important methods for my frustum class are: PointInFrustum(), SphereInFrustum() and CubeInFrustum(). These 3 functions allow me to easily test most objects in my world to see if they are inside the camera's frustum. Testing the frustum isn't a hugely computationally complex operation, but it is still something we want to avoid doing too often. If we are going to be testing to see if any object is inside the frustum every frame, (or whenever the camera changes) we should try to reduce the object to its most common denominator. Having the CubeInFrustum() test allows for most complex objects to test their bounding box, and this is usually sufficient. For some objects, such as particle from a particle effect, it is fine to just use the point check. This might mean that some particles don't get rendered if their rendered quad is half-inside and half-outside the frustum, but for a particle this is not a major problem.

## 

Point Test

```
int Frustum::PointInFrustum(const Vector3d & point) {
```

```
  int result = FRUSTUM_INSIDE;
```

```
  for (int i = 0; i < 6; i++) {
```

```
    if (planes[i].GetPointDistance(point) < 0) {
```

```
      return FRUSTUM_OUTSIDE;
```

```
    }
```

```
  }
```

```
  return (result);
```

```
}
```

## 

Sphere Test

```
int Frustum::SphereInFrustum(const Vector3d & point, float radius) {
```

```
  int result = FRUSTUM_INSIDE;
```

```
  float distance;
```

```
  for (int i = 0; i < 6; i++) {
```

```
    distance = planes[i].GetPointDistance(point);
```

```
    if (distance < -radius) {
```

```
      return FRUSTUM_OUTSIDE;
```

```
    } else if (distance < radius) {
```

```
      result = FRUSTUM_INTERSECT;
```

```
    }
```

```
  }
```

```
  return (result);
```

```
}
```

## 

Cube Test

```
int Frustum::CubeInFrustum(const Vector3d & center, float x, float y, float z) {
```

```
  // NOTE : This code can be optimized, it is just easier to read and understand as is      
```

```
  int result = FRUSTUM_INSIDE;
```

```
  for (int i = 0; i < 6; i++) { // Reset counters for corners in and out         
```

```
    int out = 0;
```

```
    int in = 0;
```

```
    if (planes[i].GetPointDistance(center + Vector3d(-x, -y, -z)) < 0) {
```

```
      out++;
```

```
    } else {
```

```
      in ++;
```

```
    }
```

```
    if (planes[i].GetPointDistance(center + Vector3d(x, -y, -z)) < 0) {
```

```
      out++;
```

```
    } else {
```

```
      in ++;
```

```
    }
```

```
    if (planes[i].GetPointDistance(center + Vector3d(-x, -y, z)) < 0) {
```

```
      out++;
```

```
    } else {
```

```
      in ++;
```

```
    }
```

```
    if (planes[i].GetPointDistance(center + Vector3d(x, -y, z)) < 0) {
```

```
      out++;
```

```
    } else {
```

```
      in ++;
```

```
    }
```

```
    if (planes[i].GetPointDistance(center + Vector3d(-x, y, -z)) < 0) {
```

```
      out++;
```

```
    } else {
```

```
      in ++;
```

```
    }
```

```
    if (planes[i].GetPointDistance(center + Vector3d(x, y, -z)) < 0) {
```

```
      out++;
```

```
    } else {
```

```
      in ++;
```

```
    }
```

```
    if (planes[i].GetPointDistance(center + Vector3d(-x, y, z)) < 0) {
```

```
      out++;
```

```
    } else {
```

```
      in ++;
```

```
    }
```

```
    if (planes[i].GetPointDistance(center + Vector3d(x, y, z)) < 0) {
```

```
      out++;
```

```
    } else {
```

```
      in ++;
```

```
    } // If all corners are out         
```

```
    if (! in ) {
```

```
      return FRUSTUM_OUTSIDE;
```

```
    } // If some corners are out and others are in         
```

```
    else if (out) {
```

```
      result = FRUSTUM_INTERSECT;
```

```
    }
```

```
  }
```

```
  return (result);
```

```
}
```

There isn't really an awful lot to saw about the code to test for whether we are inside, intersecting or outside a frustum, it is just Math and it works! Note however that each function has an early out if we find that we are actually inside or beintersecting the frustum. This helps speed up performance somewhat but it is generally a minor thing. There are probably there more efficient ways to code frustum checks and more optimized algorithms, so don't be afraid to search them out.. in fact if you find something better than what I have got, please let me know and share the knowledge with me. :)