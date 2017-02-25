#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> boxes;
map<string, mesh> walls;
map<string, mesh> barrels;
mesh floorMesh;

geometry geom;
effect eff;
//target_camera cam;
free_camera freeCam;
target_camera cam;
texture tex, texBox;
array<texture, 5> texs;

spot_light pointLight1;

//cursor pos
double cursor_x = 0.0;
double cursor_y = 0.0;

//variables
bool isFreeCamOn = false;
float wallHeight = 35.0f;


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

	floorMesh = mesh(geometry_builder::create_box());
	walls["wallLeft"] = mesh(geometry_builder::create_box());
	walls["wallRight"] = mesh(geometry_builder::create_box());
	walls["wallBack"] = mesh(geometry_builder::create_box());

	barrels["barrel1"] = mesh(geometry_builder::create_cylinder());

	// Transform objects
	boxes["box1"].get_transform().scale = vec3(6.0f, 6.0f, 6.0f);
	boxes["box1"].get_transform().translate(vec3(-12.0f, 3.5f, -35.0f));
	//box on top
	boxes["box2"].get_transform().scale = vec3(6.0f, 6.0f, 6.0f);
	boxes["box2"].get_transform().translate(vec3(-10.8f, 9.5f, -35.0f));
	boxes["box2"].get_transform().rotate(vec3(0.0f, 1.2f, 0.0f));
	
	boxes["box3"].get_transform().scale = vec3(6.0f, 6.0f, 6.0f);
	boxes["box3"].get_transform().translate(vec3(-5.5f, 3.5f, -35.0f));
	boxes["box3"].get_transform().rotate(vec3(0.0f, -1.2f, 0.0f));

	barrels["barrel1"].get_transform().scale = vec3(5.0f, 7.0f, 5.0f);
	barrels["barrel1"].get_transform().position = vec3(5.0f, 3.5f, 5.0f);

	//Walls and floor
	floorMesh.get_transform().scale = vec3(75.0f, 1.0f, 100.0f);
	floorMesh.get_transform().position = vec3(0.0f, 0.5f, 0.0f);

	walls["wallLeft"].get_transform().scale = vec3(1.0f, wallHeight, 100.0f);
	walls["wallLeft"].get_transform().position = vec3(-37.0f, 18.5f, 0.0f);

	walls["wallRight"].get_transform().scale = vec3(1.0f, wallHeight, 100.0f);
	walls["wallRight"].get_transform().position = vec3(37.0f, 18.5f, 0.0f);
	
	walls["wallBack"].get_transform().scale = vec3(1.0f, wallHeight, 75.0f)/*(75.0f, wallHeight, 1.0f)*/;
	walls["wallBack"].get_transform().position = vec3(0.0f, 18.5f, -50.0f);
	walls["wallBack"].get_transform().rotate(vec3(0.0f, half_pi<float>(), 0.0f));


	//set materials
	material mat;
	mat.set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f)); //emissive black
	mat.set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f)); //specular white
	mat.set_shininess(1.0f);
	mat.set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	
	//set material to all the boxes and walls
	boxes["box1"].set_material(mat);
	boxes["box2"].set_material(mat);
	boxes["box3"].set_material(mat);
	floorMesh.set_material(mat);
	walls["wallLeft"].set_material(mat);
	walls["wallRight"].set_material(mat);
	walls["wallBack"].set_material(mat);
	barrels["barrel1"].set_material(mat);


	// Load texture
	texs[0] = texture("textures/bricks.jpg");
	texs[1] = texture("textures/crate.jpg");
	texs[2] = texture("textures/barrel2.jpg");
	texs[3] = texture("textures/stone.jpg");

	// Set lighting values 
	// Set lighting values, Position 
	pointLight1.set_position(vec3(0.0f, 38.0f, 25.0f));
	// Light colour 
	pointLight1.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	// Light direction to forward and down (normalized)
	// Set range 
	pointLight1.set_range(28.0f);
	// Set power 

	// Load in shaders
	eff.add_shader("shaders/shaderCW.vert", GL_VERTEX_SHADER); 
	eff.add_shader("shaders/shaderCW.frag", GL_FRAGMENT_SHADER);

	// Build effect
	eff.build();

	// Set camera properties
	//free camera
	freeCam.set_position(vec3(0.0f, 50.0f, 50.0f));
	freeCam.set_target(vec3(0.0f, 0.0f, 0.0f));
	freeCam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);

	//target cam
	cam.set_position(vec3(0.0f, 15.0f, 50.0f));
	cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	return true;
}


