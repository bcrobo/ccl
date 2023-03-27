#include <ccl/union_find.hpp>
#include <iostream>

int main()
{
  auto uf = UnionFind{10};

  uf.makeSet();

  uf.appendTo(0);
  uf.appendTo(1);

  uf.makeSet();
  uf.makeSet();
  std::cout << "Nb elements: " << uf.nbElements() << "\n";
  std::cout << "Nb sets: " << uf.nbSets() << "\n";
  std::cout << uf.findRoot(2) << "\n";
  return 0;
}

