#ifndef WAVEFRONT_H
#define WAVEFRONT_H

#include <vector>
#include <glm/glm.hpp>

namespace wavefront {

  struct Vertex {
    glm::vec3 pos;
    glm::vec2 tex;
    glm::vec3 nor;
  };

  void load(const char* filename, ::std::vector<Vertex>& vertices);

}

#endif