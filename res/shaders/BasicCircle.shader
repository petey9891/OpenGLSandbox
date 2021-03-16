#shader vertex
#version 100
attribute vec3 pos;
attribute vec2 coord;

// Data to send from the vertex shader to the fragment shader
varying vec2 backgroundCoord;

void main() {
    backgroundCoord = coord;
    gl_Position = vec4(pos, 1.0);
}

#shader fragment
#version 100
const int CORES = 8;

uniform float time;

// ring color
uniform vec3 u_ringColor;
vec3 ringColor = vec3(0.7, 1.0, 0.9);

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

    // ring
    ringColor = smoothstep(50.0, 0.0, threadf)*ringColor;
    ringColor *= circle(coords, 0.25, 0.01);

    gl_FragColor = vec4(ringColor, 1.0);
}
