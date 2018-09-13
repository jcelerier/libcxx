#include <nostd/string>
#include <nostd/vector>
#include <nostd/algorithm>
#include <nostd/utility>
#include <nostd/iostream>
#include <nostd/thread>
int main()
{
  nostd::thread t([] {
    nostd::vector<int> c;
    c.resize(10);
    nostd::sort(c.begin(), c.end());
    
    nostd::cout << "hello: " << c.size() << nostd::endl;
  });
  
  t.join();
  return 0;
}
