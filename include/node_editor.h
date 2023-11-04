#pragma once

#include "graph.h"
#include "imgui.h"
#include "imnodes.h"
#include "node.h"
#include "ui_node.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <stack>
#include <string>

namespace nodes_editor {

class NodeEditor {
public:
  NodeEditor()
      : graph_(), ui_nodes_(), root_node_id_(-1),
        minimap_location_(ImNodesMiniMapLocation_BottomRight),
        current_time_s(0.0) {}

  void show() {
    std::cout << "show()" << std::endl;
    // update timer context
    current_time_s = glfwGetTime();

    buildMenuBar();
    ImGui::TextUnformatted(
        "See the output result using nodes as a processing pipeline.");
    ImGui::Columns(2);
    ImGui::TextUnformatted("A -- add node");
    ImGui::TextUnformatted("X -- delete selected node or link");

    ImNodes::BeginNodeEditor();

    handleNewNodes();

    if (root_node_id_ != -1) {
      process();
    }

    drawNodeBlocks();
    drawEdges();

    ImNodes::MiniMap(0.2f, minimap_location_);
    ImNodes::EndNodeEditor();

    handleNewLinks();
    handleDeletedLinks();

    ImGui::End();
  }

private:
  void buildMenuBar() {
    std::cout << "buildMenuBar()\n";
    auto flags = ImGuiWindowFlags_MenuBar;

    // The node editor window
    ImGui::Begin("node editor", NULL, flags);

    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("Mini-map")) {
        const char *names[] = {
            "Top Left",
            "Top Right",
            "Bottom Left",
            "Bottom Right",
        };
        int locations[] = {
            ImNodesMiniMapLocation_TopLeft,
            ImNodesMiniMapLocation_TopRight,
            ImNodesMiniMapLocation_BottomLeft,
            ImNodesMiniMapLocation_BottomRight,
        };

        for (int i = 0; i < 4; i++) {
          bool selected = minimap_location_ == locations[i];
          if (ImGui::MenuItem(names[i], NULL, &selected))
            minimap_location_ = locations[i];
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Style")) {
        if (ImGui::MenuItem("Classic")) {
          ImGui::StyleColorsClassic();
          ImNodes::StyleColorsClassic();
        }
        if (ImGui::MenuItem("Dark")) {
          ImGui::StyleColorsDark();
          ImNodes::StyleColorsDark();
        }
        if (ImGui::MenuItem("Light")) {
          ImGui::StyleColorsLight();
          ImNodes::StyleColorsLight();
        }
        ImGui::EndMenu();
      }

      ImGui::EndMenuBar();
    }
  }

