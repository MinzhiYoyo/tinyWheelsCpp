#include <iostream>
#include "list.h"

using namespace tinyWheels;

template<class T>
void print_list(const list<T>& vec, const char * name) {
  std::cout << name << " = [" << vec << "]" << std::endl;
}

int main(){
  list v1(10, 1);
  list<int> v2;
  list v3{1, 2, 3};
  v1.push_back(3);
  v1.push_back(4);
  print_list(v1, "v1");

  v2.insert(v2.begin(), 10, 2);
  print_list(v2, "v2");
  v2.insert(v2.begin(), v1.begin(), v1.end());
  print_list(v2, "v2");
  v2.erase(v2.begin() + 3, v2.begin() + 11);
  print_list(v2, "v2");
  print_list(v3, "v3");
  return 0;
}
