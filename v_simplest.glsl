#version 330 //obliczenia per wierzcholek

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 camera;

//Atrybuty
in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
in vec4 normal; //wektor normalny w przestrzeni modelu
in vec2 texCoord0; //przekazuje wspolrzedne teksturowania w przestrzeni modelu

//Zmienne interpolowane
out vec4 l1;
out vec4 l2;

out vec4 n;
out vec4 v;
out vec2 iTexCoord0;

void main(void)
{
	vec4 lp1 = vec4(3, -3, -3, 0); //wspolrzedne swiatla, przestrzen swiata
	vec4 lp2 = vec4(-3, 3, -3, 0);

	l1 = normalize(V * lp1 - V*M*vertex); //wektor do światła 1 w przestrzeni oka
	l2 = normalize(V * lp2 - V*M*vertex); //wektor do światła 2 w przestrzeni oka
	
	v=normalize(vec4(camera, 1)-V*M*vertex);
	n=normalize(V*M*normal); //znormalizowany wektor normalny w przestrzeni oka
	
    iTexCoord0=texCoord0;

	gl_Position=P*V*M*vertex;

}