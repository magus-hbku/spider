in highp vec4 vert;
in highp vec3 norm;
in highp vec2 tex;
in highp vec3 col;
in highp vec3 p_wc;
in highp vec3 n_wc;

out highp vec4 fragColor;
uniform highp vec3 lightPos;
uniform int  textureEnabled;

layout(binding = 0) uniform sampler2D sampler;
layout(binding = 2) uniform sampler2D samplerEmpty;

uniform int  enableLighting;

void main() {
    const float pi = 3.14159265359;
    highp vec3 c = col;
    if (enableLighting != 0 ) {

       float pdotn = dot(p_wc,n_wc);
       float delta = sqrt( pdotn - dot(p_wc,p_wc) + 65536.0 * 65536.0);
       float s = max( delta - pdotn, delta + pdotn);
       vec3 p_s = normalize( p_wc + s * n_wc);
       vec2 uv = vec2( 0.5*atan(p_s.z,p_s.x)/pi-0.0+0.5, 1.0-acos(p_s.y)/pi);
       if (uv.x<0) uv.x+=1;
       // Sphere norm  (simple)
       vec3 n =  norm;
       // Extract normal from texture
       //vec3 n = texture(samplerNormal, tex).rgb;
       //if (gl_FrontFacing) n = -n;
       highp vec3 L = normalize(lightPos - vert.xyz);
       highp float NL = max(dot(normalize(norm), L), 0.0);
       // convert normal into 2D tex coord
       highp vec3 env = textureLod(samplerEmpty,uv,5.0).rgb;


       highp vec3 sm = textureLod(samplerEmpty, uv, 3.0).rgb;
       highp vec3 det = textureLod(samplerEmpty, uv, 0.0).rgb;
       highp vec3 del = (det-sm)*2+sm*0.71;
       c = del;//clamp(c * 0.2 + 0.8 * mix(mix(c,vec3(1,1,1),0.7)*env,c*NL.xxx,textureEnabled), 0.0, 1.0);
       //c = n;
    }

    fragColor = vec4(c, 1.0);

}
