#version 450
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 normal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 finalColor;
out vec2 texCoord;
out vec3 fragPos;
out vec3 scaledNormal;
void main()
{
//adicionado ao shader o posicionamento de um segundo cubo por meio do "gl_InstanceID, caso necessário"
vec3 pos = position;
// pos.y -= float(gl_InstanceID) * 1.2;	
gl_Position = projection * view * model * vec4(pos, 1.0);
finalColor = color;
scaledNormal = vec3(model * vec4(normal, 1.0));;
texCoord = vec2(tex_coord.x, 1 - tex_coord.y);
fragPos = vec3(model * vec4(pos, 1.0));
}
