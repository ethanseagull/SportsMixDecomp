#include "game/msw/CC_PARAM_BOSS.hpp"
#include "game/msw/CChrCoreBoss.hpp"
#include "game/msw/CGame.hpp"
#include "bcs/New.hpp"

// Character control parameters for the boss mode.
//
// CHR_CORE_BOSS::CC_PARAM_BOSS holds the defaults. Each character has a
// cc_data_boss_<Name>::CC_PARAM_BOSS_<Name> whose constructor overrides
// part of them, and one class per game level (EASY ... EXPERT, each with a
// _STAR variant for the star player) whose constructors change nothing in
// this mode. CChrCoreBoss<Name>::NewCcParam_ picks the class from the game
// level and the star flag.
//
// Field names are placeholders (see the header). The order of the
// assignments is the order of the stores in the original.

// The original file was compiled without optimisation and without inlining
// (a generated data file, most likely). Evidence: every float constant is
// reloaded for each store, the level constructors, CChrCore::IsStarPlayer
// and CGame::GetGameLevel are called instead of inlined (the inline ones
// emitted after their first caller), and the switches become a compare
// tree. Only the two settings below reproduce all of it. They were in the
// source, not on the command line: the static initialiser, which the
// compiler builds with the command-line settings, is an optimised tail call
// in the original, and per-file flags (-opt level=0 -inline off) give a
// full stack frame there instead. Ignored by other compilers.
// file-level optimization pragma (see notes/decisions.md)
#pragma optimization_level 0
#pragma dont_inline on

