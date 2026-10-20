#include "quickreset.h"

#include "mkb/mkb.h"

#include "systems/binds.h"
#include "systems/pref.h"
#include "utils/relutil.h"

namespace quickreset {

bool can_quick_reset() {
    if (mkb::main_mode != mkb::MD_GAME) return false;

    if (mkb::main_game_mode == mkb::PRACTICE_MODE) {
        if (pref::get(pref::Pref::QuickResetPractice)) return true;
    } else if (mkb::main_game_mode == mkb::STORY_MODE) {
        if (pref::get(pref::Pref::QuickResetStory)) return true;
    } else if (mkb::main_game_mode == mkb::CHALLENGE_MODE) {
        if (pref::get(pref::Pref::QuickResetChallenge)) return true;
    } else {
        return false;
    }

    return false;
}

bool valid_sub_mode() {
    if (mkb::main_mode != mkb::MD_GAME) return false;
    if (mkb::sub_mode == mkb::SMD_GAME_READY_INIT || mkb::sub_mode == mkb::SMD_GAME_READY_MAIN ||
        mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT || mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN ||
        mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN ||
        mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_MAIN ||
        mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN ||
        mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT || mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN)
        return true;
    return false;
}

void tick() {
    int ball_index;
    mkb::Ball *ball;
    bool paused_now = *reinterpret_cast<u32 *>(relutil::relocate_addr(0x805BC474)) & 8;

    if (binds::bind_pressed(pref::get(pref::Pref::QuickResetBind))) {
        if (can_quick_reset() && valid_sub_mode() && !paused_now) {
            // this code is copied wholesale from workshop mod's "retry in challenge mode" option
            mkb::mode_info.attempt_count = mkb::mode_info.attempt_count + 1;
            ball = mkb::balls;
            ball_index = mkb::curr_player_idx;
            for (ball_index = 0; ball_index < 8; ball_index = ball_index + 1) {
                if (ball->status == mkb::STAT_NORMAL) {
                    ball->phys_flags = ball->phys_flags;
                    ball->g_effect_flags = ball->g_effect_flags | 0x80;
                }
                ball = ball + 1;
            }
            mkb::sub_mode_request = mkb::SMD_GAME_READY_INIT;
            mkb::g_fade_track_volume(100, '\n');
        }
    }
}
}  // namespace quickreset
