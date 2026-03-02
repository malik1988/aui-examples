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

#include "AUI/View/ATextField.h"
#include <AUI/Common/AString.h>
#include <AUI/Common/AVector.h>
#include <AUI/Platform/AWindow.h>
#include <AUI/Platform/Entry.h>
#include <AUI/Util/UIBuildingHelpers.h>
#include <AUI/View/AButton.h>

#include <AUI/View/ALabel.h>
#include <AUI/View/ATreeView.h>

#include "TopicModel.h"

using namespace declarative;

/// [counter]
class TreeViewWindow : public AWindow {
public:
  TreeViewWindow() : AWindow("AUI - ATreeView Test", 200_dp, 100_dp) {
    setContents(Centered{
        Vertical::Expanding{
            Label{"Topic"},
            _new<ATextField>() && Topic_,
            Button{Label{"Manual Add"},
                   [this] { receivedTopicModel_->addTopic(Topic_); }},
            Button{Label{"Test Case 1"},
                   [this] {
                     AVector<AString> topics{
                         "QTTsample/parungkuda/10/value",
                         "QTTsample/kasemen/8/value",
                         "me_sna_grid_kw",
                         "me_sna_gen_kw",
                     };
                     for (const auto &tp : topics) {
                       receivedTopicModel_->addTopic(tp);
                     }
                   }},
            Button{Label{"Test Case 2"},
                   [this] {
                     AVector<AString> topics{
                         "QTTsample/parungkuda/10/value",
                         "me_sna_grid_kw",
                         "me_sna_gen_kw",
                         "QTTsample/kasemen/8/value",
                     };
                     for (const auto &tp : topics) {
                       receivedTopicModel_->addTopic(tp);
                     }
                   }},
            _new<ATreeView>(receivedTopicModel_),
        },
    });
  }

private:
  _<ReceivedTopicModel> receivedTopicModel_ = _new<ReceivedTopicModel>();
  AProperty<AString> filterTopic_;
  AProperty<AString> Topic_;
};
/// [counter]

AUI_ENTRY {
  _new<TreeViewWindow>()->show();
  return 0;
}