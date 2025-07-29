#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D inputTexture;
uniform vec2 rippleCenter;
uniform float rippleRadius;
uniform float rippleStrength;
uniform float time;
uniform vec2 textureSize;

// Fluent Design ripple effect with realistic wave propagation
vec2 rippleEffect(vec2 uv, vec2 center, float radius, float strength, float t) {
    vec2 diff = uv - center;
    float distance = length(diff);
    
    // Normalize distance by texture size for consistent behavior
    distance *= max(textureSize.x, textureSize.y);
    
    // Wave propagation with damping
    float wave = 0.0;
    if (distance < radius && distance > 0.0) {
        // Multiple wave frequencies for more realistic ripple
        float normalizedDist = distance / radius;
        float dampening = 1.0 - normalizedDist;
        
        // Primary wave
        wave += sin((distance - t * 300.0) * 0.05) * dampening;
        
        // Secondary wave for more detail
        wave += sin((distance - t * 200.0) * 0.08) * dampening * 0.5;
        
        // Tertiary wave for fine detail
        wave += sin((distance - t * 150.0) * 0.12) * dampening * 0.25;
        
        wave *= strength;
    }
    
    // Apply distortion
    vec2 normal = normalize(diff);
    return uv + normal * wave * 0.01;
}

// Advanced ripple with multiple wave sources
vec2 multiRippleEffect(vec2 uv) {
    vec2 distortedUV = uv;
    
    // Primary ripple
    distortedUV = rippleEffect(distortedUV, rippleCenter, rippleRadius, rippleStrength, time);
    
    // Add secondary ripples for more complex interaction
    vec2 secondaryCenter = rippleCenter + vec2(0.1, 0.1);
    distortedUV = rippleEffect(distortedUV, secondaryCenter, rippleRadius * 0.7, 
                              rippleStrength * 0.5, time + 0.5);
    
    return distortedUV;
}

// Chromatic aberration effect for enhanced visual impact
vec4 chromaticAberration(sampler2D tex, vec2 uv, float strength) {
    vec2 offset = vec2(strength * 0.002);
    
    float r = texture(tex, uv + offset).r;
    float g = texture(tex, uv).g;
    float b = texture(tex, uv - offset).b;
    float a = texture(tex, uv).a;
    
    return vec4(r, g, b, a);
}

void main()
{
    vec2 uv = TexCoord;
    
    // Apply ripple distortion
    vec2 distortedUV = multiRippleEffect(uv);
    
    // Sample the texture with distortion
    vec4 color = texture(inputTexture, distortedUV);
    
    // Add chromatic aberration near ripple center for enhanced effect
    float distanceFromCenter = length(uv - rippleCenter);
    float aberrationStrength = rippleStrength * (1.0 - smoothstep(0.0, rippleRadius, distanceFromCenter));
    
    if (aberrationStrength > 0.01) {
        color = chromaticAberration(inputTexture, distortedUV, aberrationStrength);
    }
    
    // Add subtle glow effect at ripple edge
    float edgeGlow = 0.0;
    if (distanceFromCenter < rippleRadius && distanceFromCenter > rippleRadius * 0.8) {
        float edgeFactor = 1.0 - abs(distanceFromCenter - rippleRadius * 0.9) / (rippleRadius * 0.1);
        edgeGlow = edgeFactor * rippleStrength * 0.3;
    }
    
    // Combine effects
    color.rgb += vec3(edgeGlow);
    
    FragColor = color;
}
