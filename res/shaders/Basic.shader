#shader vertex
#version 100
attribute vec3 pos;
attribute vec2 coord;
varying vec2 backgroundCoord;
void main() {
    backgroundCoord = coord;
    gl_Position = vec4(pos, 1.0);
}

#shader fragment
#version 100
const int CORES = 8;
const float T1 = 40.0;
const float T2 = 60.0;
const float T3 = 80.0;
uniform float temperature;
uniform float thread[CORES];

// background colors
vec3 bColor = vec3(0.1, 0.6, 0.4);
vec3 bColorWarm = vec3(0.5, 0.5, 0.1);
vec3 bColorHot = vec3(0.6, 0.2, 0.1);

// ring colors
vec3 rColor = vec3(0.7, 1.0, 0.9);
vec3 rColorWarm = vec3(1.0, 1.0, 0.6);
vec3 rColorHot = vec3(1.0, 1.0, 1.0);

uniform float age;
varying vec2 backgroundCoord;

float phi;
float threadf = 0.0;

float circle(vec2 uv, float w0, float width) {
    float f = length(uv) + (sin(normalize(uv).y * 5.0 + time * 2.0) - sin(normalize(uv).x * 5.0 + time * 2.0)) / 100.0;
    float w = width + width*threadf*0.1;
    return smoothstep(w0-w, w0, f) - smoothstep(w0, w0+w, f);
}

void main() {
    vec2 coords = backgroundCoord.xy*0.5;
    float phi = (atan(coords.y, coords.x)+3.1415926538)/3.1415926538*float(CORES)*0.5;
    vec2 background = backgroundCoord.xy * 0.5 * 10.0 - vec2(19.0); // background
    vec2 i = background;
    // float c = 8.0; // 2-10 get darker, less saturated; 8 is almost pitch black; 10-13 seem very dark, more saturated; 13-15 are some deep colors, the smoke seems more intense
    float c = 1.0;
    float inten = 0.05; // background alpha/brightness?

    for (int n = 0; n < 8; n++) {
        float t = time * (0.7 - (0.2 / float(n+1)));
            i = background + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x)); // initial smoothing of waves
            c += 1.0 / length(vec2(background.x / (2.0 * sin(i.x + t) / inten), background.y / (cos(i.y + t) / inten)));
    }

    c /= 8.0;
    c = 1.5 - sqrt(pow(c, 2.0));

    // background
    bColor.g = clamp(coords.x, 0.0, 1.0); // adds the gradient
    bColor = smoothstep(T2, T1, temperature)*bColor + smoothstep(T1, T2, temperature)*smoothstep(T3, T2, temperature)*bColorWarm + smoothstep(T2, T3, temperature)*bColorHot;

    // ring
    rColor = smoothstep(50.0, 0.0, threadf)*rColor + smoothstep(0.0, 50.0, threadf)*smoothstep(100.0, 50.0, threadf)*rColorWarm + smoothstep(50.0, 100.0, threadf)*rColorHot;
    rColor *= circle(coords, 0.25, 0.01);

    // color output
    // multiplying c increases wave intensity
    vec3 outcolor = bColor * c * c * c * c + rColor;

    float coreIndex = 0.0;
    for (int i = 0; i < CORES; i++) {
        threadf += clamp(1.0-abs(phi-coreIndex), 0.0, 1.0)*thread[i];
        coreIndex += 1.0;
    }

    gl_FragColor = vec4(mix(outcolor, outcolor, smoothstep(5.0, 10.0, age)), 1.0);
}