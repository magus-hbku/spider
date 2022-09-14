layout(location = 0) in vec4 vertex;
layout(location = 1) in vec2 texel;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 color;

out vec4 vert;
out vec2 tex;
out vec3 norm;
out vec3 col;

uniform int pickedObjectCount;
uniform int  depthEnabled;
uniform int  textureEnabled;
uniform int  normalEnabled;
uniform int  full;

uniform float depthMax;
uniform mat4 projMatrix;
uniform mat4 camMatrix;
uniform mat4 worldMatrix;




#define MAXPICKOBJECTS 8
uniform vec3 pickColor[MAXPICKOBJECTS];


layout(binding = 0) uniform sampler2D sampler;
layout(binding = 1) uniform sampler2D samplerDepth;
layout(binding = 2) uniform sampler2D samplerEmpty;
layout(binding = 3) uniform sampler2D samplerDepthEmpty;
layout(binding = 4) uniform sampler2D samplerNorm;
layout(binding = 5) uniform sampler2D samplerNormEmpty;
layout(binding = 6) uniform sampler2D samplerSeg;


void main() {

   vec3 c = color;
   vec4 v = vertex;
   vec3 n = normal;

   int found_picked = 0;
   if (full == 0 && pickedObjectCount > 0) {
       vec3 frag_seg_color = texture(samplerSeg, texel).rgb;
       for(int i = 0; found_picked == 0 && i < pickedObjectCount; ++i) {
           vec3 v = frag_seg_color - pickColor[i];
           if ( dot(v,v) < 1.0e-6) {
               found_picked = 1;
           }
       }
   }

   if (textureEnabled != 0) {
      if (full != 0 || found_picked != 0) {
         c.rgb =  texture(sampler, texel).rgb;
      } else {
         c.rgb =  texture(samplerEmpty, texel).rgb;
      }
   }

   float depth_norm;
   if (depthEnabled != 0) {
       if (full != 0 || found_picked != 0) {
          depth_norm = texture(samplerDepth, texel).r;
       } else {
          depth_norm = texture(samplerDepthEmpty, texel).r;
       }
       v.xyz *= depth_norm * depthMax;
   }
   //color *= depth_norm;

   if (normalEnabled != 0) {
   if (full != 0 || found_picked != 0) {
      n = texture(samplerNorm, texel).rgb;
    } else {
      n = texture(samplerNormEmpty, texel).rgb;
    }
   }

   norm =  mat3(transpose(inverse(worldMatrix))) * n;
   col = c;
   tex = texel;
   //vert = projMatrix * camMatrix * worldMatrix * v;
   gl_Position = projMatrix * camMatrix * worldMatrix * v;
}
