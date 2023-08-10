ShadeRecord indirectLambertShade(vec3 hitPoint, vec3 surfaceColor, vec3 surfaceNormal, uint additionalRandomSeed) {
  ShadeRecord scat;

  scat.nextRay.origin = hitPoint;
  scat.nextRay.direction = lambertRandomDirection(buildOnb(surfaceNormal), additionalRandomSeed);

  float NoL = max(dot(surfaceNormal, normalize(scat.nextRay.direction)), 0.001f);
  float brdf = lambertBrdfValue();
  float pdf = lambertPdfValue(NoL);

  scat.radiance = partialIntegrand(surfaceColor, brdf, NoL); 
  scat.pdf = when_gt(length(scat.radiance), 0.001f) * pdf;
  
  return scat;
}

ShadeRecord indirectLambertShade(HitRecord hit, uint additionalRandomSeed) {
  return indirectLambertShade(hit.point, hit.color, hit.normal, additionalRandomSeed);
}

ShadeRecord directLambertShade(uint lightIndex, vec3 hitPoint, vec3 surfaceColor, vec3 surfaceNormal, uint additionalRandomSeed) {
  ShadeRecord scat;
  Ray shadowRay;

  scat.radiance = vec3(0.0f);
  scat.pdf = 0.01f;

  shadowRay.origin = hitPoint;
  shadowRay.direction = triangleRandomDirection(lights[lightIndex].indices, shadowRay.origin, additionalRandomSeed);

  HitRecord objectHit = hitObjectBvh(shadowRay, 0.001f, FLT_MAX);
  HitRecord lightHit = hitTriangleLightBvh(shadowRay, 0.001f, FLT_MAX);

  if (lightHit.isHit && (!objectHit.isHit || length(lightHit.point - shadowRay.origin) < length(objectHit.point - shadowRay.origin))) {
    vec3 unitLightDirection = normalize(shadowRay.direction);

    float NloL = max(dot(lightHit.normal, -1.0f * unitLightDirection), 0.001f);
    float NoL = max(dot(surfaceNormal, unitLightDirection), 0.001f);

    vec3 vectorDistance = lightHit.point - shadowRay.origin;    

    float brdf = lambertBrdfValue();
    float sqrDistance = dot(vectorDistance, vectorDistance);
    float area = triangleArea(lights[lightHit.hitIndex].indices);
    float pdf = lambertPdfValue(NoL);

    scat.radiance = partialIntegrand(surfaceColor, brdf, NoL) * Gfactor(NloL, sqrDistance, area) * lights[lightHit.hitIndex].color;
    scat.pdf = when_gt(length(scat.radiance), 0.001f) * pdf;
  }

  return scat;
}

ShadeRecord directLambertShade(uint lightIndex, HitRecord hit, uint additionalRandomSeed) {
  return directLambertShade(lightIndex, hit.point, hit.color, hit.normal, additionalRandomSeed);
}

ShadeRecord sunDirectLambertShade(vec3 hitPoint, vec3 surfaceColor, vec3 surfaceNormal, uint additionalRandomSeed) {
  ShadeRecord scat;
  Ray shadowRay;

  scat.radiance = vec3(0.0f);
  scat.pdf = 0.01f;

  shadowRay.origin = hitPoint;
  shadowRay.direction = sunLightRandomDirection(ubo.sunLight);

  HitRecord objectHit = hitObjectBvh(shadowRay, 0.001f, FLT_MAX);
  HitRecord lightHit = hitTriangleLightBvh(shadowRay, 0.001f, FLT_MAX);

  if (!lightHit.isHit && !objectHit.isHit) {
    vec3 unitLightDirection = normalize(shadowRay.direction);
    
    float NoL = max(dot(surfaceNormal, unitLightDirection), 0.001f);
    float brdf = lambertBrdfValue();
    float pdf = lambertPdfValue(NoL);

    scat.radiance = partialIntegrand(surfaceColor, brdf, NoL) * ubo.sunLight.color / pdf;
    scat.pdf = when_gt(length(scat.radiance), 0.001f) * pdf;
  }

  return scat;
}

ShadeRecord sunDirectLambertShade(HitRecord hit, uint additionalRandomSeed) {
  return sunDirectLambertShade(hit.point, hit.color, hit.normal, additionalRandomSeed);
}