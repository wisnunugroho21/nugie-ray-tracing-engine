#define KEPSILON 0.00001

// ------------- Triangle -------------

HitRecord hitTriangle(uvec3 triIndices, Ray r, float dirMin, float dirMax, uint transformIndex, uint materialIndex) {
  HitRecord hit;
  hit.isHit = false;

  vec3 v0v1 = vertices[triIndices.y].position - vertices[triIndices.x].position;
  vec3 v0v2 = vertices[triIndices.z].position - vertices[triIndices.x].position;
  vec3 pvec = cross(r.direction, v0v2);
  float det = dot(v0v1, pvec);
  
#ifdef BACKFACE_CULLING
  if (det < KEPSILON) {
    return hit;
  }
#else
  if (abs(det) < KEPSILON) {
    return hit;
  }
#endif

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
  float dir = length(mat3(transformations[transformIndex].dirMatrix) * t * r.direction);

  if (dir < dirMin || dir > dirMax) {
    return hit;
  }

  hit.isHit = true;
  hit.dir = dir;
  hit.point = (transformations[transformIndex].pointMatrix * vec4(rayAt(r, t), 1.0f)).xyz;

  if (materials[materialIndex].normalTextureIndex == 0u) {
    vec3 outwardNormal = normalize(cross(v0v1, v0v2));
    hit.normal = normalize(mat3(transformations[transformIndex].normalMatrix) * setFaceNormal(r.direction, outwardNormal));
  } else {
    vec2 uv = getTotalTextureCoordinate(triIndices, vec2(u, v));
    vec3 outwardNormal = texture(normalTextureSampler[materials[materialIndex].normalTextureIndex - 1u], uv).xyz;
    hit.normal = normalize(mat3(transformations[transformIndex].normalMatrix) * outwardNormal);
  }

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

HitRecord hitPrimitiveBvh(Ray r, float dirMin, float dirMax, uint firstBvhIndex, uint firstPrimitiveIndex, uint transformIndex) {
  HitRecord hit;
  hit.isHit = false;
  hit.dir = dirMax;

  uint stack[30];
  stack[0] = 1u;

  int stackIndex = 1;  

  r.origin = (transformations[transformIndex].pointInverseMatrix * vec4(r.origin, 1.0f)).xyz;
  r.direction = mat3(transformations[transformIndex].dirInverseMatrix) * r.direction;

  while(stackIndex > 0 && stackIndex <= 30) {
    stackIndex--;
    uint currentNode = stack[stackIndex];
    if (currentNode == 0u) {
      continue;
    }

    if (!intersectAABB(r, primitiveBvhNodes[currentNode - 1u + firstBvhIndex].minimum, primitiveBvhNodes[currentNode - 1u + firstBvhIndex].maximum)) {
      continue;
    }

    uint primIndex = primitiveBvhNodes[currentNode - 1u + firstBvhIndex].leftObjIndex;
    if (primIndex >= 1u) {
      HitRecord tempHit = hitTriangle(primitives[primIndex - 1u + firstPrimitiveIndex].indices, r, dirMin, hit.dir, transformIndex, primitives[primIndex - 1u + firstPrimitiveIndex].materialIndex);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitIndex = primIndex - 1u + firstPrimitiveIndex;
      }
    }

    primIndex = primitiveBvhNodes[currentNode - 1u + firstBvhIndex].rightObjIndex;    
    if (primIndex >= 1u) {
      HitRecord tempHit = hitTriangle(primitives[primIndex - 1u + firstPrimitiveIndex].indices, r, dirMin, hit.dir, transformIndex, primitives[primIndex - 1u + firstPrimitiveIndex].materialIndex);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitIndex = primIndex - 1u + firstPrimitiveIndex;
      }
    }

    uint bvhNode = primitiveBvhNodes[currentNode - 1u + firstBvhIndex].leftNode;
    if (bvhNode >= 1u) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }

    bvhNode = primitiveBvhNodes[currentNode - 1u + firstBvhIndex].rightNode;
    if (bvhNode >= 1u) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }
  }

  return hit;
}

HitRecord hitObjectBvh(Ray r, float dirMin, float dirMax) {
  HitRecord hit;
  hit.isHit = false;
  hit.dir = dirMax;

  uint stack[30];
  stack[0] = 1u;

  int stackIndex = 1;
  while(stackIndex > 0 && stackIndex <= 30) {
    stackIndex--;
    uint currentNode = stack[stackIndex];
    if (currentNode == 0u) {
      continue;
    }

    if (!intersectAABB(r, objectBvhNodes[currentNode - 1u].minimum, objectBvhNodes[currentNode - 1u].maximum)) {
      continue;
    }

    uint objIndex = objectBvhNodes[currentNode - 1u].leftObjIndex;
    if (objIndex >= 1u) {
      HitRecord tempHit = hitPrimitiveBvh(r, dirMin, hit.dir, objects[objIndex - 1u].firstBvhIndex, objects[objIndex - 1u].firstPrimitiveIndex, objects[objIndex - 1u].transformIndex);

      if (tempHit.isHit) {
        hit = tempHit;
      }
    }

    objIndex = objectBvhNodes[currentNode - 1u].rightObjIndex;
    if (objIndex >= 1u) {
      HitRecord tempHit = hitPrimitiveBvh(r, dirMin, hit.dir, objects[objIndex - 1u].firstBvhIndex, objects[objIndex - 1u].firstPrimitiveIndex, objects[objIndex - 1u].transformIndex);

      if (tempHit.isHit) {
        hit = tempHit;
      }
    }

    uint bvhNode = objectBvhNodes[currentNode - 1u].leftNode;
    if (bvhNode >= 1u) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }

    bvhNode = objectBvhNodes[currentNode - 1u].rightNode;
    if (bvhNode >= 1u) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }
  }

  return hit;
}
