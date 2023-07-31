// ------------- Sky Light ------------- 

vec3 sunLightFaceNormal(vec3 rayDirection, vec3 origin) {
  return -1.0f * normalize(rayDirection);
}

float sunLightArea() {
  return 1.0f;
}

vec3 sunLightRandomDirection(SunLight light) {
  return light.direction;
}

// ------------- Point Light ------------- 

vec3 pointLightFaceNormal(PointLight light, vec3 rayDirection, vec3 origin) {
  vec3 outwardNormal = normalize(light.position - origin);
  return setFaceNormal(rayDirection, outwardNormal);
}

float pointLightArea() {
  return 1.0f;
}

vec3 pointLightRandomDirection(PointLight light, vec3 origin) {
  return light.position - origin;
}

// ------------- Triangle -------------

vec3 triangleFaceNormal(uvec3 triIndices, vec3 rayDirection) {
  vec3 v0v1 = vertices[triIndices.y].position - vertices[triIndices.x].position;
  vec3 v0v2 = vertices[triIndices.z].position - vertices[triIndices.x].position;

  vec3 outwardNormal = normalize(cross(v0v1, v0v2));
  return setFaceNormal(rayDirection, outwardNormal);
}

float triangleArea(uvec3 triIndices) {
  vec3 v0v1 = vertices[triIndices.y].position - vertices[triIndices.x].position;
  vec3 v0v2 = vertices[triIndices.z].position - vertices[triIndices.x].position;

  vec3 pvec = cross(v0v1, v0v2);
  return 0.5 * sqrt(dot(pvec, pvec)); 
}

vec3 triangleRandomDirection(uvec3 triIndices, vec3 origin, uint additionalRandomSeed) {
  vec3 a = vertices[triIndices.y].position - vertices[triIndices.x].position;
  vec3 b = vertices[triIndices.z].position - vertices[triIndices.x].position;

  float u1 = randomFloat(additionalRandomSeed);
  float u2 = randomFloat(additionalRandomSeed + 1);

  if (u1 + u2 > 1) {
    u1 = 1 - u1;
    u2 = 1 - u2;
  }

  vec3 randomTriangle = u1 * a + u2 * b + vertices[triIndices.x].position;
  return randomTriangle - origin;
}