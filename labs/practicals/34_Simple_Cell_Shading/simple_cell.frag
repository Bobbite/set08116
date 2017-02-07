#version 440

// *********************************
// Declare 1D sampler
uniform sampler1D celTex;
// *********************************

// Incoming texture coordinate
layout(location = 0) in float tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Sample colour from 1D texture
  colour = texture(celTex, tex_coord);
  // *********************************
}