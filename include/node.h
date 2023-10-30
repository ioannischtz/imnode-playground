#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <functional>
#include <memory>
#include <numeric>
#include <vector>
namespace nodes_editor {

enum class NodeType {
  add,
  multiply,
  sine,
  input,
  source,
  sink,
};

struct Node {
  NodeType type;
  size_t n_inputs;
  float value;

  explicit Node(const NodeType t) : type{t}, n_inputs{1}, value{0.0f} {}
  Node(const NodeType t, const size_t n, const float v)
      : type{t}, n_inputs{n}, value{v} {}
};

} // namespace nodes_editor
