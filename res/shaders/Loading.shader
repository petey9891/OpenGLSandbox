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
uniform float time;

varying vec2 backgroundCoord;

// Rotates the circle by an angle
// angle - the angle to rotate the circle by
// note: https://en.wikipedia.org/wiki/Rotation_matrix
mat2 rotate2d(float angle) {
    return mat2(cos(angle), -sin(angle),
                sin(angle),  cos(angle));
}


// Adds the animation for the circle moving back and forth
// v1 - the coordinates of circle
// v2 - the boundtry in which the circle can move away from its coordinates?
// strength - the number of distortions on the circle
// speed - the speed of the animation
float variation(vec2 v1, vec2 v2, float strength, float speed) {
	return sin(
        dot(normalize(v1), normalize(v2)) * strength + time * speed
    ) / 100.0;
}

// Creates a painted circle vec3. Adds a variation animation to the circle.
// uv - the coordinates of the circle
// rad - the circle radius
// width - the circle's stroke
vec3 paintCircle (vec2 uv, float rad, float width) {
    
    float len = length(uv);

    // Adds variation to the top half of the circle
    // len += variation(uv, vec2(0.0, 1.0), 5.0, 1.5);

    // Adds variation to the lower half of the cirlce
    // len -= variation(uv, vec2(1.0, 0.0), 5.0, 1.5);
    
    // perform Hermite interpolation between two values
    float circle = smoothstep(rad-width, rad, len) - smoothstep(rad, rad+width, len);

    return vec3(circle);
}

void main() {
    // multiply by 0.5 to it centered in the cube
    vec2 uv = backgroundCoord.xy * 0.5;
    
    vec3 color;
    float radius = 0.35;

    // Adding adds more layers, multiplying adds colors

    // paint color circle
    // color = paintCircle(uv, radius, 0.1); // this alone creates a very thick white circle
    
    color = paintCircle(uv, radius, 0.01); // this alone creates a very thick white circle

    vec2 v = rotate2d(time) * uv; // matrix multiplication to map rotated coordinates with original coordinates
    color *= vec3(0.0, 0.0, 0.5-(v.x));

    // color with gradient
    // vec2 v = rotate2d(time) * uv;
    // color *= vec3(v.x, v.y, 0.7-v.y*v.x);
    // paint white circle
    // color += paintCircle(uv, radius, 0.01);

    
    
	gl_FragColor = vec4(color, 1.0);
}
