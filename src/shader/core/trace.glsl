// ------------- Triangle -------------

HitRecord hitTriangle(uvec3 triIndices, Ray r, float dirMin, float tMax, uint transformIndex, uint materialIndex) {
  HitRecord hit;
  hit.isHit = false;

  vec3 v0v1 = vertices[triIndices.y].position.xyz - vertices[triIndices.x].position.xyz;
  vec3 v0v2 = vertices[triIndices.z].position.xyz - vertices[triIndices.x].position.xyz;
  vec3 pvec = cross(r.direction, v0v2);
  float det = dot(v0v1, pvec);
  
  if (abs(det) < KEPSILON) {
    return hit;
  }

  vec3 tvec = r.origin - vertices[triIndices.x].position.xyz;
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

  if (t > tMax || length(mat3(transformations[transformIndex].dirMatrix) * t * r.direction) < dirMin) {
    return hit;
  }

  hit.isHit = true;
  hit.point = (transformations[transformIndex].pointMatrix * vec4(rayAt(r, t), 1.0f)).xyz;

  vec2 uv = getTotalTextureCoordinate(triIndices, vec2(u, v));

  if (materials[materialIndex].textureIndex.x == 0u) {
    hit.color = materials[materialIndex].baseColor.xyz;
  } else {
    hit.color = texture(colorTextureSampler[materials[materialIndex].textureIndex.x - 1u], uv).xyz;
  }

  if (materials[materialIndex].textureIndex.y == 0u) {
    vec3 outwardNormal = normalize(cross(v0v1, v0v2));
    hit.normal = normalize(mat3(transformations[transformIndex].normalMatrix) * setFaceNormal(r.direction, outwardNormal));
  } else {
    vec3 outwardNormal = texture(normalTextureSampler[materials[materialIndex].textureIndex.y - 1u], uv).xyz;
    hit.normal = normalize(mat3(transformations[transformIndex].normalMatrix) * outwardNormal);
  }

  hit.metallicness = materials[materialIndex].params.x;
  hit.roughness = materials[materialIndex].params.y;
  hit.fresnelReflect = materials[materialIndex].params.z;

  return hit;
}

HitRecord hitTriangleLight(uvec3 triIndices, Ray r, float dirMin, float tMax) {
  HitRecord hit;
  hit.isHit = false;

  vec3 v0v1 = vertices[triIndices.y].position.xyz - vertices[triIndices.x].position.xyz;
  vec3 v0v2 = vertices[triIndices.z].position.xyz - vertices[triIndices.x].position.xyz;
  vec3 pvec = cross(r.direction, v0v2);
  float det = dot(v0v1, pvec);
  
  if (abs(det) < KEPSILON) {
    return hit;
  }

  vec3 tvec = r.origin - vertices[triIndices.x].position.xyz;
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

HitRecord hitPrimitiveBvh(Ray r, float dirMin, float tMax, uint firstBvhIndex, uint firstPrimitiveIndex, uint transformIndex) {
  HitRecord hit;
  hit.isHit = false;
  hit.t = tMax;

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

    if (!intersectAABB(r, primitiveBvhNodes[currentNode - 1u + firstBvhIndex].minimum.xyz, primitiveBvhNodes[currentNode - 1u + firstBvhIndex].maximum.xyz)) {
      continue;
    }

    uint primIndex = primitiveBvhNodes[currentNode - 1u + firstBvhIndex].nodeObjIndex.z;
    if (primIndex >= 1u) {
      HitRecord tempHit = hitTriangle(primitives[primIndex - 1u + firstPrimitiveIndex].primitiveMaterialIndex.xyz, r, dirMin, hit.t, transformIndex, primitives[primIndex - 1u + firstPrimitiveIndex].primitiveMaterialIndex.w);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitIndex = primIndex - 1u + firstPrimitiveIndex;
      }
    }

    primIndex = primitiveBvhNodes[currentNode - 1u + firstBvhIndex].nodeObjIndex.w;    
    if (primIndex >= 1u) {
      HitRecord tempHit = hitTriangle(primitives[primIndex - 1u + firstPrimitiveIndex].primitiveMaterialIndex.xyz, r, dirMin, hit.t, transformIndex, primitives[primIndex - 1u + firstPrimitiveIndex].primitiveMaterialIndex.w);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitIndex = primIndex - 1u + firstPrimitiveIndex;
      }
    }

    uint bvhNode = primitiveBvhNodes[currentNode - 1u + firstBvhIndex].nodeObjIndex.x;
    if (bvhNode >= 1u) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }

    bvhNode = primitiveBvhNodes[currentNode - 1u + firstBvhIndex].nodeObjIndex.y;
    if (bvhNode >= 1u) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }
  }

  return hit;
}

HitRecord hitObjectBvh(Ray r, float dirMin, float tMax) {
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

    if (!intersectAABB(r, objectBvhNodes[currentNode - 1u].minimum.xyz, objectBvhNodes[currentNode - 1u].maximum.xyz)) {
      continue;
    }

    uint objIndex = objectBvhNodes[currentNode - 1u].nodeObjIndex.z;
    if (objIndex >= 1u) {
      HitRecord tempHit = hitPrimitiveBvh(r, dirMin, hit.t, objects[objIndex - 1u].bvhPrimitiveTransformIndex.x, objects[objIndex - 1u].bvhPrimitiveTransformIndex.y, objects[objIndex - 1u].bvhPrimitiveTransformIndex.z);

      if (tempHit.isHit) {
        hit = tempHit;
      }
    }

    objIndex = objectBvhNodes[currentNode - 1u].nodeObjIndex.w;
    if (objIndex >= 1u) {
      HitRecord tempHit = hitPrimitiveBvh(r, dirMin, hit.t, objects[objIndex - 1u].bvhPrimitiveTransformIndex.x, objects[objIndex - 1u].bvhPrimitiveTransformIndex.y, objects[objIndex - 1u].bvhPrimitiveTransformIndex.z);

      if (tempHit.isHit) {
        hit = tempHit;
      }
    }

    uint bvhNode = objectBvhNodes[currentNode - 1u].nodeObjIndex.x;
    if (bvhNode >= 1u) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }

    bvhNode = objectBvhNodes[currentNode - 1u].nodeObjIndex.y;
    if (bvhNode >= 1u) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }
  }

  return hit;
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

    if (!intersectAABB(r, lightBvhNodes[currentNode - 1u].minimum.xyz, lightBvhNodes[currentNode - 1u].maximum.xyz)) {
      continue;
    }

    uint lightIndex = lightBvhNodes[currentNode - 1u].nodeObjIndex.z;
    if (lightIndex >= 1u) {
      HitRecord tempHit = hitTriangleLight(lights[lightIndex - 1u].indices.xyz, r, dirMin, hit.t);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitIndex = lightIndex - 1u;
      }
    }

    lightIndex = lightBvhNodes[currentNode - 1u].nodeObjIndex.w;    
    if (lightIndex >= 1u) {
      HitRecord tempHit = hitTriangleLight(lights[lightIndex - 1u].indices.xyz, r, dirMin, hit.t);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitIndex = lightIndex - 1u;
      }
    }

    uint bvhNode = lightBvhNodes[currentNode - 1u].nodeObjIndex.x;
    if (bvhNode >= 1u) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }

    bvhNode = lightBvhNodes[currentNode - 1u].nodeObjIndex.y;
    if (bvhNode >= 1u) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }
  }

  return hit;
}