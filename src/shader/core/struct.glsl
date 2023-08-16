// ------------- Struct ------------- 

// ---------------------- buffer struct ----------------------

struct Vertex {
  vec3 position;
  vec2 textCoord;
};

struct Primitive {
  uvec3 indices;
  uint materialIndex;
};

struct Object {
  uint firstBvhIndex;
  uint firstPrimitiveIndex;
  uint transformIndex;
};

struct TriangleLight {
  uvec3 indices;
  vec3 color;
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
  uint leftNode;
  uint rightNode;
  uint leftObjIndex;
  uint rightObjIndex;

  vec3 maximum;
  vec3 minimum;
};

struct Material {
  vec3 baseColor;
	float metallicness;
  float roughness;
  float fresnelReflect;

  uint colorTextureIndex;
  uint normalTextureIndex;
};

struct Transformation {
  mat4 pointMatrix;
  mat4 dirMatrix;
  mat4 pointInverseMatrix;
  mat4 dirInverseMatrix;
  mat4 normalMatrix;
};

struct Ray {
  vec3 origin;
  vec3 direction;
};

struct RayData {
  Ray ray; 
  float dirMin;
  vec3 dirMax;

  float isPrimary;
};

struct HitRecord {
  bool isHit;
  uint hitIndex;

  vec3 point;
  vec3 dir;

  vec3 normal;
  uint materialIndex;

  float isPrimary;
};

struct IndirectShadeRecord {
  vec3 radiance;
  float pdf;
  Ray nextRay;
};

struct DirectShadeRecord {
  vec3 radiance;
  float pdf;
};

struct HitLightRecord {
  vec3 radiance;
};

struct MissRecord {
  vec3 radiance;
};

struct SamplerData {
  uint xCoord;
  uint yCoord;
  Ray nextRay;
};

// ---------------------- internal struct ----------------------

float pi = 3.14159265359;
float FLT_MAX = 1.0e+12;
float FLT_MIN = 1.0e-12;