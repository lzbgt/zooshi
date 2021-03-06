// Copyright 2016 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ZOOSHI_ADMOB_H_
#define ZOOSHI_ADMOB_H_

#include <cstring>

#include "mathfu/internal/disable_warnings_begin.h"

#include "firebase/admob.h"
#include "firebase/admob/rewarded_video.h"
#include "firebase/app.h"

#include "mathfu/internal/disable_warnings_end.h"

namespace fpl {
namespace zooshi {

struct World;

enum AdMobStatus {
  kAdMobStatusInitializing,  // The service is currently initializing.
  kAdMobStatusLoading,       // A new ad is loading in the background.
  kAdMobStatusAvailable,     // The ad is ready to be shown.
  kAdMobStatusShowing,       // The ad is currently showing.
  kAdMobStatusError,         // An error occurred.
};

enum RewardedVideoLocation {
  kRewardedVideoLocationPregame = 0,  // Offer the video before the game.
  kRewardedVideoLocationScoreScreen,  // Offer the video on the score screen.
  kRewardedVideoLocationCount,        // Total count, should be last.
};

class RewardedVideoListener : public firebase::admob::rewarded_video::Listener {
 public:
  RewardedVideoListener();

  void OnPresentationStateChanged(
      firebase::admob::rewarded_video::PresentationState state) override;

  void OnRewarded(firebase::admob::rewarded_video::RewardItem reward) override;

  firebase::admob::rewarded_video::PresentationState presentation_state()
      const {
    return presentation_state_;
  }

  const firebase::admob::rewarded_video::RewardItem reward_item() const {
    return reward_item_;
  }

  bool earned_reward() const { return earned_reward_; }

  bool expecting_state_change() const { return expecting_state_change_; }
  void set_expecting_state_change(bool val) { expecting_state_change_ = val; }

  void Reset() {
    std::memset(&reward_item_, 0, sizeof(reward_item_));
    earned_reward_ = false;
  }

 private:
  bool earned_reward_;
  bool expecting_state_change_;
  firebase::admob::rewarded_video::PresentationState presentation_state_;
  firebase::admob::rewarded_video::RewardItem reward_item_;
};

class AdMobHelper {
 public:
  ~AdMobHelper();

  void Initialize(const firebase::App& app);

  void LoadNewRewardedVideo();

  void ShowRewardedVideo();
  bool CheckShowRewardedVideo();

  AdMobStatus rewarded_video_status() const { return rewarded_video_status_; }
  bool rewarded_video_available() const {
    return rewarded_video_status_ == kAdMobStatusAvailable;
  }

  bool rewarded_video_watched() const { return listener_.earned_reward(); }

  void ResetRewardedVideo() { listener_.Reset(); }

  RewardedVideoLocation GetRewardedVideoLocation();

  float reward_value() { return listener_.reward_item().amount; }

 private:
  static void InitializeCompletion(
    const firebase::Future<void>& completed_future, void* void_helper);
  static void LoadNewRewardedVideoCompletion(
    const firebase::Future<void>& completed_future, void* void_helper);
  static void ShowRewardedVideoCompletion(
    const firebase::Future<void>& completed_future, void* void_helper);

  RewardedVideoListener listener_;
  AdMobStatus rewarded_video_status_;
};

}  // zooshi
}  // fpl

#endif  // ZOOSHI_ADMOB_H_
