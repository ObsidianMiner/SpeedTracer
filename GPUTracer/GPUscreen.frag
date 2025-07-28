
#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D prevFrame;
uniform int sphereCount;
uniform int frameCount;
uniform vec3 backgroundTopColor;
uniform vec3 backgroundBottomColor;
uniform vec3 screenParams;
uniform vec3 camPos;
uniform vec3 camRot;

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct Material
{
    vec4 color;
    vec4 emmisionColor;
    vec4 specularColor;
    float smoothness;
    float specularProbability;
    int type;
    float padding;
};

struct Sphere
{
    vec3 position;
    float radius;
    Material mat;
};

struct Triangle
{
    vec3 posA, PosB, PosC;
    vec3 normalA, normalB, normalC;
};

struct HitInfo
{
    bool hit;
    float dist;
    vec3 hitPoint;
    vec3 normal;
    Material material;
};

layout(std430, binding = 1) readonly buffer sceneData
{
    Sphere spheres[];
};

mat3 rotationMatrixXYZ(vec3 degrees) {
    vec3 radians = radians(degrees); // Convert to radians

    float cx = cos(radians.x);
    float sx = sin(radians.x);
    float cy = cos(radians.y);
    float sy = sin(radians.y);
    float cz = cos(radians.z);
    float sz = sin(radians.z);

    // Rotation matrices for X, Y, Z
    mat3 rotX = mat3(
        1.0, 0.0, 0.0,
        0.0, cx, -sx,
        0.0, sx, cx
    );

    mat3 rotY = mat3(
        cy, 0.0, sy,
        0.0, 1.0, 0.0,
        -sy, 0.0, cy
    );

    mat3 rotZ = mat3(
        cz, -sz, 0.0,
        sz, cz, 0.0,
        0.0, 0.0, 1.0
    );

    // Combined rotation (Z * Y * X)
    return rotZ * rotY * rotX;
}

vec3 rotateVec3Euler(vec3 v, vec3 degrees) {
    return rotationMatrixXYZ(degrees) * v;
}

// PCG (permuted congruential generator). Thanks to:
// www.pcg-random.org and www.shadertoy.com/view/XlGcRh
uint NextRandom(inout uint state)
{
	state = state * 747796405 + 2891336453 + frameCount * 378173;
	uint result = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;
	result = (result >> 22) ^ result;
	return result;
}
float RandomValue(inout uint state)
{
	return NextRandom(state) / 4294967295.0; // 2^32 - 1
}
// Random value in normal distribution (with mean=0 and sd=1)
float RandomValueNormalDistribution(inout uint state)
{
	// Thanks to https://stackoverflow.com/a/6178290
	float theta = 2 * 3.1415926 * RandomValue(state);
	float rho = sqrt(-2 * log(RandomValue(state)));
	return rho * cos(theta);
}
vec3 RandomDirection(inout uint state)
{
	// Thanks to https://math.stackexchange.com/a/1585996
	float x = RandomValueNormalDistribution(state);
	float y = RandomValueNormalDistribution(state);
	float z = RandomValueNormalDistribution(state);
	return normalize(vec3(x, y, z));
}
vec2 RandomPointInCircle(inout uint rngState)
{
	float angle = RandomValue(rngState) * 2 * 3.14159;
	vec2 pointOnCircle = vec2(cos(angle), sin(angle));
	return pointOnCircle * sqrt(RandomValue(rngState));
}

HitInfo RaySphere(Ray ray, vec3 position, float radius)
{
    HitInfo hitinfo;
    hitinfo.hit = false;
    hitinfo.dist = 10000.0;
    
    vec3 oc = ray.origin - position;

    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4.0 * a * c;

    if(discriminant >= 0.0)
    {
        float sqrtDiscriminant = sqrt(discriminant);
        float t1 = (-b - sqrtDiscriminant) / (2.0 * a);
        float t2 = (-b + sqrtDiscriminant) / (2.0 * a);
        
        // Choose the closest positive intersection
        float t = (t1 > 0.001) ? t1 : t2;

        if(t > 0.001) // Small epsilon to avoid self-intersection
        {
            hitinfo.hit = true;
            hitinfo.dist = t;
            hitinfo.hitPoint = ray.origin + ray.direction * t;
            hitinfo.normal = normalize(hitinfo.hitPoint - position);
        }
    }
    return hitinfo;
}

HitInfo CalculateRayCollision(Ray ray)
{
    HitInfo closestHit;
    closestHit.hit = false;
    closestHit.dist = 10000.0;

    for(int i = 0; i < sphereCount; i++)
    {
        Sphere sphere = spheres[i];
        HitInfo hitinfo = RaySphere(ray, sphere.position, sphere.radius);
        
        if(hitinfo.hit && hitinfo.dist < closestHit.dist)
        {
            closestHit = hitinfo;
            closestHit.material = sphere.mat;
        }
    }

    return closestHit;
}

vec3 Trace(Ray ray, uint rngState)
{
    int maxBounceCount = 4;
    vec3 incomingLight = vec3(0);
    vec3 rayColor = vec3(1);

    for(int bounceI = 0; bounceI <= maxBounceCount; bounceI++)
    {
        HitInfo hitInfo = CalculateRayCollision(ray);

        if(hitInfo.hit)
        {
            Material mat = hitInfo.material;

            bool isSpecualarBounce = mat.specularProbability >= RandomValue(rngState);

            ray.origin = hitInfo.hitPoint;
            vec3 diffuseDir = normalize(hitInfo.normal + RandomDirection(rngState));
            vec3 specularDir = reflect(ray.direction, hitInfo.normal);
            ray.direction = mix(diffuseDir, specularDir, isSpecualarBounce);

            vec3 emmitedLight = vec3(mat.emmisionColor);
            incomingLight += emmitedLight * rayColor;
            rayColor *= mix(vec3(mat.color), vec3(mat.specularColor), isSpecualarBounce);

            float p = max(rayColor.r, max(rayColor.g, rayColor.b));
            //if(RandomValue(rngState) >= p) break;
            rayColor *= 1.0 / p;
        }
        else
        {
            // Sky color gradient
            float t = 0.5 * (normalize(ray.direction).y + 1.0);
            incomingLight += mix(backgroundTopColor, backgroundBottomColor, t) * rayColor;
            break;
        }
    }
    return incomingLight;
}

void main()
{
    float fov = 90.0; // Field of view in degrees
    float aspectRatio = screenParams.x / screenParams.y; // Width / Height
    
    // Convert texture coordinates from [0,1] to [-1,1]
    vec2 uv = (TexCoord * 2.0) - 1.0;

    vec2 texUV = TexCoord;
    
    // Apply aspect ratio correction
    uv.x *= aspectRatio;
    
    // Calculate ray direction using field of view
    float fovRadians = radians(fov);
    float tanHalfFov = tan(fovRadians * 0.5);
    
    vec3 rayDir = normalize(vec3(
        uv.x * tanHalfFov,
        uv.y * tanHalfFov,
        1.0
    ));

    rayDir = rotateVec3Euler(rayDir, camRot);

    uint rngState = uint(gl_FragCoord.x + gl_FragCoord.y * 1920.0) * 2878931u;

    Ray ray;
    ray.origin = camPos;
    ray.direction = rayDir;
    
    vec3 currentCol = sqrt(Trace(ray, rngState));
    vec3 previousCol = texture(prevFrame, texUV).rgb;
    vec3 result = (previousCol * float(frameCount - 1) + currentCol) / float(frameCount);
    FragColor = vec4(result, 1.0);
}