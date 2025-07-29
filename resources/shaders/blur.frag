#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D inputTexture;
uniform float blurRadius;
uniform vec2 textureSize;
uniform float time;

// Gaussian blur implementation optimized for real-time performance
vec4 gaussianBlur(sampler2D tex, vec2 uv, float radius) {
    vec2 texelSize = 1.0 / textureSize;
    vec4 result = vec4(0.0);
    float totalWeight = 0.0;
    
    // Optimized 9-tap Gaussian blur
    float weights[9] = float[](
        0.0947416, 0.118318, 0.0947416,
        0.118318,  0.147761, 0.118318,
        0.0947416, 0.118318, 0.0947416
    );
    
    vec2 offsets[9] = vec2[](
        vec2(-1.0, -1.0), vec2(0.0, -1.0), vec2(1.0, -1.0),
        vec2(-1.0,  0.0), vec2(0.0,  0.0), vec2(1.0,  0.0),
        vec2(-1.0,  1.0), vec2(0.0,  1.0), vec2(1.0,  1.0)
    );
    
    for (int i = 0; i < 9; i++) {
        vec2 sampleUV = uv + offsets[i] * texelSize * radius;
        result += texture(tex, sampleUV) * weights[i];
        totalWeight += weights[i];
    }
    
    return result / totalWeight;
}

// High-quality separable Gaussian blur for better performance
vec4 separableGaussianBlur(sampler2D tex, vec2 uv, float radius, vec2 direction) {
    vec2 texelSize = 1.0 / textureSize;
    vec4 result = vec4(0.0);
    
    // 5-tap separable Gaussian
    float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
    
    result += texture(tex, uv) * weights[0];
    
    for (int i = 1; i < 5; i++) {
        vec2 offset = direction * texelSize * float(i) * radius;
        result += texture(tex, uv + offset) * weights[i];
        result += texture(tex, uv - offset) * weights[i];
    }
    
    return result;
}

void main()
{
    vec2 uv = TexCoord;
    
    // Apply blur effect
    vec4 blurredColor;
    
    if (blurRadius > 0.0) {
        // Use separable blur for better performance
        // This would typically be done in two passes, but simplified here
        blurredColor = separableGaussianBlur(inputTexture, uv, blurRadius, vec2(1.0, 0.0));
        blurredColor = separableGaussianBlur(inputTexture, uv, blurRadius, vec2(0.0, 1.0));
    } else {
        blurredColor = texture(inputTexture, uv);
    }
    
    FragColor = blurredColor;
}
