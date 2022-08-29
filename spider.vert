layout(location = 0) in vec4 vertex;
layout(location = 1) in vec2 texel;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 color;

out vec4 vert;
out vec2 tex;
out vec3 norm;
out vec3 col;

uniform int  depthEnabled;
uniform int  textureEnabled;


uniform float depthMax;
uniform mat4 projMatrix;
uniform mat4 camMatrix;
uniform mat4 worldMatrix;

layout(binding = 0) uniform sampler2D sampler;
layout(binding = 1) uniform sampler2D samplerDepth;

void main() {

   vec3 c = color;
   vec4 v = vertex;
   vec3 n = normal;

   if (textureEnabled != 0) {
      c.rgb =  texture(sampler, texel).rgb;
   }

   if (depthEnabled != 0) {
       float depth_norm = texture(samplerDepth, texel).r;
       v.xyz *= depth_norm * depthMax;
   }
   //color *= depth_norm;


   norm =  mat3(transpose(inverse(worldMatrix))) * n;
   col = c;
   tex = texel;
   //vert = projMatrix * camMatrix * worldMatrix * v;
   gl_Position = projMatrix * camMatrix * worldMatrix * v;
}