namespace msw {

CHR_CORE_BOSS::CC_PARAM_BOSS::CC_PARAM_BOSS() {
    m_Unk00 = 0;
    m_Unk04 = 0;
    m_Unk08 = 0;
    m_Unk0C = 6.3f;
    m_Unk10 = 15.0f;
    m_Unk14 = 0.07f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 25.0f;
    m_Unk24 = 0.8f;
    m_Unk28 = 1.0f;
    m_Unk2C = 1.0f;
    m_Unk30 = 1.0f;
    m_Unk34 = 0.0f;
    m_Unk38 = 17.0f;
    m_Unk3C = 2.65f;
    m_Unk40 = 12.0f;
    m_Unk44 = 0.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 1.8f;
    m_Unk64 = 1.3f;
    m_Unk68 = 3.5f;
    m_Unk6C = 10.0f;
    m_Unk70 = 20.0f;
    m_Unk74 = 3.5f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 0;
    m_Unk90 = 0.75f;
    m_Unk94 = 1.8f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 14.5f;
    m_UnkBC = 23.0f;
    m_UnkC0 = 26.0f;
    m_UnkC4 = 0.5f;
    m_UnkC8 = 25.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 17.0f;
    m_UnkDC = 26.0f;
    m_UnkE0 = 29.0f;
    m_UnkE4 = 0.8f;
    m_UnkE8 = 22.0f;
    m_UnkEC = 23.0f;
    m_UnkF0 = 38.0f;
    m_UnkF4 = 38.0f;
    m_UnkF8 = 0.5f;
    m_UnkFC = 15.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 0.75f;
    m_Unk110 = 1.8f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 10.0f;
    m_Unk128 = 8.0f;
    m_Unk12C = 0.5f;
    m_Unk130 = 11.0f;
    m_Unk134 = 6.0f;
    m_Unk138 = 1.0f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk144 = 120.0f;
    m_Unk148 = 5.3f;
    m_Unk14C = 0.0f;
    m_Unk150 = 0.0f;
    m_Unk154 = 30.0f;
    m_Unk158 = 30.0f;
    m_Unk15C = 1.1f;
    m_Unk160 = 5.0f;
    m_Unk164 = 18.5f;
    m_Unk168 = 18.5f;
    m_Unk16C = 18.5f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1;
    m_Unk178 = 4.0f;
    m_Unk17C = 4.0f;
    m_Unk180 = 0.0f;
    m_Unk184 = 4.0f;
    m_Unk188 = 2.0f;
    m_Unk18C = 20.0f;
    m_Unk190 = 3.0f;
    m_Unk194 = 14.0f;
    m_Unk198 = 6.0f;
    m_Unk19C = 1.0f;
    m_Unk1A0 = 9.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk1AC = 3.0f;
    m_Unk1B0 = 24.0f;
    m_Unk1B4 = 0;
    m_Unk1B8 = 26.0f;
    m_Unk1BC = 0.0f;
    m_Unk1C0 = 0;
    m_Unk1C4 = 11.0f;
    m_Unk1C8 = 0.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 53;
    m_Unk1D4 = 1;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 40;
    m_Unk1E0 = 0.9f;
    m_Unk1E4 = 1.0f;
    m_Unk1E8 = 0;
    m_Unk1EC = 85;
    m_Unk1F0 = 1;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 3.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 60.0f;
    m_Unk230 = 30.0f;
    m_Unk234 = 0;
}

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBoss::NewCcParam_(long nHeap) const {
    return new (nHeap) CHR_CORE_BOSS::CC_PARAM_BOSS;
}

// (what __init2 is for is not known; every one is empty)
void cc_data_boss_Mario::CC_PARAM_BOSS_Mario::__init2() {}
void cc_data_boss_Luigi::CC_PARAM_BOSS_Luigi::__init2() {}
void cc_data_boss_Yoshi::CC_PARAM_BOSS_Yoshi::__init2() {}
void cc_data_boss_Ninja::CC_PARAM_BOSS_Ninja::__init2() {}
void cc_data_boss_Mii00::CC_PARAM_BOSS_Mii00::__init2() {}
void cc_data_boss_Mii01::CC_PARAM_BOSS_Mii01::__init2() {}
void cc_data_boss_Peach::CC_PARAM_BOSS_Peach::__init2() {}
void cc_data_boss_Daisy::CC_PARAM_BOSS_Daisy::__init2() {}
void cc_data_boss_Waluigi::CC_PARAM_BOSS_Waluigi::__init2() {}
void cc_data_boss_Siroma::CC_PARAM_BOSS_Siroma::__init2() {}
void cc_data_boss_Koopa::CC_PARAM_BOSS_Koopa::__init2() {}
void cc_data_boss_Donkey::CC_PARAM_BOSS_Donkey::__init2() {}
void cc_data_boss_Wario::CC_PARAM_BOSS_Wario::__init2() {}
void cc_data_boss_KoopaJr::CC_PARAM_BOSS_KoopaJr::__init2() {}
void cc_data_boss_Kinopio::CC_PARAM_BOSS_Kinopio::__init2() {}
void cc_data_boss_Diddy::CC_PARAM_BOSS_Diddy::__init2() {}
void cc_data_boss_Moogle::CC_PARAM_BOSS_Moogle::__init2() {}
void cc_data_boss_Saboten::CC_PARAM_BOSS_Saboten::__init2() {}
void cc_data_boss_Slime::CC_PARAM_BOSS_Slime::__init2() {}
void cc_data_boss_Kuroma::CC_PARAM_BOSS_Kuroma::__init2() {}

// --------------------------------------------------------------------------
// Mario

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossMario::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Mario;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Mario_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Mario_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Mario_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Mario_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Mario_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Mario_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Mario_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Mario_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Mario {

CC_PARAM_BOSS_Mario_EASY_STAR::CC_PARAM_BOSS_Mario_EASY_STAR() {}

CC_PARAM_BOSS_Mario_EASY::CC_PARAM_BOSS_Mario_EASY() {}

CC_PARAM_BOSS_Mario::CC_PARAM_BOSS_Mario() {
    m_Unk0C = 6.3f;
    m_Unk10 = 5.0f;
    m_Unk14 = 0.07f;
    m_Unk28 = 1.0f;
    m_Unk2C = 1.0f;
    m_Unk30 = 1.0f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 8.0f;
    m_Unk24 = 0.3f;
    m_Unk00 = 0;
    m_Unk04 = 0;
    m_Unk08 = 0;
    m_Unk34 = 0.0f;
    m_Unk38 = 17.0f;
    m_Unk3C = 2.65f;
    m_Unk40 = 12.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.0f;
    m_Unk64 = 2.0f;
    m_Unk68 = 4.0f;
    m_Unk6C = 11.0f;
    m_Unk70 = 10.0f;
    m_Unk74 = 2.8f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 22.0f;
    m_Unk1C4 = 28.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 55;
    m_Unk1D4 = 1;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 40;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 120;
    m_Unk1F0 = 1;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 0.9f;
    m_Unk94 = 3.0f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 12.0f;
    m_UnkBC = 20.0f;
    m_UnkC0 = 22.0f;
    m_UnkC4 = 0.3f;
    m_UnkC8 = 27.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 14.0f;
    m_UnkDC = 22.0f;
    m_UnkE0 = 24.0f;
    m_UnkE4 = 0.4f;
    m_UnkE8 = 23.0f;
    m_UnkEC = 14.0f;
    m_UnkF0 = 22.0f;
    m_UnkF4 = 24.0f;
    m_UnkF8 = 0.4f;
    m_UnkFC = 23.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 1.1f;
    m_Unk110 = 3.0f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 20.0f;
    m_Unk128 = 8.0f;
    m_Unk12C = 0.5f;
    m_Unk130 = 15.0f;
    m_Unk134 = 20.0f;
    m_Unk138 = 0.8f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 9.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 45.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Mario_NORMAL_STAR::CC_PARAM_BOSS_Mario_NORMAL_STAR() {}

CC_PARAM_BOSS_Mario_NORMAL::CC_PARAM_BOSS_Mario_NORMAL() {}

CC_PARAM_BOSS_Mario_HARD_STAR::CC_PARAM_BOSS_Mario_HARD_STAR() {}

CC_PARAM_BOSS_Mario_HARD::CC_PARAM_BOSS_Mario_HARD() {}

CC_PARAM_BOSS_Mario_EXPERT_STAR::CC_PARAM_BOSS_Mario_EXPERT_STAR() {}

CC_PARAM_BOSS_Mario_EXPERT::CC_PARAM_BOSS_Mario_EXPERT() {}

} // namespace cc_data_boss_Mario

// --------------------------------------------------------------------------
// Luigi

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossLuigi::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Luigi;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Luigi_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Luigi_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Luigi_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Luigi_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Luigi_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Luigi_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Luigi_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Luigi_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Luigi {

CC_PARAM_BOSS_Luigi_EASY_STAR::CC_PARAM_BOSS_Luigi_EASY_STAR() {}

CC_PARAM_BOSS_Luigi_EASY::CC_PARAM_BOSS_Luigi_EASY() {}

CC_PARAM_BOSS_Luigi::CC_PARAM_BOSS_Luigi() {
    m_Unk0C = 7.1f;
    m_Unk10 = 15.0f;
    m_Unk14 = 0.05f;
    m_Unk28 = 1.0f;
    m_Unk2C = 1.0f;
    m_Unk30 = 1.0f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 8.0f;
    m_Unk24 = 0.3f;
    m_Unk00 = 0;
    m_Unk04 = 0;
    m_Unk08 = 0;
    m_Unk34 = 0.0f;
    m_Unk38 = 17.0f;
    m_Unk3C = 3.7f;
    m_Unk40 = 20.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.0f;
    m_Unk64 = 2.0f;
    m_Unk68 = 4.5f;
    m_Unk6C = 11.0f;
    m_Unk70 = 10.0f;
    m_Unk74 = 2.8f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 21.0f;
    m_Unk1C4 = 29.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 63;
    m_Unk1D4 = 1;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 50;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 120;
    m_Unk1F0 = 1;
    m_Unk1F4 = 4.2f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 0.9f;
    m_Unk94 = 3.0f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 12.0f;
    m_UnkBC = 20.0f;
    m_UnkC0 = 22.0f;
    m_UnkC4 = 0.3f;
    m_UnkC8 = 27.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 14.0f;
    m_UnkDC = 22.0f;
    m_UnkE0 = 24.0f;
    m_UnkE4 = 0.4f;
    m_UnkE8 = 23.0f;
    m_UnkEC = 14.0f;
    m_UnkF0 = 22.0f;
    m_UnkF4 = 24.0f;
    m_UnkF8 = 0.4f;
    m_UnkFC = 23.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 0.9f;
    m_Unk110 = 3.0f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 20.0f;
    m_Unk128 = 10.0f;
    m_Unk12C = 1.0f;
    m_Unk130 = 20.0f;
    m_Unk134 = 30.0f;
    m_Unk138 = 0.8f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 9.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 60.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Luigi_NORMAL_STAR::CC_PARAM_BOSS_Luigi_NORMAL_STAR() {}

CC_PARAM_BOSS_Luigi_NORMAL::CC_PARAM_BOSS_Luigi_NORMAL() {}

CC_PARAM_BOSS_Luigi_HARD_STAR::CC_PARAM_BOSS_Luigi_HARD_STAR() {}

CC_PARAM_BOSS_Luigi_HARD::CC_PARAM_BOSS_Luigi_HARD() {}

CC_PARAM_BOSS_Luigi_EXPERT_STAR::CC_PARAM_BOSS_Luigi_EXPERT_STAR() {}

CC_PARAM_BOSS_Luigi_EXPERT::CC_PARAM_BOSS_Luigi_EXPERT() {}

} // namespace cc_data_boss_Luigi

// --------------------------------------------------------------------------
// Peach

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossPeach::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Peach;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Peach_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Peach_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Peach_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Peach_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Peach_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Peach_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Peach_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Peach_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Peach {

CC_PARAM_BOSS_Peach_EASY_STAR::CC_PARAM_BOSS_Peach_EASY_STAR() {}

CC_PARAM_BOSS_Peach_EASY::CC_PARAM_BOSS_Peach_EASY() {}

CC_PARAM_BOSS_Peach::CC_PARAM_BOSS_Peach() {
    m_Unk0C = 6.3f;
    m_Unk10 = 5.0f;
    m_Unk14 = 0.07f;
    m_Unk28 = 1.1f;
    m_Unk2C = 1.1f;
    m_Unk30 = 1.1f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 8.0f;
    m_Unk24 = 0.3f;
    m_Unk00 = 1;
    m_Unk04 = 1;
    m_Unk08 = 1;
    m_Unk34 = 0.0f;
    m_Unk38 = 22.0f;
    m_Unk3C = 2.65f;
    m_Unk40 = 15.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.0f;
    m_Unk64 = 2.0f;
    m_Unk68 = 4.5f;
    m_Unk6C = 12.0f;
    m_Unk70 = 7.0f;
    m_Unk74 = 2.5f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 25.0f;
    m_Unk1C4 = 25.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 80;
    m_Unk1D4 = 0;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 60;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 120;
    m_Unk1F0 = 0;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 0.9f;
    m_Unk94 = 5.0f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 12.0f;
    m_UnkBC = 19.0f;
    m_UnkC0 = 21.0f;
    m_UnkC4 = 0.3f;
    m_UnkC8 = 29.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 13.0f;
    m_UnkDC = 21.0f;
    m_UnkE0 = 23.0f;
    m_UnkE4 = 0.4f;
    m_UnkE8 = 25.0f;
    m_UnkEC = 13.0f;
    m_UnkF0 = 21.0f;
    m_UnkF4 = 23.0f;
    m_UnkF8 = 0.4f;
    m_UnkFC = 25.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 1.0f;
    m_Unk110 = 5.0f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 180.0f;
    m_Unk124 = 10.0f;
    m_Unk128 = 8.0f;
    m_Unk12C = 0.5f;
    m_Unk130 = 15.0f;
    m_Unk134 = 20.0f;
    m_Unk138 = 0.8f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 12.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 50.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Peach_NORMAL_STAR::CC_PARAM_BOSS_Peach_NORMAL_STAR() {}

CC_PARAM_BOSS_Peach_NORMAL::CC_PARAM_BOSS_Peach_NORMAL() {}

CC_PARAM_BOSS_Peach_HARD_STAR::CC_PARAM_BOSS_Peach_HARD_STAR() {}

CC_PARAM_BOSS_Peach_HARD::CC_PARAM_BOSS_Peach_HARD() {}

CC_PARAM_BOSS_Peach_EXPERT_STAR::CC_PARAM_BOSS_Peach_EXPERT_STAR() {}

CC_PARAM_BOSS_Peach_EXPERT::CC_PARAM_BOSS_Peach_EXPERT() {}

} // namespace cc_data_boss_Peach

// --------------------------------------------------------------------------
// Daisy

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossDaisy::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Daisy;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Daisy_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Daisy_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Daisy_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Daisy_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Daisy_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Daisy_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Daisy_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Daisy_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Daisy {

CC_PARAM_BOSS_Daisy_EASY_STAR::CC_PARAM_BOSS_Daisy_EASY_STAR() {}

CC_PARAM_BOSS_Daisy_EASY::CC_PARAM_BOSS_Daisy_EASY() {}

CC_PARAM_BOSS_Daisy::CC_PARAM_BOSS_Daisy() {
    m_Unk0C = 6.6f;
    m_Unk10 = 3.0f;
    m_Unk14 = 0.07f;
    m_Unk28 = 1.15f;
    m_Unk2C = 1.15f;
    m_Unk30 = 1.15f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 10.0f;
    m_Unk24 = 0.6f;
    m_Unk00 = 1;
    m_Unk04 = 1;
    m_Unk08 = 1;
    m_Unk34 = 0.0f;
    m_Unk38 = 22.0f;
    m_Unk3C = 2.6f;
    m_Unk40 = 15.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.0f;
    m_Unk64 = 2.0f;
    m_Unk68 = 4.5f;
    m_Unk6C = 12.0f;
    m_Unk70 = 6.0f;
    m_Unk74 = 2.8f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 23.0f;
    m_Unk1C4 = 28.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 70;
    m_Unk1D4 = 1;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 50;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 120;
    m_Unk1F0 = 1;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 0.9f;
    m_Unk94 = 5.0f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 12.0f;
    m_UnkBC = 19.0f;
    m_UnkC0 = 21.0f;
    m_UnkC4 = 0.3f;
    m_UnkC8 = 29.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 13.0f;
    m_UnkDC = 21.0f;
    m_UnkE0 = 23.0f;
    m_UnkE4 = 0.4f;
    m_UnkE8 = 25.0f;
    m_UnkEC = 13.0f;
    m_UnkF0 = 21.0f;
    m_UnkF4 = 23.0f;
    m_UnkF8 = 0.4f;
    m_UnkFC = 25.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 1.0f;
    m_Unk110 = 5.0f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 17.0f;
    m_Unk128 = 10.0f;
    m_Unk12C = 0.5f;
    m_Unk130 = 25.0f;
    m_Unk134 = 26.0f;
    m_Unk138 = 0.3f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 12.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 50.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Daisy_NORMAL_STAR::CC_PARAM_BOSS_Daisy_NORMAL_STAR() {}

CC_PARAM_BOSS_Daisy_NORMAL::CC_PARAM_BOSS_Daisy_NORMAL() {}

CC_PARAM_BOSS_Daisy_HARD_STAR::CC_PARAM_BOSS_Daisy_HARD_STAR() {}

CC_PARAM_BOSS_Daisy_HARD::CC_PARAM_BOSS_Daisy_HARD() {}

CC_PARAM_BOSS_Daisy_EXPERT_STAR::CC_PARAM_BOSS_Daisy_EXPERT_STAR() {}

CC_PARAM_BOSS_Daisy_EXPERT::CC_PARAM_BOSS_Daisy_EXPERT() {}

} // namespace cc_data_boss_Daisy

// --------------------------------------------------------------------------
// Yoshi

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossYoshi::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Yoshi;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Yoshi_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Yoshi_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Yoshi_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Yoshi_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Yoshi_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Yoshi_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Yoshi_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Yoshi_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Yoshi {

CC_PARAM_BOSS_Yoshi_EASY_STAR::CC_PARAM_BOSS_Yoshi_EASY_STAR() {}

CC_PARAM_BOSS_Yoshi_EASY::CC_PARAM_BOSS_Yoshi_EASY() {}

CC_PARAM_BOSS_Yoshi::CC_PARAM_BOSS_Yoshi() {
    m_Unk0C = 6.3f;
    m_Unk10 = 5.0f;
    m_Unk14 = 0.07f;
    m_Unk28 = 1.0f;
    m_Unk2C = 1.0f;
    m_Unk30 = 1.0f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 8.0f;
    m_Unk24 = 0.3f;
    m_Unk00 = 0;
    m_Unk04 = 0;
    m_Unk08 = 0;
    m_Unk34 = 0.0f;
    m_Unk38 = 22.0f;
    m_Unk3C = 2.8f;
    m_Unk40 = 40.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.0f;
    m_Unk64 = 2.0f;
    m_Unk68 = 4.0f;
    m_Unk6C = 11.0f;
    m_Unk70 = 20.0f;
    m_Unk74 = 3.5f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 30.0f;
    m_Unk1C4 = 25.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 80;
    m_Unk1D4 = 1;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 55;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 120;
    m_Unk1F0 = 1;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 0.9f;
    m_Unk94 = 3.0f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 12.0f;
    m_UnkBC = 20.0f;
    m_UnkC0 = 22.0f;
    m_UnkC4 = 0.3f;
    m_UnkC8 = 27.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 14.0f;
    m_UnkDC = 22.0f;
    m_UnkE0 = 24.0f;
    m_UnkE4 = 0.4f;
    m_UnkE8 = 23.0f;
    m_UnkEC = 14.0f;
    m_UnkF0 = 22.0f;
    m_UnkF4 = 24.0f;
    m_UnkF8 = 0.4f;
    m_UnkFC = 23.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 0.9f;
    m_Unk110 = 3.0f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 20.0f;
    m_Unk128 = 10.0f;
    m_Unk12C = 0.5f;
    m_Unk130 = 15.0f;
    m_Unk134 = 18.0f;
    m_Unk138 = 0.5f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 9.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 60.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Yoshi_NORMAL_STAR::CC_PARAM_BOSS_Yoshi_NORMAL_STAR() {}

CC_PARAM_BOSS_Yoshi_NORMAL::CC_PARAM_BOSS_Yoshi_NORMAL() {}

CC_PARAM_BOSS_Yoshi_HARD_STAR::CC_PARAM_BOSS_Yoshi_HARD_STAR() {}

CC_PARAM_BOSS_Yoshi_HARD::CC_PARAM_BOSS_Yoshi_HARD() {}

CC_PARAM_BOSS_Yoshi_EXPERT_STAR::CC_PARAM_BOSS_Yoshi_EXPERT_STAR() {}

CC_PARAM_BOSS_Yoshi_EXPERT::CC_PARAM_BOSS_Yoshi_EXPERT() {}

} // namespace cc_data_boss_Yoshi

// --------------------------------------------------------------------------
// Wario

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossWario::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Wario;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Wario_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Wario_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Wario_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Wario_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Wario_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Wario_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Wario_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Wario_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Wario {

CC_PARAM_BOSS_Wario_EASY_STAR::CC_PARAM_BOSS_Wario_EASY_STAR() {}

CC_PARAM_BOSS_Wario_EASY::CC_PARAM_BOSS_Wario_EASY() {}

CC_PARAM_BOSS_Wario::CC_PARAM_BOSS_Wario() {
    m_Unk0C = 5.2f;
    m_Unk10 = 5.0f;
    m_Unk14 = 0.05f;
    m_Unk28 = 0.95f;
    m_Unk2C = 1.05f;
    m_Unk30 = 0.95f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 8.0f;
    m_Unk24 = 0.3f;
    m_Unk00 = 2;
    m_Unk04 = 2;
    m_Unk08 = 2;
    m_Unk34 = 7.0f;
    m_Unk38 = 20.0f;
    m_Unk3C = 3.0f;
    m_Unk40 = 20.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 180.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.0f;
    m_Unk64 = 2.0f;
    m_Unk68 = 2.3f;
    m_Unk6C = 7.0f;
    m_Unk70 = 7.0f;
    m_Unk74 = 2.5f;
    m_Unk78 = 40.0f;
    m_Unk7C = 0.95f;
    m_Unk80 = 180.0f;
    m_Unk84 = 180.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 27.0f;
    m_Unk1C4 = 20.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 50;
    m_Unk1D4 = 1;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 40;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 120;
    m_Unk1F0 = 1;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 0.9f;
    m_Unk94 = 3.0f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 14.0f;
    m_UnkBC = 24.0f;
    m_UnkC0 = 26.0f;
    m_UnkC4 = 0.5f;
    m_UnkC8 = 22.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 18.0f;
    m_UnkDC = 28.0f;
    m_UnkE0 = 30.0f;
    m_UnkE4 = 0.6f;
    m_UnkE8 = 20.0f;
    m_UnkEC = 18.0f;
    m_UnkF0 = 28.0f;
    m_UnkF4 = 30.0f;
    m_UnkF8 = 0.6f;
    m_UnkFC = 20.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 1.0f;
    m_Unk110 = 3.0f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 8.0f;
    m_Unk128 = 18.0f;
    m_Unk12C = 2.0f;
    m_Unk130 = 14.0f;
    m_Unk134 = 8.0f;
    m_Unk138 = 1.0f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 9.0f;
    m_Unk1A4 = 5.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 60.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Wario_NORMAL_STAR::CC_PARAM_BOSS_Wario_NORMAL_STAR() {}

CC_PARAM_BOSS_Wario_NORMAL::CC_PARAM_BOSS_Wario_NORMAL() {}

CC_PARAM_BOSS_Wario_HARD_STAR::CC_PARAM_BOSS_Wario_HARD_STAR() {}

CC_PARAM_BOSS_Wario_HARD::CC_PARAM_BOSS_Wario_HARD() {}

CC_PARAM_BOSS_Wario_EXPERT_STAR::CC_PARAM_BOSS_Wario_EXPERT_STAR() {}

CC_PARAM_BOSS_Wario_EXPERT::CC_PARAM_BOSS_Wario_EXPERT() {}

} // namespace cc_data_boss_Wario

// --------------------------------------------------------------------------
// Waluigi

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossWaluigi::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Waluigi;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Waluigi_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Waluigi_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Waluigi_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Waluigi_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Waluigi_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Waluigi_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Waluigi_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Waluigi_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Waluigi {

CC_PARAM_BOSS_Waluigi_EASY_STAR::CC_PARAM_BOSS_Waluigi_EASY_STAR() {}

CC_PARAM_BOSS_Waluigi_EASY::CC_PARAM_BOSS_Waluigi_EASY() {}

CC_PARAM_BOSS_Waluigi::CC_PARAM_BOSS_Waluigi() {
    m_Unk0C = 6.8f;
    m_Unk10 = 5.0f;
    m_Unk14 = 0.07f;
    m_Unk28 = 0.8f;
    m_Unk2C = 0.8f;
    m_Unk30 = 0.8f;
    m_Unk18 = 30.0f;
    m_Unk1C = 20.0f;
    m_Unk20 = 8.0f;
    m_Unk24 = 0.5f;
    m_Unk00 = 1;
    m_Unk04 = 1;
    m_Unk08 = 1;
    m_Unk34 = 0.0f;
    m_Unk38 = 17.0f;
    m_Unk3C = 2.65f;
    m_Unk40 = 10.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.0f;
    m_Unk64 = 2.0f;
    m_Unk68 = 4.5f;
    m_Unk6C = 12.0f;
    m_Unk70 = 15.0f;
    m_Unk74 = 3.0f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 25.0f;
    m_Unk1C4 = 24.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 65;
    m_Unk1D4 = 1;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 40;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 120;
    m_Unk1F0 = 1;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 0.9f;
    m_Unk94 = 3.0f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 12.0f;
    m_UnkBC = 20.0f;
    m_UnkC0 = 22.0f;
    m_UnkC4 = 0.3f;
    m_UnkC8 = 27.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 14.0f;
    m_UnkDC = 22.0f;
    m_UnkE0 = 24.0f;
    m_UnkE4 = 0.4f;
    m_UnkE8 = 23.0f;
    m_UnkEC = 14.0f;
    m_UnkF0 = 22.0f;
    m_UnkF4 = 24.0f;
    m_UnkF8 = 0.4f;
    m_UnkFC = 23.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 1.0f;
    m_Unk110 = 3.0f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 10.0f;
    m_Unk128 = 20.0f;
    m_Unk12C = 2.0f;
    m_Unk130 = 14.0f;
    m_Unk134 = 10.0f;
    m_Unk138 = 0.7f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 9.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 50.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Waluigi_NORMAL_STAR::CC_PARAM_BOSS_Waluigi_NORMAL_STAR() {}

CC_PARAM_BOSS_Waluigi_NORMAL::CC_PARAM_BOSS_Waluigi_NORMAL() {}

CC_PARAM_BOSS_Waluigi_HARD_STAR::CC_PARAM_BOSS_Waluigi_HARD_STAR() {}

CC_PARAM_BOSS_Waluigi_HARD::CC_PARAM_BOSS_Waluigi_HARD() {}

CC_PARAM_BOSS_Waluigi_EXPERT_STAR::CC_PARAM_BOSS_Waluigi_EXPERT_STAR() {}

CC_PARAM_BOSS_Waluigi_EXPERT::CC_PARAM_BOSS_Waluigi_EXPERT() {}

} // namespace cc_data_boss_Waluigi

// --------------------------------------------------------------------------
// Donkey

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossDonkey::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Donkey;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Donkey_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Donkey_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Donkey_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Donkey_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Donkey_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Donkey_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Donkey_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Donkey_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Donkey {

CC_PARAM_BOSS_Donkey_EASY_STAR::CC_PARAM_BOSS_Donkey_EASY_STAR() {}

CC_PARAM_BOSS_Donkey_EASY::CC_PARAM_BOSS_Donkey_EASY() {}

CC_PARAM_BOSS_Donkey::CC_PARAM_BOSS_Donkey() {
    m_Unk0C = 5.2f;
    m_Unk10 = 8.0f;
    m_Unk14 = 0.07f;
    m_Unk28 = 0.87f;
    m_Unk2C = 1.0f;
    m_Unk30 = 0.87f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 20.0f;
    m_Unk24 = 1.1f;
    m_Unk00 = 2;
    m_Unk04 = 2;
    m_Unk08 = 2;
    m_Unk34 = 8.0f;
    m_Unk38 = 20.0f;
    m_Unk3C = 2.65f;
    m_Unk40 = 20.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.0f;
    m_Unk64 = 2.0f;
    m_Unk68 = 2.8f;
    m_Unk6C = 11.0f;
    m_Unk70 = 5.0f;
    m_Unk74 = 1.5f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 26.0f;
    m_Unk1C4 = 21.0f;
    m_Unk1CC = 3.8f;
    m_Unk1D0 = 50;
    m_Unk1D4 = 1;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 40;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 120;
    m_Unk1F0 = 1;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 3.0f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 14.0f;
    m_UnkBC = 24.0f;
    m_UnkC0 = 26.0f;
    m_UnkC4 = 0.5f;
    m_UnkC8 = 22.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 18.0f;
    m_UnkDC = 28.0f;
    m_UnkE0 = 30.0f;
    m_UnkE4 = 0.6f;
    m_UnkE8 = 20.0f;
    m_UnkEC = 18.0f;
    m_UnkF0 = 28.0f;
    m_UnkF4 = 30.0f;
    m_UnkF8 = 0.6f;
    m_UnkFC = 20.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 1.1f;
    m_Unk110 = 3.0f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 10.0f;
    m_Unk128 = 20.0f;
    m_Unk12C = 3.0f;
    m_Unk130 = 15.0f;
    m_Unk134 = 6.0f;
    m_Unk138 = 1.5f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 9.0f;
    m_Unk1A4 = 5.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 60.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Donkey_NORMAL_STAR::CC_PARAM_BOSS_Donkey_NORMAL_STAR() {}

CC_PARAM_BOSS_Donkey_NORMAL::CC_PARAM_BOSS_Donkey_NORMAL() {}

CC_PARAM_BOSS_Donkey_HARD_STAR::CC_PARAM_BOSS_Donkey_HARD_STAR() {}

CC_PARAM_BOSS_Donkey_HARD::CC_PARAM_BOSS_Donkey_HARD() {}

CC_PARAM_BOSS_Donkey_EXPERT_STAR::CC_PARAM_BOSS_Donkey_EXPERT_STAR() {}

CC_PARAM_BOSS_Donkey_EXPERT::CC_PARAM_BOSS_Donkey_EXPERT() {}

} // namespace cc_data_boss_Donkey

// --------------------------------------------------------------------------
// Diddy

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossDiddy::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Diddy;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Diddy_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Diddy_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Diddy_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Diddy_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Diddy_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Diddy_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Diddy_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Diddy_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Diddy {

CC_PARAM_BOSS_Diddy_EASY_STAR::CC_PARAM_BOSS_Diddy_EASY_STAR() {}

CC_PARAM_BOSS_Diddy_EASY::CC_PARAM_BOSS_Diddy_EASY() {}

CC_PARAM_BOSS_Diddy::CC_PARAM_BOSS_Diddy() {
    m_Unk0C = 7.5f;
    m_Unk10 = 5.0f;
    m_Unk14 = 0.07f;
    m_Unk28 = 1.13f;
    m_Unk2C = 1.13f;
    m_Unk30 = 1.13f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 11.0f;
    m_Unk24 = 0.4f;
    m_Unk00 = 1;
    m_Unk04 = 3;
    m_Unk08 = 3;
    m_Unk34 = 7.0f;
    m_Unk38 = 16.0f;
    m_Unk3C = 2.7f;
    m_Unk40 = 9.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.1f;
    m_Unk64 = 1.8f;
    m_Unk68 = 4.5f;
    m_Unk6C = 14.0f;
    m_Unk70 = 8.0f;
    m_Unk74 = 3.5f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 25.0f;
    m_Unk1C4 = 20.0f;
    m_Unk1CC = 4.2f;
    m_Unk1D0 = 75;
    m_Unk1D4 = 1;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 48;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 120;
    m_Unk1F0 = 1;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 0.9f;
    m_Unk94 = 3.0f;
    m_Unk98 = 2.2f;
    m_Unk9C = 2.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 13.0f;
    m_UnkBC = 20.0f;
    m_UnkC0 = 22.0f;
    m_UnkC4 = 0.3f;
    m_UnkC8 = 29.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 13.0f;
    m_UnkDC = 21.0f;
    m_UnkE0 = 23.0f;
    m_UnkE4 = 0.4f;
    m_UnkE8 = 25.0f;
    m_UnkEC = 13.0f;
    m_UnkF0 = 21.0f;
    m_UnkF4 = 23.0f;
    m_UnkF8 = 0.4f;
    m_UnkFC = 25.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 0.9f;
    m_Unk110 = 3.0f;
    m_Unk114 = 2.2f;
    m_Unk118 = 2.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 10.0f;
    m_Unk128 = 8.0f;
    m_Unk12C = 0.5f;
    m_Unk130 = 20.0f;
    m_Unk134 = 35.0f;
    m_Unk138 = 1.0f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 15.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 45.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Diddy_NORMAL_STAR::CC_PARAM_BOSS_Diddy_NORMAL_STAR() {}

CC_PARAM_BOSS_Diddy_NORMAL::CC_PARAM_BOSS_Diddy_NORMAL() {}

CC_PARAM_BOSS_Diddy_HARD_STAR::CC_PARAM_BOSS_Diddy_HARD_STAR() {}

CC_PARAM_BOSS_Diddy_HARD::CC_PARAM_BOSS_Diddy_HARD() {}

CC_PARAM_BOSS_Diddy_EXPERT_STAR::CC_PARAM_BOSS_Diddy_EXPERT_STAR() {}

CC_PARAM_BOSS_Diddy_EXPERT::CC_PARAM_BOSS_Diddy_EXPERT() {}

} // namespace cc_data_boss_Diddy

// --------------------------------------------------------------------------
// Kinopio

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossKinopio::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Kinopio;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Kinopio_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Kinopio_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Kinopio_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Kinopio_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Kinopio_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Kinopio_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Kinopio_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Kinopio_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Kinopio {

CC_PARAM_BOSS_Kinopio_EASY_STAR::CC_PARAM_BOSS_Kinopio_EASY_STAR() {}

CC_PARAM_BOSS_Kinopio_EASY::CC_PARAM_BOSS_Kinopio_EASY() {}

CC_PARAM_BOSS_Kinopio::CC_PARAM_BOSS_Kinopio() {
    m_Unk0C = 7.4f;
    m_Unk10 = 5.0f;
    m_Unk14 = 0.07f;
    m_Unk28 = 1.25f;
    m_Unk2C = 1.15f;
    m_Unk30 = 1.15f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 8.0f;
    m_Unk24 = 0.3f;
    m_Unk00 = 3;
    m_Unk04 = 3;
    m_Unk08 = 3;
    m_Unk34 = 0.0f;
    m_Unk38 = 15.0f;
    m_Unk3C = 2.3f;
    m_Unk40 = 28.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.1f;
    m_Unk64 = 1.8f;
    m_Unk68 = 2.0f;
    m_Unk6C = 11.0f;
    m_Unk70 = 20.0f;
    m_Unk74 = 3.5f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 34.0f;
    m_Unk1C4 = 20.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 65;
    m_Unk1D4 = 1;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 50;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 120;
    m_Unk1F0 = 1;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 0.9f;
    m_Unk94 = 3.0f;
    m_Unk98 = 2.2f;
    m_Unk9C = 2.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 13.0f;
    m_UnkBC = 20.0f;
    m_UnkC0 = 22.0f;
    m_UnkC4 = 0.3f;
    m_UnkC8 = 29.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 13.0f;
    m_UnkDC = 21.0f;
    m_UnkE0 = 23.0f;
    m_UnkE4 = 0.4f;
    m_UnkE8 = 25.0f;
    m_UnkEC = 13.0f;
    m_UnkF0 = 21.0f;
    m_UnkF4 = 23.0f;
    m_UnkF8 = 0.4f;
    m_UnkFC = 25.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 0.9f;
    m_Unk110 = 3.0f;
    m_Unk114 = 2.2f;
    m_Unk118 = 2.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 20.0f;
    m_Unk128 = 8.0f;
    m_Unk12C = 0.5f;
    m_Unk130 = 11.0f;
    m_Unk134 = 23.0f;
    m_Unk138 = 1.0f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 15.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 60.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Kinopio_NORMAL_STAR::CC_PARAM_BOSS_Kinopio_NORMAL_STAR() {}

CC_PARAM_BOSS_Kinopio_NORMAL::CC_PARAM_BOSS_Kinopio_NORMAL() {}

CC_PARAM_BOSS_Kinopio_HARD_STAR::CC_PARAM_BOSS_Kinopio_HARD_STAR() {}

CC_PARAM_BOSS_Kinopio_HARD::CC_PARAM_BOSS_Kinopio_HARD() {}

CC_PARAM_BOSS_Kinopio_EXPERT_STAR::CC_PARAM_BOSS_Kinopio_EXPERT_STAR() {}

CC_PARAM_BOSS_Kinopio_EXPERT::CC_PARAM_BOSS_Kinopio_EXPERT() {}

} // namespace cc_data_boss_Kinopio

// --------------------------------------------------------------------------
// Koopa

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossKoopa::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Koopa;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Koopa_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Koopa_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Koopa_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Koopa_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Koopa_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Koopa_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Koopa_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Koopa_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Koopa {

CC_PARAM_BOSS_Koopa_EASY_STAR::CC_PARAM_BOSS_Koopa_EASY_STAR() {}

CC_PARAM_BOSS_Koopa_EASY::CC_PARAM_BOSS_Koopa_EASY() {}

CC_PARAM_BOSS_Koopa::CC_PARAM_BOSS_Koopa() {
    m_Unk0C = 4.8f;
    m_Unk10 = 1.0f;
    m_Unk14 = 0.1f;
    m_Unk28 = 0.9f;
    m_Unk2C = 1.0f;
    m_Unk30 = 1.0f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 20.0f;
    m_Unk24 = 0.3f;
    m_Unk00 = 2;
    m_Unk04 = 2;
    m_Unk08 = 2;
    m_Unk34 = 8.0f;
    m_Unk38 = 13.0f;
    m_Unk3C = 2.0f;
    m_Unk40 = 17.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.5f;
    m_Unk64 = 2.5f;
    m_Unk68 = 2.5f;
    m_Unk6C = 12.3f;
    m_Unk70 = 10.0f;
    m_Unk74 = 2.8f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 30.0f;
    m_Unk1C4 = 17.0f;
    m_Unk1CC = 3.5f;
    m_Unk1D0 = 50;
    m_Unk1D4 = 0;
    m_Unk1D8 = 3.5f;
    m_Unk1DC = 40;
    m_Unk1E0 = 0.6f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 90;
    m_Unk1F0 = 0;
    m_Unk1F4 = 3.5f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 3.0f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 16.0f;
    m_UnkBC = 26.0f;
    m_UnkC0 = 28.0f;
    m_UnkC4 = 0.5f;
    m_UnkC8 = 22.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 20.0f;
    m_UnkDC = 30.0f;
    m_UnkE0 = 32.0f;
    m_UnkE4 = 0.6f;
    m_UnkE8 = 20.0f;
    m_UnkEC = 20.0f;
    m_UnkF0 = 30.0f;
    m_UnkF4 = 32.0f;
    m_UnkF8 = 0.6f;
    m_UnkFC = 20.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 1.1f;
    m_Unk110 = 3.0f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 10.0f;
    m_Unk128 = 8.0f;
    m_Unk12C = 0.5f;
    m_Unk130 = 11.0f;
    m_Unk134 = 6.0f;
    m_Unk138 = 1.0f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 9.0f;
    m_Unk1A4 = 5.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 60.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Koopa_NORMAL_STAR::CC_PARAM_BOSS_Koopa_NORMAL_STAR() {}

CC_PARAM_BOSS_Koopa_NORMAL::CC_PARAM_BOSS_Koopa_NORMAL() {}

CC_PARAM_BOSS_Koopa_HARD_STAR::CC_PARAM_BOSS_Koopa_HARD_STAR() {}

CC_PARAM_BOSS_Koopa_HARD::CC_PARAM_BOSS_Koopa_HARD() {}

CC_PARAM_BOSS_Koopa_EXPERT_STAR::CC_PARAM_BOSS_Koopa_EXPERT_STAR() {}

CC_PARAM_BOSS_Koopa_EXPERT::CC_PARAM_BOSS_Koopa_EXPERT() {}

} // namespace cc_data_boss_Koopa

// --------------------------------------------------------------------------
// KoopaJr

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossKoopaJr::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_KoopaJr;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_KoopaJr_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_KoopaJr_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_KoopaJr_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_KoopaJr_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_KoopaJr_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_KoopaJr_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_KoopaJr_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_KoopaJr_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_KoopaJr {

CC_PARAM_BOSS_KoopaJr_EASY_STAR::CC_PARAM_BOSS_KoopaJr_EASY_STAR() {}

CC_PARAM_BOSS_KoopaJr_EASY::CC_PARAM_BOSS_KoopaJr_EASY() {}

CC_PARAM_BOSS_KoopaJr::CC_PARAM_BOSS_KoopaJr() {
    m_Unk0C = 7.6f;
    m_Unk10 = 5.0f;
    m_Unk14 = 0.07f;
    m_Unk28 = 1.0f;
    m_Unk2C = 1.0f;
    m_Unk30 = 1.0f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 10.0f;
    m_Unk24 = 0.3f;
    m_Unk00 = 3;
    m_Unk04 = 3;
    m_Unk08 = 3;
    m_Unk34 = 0.0f;
    m_Unk38 = 15.0f;
    m_Unk3C = 1.8f;
    m_Unk40 = 18.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.1f;
    m_Unk64 = 1.8f;
    m_Unk68 = 4.0f;
    m_Unk6C = 12.0f;
    m_Unk70 = 20.0f;
    m_Unk74 = 3.5f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 28.0f;
    m_Unk1C4 = 27.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 58;
    m_Unk1D4 = 0;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 70;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 120;
    m_Unk1F0 = 0;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 0.9f;
    m_Unk94 = 3.0f;
    m_Unk98 = 2.2f;
    m_Unk9C = 2.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 13.0f;
    m_UnkBC = 20.0f;
    m_UnkC0 = 22.0f;
    m_UnkC4 = 0.3f;
    m_UnkC8 = 29.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 13.0f;
    m_UnkDC = 21.0f;
    m_UnkE0 = 23.0f;
    m_UnkE4 = 0.4f;
    m_UnkE8 = 25.0f;
    m_UnkEC = 13.0f;
    m_UnkF0 = 21.0f;
    m_UnkF4 = 23.0f;
    m_UnkF8 = 0.4f;
    m_UnkFC = 25.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 1.0f;
    m_Unk110 = 3.0f;
    m_Unk114 = 2.2f;
    m_Unk118 = 2.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 20.0f;
    m_Unk128 = 8.0f;
    m_Unk12C = 0.5f;
    m_Unk130 = 11.0f;
    m_Unk134 = 23.0f;
    m_Unk138 = 1.0f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 15.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 60.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_KoopaJr_NORMAL_STAR::CC_PARAM_BOSS_KoopaJr_NORMAL_STAR() {}

CC_PARAM_BOSS_KoopaJr_NORMAL::CC_PARAM_BOSS_KoopaJr_NORMAL() {}

CC_PARAM_BOSS_KoopaJr_HARD_STAR::CC_PARAM_BOSS_KoopaJr_HARD_STAR() {}

CC_PARAM_BOSS_KoopaJr_HARD::CC_PARAM_BOSS_KoopaJr_HARD() {}

CC_PARAM_BOSS_KoopaJr_EXPERT_STAR::CC_PARAM_BOSS_KoopaJr_EXPERT_STAR() {}

CC_PARAM_BOSS_KoopaJr_EXPERT::CC_PARAM_BOSS_KoopaJr_EXPERT() {}

} // namespace cc_data_boss_KoopaJr

// --------------------------------------------------------------------------
// Moogle

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossMoogle::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Moogle;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Moogle_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Moogle_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Moogle_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Moogle_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Moogle_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Moogle_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Moogle_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Moogle_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Moogle {

CC_PARAM_BOSS_Moogle_EASY_STAR::CC_PARAM_BOSS_Moogle_EASY_STAR() {}

CC_PARAM_BOSS_Moogle_EASY::CC_PARAM_BOSS_Moogle_EASY() {}

CC_PARAM_BOSS_Moogle::CC_PARAM_BOSS_Moogle() {
    m_Unk0C = 6.3f;
    m_Unk10 = 5.0f;
    m_Unk14 = 0.07f;
    m_Unk28 = 1.0f;
    m_Unk2C = 1.0f;
    m_Unk30 = 1.0f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 8.0f;
    m_Unk24 = 0.3f;
    m_Unk00 = 0;
    m_Unk04 = 0;
    m_Unk08 = 0;
    m_Unk34 = 0.0f;
    m_Unk38 = 25.0f;
    m_Unk3C = 3.45f;
    m_Unk40 = 60.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.0f;
    m_Unk64 = 2.0f;
    m_Unk68 = 4.0f;
    m_Unk6C = 11.0f;
    m_Unk70 = 20.0f;
    m_Unk74 = 3.5f;
    m_Unk78 = 40.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 26.0f;
    m_Unk1C4 = 20.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 90;
    m_Unk1D4 = 1;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 80;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 150;
    m_Unk1F0 = 1;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 0.9f;
    m_Unk94 = 3.0f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 12.0f;
    m_UnkBC = 20.0f;
    m_UnkC0 = 22.0f;
    m_UnkC4 = 0.3f;
    m_UnkC8 = 27.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 14.0f;
    m_UnkDC = 22.0f;
    m_UnkE0 = 24.0f;
    m_UnkE4 = 0.4f;
    m_UnkE8 = 23.0f;
    m_UnkEC = 14.0f;
    m_UnkF0 = 22.0f;
    m_UnkF4 = 24.0f;
    m_UnkF8 = 0.4f;
    m_UnkFC = 23.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 0.9f;
    m_Unk110 = 3.0f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 10.0f;
    m_Unk128 = 8.0f;
    m_Unk12C = 0.5f;
    m_Unk130 = 15.0f;
    m_Unk134 = 25.0f;
    m_Unk138 = 1.0f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 9.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 50.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Moogle_NORMAL_STAR::CC_PARAM_BOSS_Moogle_NORMAL_STAR() {}

CC_PARAM_BOSS_Moogle_NORMAL::CC_PARAM_BOSS_Moogle_NORMAL() {}

CC_PARAM_BOSS_Moogle_HARD_STAR::CC_PARAM_BOSS_Moogle_HARD_STAR() {}

CC_PARAM_BOSS_Moogle_HARD::CC_PARAM_BOSS_Moogle_HARD() {}

CC_PARAM_BOSS_Moogle_EXPERT_STAR::CC_PARAM_BOSS_Moogle_EXPERT_STAR() {}

CC_PARAM_BOSS_Moogle_EXPERT::CC_PARAM_BOSS_Moogle_EXPERT() {}

} // namespace cc_data_boss_Moogle

// --------------------------------------------------------------------------
// Saboten

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossSaboten::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Saboten;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Saboten_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Saboten_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Saboten_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Saboten_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Saboten_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Saboten_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Saboten_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Saboten_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Saboten {

CC_PARAM_BOSS_Saboten_EASY_STAR::CC_PARAM_BOSS_Saboten_EASY_STAR() {}

CC_PARAM_BOSS_Saboten_EASY::CC_PARAM_BOSS_Saboten_EASY() {}

CC_PARAM_BOSS_Saboten::CC_PARAM_BOSS_Saboten() {
    m_Unk0C = 11.0f;
    m_Unk10 = 5.0f;
    m_Unk14 = 0.07f;
    m_Unk28 = 1.95f;
    m_Unk2C = 1.0f;
    m_Unk30 = 1.7f;
    m_Unk18 = 25.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 35.0f;
    m_Unk24 = 3.2f;
    m_Unk00 = 3;
    m_Unk04 = 3;
    m_Unk08 = 3;
    m_Unk34 = 0.0f;
    m_Unk38 = 9.0f;
    m_Unk3C = 2.8f;
    m_Unk40 = 17.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.0f;
    m_Unk64 = 2.0f;
    m_Unk68 = 4.0f;
    m_Unk6C = 11.0f;
    m_Unk70 = 20.0f;
    m_Unk74 = 3.5f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 1;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 18.0f;
    m_Unk1C4 = 35.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 80;
    m_Unk1D4 = 1;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 40;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 120;
    m_Unk1F0 = 1;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 0.9f;
    m_Unk94 = 3.0f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 12.0f;
    m_UnkBC = 20.0f;
    m_UnkC0 = 22.0f;
    m_UnkC4 = 0.3f;
    m_UnkC8 = 27.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 14.0f;
    m_UnkDC = 22.0f;
    m_UnkE0 = 24.0f;
    m_UnkE4 = 0.4f;
    m_UnkE8 = 23.0f;
    m_UnkEC = 14.0f;
    m_UnkF0 = 22.0f;
    m_UnkF4 = 24.0f;
    m_UnkF8 = 0.4f;
    m_UnkFC = 23.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 1.0f;
    m_Unk110 = 3.0f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 10.0f;
    m_Unk128 = 8.0f;
    m_Unk12C = 0.5f;
    m_Unk130 = 10.0f;
    m_Unk134 = 20.0f;
    m_Unk138 = 0.3f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 9.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 15.0f;
    m_Unk220 = 23.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 60.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Saboten_NORMAL_STAR::CC_PARAM_BOSS_Saboten_NORMAL_STAR() {}

CC_PARAM_BOSS_Saboten_NORMAL::CC_PARAM_BOSS_Saboten_NORMAL() {}

CC_PARAM_BOSS_Saboten_HARD_STAR::CC_PARAM_BOSS_Saboten_HARD_STAR() {}

CC_PARAM_BOSS_Saboten_HARD::CC_PARAM_BOSS_Saboten_HARD() {}

CC_PARAM_BOSS_Saboten_EXPERT_STAR::CC_PARAM_BOSS_Saboten_EXPERT_STAR() {}

CC_PARAM_BOSS_Saboten_EXPERT::CC_PARAM_BOSS_Saboten_EXPERT() {}

} // namespace cc_data_boss_Saboten

// --------------------------------------------------------------------------
// Ninja

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossNinja::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Ninja;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Ninja_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Ninja_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Ninja_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Ninja_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Ninja_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Ninja_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Ninja_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Ninja_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Ninja {

CC_PARAM_BOSS_Ninja_EASY_STAR::CC_PARAM_BOSS_Ninja_EASY_STAR() {}

CC_PARAM_BOSS_Ninja_EASY::CC_PARAM_BOSS_Ninja_EASY() {}

CC_PARAM_BOSS_Ninja::CC_PARAM_BOSS_Ninja() {
    m_Unk0C = 8.0f;
    m_Unk10 = 5.0f;
    m_Unk14 = 0.07f;
    m_Unk28 = 1.25f;
    m_Unk2C = 1.1f;
    m_Unk30 = 1.1f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 8.0f;
    m_Unk24 = 0.3f;
    m_Unk00 = 0;
    m_Unk04 = 0;
    m_Unk08 = 0;
    m_Unk34 = 0.0f;
    m_Unk38 = 13.0f;
    m_Unk3C = 3.0f;
    m_Unk40 = 28.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.0f;
    m_Unk64 = 2.0f;
    m_Unk68 = 4.0f;
    m_Unk6C = 11.0f;
    m_Unk70 = 20.0f;
    m_Unk74 = 3.5f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 26.0f;
    m_Unk1C4 = 20.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 53;
    m_Unk1D4 = 1;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 40;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 110;
    m_Unk1F0 = 1;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 80;
    m_Unk1FC = 30;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.02f;
    m_Unk208 = -0.1f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 0.9f;
    m_Unk94 = 3.0f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 14.0f;
    m_UnkBC = 22.0f;
    m_UnkC0 = 24.0f;
    m_UnkC4 = 0.3f;
    m_UnkC8 = 25.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 15.0f;
    m_UnkDC = 23.0f;
    m_UnkE0 = 25.0f;
    m_UnkE4 = 0.4f;
    m_UnkE8 = 21.0f;
    m_UnkEC = 15.0f;
    m_UnkF0 = 23.0f;
    m_UnkF4 = 25.0f;
    m_UnkF8 = 0.4f;
    m_UnkFC = 21.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 0.9f;
    m_Unk110 = 3.0f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 35.0f;
    m_Unk128 = 7.0f;
    m_Unk12C = 0.5f;
    m_Unk130 = 20.0f;
    m_Unk134 = 30.0f;
    m_Unk138 = 1.0f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 9.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 45.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Ninja_NORMAL_STAR::CC_PARAM_BOSS_Ninja_NORMAL_STAR() {}

CC_PARAM_BOSS_Ninja_NORMAL::CC_PARAM_BOSS_Ninja_NORMAL() {}

CC_PARAM_BOSS_Ninja_HARD_STAR::CC_PARAM_BOSS_Ninja_HARD_STAR() {}

CC_PARAM_BOSS_Ninja_HARD::CC_PARAM_BOSS_Ninja_HARD() {}

CC_PARAM_BOSS_Ninja_EXPERT_STAR::CC_PARAM_BOSS_Ninja_EXPERT_STAR() {}

CC_PARAM_BOSS_Ninja_EXPERT::CC_PARAM_BOSS_Ninja_EXPERT() {}

} // namespace cc_data_boss_Ninja

// --------------------------------------------------------------------------
// Siroma

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossSiroma::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Siroma;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Siroma_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Siroma_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Siroma_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Siroma_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Siroma_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Siroma_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Siroma_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Siroma_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Siroma {

CC_PARAM_BOSS_Siroma_EASY_STAR::CC_PARAM_BOSS_Siroma_EASY_STAR() {}

CC_PARAM_BOSS_Siroma_EASY::CC_PARAM_BOSS_Siroma_EASY() {}

CC_PARAM_BOSS_Siroma::CC_PARAM_BOSS_Siroma() {
    m_Unk0C = 6.3f;
    m_Unk10 = 5.0f;
    m_Unk14 = 0.07f;
    m_Unk28 = 1.15f;
    m_Unk2C = 1.15f;
    m_Unk30 = 1.15f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 8.0f;
    m_Unk24 = 0.3f;
    m_Unk00 = 1;
    m_Unk04 = 1;
    m_Unk08 = 1;
    m_Unk34 = 0.0f;
    m_Unk38 = 22.0f;
    m_Unk3C = 2.65f;
    m_Unk40 = 15.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.0f;
    m_Unk64 = 2.0f;
    m_Unk68 = 4.5f;
    m_Unk6C = 12.0f;
    m_Unk70 = 15.0f;
    m_Unk74 = 3.0f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 30.0f;
    m_Unk1C4 = 20.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 65;
    m_Unk1D4 = 0;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 40;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 120;
    m_Unk1F0 = 0;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 0.9f;
    m_Unk94 = 5.0f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 13.0f;
    m_UnkBC = 20.0f;
    m_UnkC0 = 22.0f;
    m_UnkC4 = 0.3f;
    m_UnkC8 = 29.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 13.0f;
    m_UnkDC = 21.0f;
    m_UnkE0 = 23.0f;
    m_UnkE4 = 0.4f;
    m_UnkE8 = 25.0f;
    m_UnkEC = 13.0f;
    m_UnkF0 = 21.0f;
    m_UnkF4 = 23.0f;
    m_UnkF8 = 0.4f;
    m_UnkFC = 25.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 0.9f;
    m_Unk110 = 5.0f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 10.0f;
    m_Unk128 = 8.0f;
    m_Unk12C = 0.5f;
    m_Unk130 = 15.0f;
    m_Unk134 = 20.0f;
    m_Unk138 = 0.8f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 12.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 60.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Siroma_NORMAL_STAR::CC_PARAM_BOSS_Siroma_NORMAL_STAR() {}

CC_PARAM_BOSS_Siroma_NORMAL::CC_PARAM_BOSS_Siroma_NORMAL() {}

CC_PARAM_BOSS_Siroma_HARD_STAR::CC_PARAM_BOSS_Siroma_HARD_STAR() {}

CC_PARAM_BOSS_Siroma_HARD::CC_PARAM_BOSS_Siroma_HARD() {}

CC_PARAM_BOSS_Siroma_EXPERT_STAR::CC_PARAM_BOSS_Siroma_EXPERT_STAR() {}

CC_PARAM_BOSS_Siroma_EXPERT::CC_PARAM_BOSS_Siroma_EXPERT() {}

} // namespace cc_data_boss_Siroma

// --------------------------------------------------------------------------
// Slime

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossSlime::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Slime;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Slime_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Slime_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Slime_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Slime_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Slime_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Slime_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Slime_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Slime_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Slime {

CC_PARAM_BOSS_Slime_EASY_STAR::CC_PARAM_BOSS_Slime_EASY_STAR() {}

CC_PARAM_BOSS_Slime_EASY::CC_PARAM_BOSS_Slime_EASY() {}

CC_PARAM_BOSS_Slime::CC_PARAM_BOSS_Slime() {
    m_Unk0C = 5.0f;
    m_Unk10 = 5.0f;
    m_Unk14 = 0.07f;
    m_Unk28 = 1.0f;
    m_Unk2C = 1.0f;
    m_Unk30 = 1.0f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 8.0f;
    m_Unk24 = 0.3f;
    m_Unk00 = 2;
    m_Unk04 = 2;
    m_Unk08 = 2;
    m_Unk34 = 8.0f;
    m_Unk38 = 20.0f;
    m_Unk3C = 2.9f;
    m_Unk40 = 20.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.0f;
    m_Unk64 = 2.0f;
    m_Unk68 = 2.0f;
    m_Unk6C = 11.0f;
    m_Unk70 = 20.0f;
    m_Unk74 = 3.5f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 28.0f;
    m_Unk1C4 = 20.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 55;
    m_Unk1D4 = 0;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 40;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 120;
    m_Unk1F0 = 0;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 3.0f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 12.0f;
    m_UnkBC = 20.0f;
    m_UnkC0 = 22.0f;
    m_UnkC4 = 0.3f;
    m_UnkC8 = 27.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 14.0f;
    m_UnkDC = 22.0f;
    m_UnkE0 = 24.0f;
    m_UnkE4 = 0.4f;
    m_UnkE8 = 23.0f;
    m_UnkEC = 14.0f;
    m_UnkF0 = 22.0f;
    m_UnkF4 = 24.0f;
    m_UnkF8 = 0.4f;
    m_UnkFC = 23.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 1.1f;
    m_Unk110 = 3.0f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 70.0f;
    m_Unk128 = 8.0f;
    m_Unk12C = 0.5f;
    m_Unk130 = 10.0f;
    m_Unk134 = 70.0f;
    m_Unk138 = 0.1f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 9.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 60.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Slime_NORMAL_STAR::CC_PARAM_BOSS_Slime_NORMAL_STAR() {}

CC_PARAM_BOSS_Slime_NORMAL::CC_PARAM_BOSS_Slime_NORMAL() {}

CC_PARAM_BOSS_Slime_HARD_STAR::CC_PARAM_BOSS_Slime_HARD_STAR() {}

CC_PARAM_BOSS_Slime_HARD::CC_PARAM_BOSS_Slime_HARD() {}

CC_PARAM_BOSS_Slime_EXPERT_STAR::CC_PARAM_BOSS_Slime_EXPERT_STAR() {}

CC_PARAM_BOSS_Slime_EXPERT::CC_PARAM_BOSS_Slime_EXPERT() {}

} // namespace cc_data_boss_Slime

// --------------------------------------------------------------------------
// Kuroma

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossKuroma::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Kuroma;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Kuroma_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Kuroma_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Kuroma_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Kuroma_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Kuroma_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Kuroma_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Kuroma_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Kuroma_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Kuroma {

CC_PARAM_BOSS_Kuroma_EASY_STAR::CC_PARAM_BOSS_Kuroma_EASY_STAR() {}

CC_PARAM_BOSS_Kuroma_EASY::CC_PARAM_BOSS_Kuroma_EASY() {}

CC_PARAM_BOSS_Kuroma::CC_PARAM_BOSS_Kuroma() {
    m_Unk0C = 6.3f;
    m_Unk10 = 5.0f;
    m_Unk14 = 0.07f;
    m_Unk28 = 1.0f;
    m_Unk2C = 1.0f;
    m_Unk30 = 1.0f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 8.0f;
    m_Unk24 = 0.3f;
    m_Unk00 = 1;
    m_Unk04 = 1;
    m_Unk08 = 1;
    m_Unk34 = 0.0f;
    m_Unk38 = 17.0f;
    m_Unk3C = 2.65f;
    m_Unk40 = 10.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.0f;
    m_Unk64 = 2.0f;
    m_Unk68 = 4.5f;
    m_Unk6C = 12.0f;
    m_Unk70 = 15.0f;
    m_Unk74 = 3.0f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 30.0f;
    m_Unk1C4 = 17.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 110;
    m_Unk1D4 = 1;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 40;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 120;
    m_Unk1F0 = 1;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 0.9f;
    m_Unk94 = 3.0f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 12.0f;
    m_UnkBC = 20.0f;
    m_UnkC0 = 22.0f;
    m_UnkC4 = 0.3f;
    m_UnkC8 = 27.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 14.0f;
    m_UnkDC = 22.0f;
    m_UnkE0 = 24.0f;
    m_UnkE4 = 0.4f;
    m_UnkE8 = 23.0f;
    m_UnkEC = 14.0f;
    m_UnkF0 = 22.0f;
    m_UnkF4 = 24.0f;
    m_UnkF8 = 0.4f;
    m_UnkFC = 23.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 1.0f;
    m_Unk110 = 3.0f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 0.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 15.0f;
    m_Unk128 = 10.0f;
    m_Unk12C = 0.5f;
    m_Unk130 = 30.0f;
    m_Unk134 = 30.0f;
    m_Unk138 = 0.3f;
    m_Unk13C = 0.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 9.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 10.0f;
    m_Unk220 = 18.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 50.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Kuroma_NORMAL_STAR::CC_PARAM_BOSS_Kuroma_NORMAL_STAR() {}

CC_PARAM_BOSS_Kuroma_NORMAL::CC_PARAM_BOSS_Kuroma_NORMAL() {}

CC_PARAM_BOSS_Kuroma_HARD_STAR::CC_PARAM_BOSS_Kuroma_HARD_STAR() {}

CC_PARAM_BOSS_Kuroma_HARD::CC_PARAM_BOSS_Kuroma_HARD() {}

CC_PARAM_BOSS_Kuroma_EXPERT_STAR::CC_PARAM_BOSS_Kuroma_EXPERT_STAR() {}

CC_PARAM_BOSS_Kuroma_EXPERT::CC_PARAM_BOSS_Kuroma_EXPERT() {}

} // namespace cc_data_boss_Kuroma

// --------------------------------------------------------------------------
// Mii00

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossMii00::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Mii00;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Mii00_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Mii00_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Mii00_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Mii00_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Mii00_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Mii00_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Mii00_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Mii00_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Mii00 {

CC_PARAM_BOSS_Mii00_EASY_STAR::CC_PARAM_BOSS_Mii00_EASY_STAR() {}

CC_PARAM_BOSS_Mii00_EASY::CC_PARAM_BOSS_Mii00_EASY() {}

CC_PARAM_BOSS_Mii00::CC_PARAM_BOSS_Mii00() {
    m_Unk0C = 6.3f;
    m_Unk10 = 5.0f;
    m_Unk14 = 0.07f;
    m_Unk28 = 1.0f;
    m_Unk2C = 1.0f;
    m_Unk30 = 1.0f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 8.0f;
    m_Unk24 = 0.3f;
    m_Unk00 = 0;
    m_Unk04 = 0;
    m_Unk08 = 0;
    m_Unk34 = 0.0f;
    m_Unk38 = 17.0f;
    m_Unk3C = 2.65f;
    m_Unk40 = 12.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.0f;
    m_Unk64 = 2.0f;
    m_Unk68 = 4.0f;
    m_Unk6C = 11.0f;
    m_Unk70 = 20.0f;
    m_Unk74 = 3.5f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 26.0f;
    m_Unk1C4 = 24.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 55;
    m_Unk1D4 = 1;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 40;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 120;
    m_Unk1F0 = 1;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 0.9f;
    m_Unk94 = 3.0f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 12.0f;
    m_UnkBC = 20.0f;
    m_UnkC0 = 22.0f;
    m_UnkC4 = 0.3f;
    m_UnkC8 = 27.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 14.0f;
    m_UnkDC = 22.0f;
    m_UnkE0 = 24.0f;
    m_UnkE4 = 0.4f;
    m_UnkE8 = 23.0f;
    m_UnkEC = 14.0f;
    m_UnkF0 = 22.0f;
    m_UnkF4 = 24.0f;
    m_UnkF8 = 0.4f;
    m_UnkFC = 23.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 0.9f;
    m_Unk110 = 3.0f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 15.0f;
    m_Unk128 = 20.0f;
    m_Unk12C = 0.5f;
    m_Unk130 = 10.0f;
    m_Unk134 = 25.0f;
    m_Unk138 = 0.5f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 9.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 60.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Mii00_NORMAL_STAR::CC_PARAM_BOSS_Mii00_NORMAL_STAR() {}

CC_PARAM_BOSS_Mii00_NORMAL::CC_PARAM_BOSS_Mii00_NORMAL() {}

CC_PARAM_BOSS_Mii00_HARD_STAR::CC_PARAM_BOSS_Mii00_HARD_STAR() {}

CC_PARAM_BOSS_Mii00_HARD::CC_PARAM_BOSS_Mii00_HARD() {}

CC_PARAM_BOSS_Mii00_EXPERT_STAR::CC_PARAM_BOSS_Mii00_EXPERT_STAR() {}

CC_PARAM_BOSS_Mii00_EXPERT::CC_PARAM_BOSS_Mii00_EXPERT() {}

} // namespace cc_data_boss_Mii00

// --------------------------------------------------------------------------
// Mii01

CHR_CORE_BOSS::CC_PARAM_BOSS *CChrCoreBossMii01::NewCcParam_(long nHeap) const {
    using namespace cc_data_boss_Mii01;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Mii01_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BOSS_Mii01_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BOSS_Mii01_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BOSS_Mii01_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BOSS_Mii01_EASY;
            case 1: return new (nHeap) CC_PARAM_BOSS_Mii01_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BOSS_Mii01_HARD;
            case 3: return new (nHeap) CC_PARAM_BOSS_Mii01_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_boss_Mii01 {

CC_PARAM_BOSS_Mii01_EASY_STAR::CC_PARAM_BOSS_Mii01_EASY_STAR() {}

CC_PARAM_BOSS_Mii01_EASY::CC_PARAM_BOSS_Mii01_EASY() {}

CC_PARAM_BOSS_Mii01::CC_PARAM_BOSS_Mii01() {
    m_Unk0C = 6.3f;
    m_Unk10 = 5.0f;
    m_Unk14 = 0.07f;
    m_Unk28 = 1.0f;
    m_Unk2C = 1.0f;
    m_Unk30 = 1.0f;
    m_Unk18 = 30.0f;
    m_Unk1C = 90.0f;
    m_Unk20 = 8.0f;
    m_Unk24 = 0.3f;
    m_Unk00 = 0;
    m_Unk04 = 0;
    m_Unk08 = 0;
    m_Unk34 = 0.0f;
    m_Unk38 = 17.0f;
    m_Unk3C = 2.65f;
    m_Unk40 = 12.0f;
    m_Unk48 = 4.0f;
    m_Unk4C = 0.0f;
    m_Unk50 = 4.0f;
    m_Unk54 = 0.35f;
    m_Unk58 = 0.45f;
    m_Unk5C = 0.6f;
    m_Unk60 = 2.0f;
    m_Unk64 = 2.0f;
    m_Unk68 = 4.0f;
    m_Unk6C = 11.0f;
    m_Unk70 = 15.0f;
    m_Unk74 = 3.5f;
    m_Unk78 = 25.0f;
    m_Unk7C = 0.89f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.0f;
    m_Unk88 = 0;
    m_Unk8C = 1;
    m_Unk1AC = 4.5f;
    m_Unk1B0 = 24.0f;
    m_Unk1B8 = 26.0f;
    m_Unk1C4 = 24.0f;
    m_Unk1CC = 4.0f;
    m_Unk1D0 = 55;
    m_Unk1D4 = 1;
    m_Unk1D8 = 4.0f;
    m_Unk1DC = 40;
    m_Unk1E0 = 0.5f;
    m_Unk1E4 = 0.5f;
    m_Unk1E8 = 0;
    m_Unk1EC = 120;
    m_Unk1F0 = 1;
    m_Unk1F4 = 4.0f;
    m_Unk1F8 = 50;
    m_Unk1FC = 50;
    m_Unk200 = 4.0f;
    m_Unk204 = 0.01f;
    m_Unk208 = -0.01f;
    m_Unk20C = 2.0f;
    m_Unk210 = 3.5f;
    m_Unk214 = 3.25f;
    m_Unk218 = 3.0f;
    m_Unk90 = 0.9f;
    m_Unk94 = 3.0f;
    m_Unk98 = 1.2f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 10.0f;
    m_UnkA4 = 8.0f;
    m_UnkA8 = 0.5f;
    m_UnkAC = 11.0f;
    m_UnkB0 = 6.0f;
    m_UnkB4 = 1.0f;
    m_UnkB8 = 12.0f;
    m_UnkBC = 20.0f;
    m_UnkC0 = 22.0f;
    m_UnkC4 = 0.3f;
    m_UnkC8 = 27.0f;
    m_UnkCC = 35.0f;
    m_UnkD0 = 0.1f;
    m_UnkD4 = 0.8f;
    m_UnkD8 = 14.0f;
    m_UnkDC = 22.0f;
    m_UnkE0 = 24.0f;
    m_UnkE4 = 0.4f;
    m_UnkE8 = 23.0f;
    m_UnkEC = 14.0f;
    m_UnkF0 = 22.0f;
    m_UnkF4 = 24.0f;
    m_UnkF8 = 0.4f;
    m_UnkFC = 23.0f;
    m_Unk100 = 35.0f;
    m_Unk104 = 0.1f;
    m_Unk108 = 0.8f;
    m_Unk10C = 0.9f;
    m_Unk110 = 3.0f;
    m_Unk114 = 1.2f;
    m_Unk118 = 1.0f;
    m_Unk11C = 2.0f;
    m_Unk120 = 0.0f;
    m_Unk124 = 15.0f;
    m_Unk128 = 20.0f;
    m_Unk12C = 0.5f;
    m_Unk130 = 10.0f;
    m_Unk134 = 25.0f;
    m_Unk138 = 0.5f;
    m_Unk13C = 2.0f;
    m_Unk140 = 0.0f;
    m_Unk1A0 = 9.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.0f;
    m_Unk21C = 5.0f;
    m_Unk220 = 13.0f;
    m_Unk224 = 4.0f;
    m_Unk228 = 5.0f;
    m_Unk22C = 60.0f;
    m_Unk230 = 15.0f;
    __init2();
}

CC_PARAM_BOSS_Mii01_NORMAL_STAR::CC_PARAM_BOSS_Mii01_NORMAL_STAR() {}

CC_PARAM_BOSS_Mii01_NORMAL::CC_PARAM_BOSS_Mii01_NORMAL() {}

CC_PARAM_BOSS_Mii01_HARD_STAR::CC_PARAM_BOSS_Mii01_HARD_STAR() {}

CC_PARAM_BOSS_Mii01_HARD::CC_PARAM_BOSS_Mii01_HARD() {}

CC_PARAM_BOSS_Mii01_EXPERT_STAR::CC_PARAM_BOSS_Mii01_EXPERT_STAR() {}

CC_PARAM_BOSS_Mii01_EXPERT::CC_PARAM_BOSS_Mii01_EXPERT() {}

} // namespace cc_data_boss_Mii01

namespace cc_data_boss_default {

CHR_CORE_BOSS::CC_PARAM_BOSS m_CcParam;

} // namespace cc_data_boss_default
} // namespace msw
