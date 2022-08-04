#version 400

// 363 lecture note[11]- 21
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// program Uniforms
uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat4 norMatrix;
uniform vec4 lgtPos;

out vec4 eyeNormal;
out vec4 lgtVector;
out vec4 halfWayVector;
out vec2 TexCoord;
out vec3 currentPatchPoint;

void lightingCalculation(vec4 position)
{
	// light face normal
	vec3 u_vector = gl_in[0].gl_Position.xyz - gl_in[2].gl_Position.xyz;
	vec3 v_vector = gl_in[1].gl_Position.xyz - gl_in[2].gl_Position.xyz;
	vec4 light_normal = vec4(normalize(cross(u_vector, v_vector)), 0);

	vec4 eye_pos = mvMatrix * position;

	eyeNormal = norMatrix * light_normal;

	lgtVector = normalize(lgtPos - eye_pos);

	vec4 view_vector = normalize(vec4(-eye_pos.xyz, 0));
	halfWayVector = normalize(lgtVector + view_vector);

}


void main()
{
	float xmin = -45, xmax = +45, zmin = 0, zmax = -90; // todo 

	for (int i=0; i<gl_in.length(); i++)
	{
		vec4 position = gl_in[i].gl_Position;
		lightingCalculation(position);

		TexCoord.s = (position.x - xmin) / (xmax - xmin);
		TexCoord.t = (position.z - zmin) / (zmax - zmin);

		currentPatchPoint = position.xyz;
		gl_Position = mvpMatrix * position;
		EmitVertex();	
	}
	EndPrimitive();
	
}