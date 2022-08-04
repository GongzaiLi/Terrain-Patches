#version 330

uniform sampler2D heightMap;

uniform sampler2D waterSimple;
uniform sampler2D grassSimple;
uniform sampler2D snowSimple;

uniform float waterHeight;
uniform float snowHeight;


in vec4 eyeNormal;
in vec4 lgtVector;
in vec4 halfWayVector;
in vec2 TexCoord;
in vec3 currentPatchPoint;

// #define WHITE vec4(1.0);

#define AMBIENT_LIGHT_COLOR vec4(0.2)  // grey
#define DIFFUSE_LIGHT_COLOR vec4(1.0)  // 
#define SPECULAR_LIGHT_COLOR vec4(0.4)  // 

out vec4 outputColor;


vec4 snowTexture()
{
    vec4 snow_color = texture(snowSimple, TexCoord);
    vec4 color;
    vec4 ambient_term;
    vec4 diffuse_term;

    ambient_term = AMBIENT_LIGHT_COLOR * snow_color; // L * material
    diffuse_term = snow_color * max(dot(lgtVector, eyeNormal), 0.0); // todo 0 or 0.1
    // todo only water has specular color??

    color = ambient_term + diffuse_term;
    
    return color;
}

vec4 waterTexture()
{
    vec4 water_color = texture(waterSimple, TexCoord);
    vec4 color;
    vec4 ambient_term;
    vec4 diffuse_term;
    vec4 specular_term;

    float height = texture(heightMap, TexCoord).r;
    float shininess = 20.0; // todo

    ambient_term = (1 - min(waterHeight - height * 10.0, 0.7)) * water_color; // todo

    specular_term = SPECULAR_LIGHT_COLOR * pow(max(dot(lgtVector, eyeNormal), 0.0), shininess);
    diffuse_term = water_color * max(dot(lgtVector, eyeNormal), 0.0); // todo 0 or 0.1

    color = ambient_term + diffuse_term + specular_term;
    
    return color;
}

vec4 grassTexture()
{
    vec4 grass_color = texture(grassSimple, TexCoord);

    vec4 color;
    vec4 ambient_term;
    vec4 diffuse_term;
    //vec4 specular_term;

    ambient_term = AMBIENT_LIGHT_COLOR * grass_color; // L * material

    diffuse_term = grass_color * max(dot(lgtVector, eyeNormal), 0.0); // todo 0 or 0.1


    color = ambient_term + diffuse_term;
    
    return color;

}

vec4 grassWithSnowTexture()
{
    vec4 grass_color = texture(grassSimple, TexCoord);
    vec4 snow_color = texture(snowSimple, TexCoord);
    vec4 grass_with_snow_color = mix(snow_color, grass_color, snowHeight - currentPatchPoint.y);

    vec4 color;
    vec4 ambient_term;
    vec4 diffuse_term;

    ambient_term = AMBIENT_LIGHT_COLOR * grass_with_snow_color; // L * material

    diffuse_term = grass_with_snow_color * max(dot(lgtVector, eyeNormal), 0.0); // todo 0 or 0.1

    color = ambient_term + diffuse_term;
    
    return color;
}

/*
vec4 waterWithGrassTexture()
{
    vec4 grass_color = texture(grassSimple, TexCoord);
    vec4 water_color = texture(waterSimple, TexCoord);
    vec4 water_with_grass_color = mix(grass_color, water_color, currentPatchPoint.y - waterHeight);

    vec4 color;
    vec4 ambient_term;
    vec4 diffuse_term;
    vec4 specular_term;

    float height = texture(heightMap, TexCoord).r;
    float shininess = 20.0; // todo

    ambient_term = (1 - min(waterHeight - height * 10.0, 0.7)) * water_with_grass_color; // todo

    specular_term = SPECULAR_LIGHT_COLOR * pow(max(dot(lgtVector, eyeNormal), 0.0), shininess);
    diffuse_term = water_with_grass_color * max(dot(lgtVector, eyeNormal), 0.0); // todo 0 or 0.1

    color = ambient_term + diffuse_term + specular_term;
    
    return color;

}
*/

void main() 
{
     // float waterWithGrassLevel = 1.0;
     float grassWithSnowLevel = 1.0;
     
     float point_level = currentPatchPoint.y;
     

     if (point_level >= snowHeight) {
        outputColor = snowTexture();

     } else if (point_level > waterHeight) { // waterWithGrassLevel
        
        if (point_level >= snowHeight - grassWithSnowLevel) {
            outputColor = grassWithSnowTexture();
        } else {
            outputColor = grassTexture();
        }

     } else {
        outputColor = waterTexture();
     }

     //gl_FragColor = vec4(0, 0, 1, 1);
     
}
