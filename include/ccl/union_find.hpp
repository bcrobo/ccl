#include <limits>
#include <vector>

constexpr std::size_t invalid_node = std::numeric_limits<std::size_t>::max();

class UnionFind
{
public:
  UnionFind(std::size_t n);
  void makeSet();
  std::size_t findRoot(std::size_t i) const;
  void appendTo(std::size_t to);

  std::size_t nbElements() const;
  std::size_t nbSets() const;
  void print() const;
private:
  std::size_t m_node_index;
  std::vector<std::size_t> m_nodes;
};
