#include <deque>
#include <fstream>
#include <iostream>

#define BUFFER_SIZE 256

class FileStreamBuffer {
 public:
  FileStreamBuffer(const std::string filepath) : file(filepath) {
    if (!file) {
      std::cerr << "Error: Unable to open file " << filepath << std::endl;
      throw;
    }
    fill_buffer();
  }

  char get_next() {
    if (buffer.empty()) { return '\0'; }
    char c = buffer.front();
    buffer.pop_front();
    fill_buffer();
    return c;
  }

  char lookahead(int index) {
    if (index >= buffer.size()) { return '\0'; }
    return buffer[index];
  }

  bool has_next() { return !buffer.empty(); }

 private:
  std::ifstream file;
  std::deque<char> buffer;

  void fill_buffer() {
    while (file && buffer.size() < BUFFER_SIZE) {
      char c;
      file.get(c);
      if (file)
        buffer.push_back(c);
      else
        break;
    }
  }
};
