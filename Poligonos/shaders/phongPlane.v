#version 430 core

layout (location = 0) in vec4 vPosition;


uniform float fTime;
uniform mat4 mTransform;
uniform vec3 vEye;


out vec3 mA;
out vec3 lA;
out vec3 mD;
out vec3 lD;
out vec3 mS;
out vec3 lS;
out float fShininess;
out vec3 lightPos;
out vec4 vertex;
out mat4 matForNormals;
out vec3 interNormal;


//formula original
float F(float x, float z, float time) 
{
	return	4.0 * sin(radians(time + (x*x + z*z)));
}

//Derivada parcial respecto a "a"
float Fp(float a, float b, float time) 
{
	return	8*a * cos(radians(a*a + b*b + time));
}

//calculamos la normal antes de aplicar transformaciones
vec3 calculateNormal (vec4 vertex, float time)
{
	vec3 p1 = vec3(1, Fp(vertex.x, vertex.z, fTime), 0);
	vec3 p2 = vec3(0, Fp(vertex.z, vertex.x, fTime), 1);

	return normalize(cross(p2,p1));
}


void main()
{
	lA = vec3( 0.2, 0.2, 0.2 );
	lD = vec3( 1. , 1. , 1. );
	lS = vec3( 1. , 1. , 1. );
	mA = vec3( 1. , 0.5, 0. );
	mD = vec3( 0.5, 0.5, 0. );
	mS = vec3( 0.6, 0.6, 0.6 );
	fShininess = 80.0;
	
	vec4 nl = mTransform * vec4(0.0 , 100.0, 0.0, 1.0);
	lightPos = nl.xyz;

	vec4 vNewVec = vPosition;
	//calculamos la altura de este vertice
	vNewVec.y = F(vNewVec.x, vNewVec.z, fTime);
	vertex = vNewVec;
	interNormal = calculateNormal(vNewVec, fTime);
	
	matForNormals = transpose(inverse(mTransform));
	
   	gl_Position = mTransform * vNewVec;
}