  void handleNewNodes() {
    std::cout << "handleNewNodes()\n";

    // These are driven by the user, so we place this code before rendering the
    // nodes
    {
      const bool open_popup =
          ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
          ImNodes::IsEditorHovered() && ImGui::IsKeyReleased(ImGuiKey_A);

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
      if (!ImGui::IsAnyItemHovered() && open_popup) {
        ImGui::OpenPopup("add node");
      }

      if (ImGui::BeginPopup("add node")) {
        const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

        if (ImGui::MenuItem("add")) {
          const auto inputNode = Node(NodeType::input, 0, 0.0f);
          const auto procNode = Node(NodeType::add, 2, 0.0f);

          UiNode ui_node;
          ui_node.type = UiNodeType::add;
          ui_node.input_node_ids.push_back(graph_.insert_node(inputNode));
          ui_node.input_node_ids.push_back(graph_.insert_node(inputNode));
          ui_node.id = graph_.insert_node(procNode);

          graph_.insert_edge(ui_node.id, ui_node.input_node_ids[0]);
          graph_.insert_edge(ui_node.id, ui_node.input_node_ids[1]);

          ui_nodes_.push_back(ui_node);
          ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
        }

        if (ImGui::MenuItem("multiply")) {
          const auto inputNode = Node(NodeType::input, 0, 0.0f);
          const auto procNode = Node(NodeType::multiply, 2, 0.0f);

          UiNode ui_node;
          ui_node.type = UiNodeType::multiply;
          ui_node.input_node_ids.push_back(graph_.insert_node(inputNode));
          ui_node.input_node_ids.push_back(graph_.insert_node(inputNode));
          ui_node.id = graph_.insert_node(procNode);

          graph_.insert_edge(ui_node.id, ui_node.input_node_ids[0]);
          graph_.insert_edge(ui_node.id, ui_node.input_node_ids[1]);

          ui_nodes_.push_back(ui_node);
          ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
        }

        if (ImGui::MenuItem("sine")) {
          const auto inputNode = Node(NodeType::input, 0, 0.0f);
          const auto procNode = Node(NodeType::sine);

          UiNode ui_node;
          ui_node.type = UiNodeType::sine;
          ui_node.input_node_ids.push_back(graph_.insert_node(inputNode));
          ui_node.id = graph_.insert_node(procNode);

          graph_.insert_edge(ui_node.id, ui_node.input_node_ids[0]);

          ui_nodes_.push_back(ui_node);
          ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
        }

        if (ImGui::MenuItem("const_source")) {
          UiNode ui_node;
          ui_node.type = UiNodeType::const_source;
          ui_node.id =
              graph_.insert_node(Node(NodeType::const_source, 1, 0.5f));

          ui_nodes_.push_back(ui_node);
          ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
        }

        if (ImGui::MenuItem("time_source")) {
          UiNode ui_node;
          ui_node.type = UiNodeType::time_source;
          ui_node.id = graph_.insert_node(Node(NodeType::time_source));

          ui_nodes_.push_back(ui_node);
          ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
        }

        if (ImGui::MenuItem("sink") && root_node_id_ == -1) {
          const auto inputNode = Node(NodeType::input, 0, 0.0f);
          const auto procNode = Node(NodeType::sink);

          UiNode ui_node;
          ui_node.type = UiNodeType::sink;
          ui_node.input_node_ids.push_back(graph_.insert_node(inputNode));
          ui_node.id = graph_.insert_node(procNode);

          graph_.insert_edge(ui_node.id, ui_node.input_node_ids[0]);

          ui_nodes_.push_back(ui_node);
          ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
          root_node_id_ = ui_node.id;
        }

        ImGui::EndPopup();
      }
      ImGui::PopStyleVar();
    }
  }

