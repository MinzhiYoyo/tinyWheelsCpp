#include <iostream>
#include "stack.h"
using namespace std;

int main(){
  tinyWheels::stack<int> st;
  for (auto i = 1; i < 20; ++i) {
    st.push(i);
  }

  while (not st.empty()) {
    auto v = st.top();
    cout << v << " ";
    st.pop();
  }
  cout << endl;

  return 0;
}
