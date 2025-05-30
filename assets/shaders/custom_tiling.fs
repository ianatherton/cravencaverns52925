#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// Custom tiling factors
uniform vec2 tiling = vec2(1.0, 3.0);  // x = horizontal tiling, y = vertical tiling

void main()
{
    // Apply custom tiling to texture coordinates
    vec2 tiledTexCoord = vec2(fragTexCoord.x * tiling.x, fragTexCoord.y * tiling.y);
    
    // Get texel color from texture
    vec4 texelColor = texture(texture0, tiledTexCoord);
    
    // Apply color tint
    finalColor = texelColor*colDiffuse;
}