  void drawNodeBlocks() {
    std::cout << "drawNodeBlocks()\n";
    for (const UiNode &ui_node : ui_nodes_) {
      switch (ui_node.type) {
      case UiNodeType::add: {
        const float node_width = 100.f;
        ImNodes::BeginNode(ui_node.id);

        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted("add");
        ImNodes::EndNodeTitleBar();

        for (const int &input_node_id : ui_node.input_node_ids) {
          {
            ImNodes::BeginInputAttribute(input_node_id);
            std::string label_txt = "input";
            label_txt += std::to_string(input_node_id);
            const char *label_txt_c = label_txt.c_str();
            const float label_width = ImGui::CalcTextSize(label_txt_c).x;
            ImGui::TextUnformatted(label_txt_c);
            if (graph_.num_edges_from_node(input_node_id) == 0ull) {
              ImGui::SameLine();
              ImGui::PushItemWidth(node_width - label_width);
              ImGui::DragFloat("##hidelabel",
                               &(graph_.node(input_node_id).value), 0.01f);
              ImGui::PopItemWidth();
            }
            ImNodes::EndInputAttribute();
          }
          ImGui::Spacing();
        }

        {
          ImNodes::BeginOutputAttribute(ui_node.id);
          const float label_width = ImGui::CalcTextSize("result").x;
          ImGui::Indent(node_width - label_width);
          ImGui::TextUnformatted("result");
          ImNodes::EndOutputAttribute();
        }

        ImNodes::EndNode();
      } break;
      case UiNodeType::multiply: {
        const float node_width = 100.0f;
        ImNodes::BeginNode(ui_node.id);

        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted("multiply");
        ImNodes::EndNodeTitleBar();

        for (const int &input_node_id : ui_node.input_node_ids) {
          {
            ImNodes::BeginInputAttribute(input_node_id);
            std::string label_txt = "input";
            label_txt += std::to_string(input_node_id);
            const char *label_txt_c = label_txt.c_str();
            const float label_width = ImGui::CalcTextSize(label_txt_c).x;
            ImGui::TextUnformatted(label_txt_c);
            if (graph_.num_edges_from_node(input_node_id) == 0ull) {
              ImGui::SameLine();
              ImGui::PushItemWidth(node_width - label_width);
              ImGui::DragFloat("##hidelabel",
                               &(graph_.node(input_node_id).value), 0.01f);
              ImGui::PopItemWidth();
            }
            ImNodes::EndInputAttribute();
          }
          ImGui::Spacing();
        }

        {
          ImNodes::BeginOutputAttribute(ui_node.id);
          const float label_width = ImGui::CalcTextSize("result").x;
          ImGui::Indent(node_width - label_width);
          ImGui::TextUnformatted("result");
          ImNodes::EndOutputAttribute();
        }

        ImNodes::EndNode();
      } break;
      case UiNodeType::sine: {
        const float node_width = 100.0f;
        ImNodes::BeginNode(ui_node.id);

        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted("sine");
        ImNodes::EndNodeTitleBar();

        {
          ImNodes::BeginInputAttribute(ui_node.input_node_ids[0]);
          const float label_width = ImGui::CalcTextSize("number").x;
          ImGui::TextUnformatted("number");
          const int input_id = ui_node.input_node_ids[0];
          if (graph_.num_edges_from_node(input_id) == 0ull) {
            ImGui::SameLine();
            ImGui::PushItemWidth(node_width - label_width);
            ImGui::DragFloat("##hidelabel", &graph_.node(input_id).value, 0.01f,
                             0.f, 1.0f);
            ImGui::PopItemWidth();
          }
          ImNodes::EndInputAttribute();
        }

        ImGui::Spacing();

        {
          ImNodes::BeginOutputAttribute(ui_node.id);
          const float label_width = ImGui::CalcTextSize("output").x;
          ImGui::Indent(node_width - label_width);
          ImGui::TextUnformatted("output");
          ImNodes::EndInputAttribute();
        }

        ImNodes::EndNode();
      } break;
      case UiNodeType::const_source: {
        ImNodes::BeginNode(ui_node.id);

        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted("source");
        ImNodes::EndNodeTitleBar();

        ImNodes::BeginOutputAttribute(ui_node.id);
        ImGui::Text("output");
        ImNodes::EndOutputAttribute();

        ImNodes::EndNode();
      } break;
      case UiNodeType::time_source: {
        ImNodes::BeginNode(ui_node.id);

        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted("time_source");
        ImNodes::EndNodeTitleBar();

        ImNodes::BeginOutputAttribute(ui_node.id);
        ImGui::Text("output");
        ImNodes::EndOutputAttribute();

        ImGui::Spacing();
        const Node node = graph_.node(ui_node.id);
        std::cout << "Time source Node Value: " << node.value
                  << std::endl; // Debug output
        ImGui::Text("%s sec", std::to_string(node.value).c_str());

        ImNodes::EndNode();
      } break;
      case UiNodeType::sink: {
        const float node_width = 100.0f;
        ImNodes::BeginNode(ui_node.id);

        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted("sink");
        ImNodes::EndNodeTitleBar();

        ImGui::Dummy(ImVec2(node_width, 0.f));

        {
          const int input_id = ui_node.input_node_ids[0];
          ImNodes::BeginInputAttribute(input_id);
          const float label_width = ImGui::CalcTextSize("out_val").x;
          ImGui::TextUnformatted("out_val");
          if (graph_.num_edges_from_node(input_id) == 0ull) {
            ImGui::SameLine();
            ImGui::PushItemWidth(node_width - label_width);
            ImGui::DragFloat("##hidelabel", &graph_.node(input_id).value, 0.01f,
                             0.f, 1.0f);
            ImGui::PopItemWidth();
          }
          ImNodes::EndInputAttribute();
        }

        ImGui::Spacing();
        const Node node = graph_.node(ui_node.id);
        std::cout << "Sink Node Value: " << node.value
                  << std::endl; // Debug output
        ImGui::Text("%s", std::to_string(node.value).c_str());

        ImNodes::EndNode();
      } break;
      case UiNodeType::input:
        break;
      }
    }
  }

  void drawEdges() {
    std::cout << "drawEdges()\n";
    for (const auto &edge : graph_.edges()) {
      // If edge doesn't start on a i/o (value) node, then it's
      // an internal edge.
      // We don't want to render node internals with visible links.
      if (graph_.node(edge.from).type != NodeType::input) {
        continue;
      }

      ImNodes::Link(edge.id, edge.from, edge.to);
    }
  }

