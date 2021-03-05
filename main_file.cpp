/*
Niniejszy program jest wolnym oprogramowaniem; możesz go rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH ZASTOSOWAŃ. W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "Parser.h"
#include <fstream>

using namespace std;
using namespace glm;

int ladybugCount = 8; //ilosc biedronek
float* speed_x = new float[ladybugCount];
float* speed_y = new float[ladybugCount];
float* go_x = new float[ladybugCount];
float* go_y = new float[ladybugCount];
float speed_z = 140.0f; //predkosc obrotu - zderzenie ze sciana
float speed = 30.0f; //predkosc ruszania noga
float speed_rotation = 60.0f; //predkosc obrotu - ruch noga 
float aspectRatio = 1; //stosunek szerokosci do wysokosci okna
float angle_leg = 0; //kat ruch noga
float angle_leg2 = 0; //kat ruch noga przy obrocie
int d_leg = 1; // wspolczynniki, w ktora strone ruszamy noga
int d_leg2 = 1;
float* kat_start = new float[ladybugCount]; //obecny kat obrotu biedronki
int* stop_wall = new int[ladybugCount]; //czy kolizja ze sciana
float* angle_rotation_wall = new float[ladybugCount]; //kat obrotu po kolizji
float* new_angle = new float[ladybugCount]; //kat obrotu po kolizji ze sciana
float* new_angle_2 = new float[ladybugCount]; //kat obrotu po kolizji ze sciana
float angle_z = 0.0f; //kat obrotu - zderzenie ze sciana
int* collision_ladybugs = new int[ladybugCount]; //czy wystapila kolizja z inna biedronka, jesli tak to z ktora
int** collision_ladybugs_2 = new int* [ladybugCount];
float* angle_rotation_ladybug = new float[ladybugCount]; //kat obrotu po kolizji z biedronka
int* kierunek = new int[ladybugCount];
int* blocked = new int[ladybugCount];
int* prev_col = new int[ladybugCount];
int* if_corner = new int[ladybugCount];
float* speeds = new float[5];
glm::vec3 camera = glm::vec3 (0, -4, -9);

vector<glm::vec4>vertices_head;
vector<glm::vec4>normals_head;
vector<glm::vec2>texCoord_head;

vector<glm::vec4>vertices_body;
vector<glm::vec4>normals_body;
vector<glm::vec2>texCoord_body;

vector<glm::vec4>vertices_r_b;
vector<glm::vec4>normals_r_b;
vector<glm::vec2>texCoord_r_b;

vector<glm::vec4>vertices_r_m;
vector<glm::vec4>normals_r_m;
vector<glm::vec2>texCoord_r_m;

vector<glm::vec4>vertices_r_f;
vector<glm::vec4>normals_r_f;
vector<glm::vec2>texCoord_r_f;

vector<glm::vec4>vertices_l_b;
vector<glm::vec4>normals_l_b;
vector<glm::vec2>texCoord_l_b;

vector<glm::vec4>vertices_l_m;
vector<glm::vec4>normals_l_m;
vector<glm::vec2>texCoord_l_m;

vector<glm::vec4>vertices_l_f;
vector<glm::vec4>normals_l_f;
vector<glm::vec2>texCoord_l_f;

vector<glm::vec4>vertices_floor;
vector<glm::vec4>normals_floor;
vector<glm::vec2>texCoord_floor;

ShaderProgram* sp;

GLuint tex0;
GLuint tex1;
GLuint tex2;
GLuint tex3;

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void obrot_start(float* tab) //ustalenie poczatkowego ustawienia biedronek
{
	for (int i = 0; i < ladybugCount; i++)
	{
		tab[i] = rand() % 360;
	}
}

void setSpeed(int i, float pred)
{
	if (kat_start[i] >= 0 && kat_start[i] <= 90)
	{
		speed_x[i] = (-1) * pred;
		speed_y[i] = (-1) * pred;
	}
	else if (kat_start[i] > 90 && kat_start[i] < 180)
	{
		speed_x[i] = pred;
		speed_y[i] = (-1) * pred;
	}
	else if (kat_start[i] >= 180 && kat_start[i] <= 270)
	{
		speed_x[i] = pred;
		speed_y[i] = pred;
	}
	else if (kat_start[i] > 270 && kat_start[i] < 360)
	{
		speed_x[i] = (-1) * pred;
		speed_y[i] = pred;
	}
}

void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
    if (action==GLFW_PRESS) {
        if (key==GLFW_KEY_1) camera = glm::vec3(0,-6, -2);
        if (key==GLFW_KEY_2) camera = glm::vec3(0, -4, -9);
        if (key==GLFW_KEY_3) camera = glm::vec3(0, 0, -9);
    }
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height); //zakres pikseli w oknie gdzie ma być obraz
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0.82f, 0.33f, 0.13f, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);

	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	tex0 = readTexture("body_tex4.png");
	tex1 = readTexture("tex2.png");
	tex2 = readTexture("floor.png");
	tex3 = readTexture("wall.png");

	loadOBJ("head_all.obj", vertices_head, normals_head, texCoord_head);
	loadOBJ("body.obj", vertices_body, normals_body, texCoord_body);
	loadOBJ("r_b.obj", vertices_r_b, normals_r_b, texCoord_r_b);
	loadOBJ("r_m_all.obj", vertices_r_m, normals_r_m, texCoord_r_m);
	loadOBJ("r_f.obj", vertices_r_f, normals_r_f, texCoord_r_f);
	loadOBJ("l_b.obj", vertices_l_b, normals_l_b, texCoord_l_b);
	loadOBJ("l_m_all.obj", vertices_l_m, normals_l_m, texCoord_l_m);
	loadOBJ("l_f.obj", vertices_l_f, normals_l_f, texCoord_l_f);
	loadOBJ("floor2.obj", vertices_floor, normals_floor, texCoord_floor);

	obrot_start(kat_start);

	for (int j = 0; j < 5; j++)
	{
		speeds[j] = (j + 2) * 0.5f;
	}
	for (int j = 0; j < ladybugCount; j++) //wypelnienie tablic
	{
		stop_wall[j] = 0;
		angle_rotation_wall[j] = 0.0f;
		angle_rotation_ladybug[j] = 0.0f;
		new_angle[j] = 0;
		new_angle_2[j] = 0;
		collision_ladybugs[j] = -1;
		go_x[j] = 0.0f;
		go_y[j] = 0.0f;
		speed_x[j] = 2.5f;
		speed_y[j] = 2.5f;
		kierunek[j] = 1;
		blocked[j] = 0;
		prev_col[j] = -1;
		//setSpeed(j, speeds[rand()%5]);
		setSpeed(j, 2.5f);
		if_corner[j] = -1;
		for (int n = 0; n < ladybugCount; n++)
		{
			collision_ladybugs_2[j][n] = -1;
		}
	}
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
	delete sp;
}

float odleglosc(float x_1, float y_1, float x_2, float y_2)
{
	return sqrt((x_1 - x_2) * (x_1 - x_2) + (y_1 - y_2) * (y_1 - y_2));
}

void corner(float** tab, int i)
{
	float dist_l_g, dist_l_d, dist_p_g, dist_p_d;
	dist_l_g = odleglosc(tab[0][i], tab[1][i], 4.0, 4.0);
	dist_l_d = odleglosc(tab[0][i], tab[1][i], 4.0, -4.0);
	dist_p_g = odleglosc(tab[0][i], tab[1][i], -4.0, 4.0);
	dist_p_d = odleglosc(tab[0][i], tab[1][i], -4.0, -4.0);
	if (dist_l_g < 2 && kat_start[i] >= 180 && kat_start[i] < 270)
	{
		if_corner[i] = 1;
		kierunek[i] = 1;
	}
	else if (dist_l_d < 2 && kat_start[i] >= 90 && kat_start[i] < 180)
	{
		if_corner[i] = 1;
		kierunek[i] = -1;
	}
	else if (dist_p_g < 2 && kat_start[i] >= 270 && kat_start[i] < 360)
	{
		if_corner[i] = 1;
		kierunek[i] = -1;
	}
	else if (dist_p_d < 2 && kat_start[i] >= 0 && kat_start[i] < 90)
	{
		if_corner[i] = 1;
		kierunek[i] = 1;
	}
}

void checkCollisionsLadybug(float** tab) //sprawdzenie czy wystapila kolizja z inna biedronka, jesli tak, z ktora
{
	for (int ind1 = 0; ind1 < ladybugCount; ind1++)
	{
		for (int ind2 = 0; ind2 < ladybugCount; ind2++)
		{
			if (ind1 != ind2 && stop_wall[ind1] == 0 && kat_start[ind1] != kat_start[ind2] && collision_ladybugs[ind1] == -1 && (ind2 != prev_col[ind1] || (ind2 == prev_col[ind1] && blocked[ind1] < 0)))
			{
				float check;
				check = odleglosc(tab[0][ind1], tab[1][ind1], tab[0][ind2], tab[1][ind2]);
				if (check < 0.6f)
				{
					collision_ladybugs[ind1] = ind2;
					collision_ladybugs_2[ind1][ind2] = 1;
					collision_ladybugs_2[ind2][ind1] = 1;
					setSpeed(ind1, 0.0f);
					setSpeed(ind2, 0.0f);
					new_angle_2[ind1] = 180.0f;
				}
			}
		}
	}
}

void checkCollisionsWall(float** tab) //sprawdzenie kolizji ze sciana - aby wystapila biedronka musi byc ustawiona w odpowiednia strone do odpowiedniej sciany
{
	for (int j = 0; j < ladybugCount; j++)
	{
		if (tab[0][j] < -3.6 && stop_wall[j] == 0 && collision_ladybugs[j] == -1 && ((kat_start[j] >= 0 && kat_start[j] < 90) || (kat_start[j] > 270 && kat_start[j] < 360)))
		{
			stop_wall[j] = 1;
			setSpeed(j, 0.0f);
			new_angle[j] = rand() % 180;
			if (kat_start[j] < 90)
			{
				kierunek[j] = 1;
			}
			else if (kat_start[j] > 270)
			{
				kierunek[j] = -1;
			}
		}
		else if (tab[0][j] > 3.6 && kat_start[j] >= 90 && kat_start[j] < 270 && stop_wall[j] == 0 && collision_ladybugs[j] == -1)
		{
			stop_wall[j] = 1;
			setSpeed(j, 0.0f);
			new_angle[j] = rand() % 180;
			if (kat_start[j] < 180)
			{
				kierunek[j] = -1;
			}
			else
			{
				kierunek[j] = 1;
			}
		}
		else if (tab[1][j] < -3.6 && kat_start[j] > 0 && kat_start[j] < 180 && stop_wall[j] == 0 && collision_ladybugs[j] == -1)
		{
			stop_wall[j] = 1;
			setSpeed(j, 0.0f);
			new_angle[j] = rand() % 180;
			if (kat_start[j] < 90)
			{
				kierunek[j] = -1;
			}
			else
			{
				kierunek[j] = 1;
			}
		}
		else if (tab[1][j] > 3.6 && kat_start[j] > 180 && kat_start[j] < 360 && stop_wall[j] == 0 && collision_ladybugs[j] == -1)
		{
			stop_wall[j] = 1;
			setSpeed(j, 0.0f);
			new_angle[j] = rand() % 180;
			if (kat_start[j] < 270)
			{
				kierunek[j] = -1;
			}
			else
			{
				kierunek[j] = 1;
			}
		}
	}
}

void setNextPositions(float** pozycje_start, int i) //krok do przodu
{
	if (kat_start[i] >= 0 && kat_start[i] <= 90)
	{
		float wsp = kat_start[i] / 90.0f;
		pozycje_start[0][i] += (1 - wsp) * go_x[i];
		pozycje_start[1][i] += wsp * go_y[i];
	}
	else if (kat_start[i] > 90 && kat_start[i] < 180)
	{
		float wsp = (kat_start[i] - 90) / 90.0f;
		pozycje_start[0][i] += wsp * go_x[i];
		pozycje_start[1][i] += (1 - wsp) * go_y[i];
	}
	else if (kat_start[i] >= 180 && kat_start[i] <= 270)
	{
		float wsp = (kat_start[i] - 180) / 90.0f;
		pozycje_start[0][i] += (1 - wsp) * go_x[i];
		pozycje_start[1][i] += wsp * go_y[i];
	}
	else if (kat_start[i] > 270 && kat_start[i] < 360)
	{
		float wsp = (kat_start[i] - 270) / 90.0f;
		pozycje_start[0][i] += wsp * go_x[i];
		pozycje_start[1][i] += (1 - wsp) * go_y[i];
	}
}

glm::vec3 setBackPosition(int i, float go_x, float go_y) //cofanie
{
	float x = 0, y = 0, wsp = 0;
	if (kat_start[i] >= 0 && kat_start[i] <= 90)
	{
		wsp = kat_start[i] / 90.0f;
		x = (1 - wsp) * go_x * (-1);
		y = wsp * go_y * (-1);
	}
	else if (kat_start[i] > 90 && kat_start[i] < 180)
	{
		wsp = (kat_start[i] - 90) / 90.0f;
		x = wsp * go_x;
		y = (1 - wsp) * go_y * (-1);
	}
	else if (kat_start[i] >= 180 && kat_start[i] <= 270)
	{
		wsp = (kat_start[i] - 180) / 90.0f;
		x = (1 - wsp) * go_x;
		y = wsp * go_y;
	}
	else if (kat_start[i] > 270 && kat_start[i] < 360)
	{
		wsp = (kat_start[i] - 270) / 90.0f;
		x = wsp * go_x * (-1);
		y = (1 - wsp) * go_y;
	}

	return glm::vec3(x, y, 0.0f);
}

glm::mat4 step_front(glm::mat4 leg, int d, float go_x, float go_y, int i) //krok przednia
{
	leg = glm::translate(leg, setBackPosition(i, go_x, go_y));
	leg = glm::rotate(leg, radians(angle_leg * d), glm::vec3(0.0f, 0.0f, 1.0f));
	return leg;
}

glm::mat4 step_middle(glm::mat4 leg, int d, float go_x, float go_y, int i) //krok srodkowa
{
	leg = glm::translate(leg, setBackPosition(i, go_x, go_y));
	leg = glm::rotate(leg, radians(-angle_leg * d), glm::vec3(1.0f, 1.0f, 1.0f));
	if (angle_leg > 7.0f || angle_leg < -7.0f)
	{
		angle_leg = 0.0f;
		d_leg = d_leg * (-1);
	}
	return leg;
}

glm::mat4 step_back(glm::mat4 leg, int d, float go_x, float go_y, int i) //krok tylnia
{
	leg = glm::translate(leg, setBackPosition(i, go_x, go_y));
	leg = glm::rotate(leg, radians(angle_leg * d), glm::vec3(0.0f, 0.0f, 1.0f));
	return leg;
}

glm::mat4 rotate(glm::mat4 leg, int d, int i, glm::vec3 os) //ruch nogami w przypadku obrotu po zderzeniu
{
	leg = glm::rotate(leg, radians(angle_leg2 * d), glm::vec3(0.0f, 0.0f, 1.0f));
	if (angle_leg2 > 7.0f || angle_leg2 < -7.0f)
	{
		angle_leg2 = 0.0f;
		d_leg2 = d_leg2 * (-1);
	}
	return leg;
}

glm::mat4 rotation_wall(glm::mat4 M, int i) //obsluga obrotu po zderzeniu - kat, nowy kat polozenia
{
	if (angle_rotation_wall[i] < new_angle[i])
	{
		M = glm::rotate(M, radians(angle_rotation_wall[i] * kierunek[i]), glm::vec3(0.0f, 0.0f, 1.0f));
		angle_rotation_wall[i] += angle_z;
	}
	else
	{
		kat_start[i] += kierunek[i] * angle_rotation_wall[i];
		if (kat_start[i] >= 360)
		{
			kat_start[i] -= 360;
		}
		else if (kat_start[i] < 0)
		{
			kat_start[i] = 360 - abs(kat_start[i]);
		}
		new_angle[i] = 0;
		stop_wall[i] = 0;
		angle_rotation_wall[i] = 0.0f;
		kierunek[i] = 1;
		//setSpeed(i, speeds[rand()%5]);
		setSpeed(i, 2.5f);
	}
	return M;
}

glm::mat4 rotationLadybug(glm::mat4 M, int i, float** tab) //obrot po zderzeniu z inna biedronka
{
	if (angle_rotation_ladybug[i] < new_angle_2[i])
	{
		M = glm::rotate(M, radians(angle_rotation_ladybug[i] * kierunek[i]), glm::vec3(0.0f, 0.0f, 1.0f));
		angle_rotation_ladybug[i] += angle_z;
	}
	else
	{
		kat_start[i] += angle_rotation_ladybug[i];
		if (kat_start[i] >= 360)
		{
			kat_start[i] -= 360;
		}
		else if (kat_start[i] < 0)
		{
			kat_start[i] = 360 - abs(kat_start[i]);
		}
		new_angle_2[i] = 0;
		prev_col[i] = collision_ladybugs[i];
		collision_ladybugs[i] = -1;
		//setSpeed(i, speeds[rand()%5]);
		setSpeed(i, 2.5f);
		angle_rotation_ladybug[i] = 0.0f;
		blocked[i] = 25;
	}
	return M;
}

glm::mat4 rotation_corner(glm::mat4 M, int i, float** tab)
{
	if (angle_rotation_ladybug[i] < 180.0f)
	{
		M = glm::rotate(M, radians(angle_rotation_ladybug[i] * kierunek[i]), glm::vec3(0.0f, 0.0f, 1.0f));
		angle_rotation_ladybug[i] += angle_z;
	}
	else
	{
		kat_start[i] += angle_rotation_ladybug[i];
		if (kat_start[i] >= 360)
		{
			kat_start[i] -= 360;
		}
		else if (kat_start[i] < 0)
		{
			kat_start[i] = 360 - abs(kat_start[i]);
		}
		if_corner[i] = -1;
		kierunek[i] = 1;
	}
	return M;
}

//g_r - go or rotation, go = 1, rotation = 2, 3 - zderzenie z inna biedronka
void drawLadybug(glm::mat4 M, float x, float y, float go_x, float go_y, int i, int g_r, float** tab) //rysowanie biedronki
{
	glm::mat4 M1 = translate(M, glm::vec3(x, y, 0.0f));
	M1 = glm::rotate(M1, radians(kat_start[i]), glm::vec3(0.0f, 0.0f, 1.0f));

	if (g_r == 2)
	{
		M1 = rotation_wall(M1, i);
	}

	if (g_r == 3)
	{
		M1 = rotationLadybug(M1, i, tab);
	}

	if (g_r == 4)
	{
		rotation_corner(M1, i, tab);
	}

	M1 = glm::scale(M1, glm::vec3(0.15f, 0.15f, 0.15f));

	glm::mat4 M_body = M1;
	glm::mat4 M_r_b = M1;
	glm::mat4 M_r_m = M1;
	glm::mat4 M_r_m_2 = M1;
	glm::mat4 M_r_f = M1;
	glm::mat4 M_l_b = M1;
	glm::mat4 M_l_m = M1;
	glm::mat4 M_l_f = M1;

	if (g_r == 1)
	{
		M_r_b = step_back(M_r_b, -1, go_x, go_y, i);
		M_r_m = step_middle(M_r_m, 1, go_x, go_y, i);
		M_r_f = step_front(M_r_f, -1, go_x, go_y, i);
		M_l_b = step_back(M_l_b, 1, go_x, go_y, i);
		M_l_m = step_middle(M_l_m, 1, go_x, go_y, i);
		M_l_f = step_front(M_l_f, 1, go_x, go_y, i);
	}
	else if (g_r == 2 || g_r == 3 || g_r == 4)
	{
		M_r_b = rotate(M_r_b, kierunek[i], i, glm::vec3(0.0f, 0.0f, 1.0f));
		M_r_m = rotate(M_r_m, 1, i, glm::vec3(1.0f, 1.0f, 1.0f));
		M_r_f = rotate(M_r_f, kierunek[i], i, glm::vec3(0.0f, 0.0f, 1.0f));
		M_l_b = rotate(M_l_b, kierunek[i], i, glm::vec3(0.0f, 0.0f, 1.0f));
		M_l_m = rotate(M_l_m, 1, i, glm::vec3(1.0f, 1.0f, 1.0f));
		M_l_f = rotate(M_l_f, kierunek[i], i, glm::vec3(0.0f, 0.0f, 1.0f));
	}


	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_body));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices_head.data());
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals_head.data());
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoord_head.data());
	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glDrawArrays(GL_QUADS, 0, vertices_head.size());

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_body));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices_body.data());
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals_body.data());
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoord_body.data());
	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);
	glDrawArrays(GL_QUADS, 0, vertices_body.size());

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_r_b));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices_r_b.data());
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals_r_b.data());
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoord_r_b.data());
	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glDrawArrays(GL_QUADS, 0, vertices_r_b.size());

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_r_m));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices_r_m.data());
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals_r_m.data());
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoord_r_m.data());
	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glDrawArrays(GL_QUADS, 0, vertices_r_m.size());

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_r_f));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices_r_f.data());
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals_r_f.data());
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoord_r_f.data());
	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glDrawArrays(GL_QUADS, 0, vertices_r_f.size());

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_l_b));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices_l_b.data());
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals_l_b.data());
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoord_l_b.data());
	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glDrawArrays(GL_QUADS, 0, vertices_l_b.size());

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_l_m));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices_l_m.data());
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals_l_m.data());
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoord_l_m.data());
	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glDrawArrays(GL_QUADS, 0, vertices_l_m.size());

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_l_f));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices_l_f.data());
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals_l_f.data());
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoord_l_f.data());
	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glDrawArrays(GL_QUADS, 0, vertices_l_f.size());
}

void drawFloor(glm::mat4 M) //rysowanie podlogi
{
	glm::mat4 M1 = M;
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M1));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices_floor.data());
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals_floor.data());
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoord_floor.data());
	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex2);
	glDrawArrays(GL_QUADS, 0, vertices_floor.size());
}

void drawWall(glm::mat4 M, float what_angle, glm::vec3 what_x_y_z, glm::vec3 os)
{
	glm::mat4 M1 = glm::rotate(M, radians(what_angle), os);
	M1 = glm::translate(M1, what_x_y_z);
	M1 = glm::scale(M1, glm::vec3(1.0f, 2.0f, 1.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M1));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices_floor.data());
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals_floor.data());
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoord_floor.data());
	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex3);
	glDrawArrays(GL_QUADS, 0, vertices_floor.size());
}

void drawScene(GLFWwindow* window, float angle_x, float angle_y, float** pozycje) { //rysowanie sceny
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V = glm::lookAt(
		camera,
		glm::vec3(0, 0, 0),
		glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz widoku

	glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f, 50.0f); //Wylicz macierz rzutowania

	glm::mat4 M = glm::mat4(1.0f);

	sp->use();
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
	glUniform3fv(sp->u("camera"), 3, glm::value_ptr(camera));

	glEnableVertexAttribArray(sp->a("vertex"));
	glEnableVertexAttribArray(sp->a("normal"));
	glEnableVertexAttribArray(sp->a("texCoord0"));
	
	glm::mat4 M1 = M;

	drawFloor(M1);
	drawWall(M1, 90, glm::vec3(0.0f, 2.0f, -4.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	drawWall(M1, 90, glm::vec3(-4.0f, 0.0f, -4.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	drawWall(M1, 270, glm::vec3(4.0f, 0.0f, -4.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	for (int i = 0; i < ladybugCount; i++)
	{
		if (stop_wall[i] == 0 && collision_ladybugs[i] == -1)
		{
			drawLadybug(M, pozycje[0][i], pozycje[1][i], go_x[i], go_y[i], i, 1, pozycje); //1 idź prosto
		}
		else if (if_corner[i] == 1)
		{
			drawLadybug(M, pozycje[0][i], pozycje[1][i], go_x[i], go_y[i], i, 4, pozycje);
		}
		else if (collision_ladybugs[i] != -1)
		{
			drawLadybug(M, pozycje[0][i], pozycje[1][i], 0, 0, i, 3, pozycje);
		}
		else if (stop_wall[i] == 1)
		{
			drawLadybug(M, pozycje[0][i], pozycje[1][i], go_x[i], go_y[i], i, 2, pozycje); //2 kolizja ze sciana
		}
	}

	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));

	glfwSwapBuffers(window);
}

void tablica_start(float** tab) //ustawianie polozen poczatkowych
{
	for (int i = 0; i < ladybugCount; i++)
	{
		int check = 0, decide = rand() % 2;
		if (decide == 1) tab[0][i] = rand() % 4;
		else tab[0][i] = (-1) * (rand() % 4);
		do {
			decide = rand() % 2;
			if (decide == 1) tab[1][i] = rand() % 4;
			else tab[1][i] = (-1) * (rand() % 4);
			check = 0;
			for (int j = 0; j < i; j++)
			{
				if (tab[0][i] == tab[0][j] && tab[1][i] == tab[1][j])
				{
					check = 1;
				}
			}
		} while (check == 1);
	}
}


int main(void)
{
	srand((unsigned)time(NULL));
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	float** pozycje_start = new float* [2];
	pozycje_start[0] = new float[ladybugCount];
	pozycje_start[1] = new float[ladybugCount];

	for (int n = 0; n < ladybugCount; n++)
	{
		collision_ladybugs_2[n] = new int[ladybugCount];
	}

	tablica_start(pozycje_start);

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(800, 800, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	float angle_x = 0; //Aktualny kąt obrotu obiektu
	float angle_y = 0; //Aktualny kąt obrotu obiektu
	glfwSetTime(0); //Zeruj timer

	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		for (int i = 0; i < ladybugCount; i++)
		{
			go_x[i] = speed_x[i] * glfwGetTime();
			go_y[i] = speed_y[i] * glfwGetTime();
			blocked[i] -= 1;
		}
		angle_z = speed_z * glfwGetTime();
		angle_leg += (speed * glfwGetTime()) * d_leg;
		angle_leg2 += speed_rotation * glfwGetTime() * d_leg2;
		glfwSetTime(0); //Zeruj timer
		drawScene(window, angle_x, angle_y, pozycje_start); //Wykonaj procedurę rysującą
		checkCollisionsWall(pozycje_start);
		checkCollisionsLadybug(pozycje_start);
		for (int i = 0; i < ladybugCount; i++)
		{
			setNextPositions(pozycje_start, i);
			corner(pozycje_start, i);
		}
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}