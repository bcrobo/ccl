#include <ccl/union_find.hpp>
#include <algorithm>
#include <iterator>
#include <iostream>

UnionFind::UnionFind(std::size_t n)
  :m_node_index{0}
  ,m_nodes(n, invalid_node)
{}

void UnionFind::makeSet()
{
  m_nodes[m_node_index++] = m_node_index;
}

void UnionFind::appendTo(std::size_t to)
{
  m_nodes[m_node_index++] = to;
}

std::size_t UnionFind::findRoot(std::size_t i) const
{
  while(m_nodes[i] != i)
    i = m_nodes[i];
  return i;
}

std::size_t UnionFind::nbElements() const
{
  return std::count_if(
		  std::begin(m_nodes),
		  std::end(m_nodes),
		  [](const auto& i) { return i != invalid_node; });
}

void UnionFind::print() const
{
  for(const auto& n : m_nodes)
  {
    std::cout << n << " ";
  }
  std::cout << "\n"; 
}
std::size_t UnionFind::nbSets() const
{
  auto i{0u}, c{0u};
  for(const auto& n : m_nodes)
  {
    if(i == n)
      ++c;
    ++i;
  }
  return c;
}
