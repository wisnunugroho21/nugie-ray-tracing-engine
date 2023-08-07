// ------------- Struct ------------- 

// ---------------------- buffer struct ----------------------

struct Vertex {
  vec4 position;
  vec4 textCoord;
};

struct Primitive {
  uvec4 primitiveMaterialIndex;
};

struct Object {
  uvec4 bvhPrimitiveTransformIndex;
};

struct TriangleLight {
  uvec4 indices;
  vec4 color;
};

struct PointLight {
  vec3 position;
  vec3 color;
};

struct SunLight {
  vec3 direction;
  vec3 color;
};

struct BvhNode {
  uvec4 nodeObjIndex;
  vec4 maximum;
  vec4 minimum;
};

struct Material {
  vec4 baseColor;
  vec4 baseNormal;
  vec4 params;
  uvec4 textureIndex;
};

struct Transformation {
  mat4 pointMatrix;
  mat4 dirMatrix;
  mat4 pointInverseMatrix;
  mat4 dirInverseMatrix;
  mat4 normalMatrix;
};

struct Pixel {
  uvec4 coord;
};

// ---------------------- internal struct ----------------------

struct Ray {
  vec3 origin;
  vec3 direction;
};

struct FaceNormal {
  bool frontFace;
  vec3 normal;
};

struct HitRecord {
  bool isHit;
  uint hitIndex;
  vec3 point;

  float t;

  vec3 color;
  vec3 normal;
  float metallicness;
  float roughness;
  float fresnelReflect;
};

struct ShadeRecord {
  vec3 radiance;  
  Ray nextRay;
  float pdf;
};

float pi = 3.14159265359;
float FLT_MAX = 3.402823e+38;
float FLT_MIN = 1.175494e-38;