  void handleNewLinks() {
    std::cout << "handleNewLinks()\n";
    // These are driven by Imnodes, so we pace the code after EndNodeEditor().

    {
      int start_attr, end_attr;
      if (ImNodes::IsLinkCreated(&start_attr, &end_attr)) {
        const NodeType start_type = graph_.node(start_attr).type;
        const NodeType end_type = graph_.node(end_attr).type;

        const bool valid_link = start_type != end_type;
        if (valid_link) {
          // ensure the edge is always directed from the value to
          // whatever produces the value
          if (start_type != NodeType::input) {
            std::swap(start_attr, end_attr);
          }
          graph_.insert_edge(start_attr, end_attr);
        }
      }
    }
  }

  void handleDeletedLinks() {
    std::cout << "handleDeletedLinks()\n";
    {
      int link_id;
      if (ImNodes::IsLinkDestroyed(&link_id)) {
        graph_.erase_edge(link_id);
      }
    }

    {
      const int num_selected = ImNodes::NumSelectedLinks();
      if (num_selected > 0 && ImGui::IsKeyReleased(ImGuiKey_X)) {
        static std::vector<int> selected_links;
        selected_links.resize(static_cast<size_t>(num_selected));
        ImNodes::GetSelectedLinks(selected_links.data());
        for (const int edge_id : selected_links) {
          graph_.erase_edge(edge_id);
        }
      }
    }

    {
      const int num_selected = ImNodes::NumSelectedNodes();
      if (num_selected > 0 && ImGui::IsKeyReleased(ImGuiKey_X)) {
        static std::vector<int> selected_nodes;
        selected_nodes.resize(static_cast<size_t>(num_selected));
        ImNodes::GetSelectedNodes(selected_nodes.data());
        for (const int node_id : selected_nodes) {
          graph_.erase_node(node_id);
          auto iter = std::find_if(ui_nodes_.begin(), ui_nodes_.end(),
                                   [node_id](const UiNode &node) -> bool {
                                     return node.id == node_id;
                                   });
          // Erase any additional internal nodes
          switch (iter->type) {
          case UiNodeType::add:
          case UiNodeType::multiply:
          case UiNodeType::sine:
            for (const int &input_node_id : iter->input_node_ids) {
              graph_.erase_node(input_node_id);
            }
            break;
          case UiNodeType::sink:
            graph_.erase_node(iter->input_node_ids[0]);
            root_node_id_ = -1;
            break;
          default:
            break;
          }
          ui_nodes_.erase(iter);
        }
      }
    }
  }

  void process() {
    std::cout << "process()\n";
    std::stack<int> postorder;
    dfs_traverse(
        graph_, root_node_id_,
        [&postorder](const int node_id) -> void { postorder.push(node_id); });

    std::stack<float> value_stack;
    while (!postorder.empty()) {
      const int id = postorder.top();
      postorder.pop();
      const Node node = graph_.node(id);

      switch (node.type) {
      case NodeType::add: {
        const float rhs = value_stack.top();
        value_stack.pop();
        const float lhs = value_stack.top();
        value_stack.pop();
        value_stack.push(lhs + rhs);
      } break;
      case NodeType::multiply: {
        const float rhs = value_stack.top();
        value_stack.pop();
        const float lhs = value_stack.top();
        value_stack.pop();
        value_stack.push(rhs * lhs);
      } break;
      case NodeType::sine: {
        const float x = value_stack.top();
        value_stack.pop();
        const float res = std::abs(std::sin(x));
        value_stack.push(res);
      } break;
      case NodeType::const_source: {
        value_stack.push(node.value);
      } break;
      case NodeType::time_source: {
        graph_.set_node_value(id, current_time_s);
        value_stack.push(current_time_s);
      } break;
      case NodeType::input: {
        // If the edge does not have an edge connecting to another node, then
        // just use the value at this node. It means the node's input pin has
        // not been connected to anything and the value comes from the node's
        // UI.
        if (graph_.num_edges_from_node(id) == 0ull) {
          value_stack.push(node.value);
        }
      } break;
      default:
        break;
      }
    }
    graph_.set_node_value(root_node_id_, value_stack.top());
  }

private:
  Graph<Node> graph_;
  std::vector<UiNode> ui_nodes_;
  int root_node_id_;
  ImNodesMiniMapLocation minimap_location_;
  double current_time_s;
};

} // namespace nodes_editor
