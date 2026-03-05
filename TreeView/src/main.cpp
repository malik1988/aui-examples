/*
 * AUI Framework - Declarative UI toolkit for modern C++20
 * Copyright (C) 2020-2025 Alex2772 and Contributors
 *
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <AUI/Common/AObject.h>
#include <AUI/Common/AProperty.h>
#include <AUI/Common/AString.h>
#include <AUI/Common/AVector.h>
#include <AUI/Common/SharedPtr.h>
#include <AUI/Common/SharedPtrTypes.h>
#include <AUI/Platform/AWindow.h>
#include <AUI/Platform/Entry.h>
#include <AUI/Util/UIBuildingHelpers.h>
#include <AUI/Util/kAUI.h>
#include <AUI/View/AButton.h>
#include <AUI/View/ALabel.h>
#include <AUI/View/AScrollArea.h>
#include <AUI/Layout/AVerticalLayout.h>
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/copy_if.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/view/filter.hpp>
#include <AUI/View/AForEachUI.h>
#include <AUI/View/ACheckBox.h>
#include <AUI/View/ATextField.h>

using namespace declarative;

struct Node {
    AString name;
    AProperty<bool> toExtract = true;
    AProperty<bool> treeExpanded = false;
    _weak<Node> parent;
    AProperty<AVector<_<Node>>> children;
    AProperty<bool> isExpandable = false;

    bool isExtractRecursiveAny() const {
        return toExtract || ranges::any_of(*children, [](const auto& f) { return f->isExtractRecursiveAny(); });
    }

    bool isExtractRecursiveAll() const {
        return toExtract && ranges::all_of(*children, [](const auto& f) { return f->isExtractRecursiveAll(); });
    }

    void setExtractRecursive(bool extract) {
        toExtract = extract;
        for (const auto& f : *children) {
            f->setExtractRecursive(extract);
        }
    }
};
static _<AView> contentTree(_<AProperty<AVector<_<Node>>>> nodes) {
    return AUI_DECLARATIVE_FOR(i, *(nodes.value()), AVerticalLayout) {
        return Horizontal { Label {
          AUI_REACT(i->treeExpanded ? "v" : ">"),
        } AUI_OVERRIDE_STYLE {
          FixedSize { 14_dp },
          ATextAlign::CENTER,
        } AUI_LET { AObject::connect(it->clicked, AObject::GENERIC_OBSERVER, [i] {
            i->treeExpanded = !i->treeExpanded;
        });
        AObject::connect(i->isExpandable, AUI_SLOT(it)::setVisible);
    },
          Vertical {
        CheckBox {
            .checked = AUI_REACT(i->isExtractRecursiveAny()),
            .onCheckedChange = [i](bool g) { i->setExtractRecursive(g); },
            .content = Label { i->name },
        },
            Horizontal {
                contentTree(AUI_PTR_ALIAS(i, children)),
            } AUI_LET {
            AObject::connect(i->treeExpanded, AObject::GENERIC_OBSERVER, [it, i, nodes] {
                auto ns = nodes;
                it->setVisibility(i->treeExpanded ? Visibility::VISIBLE : Visibility::GONE);
            });
        }
        ,
    }
};
}
;
}

class TreeViewWindow : public AWindow {
public:
    TreeViewWindow() : AWindow("AUI - ATreeView Test", 200_dp, 100_dp) {
        addTopic("wf/1/2");

        setContents(Centered {
          Vertical::Expanding {
            Label { "Topic" },
            _new<ATextField>() && topic_,
            Button {
              .content = Label { "Manual Add" },
              .onClick = [this] { addTopic(topic_); },
            },
            Button {
              .content = Label { "Test Case 1" },
              .onClick =
                  [this] {
                      AVector<AString> topics1 {
                          "QTTsample/parungkuda/10/value",
                          "QTTsample/kasemen/8/value",
                          "me_sna_grid_kw",
                          "me_sna_gen_kw",
                      };
                      for (const auto& tp : topics1) {
                          addTopic(tp);
                      }
                  } },

            AScrollArea::Builder()
                .withContents(contentTree(topics_))
                .build() AUI_OVERRIDE_STYLE {
                  Expanding(),
                  BackgroundSolid { AColor::WHITE },
                  Border { 1_px, AColor::BLACK },
                  Padding { 2_px },
                  MinSize { 200_dp },
                },
          },
        });
    }

private:
    void addTopic(const AString& topic) {
        const auto parts = topic.split('/');
        if (parts.empty())
            return;
        _<Node> parent = nullptr;
        for (auto name : parts) {
            if (name.empty())
                continue;
            auto& children = parent == nullptr ? *topics_ : parent->children;
            if (!children->empty()) {
                if (const auto& it = std::ranges::find_if(*children, [&](const auto& v) { return v->name == name; });
                    it != children->end()) {
                    parent = *it;
                    continue;
                }
            }
            auto node = _new<Node>(Node { .name = name, .parent = parent });

            children.writeScope()->push_back(node);
            if (parent)
                parent->isExpandable = true;
            parent = node;
        }
    }

private:
    AProperty<AString> filtertopic_;
    AProperty<AString> topic_;

    _<AProperty<AVector<_<Node>>>> topics_ = _new<AProperty<AVector<_<Node>>>>();
};

AUI_ENTRY {
    _new<TreeViewWindow>()->show();
    return 0;
}