#include "../core/boolean.glsl"
#include "basic.glsl"
#include "random.glsl"
#include "../core/ggx.glsl"
#include "../core/material.glsl"
#include "render.glsl"

IndirectShadeRecord indirectLambertShade(vec3 hitPoint, vec3 surfaceColor, vec3 surfaceNormal, uint additionalRandomSeed) {
  IndirectShadeRecord scat;

  scat.nextRay.origin = hitPoint;
  scat.nextRay.direction = lambertRandomDirection(buildOnb(surfaceNormal), additionalRandomSeed);

  float NoL = max(dot(surfaceNormal, normalize(scat.nextRay.direction)), 0.001f);
  float brdf = lambertBrdfValue();
  float pdf = lambertPdfValue(NoL);

  scat.radiance = partialIntegrand(surfaceColor, brdf, NoL); 
  scat.pdf = when_gt(length(scat.radiance), 0.001f) * pdf;
  
  return scat;
}

IndirectShadeRecord indirectLambertShade(HitRecord hit, uint additionalRandomSeed) {
  /* vec3 surfaceColor;
  if (materials[materialIndex].colorTextureIndex == 0u) {
    surfaceColor = materials[materialIndex].baseColor;
  } else {
    surfaceColor = texture(colorTextureSampler[materials[materialIndex].colorTextureIndex - 1u], uv).xyz;
  } */

  vec3 surfaceColor = materials[hit.materialIndex].baseColor;
  return indirectLambertShade(hit.point, surfaceColor, hit.normal, additionalRandomSeed);
}