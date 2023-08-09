#define KEPSILON 0.00001

// ------------- Triangle -------------

HitRecord hitTriangleLight(uvec3 triIndices, Ray r, float dirMin, float tMax) {
  HitRecord hit;
  hit.isHit = false;

  vec3 v0v1 = vertices[triIndices.y].position - vertices[triIndices.x].position;
  vec3 v0v2 = vertices[triIndices.z].position - vertices[triIndices.x].position;
  vec3 pvec = cross(r.direction, v0v2);
  float det = dot(v0v1, pvec);
  
  if (abs(det) < KEPSILON) {
    return hit;
  }

  vec3 tvec = r.origin - vertices[triIndices.x].position;
  float u = dot(tvec, pvec) / det;
  if (u < 0.0f || u > 1.0f) {
    return hit;
  }

  vec3 qvec = cross(tvec, v0v1);
  float v = dot(r.direction, qvec) / det;
  if (v < 0.0f || u + v > 1.0f) {
    return hit;
  }
  
  float t = dot(v0v2, qvec) / det;

  if (t > tMax || length(t * r.direction) < dirMin) {
    return hit;
  }

  hit.isHit = true;
  hit.point = rayAt(r, t);

  vec3 outwardNormal = normalize(cross(v0v1, v0v2));
  hit.normal = setFaceNormal(r.direction, outwardNormal);

  return hit;
}

// ------------- Point Light -------------

HitRecord hitPointLight(PointLight light, Ray r, float dirMin, float tMax) {
  HitRecord hit;
  hit.isHit = false;

  vec3 lightDirection = light.position - r.origin;
  vec3 lightNormal = normalize(lightDirection);

  if (dot(normalize(r.direction), lightNormal) < 0.99f || length(lightDirection) < dirMin) {
    return hit;
  }

  float t = length(lightDirection / r.direction);
  if (t > tMax) {
    return hit;
  }

  hit.isHit = true;
  hit.t = t;
  hit.point = light.position;
  hit.normal = setFaceNormal(r.direction, lightNormal);

  return hit;
}

// ------------- Bvh -------------

bool intersectAABB(Ray r, vec3 boxMin, vec3 boxMax) {
  vec3 tMin = (boxMin - r.origin) / r.direction;
  vec3 tMax = (boxMax - r.origin) / r.direction;
  vec3 t1 = min(tMin, tMax);
  vec3 t2 = max(tMin, tMax);
  float tNear = max(max(t1.x, t1.y), t1.z);
  float tFar = min(min(t2.x, t2.y), t2.z);

  return tNear < tFar;
}

// ------------- Triangle Light-------------

HitRecord hitTriangleLightBvh(Ray r, float dirMin, float tMax) {
  HitRecord hit;
  hit.isHit = false;
  hit.t = tMax;

  uint stack[30];
  stack[0] = 1u;

  int stackIndex = 1;
  while(stackIndex > 0 && stackIndex <= 30) {
    stackIndex--;
    uint currentNode = stack[stackIndex];
    if (currentNode == 0u) {
      continue;
    }

    if (!intersectAABB(r, lightBvhNodes[currentNode - 1u].minimum, lightBvhNodes[currentNode - 1u].maximum)) {
      continue;
    }

    uint lightIndex = lightBvhNodes[currentNode - 1u].leftObjIndex;
    if (lightIndex >= 1u) {
      HitRecord tempHit = hitTriangleLight(lights[lightIndex - 1u].indices, r, dirMin, hit.t);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitIndex = lightIndex - 1u;
      }
    }

    lightIndex = lightBvhNodes[currentNode - 1u].rightObjIndex;    
    if (lightIndex >= 1u) {
      HitRecord tempHit = hitTriangleLight(lights[lightIndex - 1u].indices, r, dirMin, hit.t);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitIndex = lightIndex - 1u;
      }
    }

    uint bvhNode = lightBvhNodes[currentNode - 1u].leftNode;
    if (bvhNode >= 1u) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }

    bvhNode = lightBvhNodes[currentNode - 1u].rightNode;
    if (bvhNode >= 1u) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }
  }

  return hit;
}