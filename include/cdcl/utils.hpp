#pragma once 

#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////
// Namespace

namespace cdcl {

///////////////////////////////////////////////////////////////////////////////
// Utilities

std::vector<std::string> split(std::string str, std::string separator) {
  std::vector<std::string> components = std::vector<std::string>();

  int start, end = -1 * separator.size();
  do {
      start = end + separator.size();
      end = str.find(separator, start);
      components.push_back(str.substr(start, end - start));
  } while (end != -1);

  return components;
}

} // cdcl