bool update(float delta_time) {
	// Update the camera
	#pragma region light
	// Range of the spot light
	static float range = 20.0f;

	// WSAD to move point light
	vec3 posLight;
	float movementSpeed = 0.5;
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_G))
	{
		posLight = vec3(0.0f, 0.0f, movementSpeed);
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_B))
	{
		posLight = vec3(0.0f, 0.0f, -movementSpeed);
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_V))
	{
		posLight = vec3(-movementSpeed, 0.0f, 0.0f);
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_N))
	{
		posLight = vec3(movementSpeed, 0.0f, 0.0f);
	}

	pointLight1.set_position(pointLight1.get_position() + posLight);

	// O and P to change range
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_O))
	{
		range += 1.0f;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_P))
	{
		range -= 1.0f;
	}
	// Cursor keys to rotate camera on X and Y axis
	if (glfwGetKey(renderer::get_window(), 'I')) {
		pointLight1.rotate(vec3(0.3f, 0.0f, 0.0f));
	}
	if (glfwGetKey(renderer::get_window(), 'K')) {
		pointLight1.rotate(vec3(-0.3f, 0.0f, 0.0f));
	}
	if (glfwGetKey(renderer::get_window(), 'J')) {
		pointLight1.rotate(vec3(0.0f, 0.3f, 0.0f));
	}
	if (glfwGetKey(renderer::get_window(), 'L')) {
		pointLight1.rotate(vec3(0.0f, -0.3f, 0.0f));
	}

	// *********************************

	// Set range
	pointLight1.set_range(range);
#pragma endregion


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
		movement = vec3(0.0f, 0.0f, 45.0f) * delta_time;
	}
	if (glfwGetKey(renderer::get_window(), 'S')) {
		movement = vec3(0.0f, 0.0f, -45.0f) * delta_time;
	}
	if (glfwGetKey(renderer::get_window(), 'A')) {
		movement = vec3(-45.0f, 0.0f, 0.0f) * delta_time;
	}
	if (glfwGetKey(renderer::get_window(), 'D')) {
		movement = vec3(45.0f, 0.0f, 0.0f) * delta_time;
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
	
	// Bind effect
	renderer::bind(eff);
	// Get PV
	const auto PV = freeCam.get_projection() * freeCam.get_view();
	// Set the texture value for the shader here
	//glUniform1i(eff.get_uniform_location("tex"), 0);
	// Find the lcoation for the MVP uniform
	const auto loc = eff.get_uniform_location("MVP");

	//render floor
	glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(PV * floorMesh.get_transform().get_transform_matrix()));
	// Bind floor texture
	renderer::bind(texs[3], 0);
	// Bind material
	renderer::bind(floorMesh.get_material(), "mat");
	// Render floor
	renderer::render(floorMesh);
	



	//render Boxes
	for (auto &e : boxes) 
	{
		auto b = e.second;
		auto M = b.get_transform().get_transform_matrix();
		auto MVP = PV * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(MVP));

		// Set M matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
		// Set N matrix uniform 
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(M));

		// Bind material
		renderer::bind(b.get_material(), "mat");
		// Bind texture to renderer
		renderer::bind(texs[1], 0);
		// Set tex uniform
		glUniform1i(eff.get_uniform_location("tex"), 0);
		// Set eye position - Get this from active camera
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(cam.get_position()));

		// Render mesh
		renderer::render(b);
	}
	//render walls
	for (auto &e : walls)
	{
		auto w = e.second;
		auto M = w.get_transform().get_transform_matrix();
		auto MVP = PV * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(MVP));
		// Set M matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
		// Set N matrix uniform - remember - 3x3 matrix
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(M));


		// Bind texture to renderer
		renderer::bind(texs[0], 0);
		// Render mesh 
		renderer::render(w);
	}
	//render barrels
	for (auto &e : barrels)
	{
		auto b = e.second;
		auto M = b.get_transform().get_transform_matrix();
		auto MVP = PV * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(MVP));
		// Set M matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
		// Set N matrix uniform - remember - 3x3 matrix
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(M));



		// Bind texture to renderer
		renderer::bind(texs[2], 0);
		// Render mesh
		renderer::render(b);
	}

	// Bind light 
	renderer::bind(pointLight1, "point");

	
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