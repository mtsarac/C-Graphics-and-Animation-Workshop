#include "wavefront.hpp"

#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>

namespace wavefront {

  void load(const char* filename, std::vector<Vertex>& vertices) {
    std::ifstream file(filename);
    if (!file.is_open()) {
      throw std::runtime_error("failed to open file");
    }
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::string line;
    while (std::getline(file, line)) {
      if (line.substr(0, 2) == "v ") {
        std::istringstream s(line.substr(2));
        glm::vec3 v;
        s >> v.x >> v.y >> v.z;
        positions.push_back(v);
      } else if (line.substr(0, 3) == "vt ") {
        std::istringstream s(line.substr(3));
        glm::vec2 v;
        s >> v.x >> v.y;
        texCoords.push_back(v);
      } else if (line.substr(0, 3) == "vn ") {
        std::istringstream s(line.substr(3));
        glm::vec3 v;
        s >> v.x >> v.y >> v.z;
        normals.push_back(v);
      } else if (line.substr(0, 2) == "f ") {
        std::istringstream s(line.substr(2));
        std::string vstring;
        while (std::getline(s, vstring, ' ')) {
          std::istringstream vs(vstring);
          std::string v;
          std::getline(vs, v, '/');
          int vi = std::stoi(v);
          std::getline(vs, v, '/');
          int ti = std::stoi(v);
          std::getline(vs, v, '/');
          int ni = std::stoi(v);
          Vertex vertex{};
          vertex.pos = positions[vi - 1];
          vertex.tex = texCoords[ti - 1];
          vertex.nor = normals[ni - 1];
          vertices.push_back(vertex);
        }
      }  
    }
  }

}
