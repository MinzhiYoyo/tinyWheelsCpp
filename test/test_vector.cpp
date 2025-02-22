#include <iostream>
#include "vector.h"
#include <fstream>

using namespace tinyWheels;

template<class T>
void print_vector(const vector<T>& vec, const char * name) {
  std::cout << name << " = [" << vec << "]" << std::endl;
}

int main(){
  // 以文件读写的方式打开 ../log/log.txt
  std::ofstream log_file("../log/log.txt", std::ios::out | std::ios::trunc);

  vector<int>::dataAllocator::set_log_ostream(std::cout);
  vector v1(10, 1);
  vector<int> v2;
  vector v3{1, 2, 3};
  v1.push_back(3);
  v1.push_back(4);
  print_vector(v1, "v1");
  v2.insert(v2.begin(), 10, 2);
  print_vector(v2, "v2");
  v2.insert(v2.begin(), v1.begin(), v1.end());
  print_vector(v2, "v2");
  v2.erase(v2.begin() + 3, v2.begin() + 11);
  print_vector(v2, "v2");
  print_vector(v3, "v3");
  std::cout << "v2[3] = " << v2[3] << std::endl;

  for (int i = 0; i < 10; ++i) {
    v3.push_back(i * 100);
  }
  print_vector(v3, "v3");
  return 0;
}
