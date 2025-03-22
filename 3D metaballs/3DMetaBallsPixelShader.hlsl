cbuffer CameraBuffer : register(b0)
{
    float3 camera_position;
    float3 lookAt_target;
    float3 up_vector;
};

cbuffer ResolutionBuffer : register(b1)
{
    float2 Resolution; // (width, height)
    int SphereCounter; // save place 
};


struct Metaball
{
	
    float3 center;
    float radius;
    float4 color;

};

StructuredBuffer<Metaball> Spheres : register(t0);


// Define objects with their color
struct Point
{
    float distance;
    float3 color;
    float light; // 1.0 = Use Phong Lighting, 0.0 = Flat color
};



// Function to compute the metaball field
float metaballField(float3 p, float3 center, float r)
{
    float distSq = dot(p - center, p - center);
    return (r * r) / distSq;
}

// Signed distance function for a plane
float sdPlane(float3 p, float3 n, float h)
{
    return dot(p, n) + h;
}

// 
float floorInBox(float3 p, float3 boxSize, float3 normal, float3 vecDir)
{
    float plane = 100000.0;
    float height = boxSize.y;
    
    if (vecDir.y == 1.0)
    {
        plane = sdPlane(p, normal, boxSize.y);
        if (abs(p.x) > boxSize.x || abs(p.z) > boxSize.z || plane < 0.0)
        {
            return 100000.0; // Return a large value outside the box bounds
        }
    }
    else if (vecDir.x == 1.0)
    {
        // Constrain the wall within the box's y and z bounds
        plane = sdPlane(p, normal, boxSize.x);
        if (abs(p.y) > boxSize.y || abs(p.z) > boxSize.z || plane < 0.0)
        {
            return 100000.0; // Large value outside the bounds
        }
    }
    else
    {
        plane = sdPlane(p, normal, boxSize.z);
        if (abs(p.y) > boxSize.y || abs(p.x) > boxSize.x || plane < 0.0)
        {
            return 100000.0; // Large value outside the bounds
        }
    }
    
    return plane;
}

// SDF for a box frame
float sdBoxFrame(float3 p, float3 b, float e)
{
    p = abs(p) - b;
    float3 q = abs(p + e) - e;

    return min(min(
        length(max(float3(p.x, q.y, q.z), 0.0)) + min(max(p.x, max(q.y, q.z)), 0.0),
        length(max(float3(q.x, p.y, q.z), 0.0)) + min(max(q.x, max(p.y, q.z)), 0.0)),
        length(max(float3(q.x, q.y, p.z), 0.0)) + min(max(q.x, max(q.y, p.z)), 0.0));
}


// Distance function for the world
Point mapWorld(float3 p)
{
    float threshold = 1.3;
    float sumSpheres = 0; 
    float Sphere = 0;
    float3 accumulatedColor = float3(0,0,0);
    float totalWeight =  0;
    
    float k = 10.0;
    for (int i = 0; i < SphereCounter; i++)
    {
        Sphere = metaballField(p, Spheres[i].center, Spheres[i].radius);
        sumSpheres += Sphere;
        float weight = exp(-k * Sphere);
        accumulatedColor += weight * Spheres[i].color.xyz;
        totalWeight += weight;

    }

    float field = threshold - sumSpheres;
    float3 boxSize = float3(6.0, 4.0, 3.0);
    float box = sdBoxFrame(p, boxSize, 0.15);
    float floor = sdPlane(p, float3(0.0, 1.0, 0.0), boxSize.y);
    
    
    // walls 
    float wallRight = floorInBox(p, boxSize, float3(1.0, 0.0, 0.0), float3(1.0, 0.0, 0.0));
    float roof = floorInBox(p, boxSize, float3(0.0, -1.0, 0.0), float3(0.0, 1.0, 0.0));
    float wallLeft = floorInBox(p, boxSize, float3(-1.0, 0.0, 0.0), float3(1.0, 0.0, 0.0));
    float wallBehind = floorInBox(p, boxSize, float3(0.0, 0.0, 1.0), float3(.0, 0.0, 1.0));

    float minDistance = min(field, min(box, floor));
    
    minDistance = min(minDistance, box);
    minDistance = min(minDistance, floor);
    minDistance = min(minDistance, wallRight);
    minDistance = min(minDistance, wallLeft);
    minDistance = min(minDistance, roof);
    minDistance = min(minDistance, wallBehind);
    
    Point result;

    if (minDistance == field)
    {
        
        result.color = accumulatedColor / totalWeight;
        //
        result.light = 1.0;
    }
    else if (minDistance == box)
    {
        result.color = float3(0.5, 0.3, 1.0);
        result.light = 0.0;
    }
    else
    {
        result.color = float3(0, 1, 0);
        result.light = 0.0;
    }

    result.distance = minDistance;
    return result;
}

