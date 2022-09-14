in highp vec4 vert;
in highp vec3 norm;
in highp vec2 tex;
in highp vec3 col;

out highp vec4 fragColor;
uniform highp vec3 lightPos;


uniform int  enableLighting;

void main() {

    highp vec3 c = col;
    if (enableLighting != 0 ) {
       // Sphere norm  (simple)
       vec3 n =  norm;
       // Extract normal from texture
       //vec3 n = texture(samplerNormal, tex).rgb;
       //if (gl_FrontFacing) n = -n;
       highp vec3 L = normalize(lightPos - vert.xyz);
       highp float NL = max(dot(normalize(norm), L), 0.0);
       c = clamp(c * 0.2 + c * 0.8 * NL, 0.0, 1.0);
       //c = n;
    }

    fragColor = vec4(c, 1.0);

}
