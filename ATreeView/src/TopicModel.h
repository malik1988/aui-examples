//
// Created by malik on 2026/2/7.
//

#pragma once
#include <AUI/Logging/ALogger.h>
#include <AUI/Model/ITreeModel.h>

class ReceivedTopicModel : public ITreeModel<AString> {
public:
  struct Node {
    AString value;
    AString topicFull;
    AVector<_<Node>> children;
    _weak<Node> parent;
  };

public:
  ReceivedTopicModel() = default;

  ~ReceivedTopicModel() override = default;

  size_t childrenCount(const ATreeModelIndexOrRoot &vertex) override {
    auto &container = vertex == ATreeModelIndex::ROOT
                          ? mChildren
                          : (*vertex).as<_<Node>>()->children;
    return container.size();
  }

  AString itemAt(const ATreeModelIndex &index) override {
    return index.as<_<Node>>()->value;
  }

  ATreeModelIndex indexOfChild(size_t row, size_t column,
                               const ATreeModelIndexOrRoot &vertex) override {
    auto &container = vertex == ATreeModelIndex::ROOT
                          ? mChildren
                          : (*vertex).as<_<Node>>()->children;
    return ATreeModelIndex(row, column, container[row]);
  }

  ATreeModelIndex makeIndex(_<Node> node) {
    auto parent = node->parent.lock();
    auto &children = parent ? parent->children : mChildren;
    return ATreeModelIndex(children.indexOf(node).valueOr(0), 0,
                           std::move(node));
  }

  ATreeModelIndexOrRoot parent(const ATreeModelIndex &vertex) override {
    if (auto p = vertex.as<_<Node>>()->parent.lock()) {
      return makeIndex(std::move(p));
    }
    return ATreeModelIndex::ROOT;
  }

  void addTopic(const AString &topic) {
    const auto parts = topic.split('/');
    if (parts.empty())
      return;
    _<Node> parent = nullptr;
    _<Node> first = nullptr;
    int depth = 0;
    bool changed = false;
    for (auto name : parts) {
      if (name.empty())
        continue;
      depth++;
      auto &children = parent == nullptr ? mChildren : parent->children;
      if (!children.empty()) {
        if (const auto &it = std::ranges::find_if(
                children, [&](const auto &v) { return v->value == name; });
            it != children.end()) {
          parent = *it;
          continue;
        }
      }
      auto node = _new<Node>(
          Node{.value = name,
               .topicFull = parent == nullptr
                                ? name
                                : "{}/{}"_format(parent->topicFull, name),
               .parent = parent});

      if (first == nullptr) {
        changed = parent == nullptr ? false : children.empty();
        if (changed)
          first = parent;
        else
          first = node;
      }
      children.push_back(node);
      parent = node;
    }
    if (first) {
      const auto index = makeIndex(first);
      if (changed)
        emit dataChanged(index);
      else
        emit dataInserted(index);
    }
  }

private:
  AVector<_<Node>> mChildren;
};
