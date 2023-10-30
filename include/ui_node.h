#pragma once

#include <vector>
namespace nodes_editor {

enum class UiNodeType {
  add,
  multiply,
  sine,
  input,
  source,
  sink,
};

struct UiNode {
  UiNodeType type;
  int id;
  std::vector<int> input_node_ids;
};

} // namespace nodes_editor
