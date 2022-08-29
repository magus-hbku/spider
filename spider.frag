in highp vec4 vert;
in highp vec3 norm;
in highp vec2 tex;
in highp vec3 col;

out highp vec4 fragColor;
uniform highp vec3 lightPos;

uniform int  depthColor;
uniform int  enableLighting;
uniform int  pickedObjectCount;
uniform float blendFactor;


#define MAXPICKOBJECTS 8
uniform vec3 pickColor[MAXPICKOBJECTS];

layout(binding = 1) uniform sampler2D samplerDepth;
layout(binding = 2) uniform sampler2D samplerSeg;
layout(binding = 3) uniform sampler2D samplerColor;
layout(binding = 4) uniform sampler2D samplerNormal;


void main() {
    highp vec3 c = col;
    if (enableLighting != 0 ) {
       // Sphere norm  (simple)
       // vec3 n =  norm;
       // Extract normal from texture
       vec3 n = texture(samplerNormal, tex).rgb;
       //if (gl_FrontFacing) n = -n;
       highp vec3 L = normalize(lightPos - vert.xyz);
       highp float NL = max(dot(normalize(norm), L), 0.0);
       c = clamp(c * 0.2 + c * 0.8 * NL, 0.0, 1.0);
       //c = n;
    }


    vec3 yellow = vec3(1.0f, 207.0f/255.0f, 32.0f/255.0f);
    vec3 green = vec3(16.0f/255.0f, 165.0f/255.0f, 61.0f/255.0f);
    vec3 light_blue = vec3(47.0f/255.0f, 154.0f/255.0f, 160.0f/255.0f);
    vec3 dark_blue = vec3(58.0f/255.0f, 94.0f/255.0f, 140.0f/255.0f);
    vec3 purple = vec3(84.0f/255.0f, 19.0f/255.0f, 82.0f/255.0f);

    if (depthColor != 0) {
       float depth_norm = texture(samplerDepth, tex).r;
       float t  = 0.0f;
       if (depth_norm < 0.0f) {
           c = vec3(1.0f, 0.0f, 0.0f);
       } else  if (depth_norm < 0.03f) {
           t = depth_norm / 0.03f;
           c = (1.0f - t) * purple + t * dark_blue;
       } else if ( depth_norm < 0.06f) {
           t = (depth_norm - 0.03f) / 0.03f;
           c = (1.0f - t) * dark_blue + t * light_blue;
       } else if ( depth_norm < 0.09f) {
           t = (depth_norm - 0.06f) / 0.03f;
           c = (1.0f - t) * light_blue + t * green;
       } else {
           t = (depth_norm - 0.09f) / 0.03f;
           c = (1.0f - t) * green + t * yellow;
       }
       //c = vec3(depth_norm, 0.0f, 0.0f);
    }

    fragColor = vec4(c, 1.0);
    vec3 frag_seg_color = texture(samplerSeg, tex).rgb;

   if (pickedObjectCount > 0) {
       int found_picked = 0;
       for(int i = 0; found_picked == 0 && i < pickedObjectCount; ++i) {
           vec3 v = frag_seg_color - pickColor[i];
           if ( dot(v,v) < 1.0e-6) {
               found_picked = 1;
           }
       }
       if ( found_picked != 0 ) {
           vec4 c = vec4( texture(samplerColor, tex).rgb, 1.0);
           fragColor = blendFactor * c + (1.0f - blendFactor) * vec4( frag_seg_color, 1.0);
           //fragColor = vec4(pickColor[0],1.0);   //vec4(frag_seg_color, 1.0f);
       }
       //fragColor = vec4(pickColor[pickedObjectCount-1],1.0);
   }

}