// Compute normal using central differences with help of gradient 
float3 calculateNormal(float3 p)
{
    const float3 eps = float3(0.001, 0.0, 0.0);
    float3 normal = float3(
        mapWorld(p + eps.xyy).distance - mapWorld(p - eps.xyy).distance,
        mapWorld(p + eps.yxy).distance - mapWorld(p - eps.yxy).distance,
        mapWorld(p + eps.yyx).distance - mapWorld(p - eps.yyx).distance
    );
    return normalize(normal);
}

// Phong lighting model
float3 phongLighting(float3 normal, float3 viewDir, float3 fragPos)
{
    float3 lightPos1 = float3(5.0, 4.0, 2.0);
    float3 lightColor1 = float3(1.0, 1.0, 1.0);
    
    
    float3 lightPos2 = float3(-5.0, 4.0, 2.0);
    float3 lightColor2 = float3(1.0, 0.5, 0.5);

    float3 ambient = 0.1 * float3(1.0, 1.0, 1.0);

    float3 lightDir1 = normalize(lightPos1 - fragPos);
    float diff1 = max(dot(normal, lightDir1), 0.0);
    float3 diffuse1 = diff1 * lightColor1;
    
    float3 reflectDir1 = reflect(-lightDir1, normal);
    float spec1 = pow(max(dot(viewDir, reflectDir1), 0.0), 32.0);
    float3 specular1 = spec1 * lightColor1;

    float3 lightDir2 = normalize(lightPos2 - fragPos);
    float diff2 = max(dot(normal, lightDir2), 0.0);
    float3 diffuse2 = diff2 * lightColor2;

    float3 reflectDir2 = reflect(-lightDir2, normal);
    float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), 32.0);
    float3 specular2 = spec2 * lightColor2;

    return ambient + diffuse1 + specular1 + diffuse2 + specular2;
}


// Ray marching function
float4 rayMarch(float3 ro, float3 rd)
{
    float totalDist = 0.0;
    const int MAX_STEPS = 32;
    const float HIT_THRESHOLD = 0.1;
    const float MAX_DIST = 10000.0;

    for (int i = 0; i < MAX_STEPS; i++)
    {
        float3 pos = ro + totalDist * rd;
        Point pStruct = mapWorld(pos);

        if (pStruct.distance < HIT_THRESHOLD)
        {
            if (pStruct.light == 1.0)
            {
                float3 normal = calculateNormal(pos);
                float3 light = phongLighting(normal, rd, pos);
                return float4(pStruct.color * light, 1.0);
            }
            return float4(pStruct.color, 1.0);
        }

        if (totalDist > MAX_DIST)
            break;

        totalDist += pStruct.distance;
    }

    // Background gradient
    float t = 0.5 * (rd.y + 1.0);
    float3 color = lerp(float3(0.0, 0.0, 0.5), float3(0.5, 0.7, 1.0), t);
    return float4(color, 1.0);
}


struct PS_INPUT
{
    float4 pos : SV_POSITION;
    
};


// Pixel shader entry point
float4 main(PS_INPUT input) : SV_Target
{
    float2 uv = (input.pos.xy / Resolution) * 2.0 - 1.0;
    uv.x *= (Resolution.x / Resolution.y); // Aspect ratio fix
    uv.y = -uv.y; // Flip Y to correct DirectX's flipped coordinate system // might have with vertex positions 

    float3 z = normalize(camera_position - lookAt_target);
    float3 x = normalize(cross(up_vector, z));
    float3 y = cross(z, x);

    float3 ro = camera_position;
    float3 rd = normalize(uv.x * x + uv.y * y - z);

    return rayMarch(ro, rd);
}
