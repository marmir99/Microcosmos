#pragma once
/*S�u�y do parsowania plik�w .obj �ci�gnietych z internetu
   Dostajemy wsp�rzedne wierzcho�k�w, wsp�rz�dne wekt. normalnych i wsp�rz�dne teksturowania*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <string>
#include "lodepng.h"


using namespace std;

	//vector<glm::vec4> ftmp;
	vector<string> split(string line, const char* sep);
	GLuint readTexture(const char* filename);
	void loadOBJ(const char* path, std::vector <glm::vec4>& out_vertices, std::vector <glm::vec4>& out_normals, std::vector <glm::vec2>& out_uvs);
	//inline vector<glm::vec4> getVtmp();
	//inline vector<glm::vec4> getVnmp();
	//inline vector<glm::vec2> getVttmp();
	//inline vector<glm::vec4> getFtmp();
