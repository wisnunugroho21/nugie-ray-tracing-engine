// ------------- Basic -------------

vec3 rayAt(Ray r, float t) {
  return r.origin + t * r.direction;
}

vec3[3] buildOnb(vec3 normal) {
  vec3 a = abs(normalize(normal).x) > 0.9 ? vec3(0.0f, 1.0f, 0.0f) : vec3(1.0f, 0.0f, 0.0f);

  vec3 z = normalize(normal);
  vec3 y = normalize(cross(z, a));
  vec3 x = cross(z, y);

  return vec3[3](x, y, z);
}

vec3 setFaceNormal(vec3 r_direction, vec3 outwardNormal) {
  return dot(r_direction, outwardNormal) < 0.0f ? outwardNormal : -1.0f * outwardNormal;
}

vec2 getTotalTextureCoordinate(uvec3 triIndices, vec2 uv) {
  return (1.0f - uv.x - uv.y) * vertices[triIndices.x].textCoord + uv.x * vertices[triIndices.y].textCoord + uv.y * vertices[triIndices.z].textCoord;
}