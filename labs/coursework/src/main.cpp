#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> boxes;
map<string, mesh> walls;
map<string, mesh> barrels;

geometry geom;
effect eff;
//target_camera cam;
free_camera freeCam;
texture tex, texBox;
array<texture, 5> texs;
//cursor pos
double cursor_x = 0.0;
double cursor_y = 0.0;

//variables
bool isFreeCamOn = false;

bool initialise() {
	// Set input mode - hide the cursor
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Capture initial mouse position
	glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);

	return true;
}


bool load_content() {
	// Create plane mesh
	//meshes["plane"] = mesh(geometry_builder::create_plane());
	// Create scene
	boxes["box1"] = mesh(geometry_builder::create_box());
	boxes["box2"] = mesh(geometry_builder::create_box());
	boxes["box3"] = mesh(geometry_builder::create_box());

	mesh floor = mesh(geometry_builder::create_box());
	walls["wallLeft"] = mesh(geometry_builder::create_box());
	walls["wallRight"] = mesh(geometry_builder::create_box());
	walls["wallBack"] = mesh(geometry_builder::create_box());

	barrels["barrel1"] = mesh(geometry_builder::create_cylinder());

	// Transform objects
	boxes["box1"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	boxes["box1"].get_transform().translate(vec3(-10.0f, 3.5f, -30.0f));

	boxes["box2"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	boxes["box2"].get_transform().translate(vec3(-8.8f, 8.5f, -30.0f));
	boxes["box2"].get_transform().rotate(vec3(0.0f, 1.2f, 0.0f));
	
	boxes["box3"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	boxes["box3"].get_transform().translate(vec3(-4.1f, 3.5f, -30.0f));
	boxes["box3"].get_transform().rotate(vec3(0.0f, -1.2f, 0.0f));

	barrels["barrel1"].get_transform().scale = vec3(5.0f, 7.0f, 5.0f);

	//Walls and floor
	floor.get_transform().scale = vec3(50.0f, 1.0f, 75.0f);
	floor.get_transform().position = vec3(0.0f, 0.5f, 0.0f);

	walls["wallLeft"].get_transform().scale = vec3(1.0f, 28.0f, 75.0f);
	walls["wallLeft"].get_transform().position = vec3(-25.0f, 14.0f, 0.0f);

	walls["wallRight"].get_transform().scale = vec3(1.0f, 28.0f, 75.0f);
	walls["wallRight"].get_transform().position = vec3(25.0f, 14.0f, 0.0f);
	
	walls["wallBack"].get_transform().scale = vec3(50.0f, 28.0f, 1.0f);
	walls["wallBack"].get_transform().position = vec3(0.0f, 14.0f, -37.0f);

	// Load texture
	texs[0] = texture("textures/stone.jpg");
	texs[1] = texture("textures/crate.jpg");
	texs[2] = texture("textures/barrel2.jpg");

	// Load in shaders
	eff.add_shader("shaders/simple_texture.vert", GL_VERTEX_SHADER); 
	eff.add_shader("shaders/simple_texture.frag", GL_FRAGMENT_SHADER);
	// Build effect
	eff.build();

	// Set camera properties
	freeCam.set_position(vec3(0.0f, 15.0f, 50.0f));
	freeCam.set_target(vec3(0.0f, 0.0f, 0.0f));
	freeCam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	return true;
}


bool update(float delta_time) {
	// Update the camera

	#pragma region free camera
	// The ratio of pixels to rotation
	static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
	static double ratio_height =
		(quarter_pi<float>() *
		(static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) /
		static_cast<float>(renderer::get_screen_height());

	double current_x;
	double current_y;
	// *********************************
	// Get the current cursor position
	glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);
	// Calculate delta of cursor positions from last frame
	float delta_x = current_x - cursor_x;
	float delta_y = current_y - cursor_y;


	// Multiply deltas by ratios - gets actual change in orientation
	delta_x *= ratio_width;
	delta_y *= ratio_height;

	// Rotate cameras by delta
	// delta_y - x-axis rotation
	// delta_x - y-axis rotation
	freeCam.rotate(delta_x, -delta_y);

	// Use keyboard to move the camera - WSAD
	vec3 movement;
	if (glfwGetKey(renderer::get_window(), 'W')) {
		movement = vec3(0.0f, 0.0f, 25.0f) * delta_time;
	}
	if (glfwGetKey(renderer::get_window(), 'S')) {
		movement = vec3(0.0f, 0.0f, -25.0f) * delta_time;
	}
	if (glfwGetKey(renderer::get_window(), 'A')) {
		movement = vec3(-25.0f, 0.0f, 0.0f) * delta_time;
	}
	if (glfwGetKey(renderer::get_window(), 'D')) {
		movement = vec3(25.0f, 0.0f, 0.0f) * delta_time;
	}
	// Move camera
	freeCam.move(movement);
	// Update the camera
	freeCam.update(delta_time);
	// Update cursor pos
	cursor_x = current_x;
	cursor_y = current_y;
#pragma endregion 

	return true;
}

bool render() {
	#pragma region boxes render
	for (auto &e : boxes) {
		auto b = e.second;
		// Bind effect
		renderer::bind(eff);
		// Create MVP matrix
		auto M = b.get_transform().get_transform_matrix();
		auto V = freeCam.get_view();
		auto P = freeCam.get_projection();
		auto MVP = P * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		// Bind texture to renderer
		renderer::bind(texs[1], 0);
		// Set the texture value for the shader here
		glUniform1i(eff.get_uniform_location("tex"), 0);
		// Render mesh
		renderer::render(b);
	}
#pragma endregion

	#pragma region walls render
	//render the walls and assign them a texture
	for (auto &e : walls) {
		auto w = e.second;
		// Bind effect
		renderer::bind(eff);
		// Create MVP matrix
		auto M = w.get_transform().get_transform_matrix();
		auto V = freeCam.get_view();
		auto P = freeCam.get_projection();
		auto MVP = P * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		// Bind texture to renderer
		renderer::bind(texs[0], 1);
		// Set the texture value for the shader here
		glUniform1i(eff.get_uniform_location("tex"), 1);
		// Render mesh
		renderer::render(w);
	}
#pragma endregion

	#pragma region barrels render
	//render the barrels and assing them a texture
	for (auto &e : barrels) {
		auto b = e.second;
		// Bind effect
		renderer::bind(eff);
		// Create MVP matrix
		auto M = b.get_transform().get_transform_matrix();
		auto V = freeCam.get_view();
		auto P = freeCam.get_projection();
		auto MVP = P * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		// Bind texture to renderer
		renderer::bind(texs[2], 2);
		// Set the texture value for the shader here
		glUniform1i(eff.get_uniform_location("tex"), 2);
		// Render mesh
		renderer::render(b);
	}
#pragma endregion

	return true;
}

void main() {
	// Create application
	app application("Graphics Coursework");
	// Set load content, update and render methods
	application.set_initialise(initialise);
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}