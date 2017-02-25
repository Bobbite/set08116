#version 440

// Sampler used to get texture colour
uniform sampler2D tex;

// Point light information
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

/*
// Spot light data
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};
*/

// Material data
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Spot light being used in the scene
uniform point_light point;
// Material of the object being rendered
uniform material mat;
// Position of the eye (camera)
uniform vec3 eye_pos;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 1) in vec3 normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;
// Outgoing colour
layout(location = 0) out vec4 out_colour;

void main() {
  // *********************************
  // Set out colour to sampled texture colour
 // Calculate direction to the light
  vec3 light_dir = normalize(point.position - position);
  // Calculate distance to light
  float d = distance(point.position, position);
  // Calculate attenuation value
  float att = point.constant + point.linear * d + point.quadratic * pow(d, 2);
  // Calculate spot light intensity
  //float intensity = pow(max(dot(-1 * spot.direction, light_dir), 0.0f), spot.power);
  // Calculate light colour
  vec4 light_colour = (1 / att) * point.light_colour;
  // Calculate view direction
  vec3 view_dir = normalize(eye_pos - position);
  // standard phong shading but using calculated light colour and direction
  // no ambient
  vec4 diffuse = max(dot(normal, light_dir), 0.0f) * (mat.diffuse_reflection * light_colour);
  vec3 half_vec = normalize(light_dir + view_dir);
  vec4 specular = pow(max(dot(half_vec, normal), 0.0f), mat.shininess) * (light_colour * mat.specular_reflection);
  vec4 tex_col = texture(tex, tex_coord);
  vec4 primary = mat.emissive + diffuse;
  vec4 finalColour = primary * tex_col + specular;
  finalColour.a = 1.0f;
  out_colour = finalColour;
  // *********************************
}