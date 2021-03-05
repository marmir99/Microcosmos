#include "Parser.h"

vector<string> split(string line, const char* sep) {

	char* c = new char[line.size() + 1];
	memcpy(c, line.c_str(), line.size() + 1);

	char* next = NULL;
	char* p = strtok_s(c, sep, &next);

	vector<string> res;

	while (p != NULL) {
		res.push_back(string(p));
		p = strtok_s(NULL, sep, &next);
	}

	return res;
};



GLuint readTexture(const char* filename) {
		GLuint tex;
	
		//Wczytanie do pamiêci komputera
		std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
		unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
		//Wczytaj obrazek
		unsigned error = lodepng::decode(image, width, height, filename);
	
		//Import do pamiêci karty graficznej
		glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
		glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
		//Wczytaj obrazek do pamiêci KG skojarzonej z uchwytem
		glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
		return tex;
	}


void loadOBJ(const char* filename, std::vector <glm::vec4>& out_vertices, std::vector <glm::vec4>& out_normals, std::vector <glm::vec2>& out_uvs)
{
	{
		std::vector<glm::vec4> tmpVertices;
		std::vector<glm::vec4> tmpNormals;
		std::vector<glm::vec2> tmpUvs;
		std::vector<unsigned int> vertexIndices, normalIndices, uvIndices;
		std::ifstream file(filename);
		std::string head, line;
		float x, y, z, minY = 0;
		unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];

		while (file >> head) {
			//wierzcholki
			if (head == "v") {
				file >> x >> y >> z;
				tmpVertices.push_back(glm::vec4(x, y, z, 1.0f));

			}
			//wsp. teksturowania
			else if (head == "vt") {
				file >> x >> y;
				tmpUvs.push_back(glm::vec2(x, y));
			}
			//wek. normalny
			else if (head == "vn") {
				file >> x >> y >> z;
				tmpNormals.push_back(glm::vec4(x, y, z, 0.0f));

			}
			//ktory wierzcholek, z ktorym wsp. teksturowania i wek. normalanym
			//odpowiednia kolejnoœæ rysowania
			else if (head == "f") {
				std::getline(file, line);
				sscanf_s(line.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", vertexIndex, uvIndex, normalIndex, vertexIndex + 1,
					uvIndex + 1, normalIndex + 1, vertexIndex + 2, uvIndex + 2, normalIndex + 2, vertexIndex + 3, uvIndex + 3, normalIndex + 3);

				vertexIndices.push_back(vertexIndex[0] - 1);
				vertexIndices.push_back(vertexIndex[1] - 1);
				vertexIndices.push_back(vertexIndex[2] - 1);
				vertexIndices.push_back(vertexIndex[3] - 1);
				uvIndices.push_back(uvIndex[0] - 1);
				uvIndices.push_back(uvIndex[1] - 1);
				uvIndices.push_back(uvIndex[2] - 1);
				uvIndices.push_back(uvIndex[3] - 1);
				normalIndices.push_back(normalIndex[0] - 1);
				normalIndices.push_back(normalIndex[1] - 1);
				normalIndices.push_back(normalIndex[2] - 1);
				normalIndices.push_back(normalIndex[3] - 1);
			}
		}


		for (auto& index : vertexIndices) {
			auto& vertex = tmpVertices[index];
			out_vertices.push_back(vertex);
		}
		for (auto& index : normalIndices) {
			auto& normal = tmpNormals[index];
			out_normals.push_back(normal);
		}
		for (auto& index : uvIndices) {
			auto& uv = tmpUvs[index];
			out_uvs.push_back(uv);
		}
		file.close();
	}
}



