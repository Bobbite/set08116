#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm\gtx\projection.hpp>
#include <iostream>

using namespace std;
using namespace glm;

int main() {

	mat4 m(1.0f);
	mat4 t(1.0f);
	mat4 l(1.0f);

	mat4 p = t + l;

	mat4 u = 5.0f * t * l;

	mat3 m3(1.0f);


	
	vec3 poop(1, 0, 0);
	quat q = rotate(quat(), 45, poop);

}