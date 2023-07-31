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
  vec3 baseNormal;

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