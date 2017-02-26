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
}

;
// Spot light data
struct spot_light {
    vec4 light_colour;
    vec3 position;
    vec3 direction;
    float constant;
    float linear;
    float quadratic;
    float power;
}

;
// Material data
struct material {
    vec4 emissive;
    vec4 diffuse_reflection;
    vec4 specular_reflection;
    float shininess;
}

;
// Spot light being used in the scene
uniform spot_light spots[1];
uniform point_light points[2];
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


vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_col)
{
    // Calculate direction to the light
  vec3 light_dir = normalize(spot.position - position);
    // Calculate distance to light
  float d = distance(spot.position, position);
    // Calculate attenuation value
  float att = 1.0f/ (spot.constant + spot.linear * d + spot.quadratic * pow(d, 2));
    // Calculate spot light intensity
  float intensity = pow(max(dot(-1.0f * spot.direction, light_dir), 0.0f), spot.power);
    // Calculate light colour
  vec4 light_colour = intensity* att * spot.light_colour;
    // standard phong shading but using calculated light colour and direction
  // no ambient
  vec4 diffuse = max(dot(normal, light_dir), 0.0f) * (mat.diffuse_reflection * light_colour);
    vec3 half_vec = normalize(light_dir + view_dir);
    vec4 specular = pow(max(dot(half_vec, normal), 0.0f), mat.shininess) * (light_colour * mat.specular_reflection);
    vec4 primary = mat.emissive + diffuse;    
   vec4 out_colour_spot = primary * tex_col + specular;
	out_colour_spot.a = 1.0f;
    return out_colour_spot;
}



void main() {
    out_colour = vec4(0.0, 0.0, 0.0, 1.0);
    // Calculate view direction
  vec3 view_dir = normalize(eye_pos - position);
    // Sample texture
  vec4 tex_col = texture(tex, tex_coord);

  /*
    // Sum point lights
  for (int i = 0; i < 4; ++i)
  {
        out_colour += calculate_point(points[i], mat, position, normal, view_dir, tex_col);
    }
	*/

  // Sum spot lights
  for (int i = 0; i < 1; ++i)
  {
        out_colour += calculate_spot(spots[i], mat, position, normal, view_dir, tex_col);
  }

    }