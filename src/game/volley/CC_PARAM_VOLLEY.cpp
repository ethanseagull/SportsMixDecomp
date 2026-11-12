#include "game/msw/CC_PARAM_VOLLEY.hpp"
#include "game/msw/CChrCoreVolley.hpp"
#include "game/msw/CGame.hpp"
#include "bcs/New.hpp"

// Character control parameters for the volleyball mode.
//
// CHR_CORE_VOLLEY::CC_PARAM_VOLLEY holds the defaults. Each character has a
// cc_data_volley_<Name>::CC_PARAM_VOLLEY_<Name> whose constructor overrides
// part of them, and one class per game level (EASY ... EXPERT, each with a
// _STAR variant for the star player). The EASY and NORMAL classes and their
// _STAR variants change some values again; HARD and EXPERT change nothing.
// CChrCoreVolley<Name>::NewCcParam_ picks the class from the game level and
// the star flag.
//
// Field names are placeholders (see the header). The order of the
// assignments is the order of the stores in the original.

// The original file was compiled without optimisation and without inlining,
// like game/boss/CC_PARAM_BOSS.cpp: every float constant is reloaded for
// each store, the level constructors, CChrCore::IsStarPlayer and
// CGame::GetGameLevel are called instead of inlined (the inline ones emitted
// after their first caller), the switches become a compare tree, and the
// static initialiser is an optimised tail call (so the settings were in the
// source, not on the command line). Ignored by other compilers.
// file-level optimization pragma (see notes/decisions.md)
#pragma optimization_level 0
#pragma dont_inline on

namespace msw {

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY::CC_PARAM_VOLLEY() {
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
    m_Unk54 = 0.8f;
    m_Unk58 = 1.5f;
    m_Unk5C = 0.0f;
    m_Unk60 = 0.9f;
    m_Unk64 = 1.3f;
    m_Unk68 = 2.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = 0.85f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.0f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 12.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 0.0f;
    m_Unk98 = 0.0f;
    m_Unk9C = 0.0f;
    m_UnkA0 = 12.0f;
    m_UnkA4 = 1.0f;
    m_UnkA8 = 0.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.0f;
    m_UnkB4 = 37.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 1.8f;
    m_UnkD8 = 1.3f;
    m_UnkDC = 3.5f;
    m_UnkE0 = 10.0f;
    m_UnkE4 = 20.0f;
    m_UnkE8 = 3.5f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 0;
    m_Unk104 = 12.0f;
    m_Unk108 = 2.65f;
    m_Unk10C = 60.0f;
    m_Unk110 = 40.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 20.0f;
    m_Unk11C = 26.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 2.65f;
    m_Unk138 = 60.0f;
    m_Unk13C = 45.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 15.0f;
    m_Unk148 = 20.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.0f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.0f;
    m_Unk178 = 49.0f;
    m_Unk17C = 36.0f;
    m_Unk180 = 2.0f;
    m_Unk184 = 24.0f;
    m_Unk188 = 31.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 5.0f;
    m_Unk198 = 5.0f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 7.0f;
    m_Unk1B8 = 7.0f;
    m_Unk1BC = 7.0f;
    m_Unk1C0 = 7.0f;
    m_Unk1C4 = 7.0f;
    m_Unk1C8 = 7.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 8.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 3.5f;
    m_Unk1E4 = 3.5f;
    m_Unk1E8 = 3.5f;
    m_Unk1EC = 12.0f;
    m_Unk1F0 = 12.5f;
    m_Unk1F4 = 12.0f;
    m_Unk1F8 = 8.5f;
    m_Unk1FC = 8.5f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.75f;
    m_Unk208 = 1.8f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 15.0f;
    m_Unk230 = 7.0f;
    m_Unk234 = 11.0f;
    m_Unk238 = 17.0f;
    m_Unk23C = 0.5f;
    m_Unk240 = 25.0f;
    m_Unk244 = 0.0f;
    m_Unk248 = 2.7f;
    m_Unk24C = 2.7f;
    m_Unk250 = 14.5f;
    m_Unk254 = 23.0f;
    m_Unk258 = 26.0f;
    m_Unk25C = 0.5f;
    m_Unk260 = 25.0f;
    m_Unk264 = 35.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 17.0f;
    m_Unk274 = 26.0f;
    m_Unk278 = 29.0f;
    m_Unk27C = 0.8f;
    m_Unk280 = 22.0f;
    m_Unk284 = 23.0f;
    m_Unk288 = 38.0f;
    m_Unk28C = 38.0f;
    m_Unk290 = 0.5f;
    m_Unk294 = 15.0f;
    m_Unk298 = 35.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 28.0f;
    m_Unk2A8 = 48.0f;
    m_Unk2AC = 48.0f;
    m_Unk2B0 = 0.8f;
    m_Unk2B4 = 15.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 2.8f;
    m_Unk304 = 2.8f;
    m_Unk308 = 0.75f;
    m_Unk30C = 1.8f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 10.0f;
    m_Unk324 = 8.0f;
    m_Unk328 = 0.5f;
    m_Unk32C = 11.0f;
    m_Unk330 = 6.0f;
    m_Unk334 = 1.0f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 6.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 9.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 45.0f;
    m_Unk360 = 0.4f;
    m_Unk364 = 9.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 55.0f;
    m_Unk370 = 0.7f;
    m_Unk374 = 10.0f;
    m_Unk378 = 120.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 30.0f;
    m_Unk38C = 30.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 18.5f;
    m_Unk39C = 18.5f;
    m_Unk3A0 = 18.5f;
    m_Unk3A4 = 4.0f;
    m_Unk3A8 = 1;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3D4 = 9.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    m_Unk3E0 = 0;
}

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolley::NewCcParam_(long nHeap) const {
    return new (nHeap) CHR_CORE_VOLLEY::CC_PARAM_VOLLEY;
}

// (what __init2 is for is not known; every one is empty)
void cc_data_volley_Mario::CC_PARAM_VOLLEY_Mario::__init2() {}
void cc_data_volley_Luigi::CC_PARAM_VOLLEY_Luigi::__init2() {}
void cc_data_volley_Yoshi::CC_PARAM_VOLLEY_Yoshi::__init2() {}
void cc_data_volley_Ninja::CC_PARAM_VOLLEY_Ninja::__init2() {}
void cc_data_volley_Mii00::CC_PARAM_VOLLEY_Mii00::__init2() {}
void cc_data_volley_Mii01::CC_PARAM_VOLLEY_Mii01::__init2() {}
void cc_data_volley_Peach::CC_PARAM_VOLLEY_Peach::__init2() {}
void cc_data_volley_Daisy::CC_PARAM_VOLLEY_Daisy::__init2() {}
void cc_data_volley_Waluigi::CC_PARAM_VOLLEY_Waluigi::__init2() {}
void cc_data_volley_Siroma::CC_PARAM_VOLLEY_Siroma::__init2() {}
void cc_data_volley_Koopa::CC_PARAM_VOLLEY_Koopa::__init2() {}
void cc_data_volley_Donkey::CC_PARAM_VOLLEY_Donkey::__init2() {}
void cc_data_volley_Wario::CC_PARAM_VOLLEY_Wario::__init2() {}
void cc_data_volley_KoopaJr::CC_PARAM_VOLLEY_KoopaJr::__init2() {}
void cc_data_volley_Kinopio::CC_PARAM_VOLLEY_Kinopio::__init2() {}
void cc_data_volley_Diddy::CC_PARAM_VOLLEY_Diddy::__init2() {}
void cc_data_volley_Moogle::CC_PARAM_VOLLEY_Moogle::__init2() {}
void cc_data_volley_Saboten::CC_PARAM_VOLLEY_Saboten::__init2() {}
void cc_data_volley_Slime::CC_PARAM_VOLLEY_Slime::__init2() {}
void cc_data_volley_Kuroma::CC_PARAM_VOLLEY_Kuroma::__init2() {}

// --------------------------------------------------------------------------
// Mario

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleyMario::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Mario;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Mario_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Mario_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Mario_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Mario_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Mario_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Mario_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Mario_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Mario_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Mario {

CC_PARAM_VOLLEY_Mario_EASY_STAR::CC_PARAM_VOLLEY_Mario_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Mario_EASY::CC_PARAM_VOLLEY_Mario_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.02f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.02f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.03f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_Mario::CC_PARAM_VOLLEY_Mario() {
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
    m_Unk54 = 0.8f;
    m_Unk58 = 1.5f;
    m_Unk5C = 0.0f;
    m_Unk60 = 0.9f;
    m_Unk64 = 1.3f;
    m_Unk68 = 2.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = 0.85f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.5f;
    m_Unk80 = 8.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 12.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 0.0f;
    m_Unk98 = 0.0f;
    m_Unk9C = 0.0f;
    m_UnkA0 = 35.0f;
    m_UnkA4 = 4.0f;
    m_UnkA8 = 20.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.5f;
    m_UnkB4 = 37.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.0f;
    m_UnkD8 = 2.0f;
    m_UnkDC = 4.0f;
    m_UnkE0 = 11.0f;
    m_UnkE4 = 10.0f;
    m_UnkE8 = 2.8f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.0f;
    m_Unk10C = 70.0f;
    m_Unk110 = 36.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 2.8f;
    m_Unk138 = 70.0f;
    m_Unk13C = 32.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.3f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 11.0f;
    m_Unk1B8 = 4.0f;
    m_Unk1BC = 2.0f;
    m_Unk1C0 = 12.0f;
    m_Unk1C4 = 8.0f;
    m_Unk1C8 = 8.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 12.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 3.5f;
    m_Unk1E4 = 3.5f;
    m_Unk1E8 = 3.5f;
    m_Unk1EC = 17.0f;
    m_Unk1F0 = 17.5f;
    m_Unk1F4 = 17.0f;
    m_Unk1F8 = 13.0f;
    m_Unk1FC = 8.0f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.9f;
    m_Unk208 = 3.0f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.3f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.3f;
    m_Unk260 = 27.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.4f;
    m_Unk280 = 23.0f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.4f;
    m_Unk294 = 23.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2B4 = 18.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 3.7f;
    m_Unk304 = 3.7f;
    m_Unk308 = 1.1f;
    m_Unk30C = 3.0f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 20.0f;
    m_Unk324 = 8.0f;
    m_Unk328 = 0.5f;
    m_Unk32C = 15.0f;
    m_Unk330 = 20.0f;
    m_Unk334 = 0.8f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 9.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 40.0f;
    m_Unk360 = 0.2f;
    m_Unk364 = 9.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 55.0f;
    m_Unk370 = 0.7f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 50.0f;
    m_Unk388 = 30.0f;
    m_Unk38C = 30.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 25.0f;
    m_Unk39C = 28.0f;
    m_Unk3A0 = 30.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 9.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Mario_NORMAL_STAR::CC_PARAM_VOLLEY_Mario_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.3f;
    m_Unk25C = 0.3f;
    m_Unk27C = 0.4f;
    m_Unk290 = 0.4f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Mario_NORMAL::CC_PARAM_VOLLEY_Mario_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Mario_HARD_STAR::CC_PARAM_VOLLEY_Mario_HARD_STAR() {}

CC_PARAM_VOLLEY_Mario_HARD::CC_PARAM_VOLLEY_Mario_HARD() {}

CC_PARAM_VOLLEY_Mario_EXPERT_STAR::CC_PARAM_VOLLEY_Mario_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Mario_EXPERT::CC_PARAM_VOLLEY_Mario_EXPERT() {}

} // namespace cc_data_volley_Mario

// --------------------------------------------------------------------------
// Luigi

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleyLuigi::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Luigi;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Luigi_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Luigi_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Luigi_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Luigi_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Luigi_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Luigi_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Luigi_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Luigi_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Luigi {

CC_PARAM_VOLLEY_Luigi_EASY_STAR::CC_PARAM_VOLLEY_Luigi_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Luigi_EASY::CC_PARAM_VOLLEY_Luigi_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.02f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.02f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.03f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_Luigi::CC_PARAM_VOLLEY_Luigi() {
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
    m_Unk54 = 0.5f;
    m_Unk58 = 1.8f;
    m_Unk5C = -0.3f;
    m_Unk60 = 0.9f;
    m_Unk64 = 1.3f;
    m_Unk68 = 2.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = 0.85f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.5f;
    m_Unk80 = 10.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 12.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 0.0f;
    m_Unk98 = 0.0f;
    m_Unk9C = 0.0f;
    m_UnkA0 = 35.0f;
    m_UnkA4 = 4.0f;
    m_UnkA8 = 20.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.5f;
    m_UnkB4 = 37.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.0f;
    m_UnkD8 = 2.0f;
    m_UnkDC = 4.5f;
    m_UnkE0 = 11.0f;
    m_UnkE4 = 10.0f;
    m_UnkE8 = 2.8f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.0f;
    m_Unk10C = 70.0f;
    m_Unk110 = 36.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 2.8f;
    m_Unk138 = 70.0f;
    m_Unk13C = 32.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.0f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 11.0f;
    m_Unk1B8 = 4.0f;
    m_Unk1BC = 2.0f;
    m_Unk1C0 = 12.0f;
    m_Unk1C4 = 8.0f;
    m_Unk1C8 = 8.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 12.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 3.5f;
    m_Unk1E4 = 3.5f;
    m_Unk1E8 = 3.5f;
    m_Unk1EC = 17.0f;
    m_Unk1F0 = 17.5f;
    m_Unk1F4 = 17.0f;
    m_Unk1F8 = 7.5f;
    m_Unk1FC = 7.5f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.9f;
    m_Unk208 = 3.0f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.3f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.3f;
    m_Unk260 = 27.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.4f;
    m_Unk280 = 23.0f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.4f;
    m_Unk294 = 23.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2B4 = 18.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 3.7f;
    m_Unk304 = 3.7f;
    m_Unk308 = 0.9f;
    m_Unk30C = 3.0f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 20.0f;
    m_Unk324 = 10.0f;
    m_Unk328 = 1.0f;
    m_Unk32C = 20.0f;
    m_Unk330 = 30.0f;
    m_Unk334 = 0.8f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 9.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 40.0f;
    m_Unk360 = 0.2f;
    m_Unk364 = 9.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 55.0f;
    m_Unk370 = 0.7f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 150.0f;
    m_Unk38C = 20.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 18.0f;
    m_Unk39C = 25.0f;
    m_Unk3A0 = 30.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 9.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Luigi_NORMAL_STAR::CC_PARAM_VOLLEY_Luigi_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.3f;
    m_Unk25C = 0.3f;
    m_Unk27C = 0.4f;
    m_Unk290 = 0.4f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Luigi_NORMAL::CC_PARAM_VOLLEY_Luigi_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Luigi_HARD_STAR::CC_PARAM_VOLLEY_Luigi_HARD_STAR() {}

CC_PARAM_VOLLEY_Luigi_HARD::CC_PARAM_VOLLEY_Luigi_HARD() {}

CC_PARAM_VOLLEY_Luigi_EXPERT_STAR::CC_PARAM_VOLLEY_Luigi_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Luigi_EXPERT::CC_PARAM_VOLLEY_Luigi_EXPERT() {}

} // namespace cc_data_volley_Luigi

// --------------------------------------------------------------------------
// Peach

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleyPeach::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Peach;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Peach_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Peach_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Peach_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Peach_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Peach_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Peach_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Peach_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Peach_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Peach {

CC_PARAM_VOLLEY_Peach_EASY_STAR::CC_PARAM_VOLLEY_Peach_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Peach_EASY::CC_PARAM_VOLLEY_Peach_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.02f;
    m_Unk250 = 12.0f;
    m_Unk254 = 19.0f;
    m_Unk258 = 21.0f;
    m_Unk25C = 0.02f;
    m_Unk270 = 13.0f;
    m_Unk274 = 21.0f;
    m_Unk278 = 23.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 13.0f;
    m_Unk288 = 21.0f;
    m_Unk28C = 23.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.03f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_Peach::CC_PARAM_VOLLEY_Peach() {
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
    m_Unk54 = 0.8f;
    m_Unk58 = 1.5f;
    m_Unk5C = 0.0f;
    m_Unk60 = 0.9f;
    m_Unk64 = 1.3f;
    m_Unk68 = 2.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = 0.85f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.6f;
    m_Unk80 = 5.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 25.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 0.0f;
    m_Unk98 = 0.0f;
    m_Unk9C = 0.0f;
    m_UnkA0 = 50.0f;
    m_UnkA4 = 4.0f;
    m_UnkA8 = 25.0f;
    m_UnkAC = 5.0f;
    m_UnkB0 = 0.5f;
    m_UnkB4 = 37.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.0f;
    m_UnkD8 = 2.0f;
    m_UnkDC = 4.5f;
    m_UnkE0 = 12.0f;
    m_UnkE4 = 7.0f;
    m_UnkE8 = 2.5f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.0f;
    m_Unk10C = 70.0f;
    m_Unk110 = 32.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 2.8f;
    m_Unk138 = 70.0f;
    m_Unk13C = 32.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.7f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 9.0f;
    m_Unk1B8 = 4.0f;
    m_Unk1BC = 6.0f;
    m_Unk1C0 = 16.0f;
    m_Unk1C4 = 5.0f;
    m_Unk1C8 = 5.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 12.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 1.0f;
    m_Unk1E4 = 1.0f;
    m_Unk1E8 = 1.0f;
    m_Unk1EC = 20.0f;
    m_Unk1F0 = 20.5f;
    m_Unk1F4 = 20.0f;
    m_Unk1F8 = 6.0f;
    m_Unk1FC = 6.0f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.9f;
    m_Unk208 = 5.0f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.3f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 12.0f;
    m_Unk254 = 19.0f;
    m_Unk258 = 21.0f;
    m_Unk25C = 0.3f;
    m_Unk260 = 29.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 13.0f;
    m_Unk274 = 21.0f;
    m_Unk278 = 23.0f;
    m_Unk27C = 0.4f;
    m_Unk280 = 25.0f;
    m_Unk284 = 13.0f;
    m_Unk288 = 21.0f;
    m_Unk28C = 23.0f;
    m_Unk290 = 0.4f;
    m_Unk294 = 25.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2B4 = 18.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 3.0f;
    m_Unk304 = 3.0f;
    m_Unk308 = 1.0f;
    m_Unk30C = 5.0f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 180.0f;
    m_Unk320 = 10.0f;
    m_Unk324 = 8.0f;
    m_Unk328 = 0.5f;
    m_Unk32C = 15.0f;
    m_Unk330 = 20.0f;
    m_Unk334 = 0.8f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 12.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 40.0f;
    m_Unk360 = 0.01f;
    m_Unk364 = 7.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 60.0f;
    m_Unk370 = 1.5f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 60.0f;
    m_Unk38C = 0.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 12.0f;
    m_Unk39C = 15.0f;
    m_Unk3A0 = 17.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 12.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Peach_NORMAL_STAR::CC_PARAM_VOLLEY_Peach_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.3f;
    m_Unk25C = 0.3f;
    m_Unk27C = 0.4f;
    m_Unk290 = 0.4f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Peach_NORMAL::CC_PARAM_VOLLEY_Peach_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Peach_HARD_STAR::CC_PARAM_VOLLEY_Peach_HARD_STAR() {}

CC_PARAM_VOLLEY_Peach_HARD::CC_PARAM_VOLLEY_Peach_HARD() {}

CC_PARAM_VOLLEY_Peach_EXPERT_STAR::CC_PARAM_VOLLEY_Peach_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Peach_EXPERT::CC_PARAM_VOLLEY_Peach_EXPERT() {}

} // namespace cc_data_volley_Peach

// --------------------------------------------------------------------------
// Daisy

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleyDaisy::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Daisy;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Daisy_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Daisy_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Daisy_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Daisy_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Daisy_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Daisy_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Daisy_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Daisy_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Daisy {

CC_PARAM_VOLLEY_Daisy_EASY_STAR::CC_PARAM_VOLLEY_Daisy_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Daisy_EASY::CC_PARAM_VOLLEY_Daisy_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.02f;
    m_Unk250 = 12.0f;
    m_Unk254 = 19.0f;
    m_Unk258 = 21.0f;
    m_Unk25C = 0.02f;
    m_Unk270 = 13.0f;
    m_Unk274 = 21.0f;
    m_Unk278 = 23.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 13.0f;
    m_Unk288 = 21.0f;
    m_Unk28C = 23.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.03f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_Daisy::CC_PARAM_VOLLEY_Daisy() {
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
    m_Unk54 = 1.0f;
    m_Unk58 = 1.6f;
    m_Unk5C = -0.05f;
    m_Unk60 = 0.9f;
    m_Unk64 = 1.3f;
    m_Unk68 = 2.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = 0.85f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.0f;
    m_Unk80 = 8.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 25.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 0.0f;
    m_Unk98 = 0.0f;
    m_Unk9C = 0.0f;
    m_UnkA0 = 35.0f;
    m_UnkA4 = 2.5f;
    m_UnkA8 = 20.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.5f;
    m_UnkB4 = 37.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.0f;
    m_UnkD8 = 2.0f;
    m_UnkDC = 4.5f;
    m_UnkE0 = 12.0f;
    m_UnkE4 = 6.0f;
    m_UnkE8 = 2.8f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.0f;
    m_Unk10C = 70.0f;
    m_Unk110 = 36.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 2.8f;
    m_Unk138 = 70.0f;
    m_Unk13C = 32.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.0f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 7.0f;
    m_Unk1B8 = 2.0f;
    m_Unk1BC = 5.0f;
    m_Unk1C0 = 12.0f;
    m_Unk1C4 = 5.0f;
    m_Unk1C8 = 5.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 12.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 1.0f;
    m_Unk1E4 = 1.0f;
    m_Unk1E8 = 1.0f;
    m_Unk1EC = 20.0f;
    m_Unk1F0 = 20.5f;
    m_Unk1F4 = 20.0f;
    m_Unk1F8 = 8.5f;
    m_Unk1FC = 8.5f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.9f;
    m_Unk208 = 5.0f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.3f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 12.0f;
    m_Unk254 = 19.0f;
    m_Unk258 = 21.0f;
    m_Unk25C = 0.3f;
    m_Unk260 = 29.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 13.0f;
    m_Unk274 = 21.0f;
    m_Unk278 = 23.0f;
    m_Unk27C = 0.4f;
    m_Unk280 = 25.0f;
    m_Unk284 = 13.0f;
    m_Unk288 = 21.0f;
    m_Unk28C = 23.0f;
    m_Unk290 = 0.4f;
    m_Unk294 = 25.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2B4 = 18.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 3.0f;
    m_Unk304 = 3.0f;
    m_Unk308 = 1.0f;
    m_Unk30C = 5.0f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 17.0f;
    m_Unk324 = 10.0f;
    m_Unk328 = 0.5f;
    m_Unk32C = 25.0f;
    m_Unk330 = 26.0f;
    m_Unk334 = 0.3f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 12.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 40.0f;
    m_Unk360 = 0.01f;
    m_Unk364 = 7.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 60.0f;
    m_Unk370 = 0.7f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 30.0f;
    m_Unk38C = 30.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 11.0f;
    m_Unk39C = 14.0f;
    m_Unk3A0 = 18.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 4.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 12.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Daisy_NORMAL_STAR::CC_PARAM_VOLLEY_Daisy_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.3f;
    m_Unk25C = 0.3f;
    m_Unk27C = 0.4f;
    m_Unk290 = 0.4f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Daisy_NORMAL::CC_PARAM_VOLLEY_Daisy_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Daisy_HARD_STAR::CC_PARAM_VOLLEY_Daisy_HARD_STAR() {}

CC_PARAM_VOLLEY_Daisy_HARD::CC_PARAM_VOLLEY_Daisy_HARD() {}

CC_PARAM_VOLLEY_Daisy_EXPERT_STAR::CC_PARAM_VOLLEY_Daisy_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Daisy_EXPERT::CC_PARAM_VOLLEY_Daisy_EXPERT() {}

} // namespace cc_data_volley_Daisy

// --------------------------------------------------------------------------
// Yoshi

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleyYoshi::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Yoshi;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Yoshi_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Yoshi_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Yoshi_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Yoshi_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Yoshi_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Yoshi_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Yoshi_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Yoshi_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Yoshi {

CC_PARAM_VOLLEY_Yoshi_EASY_STAR::CC_PARAM_VOLLEY_Yoshi_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 17.0f;
    m_Unk2A8 = 26.0f;
    m_Unk2AC = 29.0f;
    m_Unk2B0 = 0.32f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Yoshi_EASY::CC_PARAM_VOLLEY_Yoshi_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.02f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.02f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 16.0f;
    m_Unk2A8 = 25.0f;
    m_Unk2AC = 28.0f;
    m_Unk2B0 = 0.02f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_Yoshi::CC_PARAM_VOLLEY_Yoshi() {
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
    m_Unk54 = 0.8f;
    m_Unk58 = 1.5f;
    m_Unk5C = 0.0f;
    m_Unk60 = 0.9f;
    m_Unk64 = 1.3f;
    m_Unk68 = 2.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = 0.85f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.0f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 12.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 0.0f;
    m_Unk98 = 0.0f;
    m_Unk9C = 0.0f;
    m_UnkA0 = 35.0f;
    m_UnkA4 = 4.0f;
    m_UnkA8 = 20.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.5f;
    m_UnkB4 = 37.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.0f;
    m_UnkD8 = 2.0f;
    m_UnkDC = 4.0f;
    m_UnkE0 = 11.0f;
    m_UnkE4 = 20.0f;
    m_UnkE8 = 3.5f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.0f;
    m_Unk10C = 70.0f;
    m_Unk110 = 36.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 2.8f;
    m_Unk138 = 70.0f;
    m_Unk13C = 32.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.2f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 11.0f;
    m_Unk1B8 = 4.0f;
    m_Unk1BC = 2.0f;
    m_Unk1C0 = 12.0f;
    m_Unk1C4 = 8.0f;
    m_Unk1C8 = 8.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 12.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 3.5f;
    m_Unk1E4 = 3.5f;
    m_Unk1E8 = 3.5f;
    m_Unk1EC = 17.0f;
    m_Unk1F0 = 17.5f;
    m_Unk1F4 = 17.0f;
    m_Unk1F8 = 6.0f;
    m_Unk1FC = 6.0f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.9f;
    m_Unk208 = 3.0f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.3f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.3f;
    m_Unk260 = 27.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.4f;
    m_Unk280 = 23.0f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.4f;
    m_Unk294 = 23.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 18.0f;
    m_Unk2A8 = 27.0f;
    m_Unk2AC = 30.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2B4 = 25.0f;
    m_Unk2B8 = 2;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 3.0f;
    m_Unk2C4 = 3.0f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 3.7f;
    m_Unk304 = 3.7f;
    m_Unk308 = 0.9f;
    m_Unk30C = 3.0f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 20.0f;
    m_Unk324 = 10.0f;
    m_Unk328 = 0.5f;
    m_Unk32C = 15.0f;
    m_Unk330 = 18.0f;
    m_Unk334 = 0.5f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 9.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 40.0f;
    m_Unk360 = 1.0f;
    m_Unk364 = 9.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 55.0f;
    m_Unk370 = 0.7f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 30.0f;
    m_Unk38C = 30.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 11.0f;
    m_Unk39C = 14.0f;
    m_Unk3A0 = 15.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 9.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Yoshi_NORMAL_STAR::CC_PARAM_VOLLEY_Yoshi_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.3f;
    m_Unk25C = 0.3f;
    m_Unk27C = 0.4f;
    m_Unk290 = 0.4f;
    m_Unk2A4 = 18.0f;
    m_Unk2A8 = 27.0f;
    m_Unk2AC = 30.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Yoshi_NORMAL::CC_PARAM_VOLLEY_Yoshi_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 17.0f;
    m_Unk2A8 = 26.0f;
    m_Unk2AC = 29.0f;
    m_Unk2B0 = 0.32f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Yoshi_HARD_STAR::CC_PARAM_VOLLEY_Yoshi_HARD_STAR() {}

CC_PARAM_VOLLEY_Yoshi_HARD::CC_PARAM_VOLLEY_Yoshi_HARD() {}

CC_PARAM_VOLLEY_Yoshi_EXPERT_STAR::CC_PARAM_VOLLEY_Yoshi_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Yoshi_EXPERT::CC_PARAM_VOLLEY_Yoshi_EXPERT() {}

} // namespace cc_data_volley_Yoshi

// --------------------------------------------------------------------------
// Wario

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleyWario::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Wario;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Wario_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Wario_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Wario_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Wario_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Wario_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Wario_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Wario_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Wario_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Wario {

CC_PARAM_VOLLEY_Wario_EASY_STAR::CC_PARAM_VOLLEY_Wario_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.41f;
    m_Unk25C = 0.41f;
    m_Unk27C = 0.48f;
    m_Unk290 = 0.48f;
    m_Unk2A4 = 23.0f;
    m_Unk2A8 = 43.0f;
    m_Unk2AC = 43.0f;
    m_Unk2B0 = 0.48f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Wario_EASY::CC_PARAM_VOLLEY_Wario_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 14.0f;
    m_Unk234 = 24.0f;
    m_Unk238 = 26.0f;
    m_Unk23C = 0.03f;
    m_Unk250 = 14.0f;
    m_Unk254 = 24.0f;
    m_Unk258 = 26.0f;
    m_Unk25C = 0.03f;
    m_Unk270 = 18.0f;
    m_Unk274 = 28.0f;
    m_Unk278 = 30.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 18.0f;
    m_Unk288 = 28.0f;
    m_Unk28C = 30.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 23.0f;
    m_Unk2A8 = 43.0f;
    m_Unk2AC = 43.0f;
    m_Unk2B0 = 0.03f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_Wario::CC_PARAM_VOLLEY_Wario() {
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
    m_Unk54 = 1.5f;
    m_Unk58 = 2.5f;
    m_Unk5C = -0.3f;
    m_Unk60 = 0.9f;
    m_Unk64 = 1.3f;
    m_Unk68 = 2.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = 0.85f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.0f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 12.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 0.0f;
    m_Unk98 = 0.0f;
    m_Unk9C = 0.0f;
    m_UnkA0 = 12.0f;
    m_UnkA4 = 1.0f;
    m_UnkA8 = 0.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.0f;
    m_UnkB4 = 34.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.0f;
    m_UnkD8 = 2.0f;
    m_UnkDC = 2.3f;
    m_UnkE0 = 7.0f;
    m_UnkE4 = 7.0f;
    m_UnkE8 = 2.5f;
    m_UnkEC = 40.0f;
    m_UnkF0 = 0.95f;
    m_UnkF4 = 180.0f;
    m_UnkF8 = 180.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.0f;
    m_Unk10C = 70.0f;
    m_Unk110 = 34.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 2.8f;
    m_Unk138 = 70.0f;
    m_Unk13C = 32.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.0f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 11.0f;
    m_Unk1B8 = 6.0f;
    m_Unk1BC = 2.0f;
    m_Unk1C0 = 10.0f;
    m_Unk1C4 = 3.0f;
    m_Unk1C8 = 3.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 1.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 33.0f;
    m_Unk1E0 = 1.5f;
    m_Unk1E4 = 1.5f;
    m_Unk1E8 = 1.5f;
    m_Unk1EC = 8.5f;
    m_Unk1F0 = 8.5f;
    m_Unk1F4 = 8.5f;
    m_Unk1F8 = 8.5f;
    m_Unk1FC = 8.5f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.9f;
    m_Unk208 = 3.0f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 14.0f;
    m_Unk234 = 24.0f;
    m_Unk238 = 26.0f;
    m_Unk23C = 0.5f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 14.0f;
    m_Unk254 = 24.0f;
    m_Unk258 = 26.0f;
    m_Unk25C = 0.5f;
    m_Unk260 = 22.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 18.0f;
    m_Unk274 = 28.0f;
    m_Unk278 = 30.0f;
    m_Unk27C = 0.6f;
    m_Unk280 = 20.0f;
    m_Unk284 = 18.0f;
    m_Unk288 = 28.0f;
    m_Unk28C = 30.0f;
    m_Unk290 = 0.6f;
    m_Unk294 = 20.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 23.0f;
    m_Unk2A8 = 43.0f;
    m_Unk2AC = 43.0f;
    m_Unk2B0 = 0.6f;
    m_Unk2B4 = 17.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 3.7f;
    m_Unk304 = 3.7f;
    m_Unk308 = 1.0f;
    m_Unk30C = 3.0f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 8.0f;
    m_Unk324 = 18.0f;
    m_Unk328 = 2.0f;
    m_Unk32C = 14.0f;
    m_Unk330 = 8.0f;
    m_Unk334 = 1.0f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 7.0f;
    m_Unk358 = 1.2f;
    m_Unk35C = 50.0f;
    m_Unk360 = 0.3f;
    m_Unk364 = 15.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 50.0f;
    m_Unk370 = 0.5f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 30.0f;
    m_Unk38C = 30.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 13.0f;
    m_Unk39C = 18.0f;
    m_Unk3A0 = 20.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 9.0f;
    m_Unk3D8 = 5.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Wario_NORMAL_STAR::CC_PARAM_VOLLEY_Wario_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.5f;
    m_Unk25C = 0.5f;
    m_Unk27C = 0.6f;
    m_Unk290 = 0.6f;
    m_Unk2A4 = 23.0f;
    m_Unk2A8 = 43.0f;
    m_Unk2AC = 43.0f;
    m_Unk2B0 = 0.6f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Wario_NORMAL::CC_PARAM_VOLLEY_Wario_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.41f;
    m_Unk25C = 0.41f;
    m_Unk27C = 0.48f;
    m_Unk290 = 0.48f;
    m_Unk2A4 = 23.0f;
    m_Unk2A8 = 43.0f;
    m_Unk2AC = 43.0f;
    m_Unk2B0 = 0.48f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Wario_HARD_STAR::CC_PARAM_VOLLEY_Wario_HARD_STAR() {}

CC_PARAM_VOLLEY_Wario_HARD::CC_PARAM_VOLLEY_Wario_HARD() {}

CC_PARAM_VOLLEY_Wario_EXPERT_STAR::CC_PARAM_VOLLEY_Wario_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Wario_EXPERT::CC_PARAM_VOLLEY_Wario_EXPERT() {}

} // namespace cc_data_volley_Wario

// --------------------------------------------------------------------------
// Waluigi

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleyWaluigi::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Waluigi;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Waluigi_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Waluigi_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Waluigi_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Waluigi_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Waluigi_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Waluigi_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Waluigi_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Waluigi_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Waluigi {

CC_PARAM_VOLLEY_Waluigi_EASY_STAR::CC_PARAM_VOLLEY_Waluigi_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Waluigi_EASY::CC_PARAM_VOLLEY_Waluigi_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.02f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.02f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.03f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_Waluigi::CC_PARAM_VOLLEY_Waluigi() {
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
    m_Unk54 = 1.2f;
    m_Unk58 = 2.0f;
    m_Unk5C = 0.0f;
    m_Unk60 = 0.85f;
    m_Unk64 = 0.7f;
    m_Unk68 = 2.2f;
    m_Unk6C = -0.1f;
    m_Unk70 = 0.9f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.5f;
    m_Unk80 = 8.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 12.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 0.0f;
    m_Unk98 = 0.0f;
    m_Unk9C = 0.0f;
    m_UnkA0 = 12.0f;
    m_UnkA4 = 1.0f;
    m_UnkA8 = 0.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.0f;
    m_UnkB4 = 37.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.0f;
    m_UnkD8 = 2.0f;
    m_UnkDC = 4.5f;
    m_UnkE0 = 12.0f;
    m_UnkE4 = 15.0f;
    m_UnkE8 = 3.0f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.5f;
    m_Unk10C = 70.0f;
    m_Unk110 = 34.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 3.5f;
    m_Unk138 = 70.0f;
    m_Unk13C = 28.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.0f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 0.0f;
    m_Unk1B8 = 18.0f;
    m_Unk1BC = 12.0f;
    m_Unk1C0 = 0.0f;
    m_Unk1C4 = 6.0f;
    m_Unk1C8 = 6.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 12.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 3.5f;
    m_Unk1E4 = 3.5f;
    m_Unk1E8 = 3.5f;
    m_Unk1EC = 17.0f;
    m_Unk1F0 = 17.5f;
    m_Unk1F4 = 17.0f;
    m_Unk1F8 = 7.0f;
    m_Unk1FC = 7.0f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.9f;
    m_Unk208 = 3.0f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.3f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.3f;
    m_Unk260 = 27.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.4f;
    m_Unk280 = 23.0f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.4f;
    m_Unk294 = 23.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2B4 = 18.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 1;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 3.0f;
    m_Unk304 = 3.0f;
    m_Unk308 = 1.0f;
    m_Unk30C = 3.0f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 10.0f;
    m_Unk324 = 20.0f;
    m_Unk328 = 2.0f;
    m_Unk32C = 14.0f;
    m_Unk330 = 10.0f;
    m_Unk334 = 0.7f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 9.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 40.0f;
    m_Unk360 = 0.2f;
    m_Unk364 = 9.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 55.0f;
    m_Unk370 = 0.7f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 30.0f;
    m_Unk38C = 30.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 15.5f;
    m_Unk39C = 17.5f;
    m_Unk3A0 = 20.5f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 9.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Waluigi_NORMAL_STAR::CC_PARAM_VOLLEY_Waluigi_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.3f;
    m_Unk25C = 0.3f;
    m_Unk27C = 0.4f;
    m_Unk290 = 0.4f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Waluigi_NORMAL::CC_PARAM_VOLLEY_Waluigi_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Waluigi_HARD_STAR::CC_PARAM_VOLLEY_Waluigi_HARD_STAR() {}

CC_PARAM_VOLLEY_Waluigi_HARD::CC_PARAM_VOLLEY_Waluigi_HARD() {}

CC_PARAM_VOLLEY_Waluigi_EXPERT_STAR::CC_PARAM_VOLLEY_Waluigi_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Waluigi_EXPERT::CC_PARAM_VOLLEY_Waluigi_EXPERT() {}

} // namespace cc_data_volley_Waluigi

// --------------------------------------------------------------------------
// Donkey

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleyDonkey::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Donkey;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Donkey_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Donkey_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Donkey_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Donkey_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Donkey_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Donkey_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Donkey_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Donkey_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Donkey {

CC_PARAM_VOLLEY_Donkey_EASY_STAR::CC_PARAM_VOLLEY_Donkey_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.41f;
    m_Unk25C = 0.41f;
    m_Unk27C = 0.48f;
    m_Unk290 = 0.48f;
    m_Unk2A4 = 23.0f;
    m_Unk2A8 = 43.0f;
    m_Unk2AC = 43.0f;
    m_Unk2B0 = 0.48f;
    m_Unk2C8 = 28.0f;
    m_Unk2CC = 48.0f;
    m_Unk2D0 = 48.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Donkey_EASY::CC_PARAM_VOLLEY_Donkey_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 14.0f;
    m_Unk234 = 24.0f;
    m_Unk238 = 26.0f;
    m_Unk23C = 0.03f;
    m_Unk250 = 14.0f;
    m_Unk254 = 24.0f;
    m_Unk258 = 26.0f;
    m_Unk25C = 0.03f;
    m_Unk270 = 18.0f;
    m_Unk274 = 28.0f;
    m_Unk278 = 30.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 18.0f;
    m_Unk288 = 28.0f;
    m_Unk28C = 30.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 23.0f;
    m_Unk2A8 = 43.0f;
    m_Unk2AC = 43.0f;
    m_Unk2B0 = 0.03f;
    m_Unk2C8 = 28.0f;
    m_Unk2CC = 48.0f;
    m_Unk2D0 = 48.0f;
    m_Unk2D4 = 0.05f;
}

CC_PARAM_VOLLEY_Donkey::CC_PARAM_VOLLEY_Donkey() {
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
    m_Unk54 = 1.5f;
    m_Unk58 = 2.5f;
    m_Unk5C = -0.3f;
    m_Unk60 = 0.9f;
    m_Unk64 = 1.3f;
    m_Unk68 = 2.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = 0.85f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.0f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 12.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 0.0f;
    m_Unk98 = 0.0f;
    m_Unk9C = 0.0f;
    m_UnkA0 = 12.0f;
    m_UnkA4 = 1.0f;
    m_UnkA8 = 0.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.0f;
    m_UnkB4 = 34.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.0f;
    m_UnkD8 = 2.0f;
    m_UnkDC = 2.8f;
    m_UnkE0 = 11.0f;
    m_UnkE4 = 5.0f;
    m_UnkE8 = 1.5f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 15.0f;
    m_Unk108 = 3.8f;
    m_Unk10C = 70.0f;
    m_Unk110 = 32.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 15.0f;
    m_Unk134 = 3.1f;
    m_Unk138 = 70.0f;
    m_Unk13C = 30.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.0f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 11.0f;
    m_Unk1B8 = 6.0f;
    m_Unk1BC = 2.0f;
    m_Unk1C0 = 10.0f;
    m_Unk1C4 = 3.0f;
    m_Unk1C8 = 3.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 1.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 33.0f;
    m_Unk1E0 = 1.5f;
    m_Unk1E4 = 1.5f;
    m_Unk1E8 = 1.5f;
    m_Unk1EC = 7.0f;
    m_Unk1F0 = 7.0f;
    m_Unk1F4 = 7.0f;
    m_Unk1F8 = 8.0f;
    m_Unk1FC = 8.0f;
    m_Unk200 = 0.0f;
    m_Unk204 = 1.0f;
    m_Unk208 = 3.0f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 14.0f;
    m_Unk234 = 24.0f;
    m_Unk238 = 26.0f;
    m_Unk23C = 0.5f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 14.0f;
    m_Unk254 = 24.0f;
    m_Unk258 = 26.0f;
    m_Unk25C = 0.5f;
    m_Unk260 = 22.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 18.0f;
    m_Unk274 = 28.0f;
    m_Unk278 = 30.0f;
    m_Unk27C = 0.6f;
    m_Unk280 = 20.0f;
    m_Unk284 = 18.0f;
    m_Unk288 = 28.0f;
    m_Unk28C = 30.0f;
    m_Unk290 = 0.6f;
    m_Unk294 = 20.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 23.0f;
    m_Unk2A8 = 43.0f;
    m_Unk2AC = 43.0f;
    m_Unk2B0 = 0.6f;
    m_Unk2B4 = 17.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 28.0f;
    m_Unk2CC = 48.0f;
    m_Unk2D0 = 48.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 0.5f;
    m_Unk304 = 0.5f;
    m_Unk308 = 1.1f;
    m_Unk30C = 3.0f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 10.0f;
    m_Unk324 = 20.0f;
    m_Unk328 = 3.0f;
    m_Unk32C = 15.0f;
    m_Unk330 = 6.0f;
    m_Unk334 = 1.5f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 7.0f;
    m_Unk358 = 1.2f;
    m_Unk35C = 50.0f;
    m_Unk360 = 0.3f;
    m_Unk364 = 15.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 50.0f;
    m_Unk370 = 0.5f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 90.0f;
    m_Unk38C = 30.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 18.0f;
    m_Unk39C = 23.0f;
    m_Unk3A0 = 28.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 9.0f;
    m_Unk3D8 = 5.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Donkey_NORMAL_STAR::CC_PARAM_VOLLEY_Donkey_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.5f;
    m_Unk25C = 0.5f;
    m_Unk27C = 0.6f;
    m_Unk290 = 0.6f;
    m_Unk2A4 = 23.0f;
    m_Unk2A8 = 43.0f;
    m_Unk2AC = 43.0f;
    m_Unk2B0 = 0.6f;
    m_Unk2C8 = 28.0f;
    m_Unk2CC = 48.0f;
    m_Unk2D0 = 48.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Donkey_NORMAL::CC_PARAM_VOLLEY_Donkey_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.41f;
    m_Unk25C = 0.41f;
    m_Unk27C = 0.48f;
    m_Unk290 = 0.48f;
    m_Unk2A4 = 23.0f;
    m_Unk2A8 = 43.0f;
    m_Unk2AC = 43.0f;
    m_Unk2B0 = 0.48f;
    m_Unk2C8 = 28.0f;
    m_Unk2CC = 48.0f;
    m_Unk2D0 = 48.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Donkey_HARD_STAR::CC_PARAM_VOLLEY_Donkey_HARD_STAR() {}

CC_PARAM_VOLLEY_Donkey_HARD::CC_PARAM_VOLLEY_Donkey_HARD() {}

CC_PARAM_VOLLEY_Donkey_EXPERT_STAR::CC_PARAM_VOLLEY_Donkey_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Donkey_EXPERT::CC_PARAM_VOLLEY_Donkey_EXPERT() {}

} // namespace cc_data_volley_Donkey

// --------------------------------------------------------------------------
// Diddy

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleyDiddy::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Diddy;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Diddy_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Diddy_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Diddy_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Diddy_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Diddy_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Diddy_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Diddy_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Diddy_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Diddy {

CC_PARAM_VOLLEY_Diddy_EASY_STAR::CC_PARAM_VOLLEY_Diddy_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 17.0f;
    m_Unk2A8 = 37.0f;
    m_Unk2AC = 37.0f;
    m_Unk2B0 = 0.32f;
    m_Unk2C8 = 12.0f;
    m_Unk2CC = 20.0f;
    m_Unk2D0 = 25.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Diddy_EASY::CC_PARAM_VOLLEY_Diddy_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.02f;
    m_Unk250 = 13.0f;
    m_Unk254 = 20.0f;
    m_Unk258 = 22.0f;
    m_Unk25C = 0.02f;
    m_Unk270 = 13.0f;
    m_Unk274 = 21.0f;
    m_Unk278 = 23.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 13.0f;
    m_Unk288 = 21.0f;
    m_Unk28C = 23.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 17.0f;
    m_Unk2A8 = 37.0f;
    m_Unk2AC = 37.0f;
    m_Unk2B0 = 0.02f;
    m_Unk2C8 = 10.0f;
    m_Unk2CC = 18.0f;
    m_Unk2D0 = 23.0f;
    m_Unk2D4 = 0.05f;
}

CC_PARAM_VOLLEY_Diddy::CC_PARAM_VOLLEY_Diddy() {
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
    m_Unk54 = 0.5f;
    m_Unk58 = 2.1f;
    m_Unk5C = -0.3f;
    m_Unk60 = 0.9f;
    m_Unk64 = 1.3f;
    m_Unk68 = 2.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = 0.85f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.0f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 15.0f;
    m_Unk90 = 0.5f;
    m_Unk94 = 15.0f;
    m_Unk98 = 10.0f;
    m_Unk9C = 1.2f;
    m_UnkA0 = 50.0f;
    m_UnkA4 = 4.0f;
    m_UnkA8 = 30.0f;
    m_UnkAC = 10.0f;
    m_UnkB0 = 1.5f;
    m_UnkB4 = 45.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.1f;
    m_UnkD8 = 1.8f;
    m_UnkDC = 4.5f;
    m_UnkE0 = 14.0f;
    m_UnkE4 = 8.0f;
    m_UnkE8 = 3.5f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.3f;
    m_Unk10C = 70.0f;
    m_Unk110 = 30.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 2.8f;
    m_Unk138 = 70.0f;
    m_Unk13C = 32.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.0f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 8.0f;
    m_Unk1B8 = 5.0f;
    m_Unk1BC = 3.0f;
    m_Unk1C0 = 9.0f;
    m_Unk1C4 = 12.0f;
    m_Unk1C8 = 12.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 12.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 3.5f;
    m_Unk1E4 = 3.5f;
    m_Unk1E8 = 3.5f;
    m_Unk1EC = 17.0f;
    m_Unk1F0 = 17.5f;
    m_Unk1F4 = 17.0f;
    m_Unk1F8 = 7.0f;
    m_Unk1FC = 7.0f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.9f;
    m_Unk208 = 3.0f;
    m_Unk20C = 2.2f;
    m_Unk210 = 2.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.3f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 13.0f;
    m_Unk254 = 20.0f;
    m_Unk258 = 22.0f;
    m_Unk25C = 0.3f;
    m_Unk260 = 29.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 13.0f;
    m_Unk274 = 21.0f;
    m_Unk278 = 23.0f;
    m_Unk27C = 0.4f;
    m_Unk280 = 25.0f;
    m_Unk284 = 13.0f;
    m_Unk288 = 21.0f;
    m_Unk28C = 23.0f;
    m_Unk290 = 0.4f;
    m_Unk294 = 25.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 17.0f;
    m_Unk2A8 = 37.0f;
    m_Unk2AC = 37.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2B4 = 20.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 28.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = -0.05f;
    m_Unk2F4 = 2.5f;
    m_Unk2F8 = 2.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 0.5f;
    m_Unk304 = 0.5f;
    m_Unk308 = 0.9f;
    m_Unk30C = 3.0f;
    m_Unk310 = 2.2f;
    m_Unk314 = 2.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 10.0f;
    m_Unk324 = 8.0f;
    m_Unk328 = 0.5f;
    m_Unk32C = 20.0f;
    m_Unk330 = 35.0f;
    m_Unk334 = 1.0f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 9.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 40.0f;
    m_Unk360 = 0.2f;
    m_Unk364 = 9.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 55.0f;
    m_Unk370 = 0.7f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 110.0f;
    m_Unk38C = 0.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 14.0f;
    m_Unk39C = 16.0f;
    m_Unk3A0 = 19.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 15.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Diddy_NORMAL_STAR::CC_PARAM_VOLLEY_Diddy_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.3f;
    m_Unk25C = 0.3f;
    m_Unk27C = 0.4f;
    m_Unk290 = 0.4f;
    m_Unk2A4 = 17.0f;
    m_Unk2A8 = 37.0f;
    m_Unk2AC = 37.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 28.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Diddy_NORMAL::CC_PARAM_VOLLEY_Diddy_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 17.0f;
    m_Unk2A8 = 37.0f;
    m_Unk2AC = 37.0f;
    m_Unk2B0 = 0.32f;
    m_Unk2C8 = 12.0f;
    m_Unk2CC = 20.0f;
    m_Unk2D0 = 25.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Diddy_HARD_STAR::CC_PARAM_VOLLEY_Diddy_HARD_STAR() {}

CC_PARAM_VOLLEY_Diddy_HARD::CC_PARAM_VOLLEY_Diddy_HARD() {}

CC_PARAM_VOLLEY_Diddy_EXPERT_STAR::CC_PARAM_VOLLEY_Diddy_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Diddy_EXPERT::CC_PARAM_VOLLEY_Diddy_EXPERT() {}

} // namespace cc_data_volley_Diddy

// --------------------------------------------------------------------------
// Kinopio

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleyKinopio::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Kinopio;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Kinopio_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Kinopio_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Kinopio_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Kinopio_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Kinopio_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Kinopio_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Kinopio_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Kinopio_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Kinopio {

CC_PARAM_VOLLEY_Kinopio_EASY_STAR::CC_PARAM_VOLLEY_Kinopio_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 17.0f;
    m_Unk2A8 = 37.0f;
    m_Unk2AC = 37.0f;
    m_Unk2B0 = 0.32f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Kinopio_EASY::CC_PARAM_VOLLEY_Kinopio_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.02f;
    m_Unk250 = 13.0f;
    m_Unk254 = 20.0f;
    m_Unk258 = 22.0f;
    m_Unk25C = 0.02f;
    m_Unk270 = 13.0f;
    m_Unk274 = 21.0f;
    m_Unk278 = 23.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 13.0f;
    m_Unk288 = 21.0f;
    m_Unk28C = 23.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 17.0f;
    m_Unk2A8 = 37.0f;
    m_Unk2AC = 37.0f;
    m_Unk2B0 = 0.02f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_Kinopio::CC_PARAM_VOLLEY_Kinopio() {
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
    m_Unk54 = 0.6f;
    m_Unk58 = 1.3f;
    m_Unk5C = 0.2f;
    m_Unk60 = 0.9f;
    m_Unk64 = 1.3f;
    m_Unk68 = 2.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = 0.85f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.0f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 12.0f;
    m_Unk90 = 0.5f;
    m_Unk94 = 0.0f;
    m_Unk98 = 0.0f;
    m_Unk9C = 0.0f;
    m_UnkA0 = 50.0f;
    m_UnkA4 = 4.0f;
    m_UnkA8 = 0.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.0f;
    m_UnkB4 = 45.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.1f;
    m_UnkD8 = 1.8f;
    m_UnkDC = 3.5f;
    m_UnkE0 = 11.0f;
    m_UnkE4 = 20.0f;
    m_UnkE8 = 3.5f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.0f;
    m_Unk10C = 70.0f;
    m_Unk110 = 36.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 2.8f;
    m_Unk138 = 70.0f;
    m_Unk13C = 32.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.4f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 8.0f;
    m_Unk1B8 = 5.0f;
    m_Unk1BC = 3.0f;
    m_Unk1C0 = 9.0f;
    m_Unk1C4 = 12.0f;
    m_Unk1C8 = 12.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 12.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 3.5f;
    m_Unk1E4 = 3.5f;
    m_Unk1E8 = 3.5f;
    m_Unk1EC = 17.0f;
    m_Unk1F0 = 17.5f;
    m_Unk1F4 = 17.0f;
    m_Unk1F8 = 8.5f;
    m_Unk1FC = 8.5f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.9f;
    m_Unk208 = 3.0f;
    m_Unk20C = 2.2f;
    m_Unk210 = 2.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.3f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 13.0f;
    m_Unk254 = 20.0f;
    m_Unk258 = 22.0f;
    m_Unk25C = 0.3f;
    m_Unk260 = 29.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 13.0f;
    m_Unk274 = 21.0f;
    m_Unk278 = 23.0f;
    m_Unk27C = 0.4f;
    m_Unk280 = 25.0f;
    m_Unk284 = 13.0f;
    m_Unk288 = 21.0f;
    m_Unk28C = 23.0f;
    m_Unk290 = 0.4f;
    m_Unk294 = 25.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 17.0f;
    m_Unk2A8 = 37.0f;
    m_Unk2AC = 37.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2B4 = 20.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 3.7f;
    m_Unk304 = 3.7f;
    m_Unk308 = 0.9f;
    m_Unk30C = 3.0f;
    m_Unk310 = 2.2f;
    m_Unk314 = 2.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 20.0f;
    m_Unk324 = 8.0f;
    m_Unk328 = 0.5f;
    m_Unk32C = 11.0f;
    m_Unk330 = 23.0f;
    m_Unk334 = 1.0f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 9.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 40.0f;
    m_Unk360 = 0.2f;
    m_Unk364 = 9.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 55.0f;
    m_Unk370 = 0.7f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 90.0f;
    m_Unk38C = 30.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 11.0f;
    m_Unk39C = 13.0f;
    m_Unk3A0 = 16.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 15.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Kinopio_NORMAL_STAR::CC_PARAM_VOLLEY_Kinopio_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.3f;
    m_Unk25C = 0.3f;
    m_Unk27C = 0.4f;
    m_Unk290 = 0.4f;
    m_Unk2A4 = 17.0f;
    m_Unk2A8 = 37.0f;
    m_Unk2AC = 37.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Kinopio_NORMAL::CC_PARAM_VOLLEY_Kinopio_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 17.0f;
    m_Unk2A8 = 37.0f;
    m_Unk2AC = 37.0f;
    m_Unk2B0 = 0.32f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Kinopio_HARD_STAR::CC_PARAM_VOLLEY_Kinopio_HARD_STAR() {}

CC_PARAM_VOLLEY_Kinopio_HARD::CC_PARAM_VOLLEY_Kinopio_HARD() {}

CC_PARAM_VOLLEY_Kinopio_EXPERT_STAR::CC_PARAM_VOLLEY_Kinopio_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Kinopio_EXPERT::CC_PARAM_VOLLEY_Kinopio_EXPERT() {}

} // namespace cc_data_volley_Kinopio

// --------------------------------------------------------------------------
// Koopa

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleyKoopa::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Koopa;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Koopa_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Koopa_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Koopa_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Koopa_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Koopa_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Koopa_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Koopa_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Koopa_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Koopa {

CC_PARAM_VOLLEY_Koopa_EASY_STAR::CC_PARAM_VOLLEY_Koopa_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.41f;
    m_Unk25C = 0.41f;
    m_Unk27C = 0.48f;
    m_Unk290 = 0.48f;
    m_Unk2A4 = 25.0f;
    m_Unk2A8 = 45.0f;
    m_Unk2AC = 45.0f;
    m_Unk2B0 = 0.48f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Koopa_EASY::CC_PARAM_VOLLEY_Koopa_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 16.0f;
    m_Unk234 = 26.0f;
    m_Unk238 = 28.0f;
    m_Unk23C = 0.03f;
    m_Unk250 = 16.0f;
    m_Unk254 = 26.0f;
    m_Unk258 = 28.0f;
    m_Unk25C = 0.03f;
    m_Unk270 = 20.0f;
    m_Unk274 = 30.0f;
    m_Unk278 = 32.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 20.0f;
    m_Unk288 = 30.0f;
    m_Unk28C = 32.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 25.0f;
    m_Unk2A8 = 45.0f;
    m_Unk2AC = 45.0f;
    m_Unk2B0 = 0.03f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_Koopa::CC_PARAM_VOLLEY_Koopa() {
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
    m_Unk54 = 1.5f;
    m_Unk58 = 2.5f;
    m_Unk5C = -0.3f;
    m_Unk60 = 0.9f;
    m_Unk64 = 1.3f;
    m_Unk68 = 2.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = 0.85f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.0f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 12.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 6.0f;
    m_Unk98 = 15.0f;
    m_Unk9C = 0.5f;
    m_UnkA0 = 12.0f;
    m_UnkA4 = 1.0f;
    m_UnkA8 = 0.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.0f;
    m_UnkB4 = 32.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.5f;
    m_UnkD8 = 2.5f;
    m_UnkDC = 2.5f;
    m_UnkE0 = 12.3f;
    m_UnkE4 = 10.0f;
    m_UnkE8 = 2.8f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 16.0f;
    m_Unk108 = 3.8f;
    m_Unk10C = 70.0f;
    m_Unk110 = 30.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 16.0f;
    m_Unk134 = 3.1f;
    m_Unk138 = 70.0f;
    m_Unk13C = 26.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.0f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 11.0f;
    m_Unk1B8 = 6.0f;
    m_Unk1BC = 2.0f;
    m_Unk1C0 = 10.0f;
    m_Unk1C4 = 3.0f;
    m_Unk1C8 = 3.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 1.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 33.0f;
    m_Unk1E0 = 1.5f;
    m_Unk1E4 = 1.5f;
    m_Unk1E8 = 1.5f;
    m_Unk1EC = 6.0f;
    m_Unk1F0 = 6.0f;
    m_Unk1F4 = 6.0f;
    m_Unk1F8 = 7.0f;
    m_Unk1FC = 7.0f;
    m_Unk200 = 0.0f;
    m_Unk204 = 1.0f;
    m_Unk208 = 3.0f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 16.0f;
    m_Unk234 = 26.0f;
    m_Unk238 = 28.0f;
    m_Unk23C = 0.5f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 16.0f;
    m_Unk254 = 26.0f;
    m_Unk258 = 28.0f;
    m_Unk25C = 0.5f;
    m_Unk260 = 22.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 20.0f;
    m_Unk274 = 30.0f;
    m_Unk278 = 32.0f;
    m_Unk27C = 0.6f;
    m_Unk280 = 20.0f;
    m_Unk284 = 20.0f;
    m_Unk288 = 30.0f;
    m_Unk28C = 32.0f;
    m_Unk290 = 0.6f;
    m_Unk294 = 20.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 25.0f;
    m_Unk2A8 = 45.0f;
    m_Unk2AC = 45.0f;
    m_Unk2B0 = 0.6f;
    m_Unk2B4 = 16.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 3.7f;
    m_Unk304 = 3.7f;
    m_Unk308 = 1.1f;
    m_Unk30C = 3.0f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 10.0f;
    m_Unk324 = 8.0f;
    m_Unk328 = 0.5f;
    m_Unk32C = 11.0f;
    m_Unk330 = 6.0f;
    m_Unk334 = 1.0f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 9.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 45.0f;
    m_Unk360 = 0.4f;
    m_Unk364 = 15.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 50.0f;
    m_Unk370 = 0.5f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 7.0f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 60.0f;
    m_Unk38C = 20.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 20.0f;
    m_Unk39C = 30.0f;
    m_Unk3A0 = 35.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 9.0f;
    m_Unk3D8 = 5.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Koopa_NORMAL_STAR::CC_PARAM_VOLLEY_Koopa_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.5f;
    m_Unk25C = 0.5f;
    m_Unk27C = 0.6f;
    m_Unk290 = 0.6f;
    m_Unk2A4 = 25.0f;
    m_Unk2A8 = 45.0f;
    m_Unk2AC = 45.0f;
    m_Unk2B0 = 0.6f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Koopa_NORMAL::CC_PARAM_VOLLEY_Koopa_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.41f;
    m_Unk25C = 0.41f;
    m_Unk27C = 0.48f;
    m_Unk290 = 0.48f;
    m_Unk2A4 = 25.0f;
    m_Unk2A8 = 45.0f;
    m_Unk2AC = 45.0f;
    m_Unk2B0 = 0.48f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Koopa_HARD_STAR::CC_PARAM_VOLLEY_Koopa_HARD_STAR() {}

CC_PARAM_VOLLEY_Koopa_HARD::CC_PARAM_VOLLEY_Koopa_HARD() {}

CC_PARAM_VOLLEY_Koopa_EXPERT_STAR::CC_PARAM_VOLLEY_Koopa_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Koopa_EXPERT::CC_PARAM_VOLLEY_Koopa_EXPERT() {}

} // namespace cc_data_volley_Koopa

// --------------------------------------------------------------------------
// KoopaJr

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleyKoopaJr::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_KoopaJr;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_KoopaJr_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_KoopaJr_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_KoopaJr_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_KoopaJr_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_KoopaJr_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_KoopaJr_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_KoopaJr_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_KoopaJr_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_KoopaJr {

CC_PARAM_VOLLEY_KoopaJr_EASY_STAR::CC_PARAM_VOLLEY_KoopaJr_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 17.0f;
    m_Unk2A8 = 37.0f;
    m_Unk2AC = 37.0f;
    m_Unk2B0 = 0.32f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_KoopaJr_EASY::CC_PARAM_VOLLEY_KoopaJr_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.02f;
    m_Unk250 = 13.0f;
    m_Unk254 = 20.0f;
    m_Unk258 = 22.0f;
    m_Unk25C = 0.02f;
    m_Unk270 = 13.0f;
    m_Unk274 = 21.0f;
    m_Unk278 = 23.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 13.0f;
    m_Unk288 = 21.0f;
    m_Unk28C = 23.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 17.0f;
    m_Unk2A8 = 37.0f;
    m_Unk2AC = 37.0f;
    m_Unk2B0 = 0.02f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_KoopaJr::CC_PARAM_VOLLEY_KoopaJr() {
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
    m_Unk54 = 1.5f;
    m_Unk58 = 2.8f;
    m_Unk5C = -1.4f;
    m_Unk60 = 0.85f;
    m_Unk64 = 0.9f;
    m_Unk68 = 1.8f;
    m_Unk6C = -0.9f;
    m_Unk70 = 0.9f;
    m_Unk74 = 1.0f;
    m_Unk78 = 30.0f;
    m_Unk7C = 0.0f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 12.0f;
    m_Unk90 = 0.5f;
    m_Unk94 = 0.0f;
    m_Unk98 = 0.0f;
    m_Unk9C = 0.0f;
    m_UnkA0 = 50.0f;
    m_UnkA4 = 4.0f;
    m_UnkA8 = 0.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.0f;
    m_UnkB4 = 45.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.1f;
    m_UnkD8 = 1.8f;
    m_UnkDC = 4.0f;
    m_UnkE0 = 12.0f;
    m_UnkE4 = 20.0f;
    m_UnkE8 = 3.5f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.0f;
    m_Unk10C = 70.0f;
    m_Unk110 = 34.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 2.8f;
    m_Unk138 = 70.0f;
    m_Unk13C = 32.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.0f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 8.0f;
    m_Unk1B8 = 5.0f;
    m_Unk1BC = 3.0f;
    m_Unk1C0 = 9.0f;
    m_Unk1C4 = 12.0f;
    m_Unk1C8 = 12.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 12.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 3.5f;
    m_Unk1E4 = 3.5f;
    m_Unk1E8 = 3.5f;
    m_Unk1EC = 17.0f;
    m_Unk1F0 = 17.5f;
    m_Unk1F4 = 17.0f;
    m_Unk1F8 = 8.5f;
    m_Unk1FC = 8.5f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.9f;
    m_Unk208 = 3.0f;
    m_Unk20C = 2.2f;
    m_Unk210 = 2.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.3f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 13.0f;
    m_Unk254 = 20.0f;
    m_Unk258 = 22.0f;
    m_Unk25C = 0.3f;
    m_Unk260 = 29.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 13.0f;
    m_Unk274 = 21.0f;
    m_Unk278 = 23.0f;
    m_Unk27C = 0.4f;
    m_Unk280 = 25.0f;
    m_Unk284 = 13.0f;
    m_Unk288 = 21.0f;
    m_Unk28C = 23.0f;
    m_Unk290 = 0.4f;
    m_Unk294 = 25.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 17.0f;
    m_Unk2A8 = 37.0f;
    m_Unk2AC = 37.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2B4 = 20.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 3.7f;
    m_Unk304 = 3.7f;
    m_Unk308 = 1.0f;
    m_Unk30C = 3.0f;
    m_Unk310 = 2.2f;
    m_Unk314 = 2.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 20.0f;
    m_Unk324 = 8.0f;
    m_Unk328 = 0.5f;
    m_Unk32C = 11.0f;
    m_Unk330 = 23.0f;
    m_Unk334 = 1.0f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 9.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 40.0f;
    m_Unk360 = 0.2f;
    m_Unk364 = 9.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 55.0f;
    m_Unk370 = 0.7f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 80.0f;
    m_Unk38C = 30.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 4.0f;
    m_Unk39C = 6.0f;
    m_Unk3A0 = 7.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 15.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_KoopaJr_NORMAL_STAR::CC_PARAM_VOLLEY_KoopaJr_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.3f;
    m_Unk25C = 0.3f;
    m_Unk27C = 0.4f;
    m_Unk290 = 0.4f;
    m_Unk2A4 = 17.0f;
    m_Unk2A8 = 37.0f;
    m_Unk2AC = 37.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_KoopaJr_NORMAL::CC_PARAM_VOLLEY_KoopaJr_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 17.0f;
    m_Unk2A8 = 37.0f;
    m_Unk2AC = 37.0f;
    m_Unk2B0 = 0.32f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_KoopaJr_HARD_STAR::CC_PARAM_VOLLEY_KoopaJr_HARD_STAR() {}

CC_PARAM_VOLLEY_KoopaJr_HARD::CC_PARAM_VOLLEY_KoopaJr_HARD() {}

CC_PARAM_VOLLEY_KoopaJr_EXPERT_STAR::CC_PARAM_VOLLEY_KoopaJr_EXPERT_STAR() {}

CC_PARAM_VOLLEY_KoopaJr_EXPERT::CC_PARAM_VOLLEY_KoopaJr_EXPERT() {}

} // namespace cc_data_volley_KoopaJr

// --------------------------------------------------------------------------
// Moogle

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleyMoogle::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Moogle;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Moogle_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Moogle_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Moogle_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Moogle_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Moogle_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Moogle_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Moogle_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Moogle_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Moogle {

CC_PARAM_VOLLEY_Moogle_EASY_STAR::CC_PARAM_VOLLEY_Moogle_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Moogle_EASY::CC_PARAM_VOLLEY_Moogle_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.02f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.02f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.03f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_Moogle::CC_PARAM_VOLLEY_Moogle() {
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
    m_Unk54 = 0.8f;
    m_Unk58 = 1.8f;
    m_Unk5C = -0.4f;
    m_Unk60 = 0.9f;
    m_Unk64 = 1.2f;
    m_Unk68 = 2.1f;
    m_Unk6C = -0.4f;
    m_Unk70 = 0.85f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.01f;
    m_Unk80 = 15.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 20.0f;
    m_Unk90 = 0.3f;
    m_Unk94 = 30.0f;
    m_Unk98 = 35.0f;
    m_Unk9C = 1.0f;
    m_UnkA0 = 50.0f;
    m_UnkA4 = 4.0f;
    m_UnkA8 = 50.0f;
    m_UnkAC = 10.0f;
    m_UnkB0 = 1.5f;
    m_UnkB4 = 37.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.0f;
    m_UnkD8 = 2.0f;
    m_UnkDC = 4.0f;
    m_UnkE0 = 11.0f;
    m_UnkE4 = 20.0f;
    m_UnkE8 = 3.5f;
    m_UnkEC = 40.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.0f;
    m_Unk10C = 70.0f;
    m_Unk110 = 36.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 2.8f;
    m_Unk138 = 70.0f;
    m_Unk13C = 32.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.9f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 11.0f;
    m_Unk1B8 = 4.0f;
    m_Unk1BC = 2.0f;
    m_Unk1C0 = 12.0f;
    m_Unk1C4 = 8.0f;
    m_Unk1C8 = 8.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 12.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 3.5f;
    m_Unk1E4 = 3.5f;
    m_Unk1E8 = 3.5f;
    m_Unk1EC = 17.0f;
    m_Unk1F0 = 17.5f;
    m_Unk1F4 = 17.0f;
    m_Unk1F8 = 9.0f;
    m_Unk1FC = 9.0f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.9f;
    m_Unk208 = 3.0f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.3f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.3f;
    m_Unk260 = 27.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.4f;
    m_Unk280 = 23.0f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.4f;
    m_Unk294 = 23.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2B4 = 18.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 3.7f;
    m_Unk304 = 3.7f;
    m_Unk308 = 0.9f;
    m_Unk30C = 3.0f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 10.0f;
    m_Unk324 = 8.0f;
    m_Unk328 = 0.5f;
    m_Unk32C = 15.0f;
    m_Unk330 = 25.0f;
    m_Unk334 = 1.0f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 9.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 40.0f;
    m_Unk360 = 0.2f;
    m_Unk364 = 9.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 55.0f;
    m_Unk370 = 0.7f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 60.0f;
    m_Unk38C = 20.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 14.0f;
    m_Unk39C = 17.0f;
    m_Unk3A0 = 19.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 9.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Moogle_NORMAL_STAR::CC_PARAM_VOLLEY_Moogle_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.3f;
    m_Unk25C = 0.3f;
    m_Unk27C = 0.4f;
    m_Unk290 = 0.4f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Moogle_NORMAL::CC_PARAM_VOLLEY_Moogle_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Moogle_HARD_STAR::CC_PARAM_VOLLEY_Moogle_HARD_STAR() {}

CC_PARAM_VOLLEY_Moogle_HARD::CC_PARAM_VOLLEY_Moogle_HARD() {}

CC_PARAM_VOLLEY_Moogle_EXPERT_STAR::CC_PARAM_VOLLEY_Moogle_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Moogle_EXPERT::CC_PARAM_VOLLEY_Moogle_EXPERT() {}

} // namespace cc_data_volley_Moogle

// --------------------------------------------------------------------------
// Saboten

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleySaboten::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Saboten;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Saboten_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Saboten_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Saboten_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Saboten_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Saboten_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Saboten_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Saboten_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Saboten_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Saboten {

CC_PARAM_VOLLEY_Saboten_EASY_STAR::CC_PARAM_VOLLEY_Saboten_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Saboten_EASY::CC_PARAM_VOLLEY_Saboten_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.02f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.02f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.03f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_Saboten::CC_PARAM_VOLLEY_Saboten() {
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
    m_Unk54 = 0.6f;
    m_Unk58 = 1.3f;
    m_Unk5C = 0.2f;
    m_Unk60 = 0.9f;
    m_Unk64 = 1.3f;
    m_Unk68 = 2.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = 0.85f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.0f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 12.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 10.0f;
    m_Unk98 = 8.0f;
    m_Unk9C = 0.3f;
    m_UnkA0 = 12.0f;
    m_UnkA4 = 1.0f;
    m_UnkA8 = 0.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.3f;
    m_UnkB4 = 37.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.0f;
    m_UnkD8 = 2.0f;
    m_UnkDC = 4.0f;
    m_UnkE0 = 11.0f;
    m_UnkE4 = 20.0f;
    m_UnkE8 = 3.5f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 1;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.0f;
    m_Unk10C = 70.0f;
    m_Unk110 = 36.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 3.0f;
    m_Unk138 = 70.0f;
    m_Unk13C = 32.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.2f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 11.0f;
    m_Unk1B8 = 4.0f;
    m_Unk1BC = 2.0f;
    m_Unk1C0 = 12.0f;
    m_Unk1C4 = 8.0f;
    m_Unk1C8 = 8.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 12.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 3.5f;
    m_Unk1E4 = 3.5f;
    m_Unk1E8 = 3.5f;
    m_Unk1EC = 17.0f;
    m_Unk1F0 = 17.5f;
    m_Unk1F4 = 17.0f;
    m_Unk1F8 = 6.0f;
    m_Unk1FC = 6.0f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.9f;
    m_Unk208 = 3.0f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.3f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.3f;
    m_Unk260 = 27.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.4f;
    m_Unk280 = 23.0f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.4f;
    m_Unk294 = 23.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2B4 = 18.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 3.7f;
    m_Unk304 = 3.7f;
    m_Unk308 = 1.0f;
    m_Unk30C = 3.0f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 10.0f;
    m_Unk324 = 8.0f;
    m_Unk328 = 0.5f;
    m_Unk32C = 10.0f;
    m_Unk330 = 20.0f;
    m_Unk334 = 0.3f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 9.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 40.0f;
    m_Unk360 = 0.2f;
    m_Unk364 = 9.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 55.0f;
    m_Unk370 = 0.7f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 60.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 80.0f;
    m_Unk38C = 0.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 9.0f;
    m_Unk39C = 12.0f;
    m_Unk3A0 = 14.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 9.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Saboten_NORMAL_STAR::CC_PARAM_VOLLEY_Saboten_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.3f;
    m_Unk25C = 0.3f;
    m_Unk27C = 0.4f;
    m_Unk290 = 0.4f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Saboten_NORMAL::CC_PARAM_VOLLEY_Saboten_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Saboten_HARD_STAR::CC_PARAM_VOLLEY_Saboten_HARD_STAR() {}

CC_PARAM_VOLLEY_Saboten_HARD::CC_PARAM_VOLLEY_Saboten_HARD() {}

CC_PARAM_VOLLEY_Saboten_EXPERT_STAR::CC_PARAM_VOLLEY_Saboten_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Saboten_EXPERT::CC_PARAM_VOLLEY_Saboten_EXPERT() {}

} // namespace cc_data_volley_Saboten

// --------------------------------------------------------------------------
// Ninja

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleyNinja::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Ninja;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Ninja_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Ninja_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Ninja_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Ninja_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Ninja_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Ninja_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Ninja_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Ninja_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Ninja {

CC_PARAM_VOLLEY_Ninja_EASY_STAR::CC_PARAM_VOLLEY_Ninja_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 20.0f;
    m_Unk2A8 = 40.0f;
    m_Unk2AC = 40.0f;
    m_Unk2B0 = 0.32f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Ninja_EASY::CC_PARAM_VOLLEY_Ninja_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.02f;
    m_Unk250 = 14.0f;
    m_Unk254 = 22.0f;
    m_Unk258 = 24.0f;
    m_Unk25C = 0.02f;
    m_Unk270 = 15.0f;
    m_Unk274 = 23.0f;
    m_Unk278 = 25.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 15.0f;
    m_Unk288 = 23.0f;
    m_Unk28C = 25.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 20.0f;
    m_Unk2A8 = 40.0f;
    m_Unk2AC = 40.0f;
    m_Unk2B0 = 0.02f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_Ninja::CC_PARAM_VOLLEY_Ninja() {
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
    m_Unk54 = 0.8f;
    m_Unk58 = 1.5f;
    m_Unk5C = 0.0f;
    m_Unk60 = 0.9f;
    m_Unk64 = 1.3f;
    m_Unk68 = 2.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = 0.85f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.0f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 12.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 0.0f;
    m_Unk98 = 0.0f;
    m_Unk9C = 0.0f;
    m_UnkA0 = 12.0f;
    m_UnkA4 = 1.0f;
    m_UnkA8 = 0.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.0f;
    m_UnkB4 = 37.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.0f;
    m_UnkD8 = 2.0f;
    m_UnkDC = 4.0f;
    m_UnkE0 = 11.0f;
    m_UnkE4 = 20.0f;
    m_UnkE8 = 3.5f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.0f;
    m_Unk10C = 70.0f;
    m_Unk110 = 36.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 2.8f;
    m_Unk138 = 70.0f;
    m_Unk13C = 32.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.9f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 11.0f;
    m_Unk1B8 = 4.0f;
    m_Unk1BC = 2.0f;
    m_Unk1C0 = 12.0f;
    m_Unk1C4 = 8.0f;
    m_Unk1C8 = 8.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 12.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 3.5f;
    m_Unk1E4 = 3.5f;
    m_Unk1E8 = 3.5f;
    m_Unk1EC = 17.0f;
    m_Unk1F0 = 17.5f;
    m_Unk1F4 = 17.0f;
    m_Unk1F8 = 8.0f;
    m_Unk1FC = 8.0f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.9f;
    m_Unk208 = 3.0f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.3f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 14.0f;
    m_Unk254 = 22.0f;
    m_Unk258 = 24.0f;
    m_Unk25C = 0.3f;
    m_Unk260 = 25.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 15.0f;
    m_Unk274 = 23.0f;
    m_Unk278 = 25.0f;
    m_Unk27C = 0.4f;
    m_Unk280 = 21.0f;
    m_Unk284 = 15.0f;
    m_Unk288 = 23.0f;
    m_Unk28C = 25.0f;
    m_Unk290 = 0.4f;
    m_Unk294 = 21.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 20.0f;
    m_Unk2A8 = 40.0f;
    m_Unk2AC = 40.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2B4 = 18.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 3.7f;
    m_Unk304 = 3.7f;
    m_Unk308 = 0.9f;
    m_Unk30C = 3.0f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 35.0f;
    m_Unk324 = 7.0f;
    m_Unk328 = 0.5f;
    m_Unk32C = 20.0f;
    m_Unk330 = 30.0f;
    m_Unk334 = 1.0f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 9.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 40.0f;
    m_Unk360 = 0.2f;
    m_Unk364 = 9.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 55.0f;
    m_Unk370 = 0.7f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 80.0f;
    m_Unk38C = 0.0f;
    m_Unk390 = 0.0f;
    m_Unk394 = 5.0f;
    m_Unk398 = 12.0f;
    m_Unk39C = 15.0f;
    m_Unk3A0 = 18.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 9.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Ninja_NORMAL_STAR::CC_PARAM_VOLLEY_Ninja_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.3f;
    m_Unk25C = 0.3f;
    m_Unk27C = 0.4f;
    m_Unk290 = 0.4f;
    m_Unk2A4 = 20.0f;
    m_Unk2A8 = 40.0f;
    m_Unk2AC = 40.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Ninja_NORMAL::CC_PARAM_VOLLEY_Ninja_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 20.0f;
    m_Unk2A8 = 40.0f;
    m_Unk2AC = 40.0f;
    m_Unk2B0 = 0.32f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Ninja_HARD_STAR::CC_PARAM_VOLLEY_Ninja_HARD_STAR() {}

CC_PARAM_VOLLEY_Ninja_HARD::CC_PARAM_VOLLEY_Ninja_HARD() {}

CC_PARAM_VOLLEY_Ninja_EXPERT_STAR::CC_PARAM_VOLLEY_Ninja_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Ninja_EXPERT::CC_PARAM_VOLLEY_Ninja_EXPERT() {}

} // namespace cc_data_volley_Ninja

// --------------------------------------------------------------------------
// Siroma

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleySiroma::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Siroma;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Siroma_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Siroma_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Siroma_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Siroma_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Siroma_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Siroma_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Siroma_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Siroma_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Siroma {

CC_PARAM_VOLLEY_Siroma_EASY_STAR::CC_PARAM_VOLLEY_Siroma_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Siroma_EASY::CC_PARAM_VOLLEY_Siroma_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.02f;
    m_Unk250 = 13.0f;
    m_Unk254 = 20.0f;
    m_Unk258 = 22.0f;
    m_Unk25C = 0.02f;
    m_Unk270 = 13.0f;
    m_Unk274 = 21.0f;
    m_Unk278 = 23.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 13.0f;
    m_Unk288 = 21.0f;
    m_Unk28C = 23.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.03f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_Siroma::CC_PARAM_VOLLEY_Siroma() {
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
    m_Unk54 = 1.4f;
    m_Unk58 = 3.6f;
    m_Unk5C = -1.8f;
    m_Unk60 = -0.05f;
    m_Unk64 = 0.0f;
    m_Unk68 = 0.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = -0.1f;
    m_Unk74 = 1.0f;
    m_Unk78 = 100.0f;
    m_Unk7C = 0.5f;
    m_Unk80 = 8.0f;
    m_Unk84 = -0.05f;
    m_Unk88 = 0.01f;
    m_Unk8C = 12.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 0.0f;
    m_Unk98 = 0.0f;
    m_Unk9C = 0.0f;
    m_UnkA0 = 12.0f;
    m_UnkA4 = 1.0f;
    m_UnkA8 = 0.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.0f;
    m_UnkB4 = 37.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.0f;
    m_UnkD8 = 2.0f;
    m_UnkDC = 3.3f;
    m_UnkE0 = 12.0f;
    m_UnkE4 = 15.0f;
    m_UnkE8 = 3.0f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.0f;
    m_Unk10C = 70.0f;
    m_Unk110 = 32.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 2.8f;
    m_Unk138 = 70.0f;
    m_Unk13C = 32.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.2f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 7.0f;
    m_Unk1B8 = 2.0f;
    m_Unk1BC = 5.0f;
    m_Unk1C0 = 12.0f;
    m_Unk1C4 = 5.0f;
    m_Unk1C8 = 5.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 12.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 1.0f;
    m_Unk1E4 = 1.0f;
    m_Unk1E8 = 1.0f;
    m_Unk1EC = 20.0f;
    m_Unk1F0 = 20.5f;
    m_Unk1F4 = 20.0f;
    m_Unk1F8 = 0.0f;
    m_Unk1FC = 0.0f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.9f;
    m_Unk208 = 5.0f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.3f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 13.0f;
    m_Unk254 = 20.0f;
    m_Unk258 = 22.0f;
    m_Unk25C = 0.3f;
    m_Unk260 = 29.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 13.0f;
    m_Unk274 = 21.0f;
    m_Unk278 = 23.0f;
    m_Unk27C = 0.4f;
    m_Unk280 = 25.0f;
    m_Unk284 = 13.0f;
    m_Unk288 = 21.0f;
    m_Unk28C = 23.0f;
    m_Unk290 = 0.4f;
    m_Unk294 = 25.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2B4 = 18.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 3.0f;
    m_Unk304 = 3.0f;
    m_Unk308 = 0.9f;
    m_Unk30C = 5.0f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 10.0f;
    m_Unk324 = 8.0f;
    m_Unk328 = 0.5f;
    m_Unk32C = 15.0f;
    m_Unk330 = 20.0f;
    m_Unk334 = 0.8f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 12.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 40.0f;
    m_Unk360 = 0.01f;
    m_Unk364 = 7.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 60.0f;
    m_Unk370 = 0.7f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 60.0f;
    m_Unk38C = 0.0f;
    m_Unk390 = 0.0f;
    m_Unk394 = 5.0f;
    m_Unk398 = 50.0f;
    m_Unk39C = 50.0f;
    m_Unk3A0 = 50.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 12.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Siroma_NORMAL_STAR::CC_PARAM_VOLLEY_Siroma_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.3f;
    m_Unk25C = 0.3f;
    m_Unk27C = 0.4f;
    m_Unk290 = 0.4f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Siroma_NORMAL::CC_PARAM_VOLLEY_Siroma_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 8.0f;
    m_Unk2CC = 13.0f;
    m_Unk2D0 = 15.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Siroma_HARD_STAR::CC_PARAM_VOLLEY_Siroma_HARD_STAR() {}

CC_PARAM_VOLLEY_Siroma_HARD::CC_PARAM_VOLLEY_Siroma_HARD() {}

CC_PARAM_VOLLEY_Siroma_EXPERT_STAR::CC_PARAM_VOLLEY_Siroma_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Siroma_EXPERT::CC_PARAM_VOLLEY_Siroma_EXPERT() {}

} // namespace cc_data_volley_Siroma

// --------------------------------------------------------------------------
// Slime

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleySlime::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Slime;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Slime_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Slime_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Slime_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Slime_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Slime_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Slime_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Slime_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Slime_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Slime {

CC_PARAM_VOLLEY_Slime_EASY_STAR::CC_PARAM_VOLLEY_Slime_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Slime_EASY::CC_PARAM_VOLLEY_Slime_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.02f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.02f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.03f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_Slime::CC_PARAM_VOLLEY_Slime() {
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
    m_Unk54 = 0.8f;
    m_Unk58 = 1.5f;
    m_Unk5C = 0.0f;
    m_Unk60 = 0.9f;
    m_Unk64 = 1.3f;
    m_Unk68 = 2.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = 0.85f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.0f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 0.0f;
    m_Unk90 = 0.0f;
    m_Unk94 = 1.0f;
    m_Unk98 = 60.0f;
    m_Unk9C = 0.0f;
    m_UnkA0 = 12.0f;
    m_UnkA4 = 1.0f;
    m_UnkA8 = 0.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.0f;
    m_UnkB4 = 34.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.0f;
    m_UnkD8 = 2.0f;
    m_UnkDC = 2.0f;
    m_UnkE0 = 11.0f;
    m_UnkE4 = 20.0f;
    m_UnkE8 = 3.5f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.0f;
    m_Unk10C = 70.0f;
    m_Unk110 = 36.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 2.8f;
    m_Unk138 = 70.0f;
    m_Unk13C = 32.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 5.0f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 11.0f;
    m_Unk1B8 = 4.0f;
    m_Unk1BC = 2.0f;
    m_Unk1C0 = 12.0f;
    m_Unk1C4 = 8.0f;
    m_Unk1C8 = 8.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 12.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 3.5f;
    m_Unk1E4 = 3.5f;
    m_Unk1E8 = 3.5f;
    m_Unk1EC = 17.0f;
    m_Unk1F0 = 17.5f;
    m_Unk1F4 = 17.0f;
    m_Unk1F8 = 6.0f;
    m_Unk1FC = 6.0f;
    m_Unk200 = 0.0f;
    m_Unk204 = 1.0f;
    m_Unk208 = 3.0f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.3f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.3f;
    m_Unk260 = 27.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.4f;
    m_Unk280 = 23.0f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.4f;
    m_Unk294 = 23.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2B4 = 18.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 3.7f;
    m_Unk304 = 3.7f;
    m_Unk308 = 1.1f;
    m_Unk30C = 3.0f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 70.0f;
    m_Unk324 = 8.0f;
    m_Unk328 = 0.5f;
    m_Unk32C = 10.0f;
    m_Unk330 = 70.0f;
    m_Unk334 = 0.1f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 9.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 40.0f;
    m_Unk360 = 0.2f;
    m_Unk364 = 9.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 55.0f;
    m_Unk370 = 0.7f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 90.0f;
    m_Unk38C = 30.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 16.0f;
    m_Unk39C = 19.0f;
    m_Unk3A0 = 21.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 9.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Slime_NORMAL_STAR::CC_PARAM_VOLLEY_Slime_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.3f;
    m_Unk25C = 0.3f;
    m_Unk27C = 0.4f;
    m_Unk290 = 0.4f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Slime_NORMAL::CC_PARAM_VOLLEY_Slime_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Slime_HARD_STAR::CC_PARAM_VOLLEY_Slime_HARD_STAR() {}

CC_PARAM_VOLLEY_Slime_HARD::CC_PARAM_VOLLEY_Slime_HARD() {}

CC_PARAM_VOLLEY_Slime_EXPERT_STAR::CC_PARAM_VOLLEY_Slime_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Slime_EXPERT::CC_PARAM_VOLLEY_Slime_EXPERT() {}

} // namespace cc_data_volley_Slime

// --------------------------------------------------------------------------
// Kuroma

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleyKuroma::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Kuroma;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Kuroma_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Kuroma_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Kuroma_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Kuroma_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Kuroma_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Kuroma_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Kuroma_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Kuroma_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Kuroma {

CC_PARAM_VOLLEY_Kuroma_EASY_STAR::CC_PARAM_VOLLEY_Kuroma_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Kuroma_EASY::CC_PARAM_VOLLEY_Kuroma_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.02f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.02f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.03f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_Kuroma::CC_PARAM_VOLLEY_Kuroma() {
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
    m_Unk54 = 1.0f;
    m_Unk58 = 5.1f;
    m_Unk5C = -3.0f;
    m_Unk60 = -0.05f;
    m_Unk64 = 0.0f;
    m_Unk68 = 0.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = -0.1f;
    m_Unk74 = 1.0f;
    m_Unk78 = 20.0f;
    m_Unk7C = 0.5f;
    m_Unk80 = 4.0f;
    m_Unk84 = -0.05f;
    m_Unk88 = 0.01f;
    m_Unk8C = 12.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 0.0f;
    m_Unk98 = 0.0f;
    m_Unk9C = 0.0f;
    m_UnkA0 = 12.0f;
    m_UnkA4 = 1.0f;
    m_UnkA8 = 0.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.0f;
    m_UnkB4 = 37.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.0f;
    m_UnkD8 = 2.0f;
    m_UnkDC = 4.5f;
    m_UnkE0 = 12.0f;
    m_UnkE4 = 15.0f;
    m_UnkE8 = 3.0f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.0f;
    m_Unk10C = 70.0f;
    m_Unk110 = 36.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 2.8f;
    m_Unk138 = 70.0f;
    m_Unk13C = 32.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.0f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 11.0f;
    m_Unk1B8 = 4.0f;
    m_Unk1BC = 2.0f;
    m_Unk1C0 = 12.0f;
    m_Unk1C4 = 8.0f;
    m_Unk1C8 = 8.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 12.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 3.5f;
    m_Unk1E4 = 3.5f;
    m_Unk1E8 = 3.5f;
    m_Unk1EC = 17.0f;
    m_Unk1F0 = 17.5f;
    m_Unk1F4 = 17.0f;
    m_Unk1F8 = 7.0f;
    m_Unk1FC = 7.0f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.9f;
    m_Unk208 = 3.0f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.3f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.3f;
    m_Unk260 = 27.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.4f;
    m_Unk280 = 23.0f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.4f;
    m_Unk294 = 23.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2B4 = 18.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 4.0f;
    m_Unk304 = 4.0f;
    m_Unk308 = 1.0f;
    m_Unk30C = 3.0f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 0.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 15.0f;
    m_Unk324 = 10.0f;
    m_Unk328 = 0.5f;
    m_Unk32C = 30.0f;
    m_Unk330 = 30.0f;
    m_Unk334 = 0.3f;
    m_Unk338 = 0.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 9.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 40.0f;
    m_Unk360 = 0.2f;
    m_Unk364 = 9.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 55.0f;
    m_Unk370 = 0.7f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 100.0f;
    m_Unk38C = 0.0f;
    m_Unk390 = 0.0f;
    m_Unk394 = 5.0f;
    m_Unk398 = 25.0f;
    m_Unk39C = 28.0f;
    m_Unk3A0 = 30.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 9.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Kuroma_NORMAL_STAR::CC_PARAM_VOLLEY_Kuroma_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.3f;
    m_Unk25C = 0.3f;
    m_Unk27C = 0.4f;
    m_Unk290 = 0.4f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Kuroma_NORMAL::CC_PARAM_VOLLEY_Kuroma_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Kuroma_HARD_STAR::CC_PARAM_VOLLEY_Kuroma_HARD_STAR() {}

CC_PARAM_VOLLEY_Kuroma_HARD::CC_PARAM_VOLLEY_Kuroma_HARD() {}

CC_PARAM_VOLLEY_Kuroma_EXPERT_STAR::CC_PARAM_VOLLEY_Kuroma_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Kuroma_EXPERT::CC_PARAM_VOLLEY_Kuroma_EXPERT() {}

} // namespace cc_data_volley_Kuroma

// --------------------------------------------------------------------------
// Mii00

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleyMii00::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Mii00;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Mii00_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Mii00_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Mii00_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Mii00_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Mii00_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Mii00_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Mii00_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Mii00_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Mii00 {

CC_PARAM_VOLLEY_Mii00_EASY_STAR::CC_PARAM_VOLLEY_Mii00_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Mii00_EASY::CC_PARAM_VOLLEY_Mii00_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.02f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.02f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.03f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_Mii00::CC_PARAM_VOLLEY_Mii00() {
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
    m_Unk54 = 0.8f;
    m_Unk58 = 1.5f;
    m_Unk5C = 0.0f;
    m_Unk60 = 0.9f;
    m_Unk64 = 1.3f;
    m_Unk68 = 2.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = 0.85f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.0f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 12.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 0.0f;
    m_Unk98 = 0.0f;
    m_Unk9C = 0.0f;
    m_UnkA0 = 35.0f;
    m_UnkA4 = 4.0f;
    m_UnkA8 = 20.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.5f;
    m_UnkB4 = 37.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.0f;
    m_UnkD8 = 2.0f;
    m_UnkDC = 4.0f;
    m_UnkE0 = 11.0f;
    m_UnkE4 = 20.0f;
    m_UnkE8 = 3.5f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.0f;
    m_Unk10C = 70.0f;
    m_Unk110 = 36.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 2.8f;
    m_Unk138 = 70.0f;
    m_Unk13C = 32.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 5.0f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 11.0f;
    m_Unk1B8 = 4.0f;
    m_Unk1BC = 2.0f;
    m_Unk1C0 = 12.0f;
    m_Unk1C4 = 8.0f;
    m_Unk1C8 = 8.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 12.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 3.5f;
    m_Unk1E4 = 3.5f;
    m_Unk1E8 = 3.5f;
    m_Unk1EC = 17.0f;
    m_Unk1F0 = 17.5f;
    m_Unk1F4 = 17.0f;
    m_Unk1F8 = 13.0f;
    m_Unk1FC = 8.0f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.9f;
    m_Unk208 = 3.0f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.3f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.3f;
    m_Unk260 = 27.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.4f;
    m_Unk280 = 23.0f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.4f;
    m_Unk294 = 23.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2B4 = 18.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 3.7f;
    m_Unk304 = 3.7f;
    m_Unk308 = 0.9f;
    m_Unk30C = 3.0f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 15.0f;
    m_Unk324 = 20.0f;
    m_Unk328 = 0.5f;
    m_Unk32C = 10.0f;
    m_Unk330 = 25.0f;
    m_Unk334 = 0.5f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 9.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 40.0f;
    m_Unk360 = 0.2f;
    m_Unk364 = 9.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 55.0f;
    m_Unk370 = 0.7f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 30.0f;
    m_Unk38C = 30.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 25.0f;
    m_Unk39C = 28.0f;
    m_Unk3A0 = 30.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 9.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Mii00_NORMAL_STAR::CC_PARAM_VOLLEY_Mii00_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.3f;
    m_Unk25C = 0.3f;
    m_Unk27C = 0.4f;
    m_Unk290 = 0.4f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Mii00_NORMAL::CC_PARAM_VOLLEY_Mii00_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Mii00_HARD_STAR::CC_PARAM_VOLLEY_Mii00_HARD_STAR() {}

CC_PARAM_VOLLEY_Mii00_HARD::CC_PARAM_VOLLEY_Mii00_HARD() {}

CC_PARAM_VOLLEY_Mii00_EXPERT_STAR::CC_PARAM_VOLLEY_Mii00_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Mii00_EXPERT::CC_PARAM_VOLLEY_Mii00_EXPERT() {}

} // namespace cc_data_volley_Mii00

// --------------------------------------------------------------------------
// Mii01

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *CChrCoreVolleyMii01::NewCcParam_(long nHeap) const {
    using namespace cc_data_volley_Mii01;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Mii01_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Mii01_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Mii01_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Mii01_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_VOLLEY_Mii01_EASY;
            case 1: return new (nHeap) CC_PARAM_VOLLEY_Mii01_NORMAL;
            case 2: return new (nHeap) CC_PARAM_VOLLEY_Mii01_HARD;
            case 3: return new (nHeap) CC_PARAM_VOLLEY_Mii01_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_volley_Mii01 {

CC_PARAM_VOLLEY_Mii01_EASY_STAR::CC_PARAM_VOLLEY_Mii01_EASY_STAR() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Mii01_EASY::CC_PARAM_VOLLEY_Mii01_EASY() {
    m_Unk120 = 0.4f;
    m_Unk14C = 0.5f;
    m_Unk18C = 0.4f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.02f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.02f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.02f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.02f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.03f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.3f;
}

CC_PARAM_VOLLEY_Mii01::CC_PARAM_VOLLEY_Mii01() {
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
    m_Unk54 = 0.8f;
    m_Unk58 = 1.5f;
    m_Unk5C = 0.0f;
    m_Unk60 = 0.9f;
    m_Unk64 = 1.3f;
    m_Unk68 = 2.0f;
    m_Unk6C = -0.3f;
    m_Unk70 = 0.85f;
    m_Unk74 = 1.0f;
    m_Unk78 = 5.0f;
    m_Unk7C = 0.0f;
    m_Unk80 = 0.0f;
    m_Unk84 = 0.9f;
    m_Unk88 = 0.01f;
    m_Unk8C = 12.0f;
    m_Unk90 = 1.0f;
    m_Unk94 = 0.0f;
    m_Unk98 = 0.0f;
    m_Unk9C = 0.0f;
    m_UnkA0 = 35.0f;
    m_UnkA4 = 4.0f;
    m_UnkA8 = 20.0f;
    m_UnkAC = 0.0f;
    m_UnkB0 = 0.5f;
    m_UnkB4 = 37.0f;
    m_UnkB8 = 10.0f;
    m_UnkBC = 4.0f;
    m_UnkC0 = 1.0f;
    m_UnkC4 = 2.0f;
    m_UnkC8 = 0.35f;
    m_UnkCC = 0.45f;
    m_UnkD0 = 0.6f;
    m_UnkD4 = 2.0f;
    m_UnkD8 = 2.0f;
    m_UnkDC = 4.0f;
    m_UnkE0 = 11.0f;
    m_UnkE4 = 15.0f;
    m_UnkE8 = 3.5f;
    m_UnkEC = 25.0f;
    m_UnkF0 = 0.89f;
    m_UnkF4 = 0.0f;
    m_UnkF8 = 0.0f;
    m_UnkFC = 0;
    m_Unk100 = 1;
    m_Unk104 = 12.0f;
    m_Unk108 = 3.0f;
    m_Unk10C = 70.0f;
    m_Unk110 = 36.0f;
    m_Unk114 = 5.0f;
    m_Unk118 = 17.0f;
    m_Unk11C = 23.0f;
    m_Unk120 = 0.6f;
    m_Unk124 = 30.0f;
    m_Unk128 = 4.7f;
    m_Unk12C = 4.7f;
    m_Unk130 = 12.0f;
    m_Unk134 = 2.8f;
    m_Unk138 = 70.0f;
    m_Unk13C = 32.0f;
    m_Unk140 = 5.0f;
    m_Unk144 = 13.0f;
    m_Unk148 = 18.0f;
    m_Unk14C = 0.8f;
    m_Unk150 = 30.0f;
    m_Unk154 = 5.7f;
    m_Unk158 = 5.7f;
    m_Unk15C = 2.0f;
    m_Unk160 = 2.5f;
    m_Unk164 = 55.0f;
    m_Unk168 = 4.9f;
    m_Unk16C = 10.0f;
    m_Unk170 = 4.0f;
    m_Unk174 = 1.3f;
    m_Unk178 = 70.0f;
    m_Unk17C = 40.0f;
    m_Unk180 = 5.0f;
    m_Unk184 = 20.0f;
    m_Unk188 = 27.0f;
    m_Unk18C = 0.6f;
    m_Unk190 = 30.0f;
    m_Unk194 = 1.5f;
    m_Unk198 = 1.5f;
    m_Unk19C = 0.0f;
    m_Unk1A0 = 0.0f;
    m_Unk1A4 = 12.0f;
    m_Unk1A8 = 1.3f;
    m_Unk1AC = 6.0f;
    m_Unk1B0 = 2.0f;
    m_Unk1B4 = 11.0f;
    m_Unk1B8 = 4.0f;
    m_Unk1BC = 2.0f;
    m_Unk1C0 = 12.0f;
    m_Unk1C4 = 8.0f;
    m_Unk1C8 = 8.0f;
    m_Unk1CC = 30.0f;
    m_Unk1D0 = 12.0f;
    m_Unk1D4 = 0.0f;
    m_Unk1D8 = 8.0f;
    m_Unk1DC = 8.0f;
    m_Unk1E0 = 3.5f;
    m_Unk1E4 = 3.5f;
    m_Unk1E8 = 3.5f;
    m_Unk1EC = 17.0f;
    m_Unk1F0 = 17.5f;
    m_Unk1F4 = 17.0f;
    m_Unk1F8 = 13.0f;
    m_Unk1FC = 8.0f;
    m_Unk200 = 0.0f;
    m_Unk204 = 0.9f;
    m_Unk208 = 3.0f;
    m_Unk20C = 1.2f;
    m_Unk210 = 1.0f;
    m_Unk214 = 10.0f;
    m_Unk218 = 8.0f;
    m_Unk21C = 0.5f;
    m_Unk220 = 11.0f;
    m_Unk224 = 6.0f;
    m_Unk228 = 1.0f;
    m_Unk22C = 1.0f;
    m_Unk230 = 12.0f;
    m_Unk234 = 20.0f;
    m_Unk238 = 22.0f;
    m_Unk23C = 0.3f;
    m_Unk240 = 27.0f;
    m_Unk244 = 10.0f;
    m_Unk248 = 0.1f;
    m_Unk24C = 0.8f;
    m_Unk250 = 13.0f;
    m_Unk254 = 21.0f;
    m_Unk258 = 23.0f;
    m_Unk25C = 0.3f;
    m_Unk260 = 27.0f;
    m_Unk264 = 10.0f;
    m_Unk268 = 0.1f;
    m_Unk26C = 0.8f;
    m_Unk270 = 14.0f;
    m_Unk274 = 22.0f;
    m_Unk278 = 24.0f;
    m_Unk27C = 0.4f;
    m_Unk280 = 23.0f;
    m_Unk284 = 14.0f;
    m_Unk288 = 22.0f;
    m_Unk28C = 24.0f;
    m_Unk290 = 0.4f;
    m_Unk294 = 23.0f;
    m_Unk298 = 10.0f;
    m_Unk29C = 0.1f;
    m_Unk2A0 = 0.8f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2B4 = 18.0f;
    m_Unk2B8 = 0;
    m_Unk2BC = 0.0f;
    m_Unk2C0 = 0.1f;
    m_Unk2C4 = 0.2f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
    m_Unk2D8 = 22.0f;
    m_Unk2DC = 0;
    m_Unk2E0 = 0.0f;
    m_Unk2E4 = 0.1f;
    m_Unk2E8 = 0.8f;
    m_Unk2EC = 0;
    m_Unk2F0 = 0.05f;
    m_Unk2F4 = 0.5f;
    m_Unk2F8 = 0.5f;
    m_Unk2FC = 0.4f;
    m_Unk300 = 3.7f;
    m_Unk304 = 3.7f;
    m_Unk308 = 0.9f;
    m_Unk30C = 3.0f;
    m_Unk310 = 1.2f;
    m_Unk314 = 1.0f;
    m_Unk318 = 2.0f;
    m_Unk31C = 0.0f;
    m_Unk320 = 15.0f;
    m_Unk324 = 20.0f;
    m_Unk328 = 0.5f;
    m_Unk32C = 10.0f;
    m_Unk330 = 25.0f;
    m_Unk334 = 0.5f;
    m_Unk338 = 2.0f;
    m_Unk33C = 0.0f;
    m_Unk340 = 0.75f;
    m_Unk344 = 20.0f;
    m_Unk348 = 19.0f;
    m_Unk34C = 0.0f;
    m_Unk350 = 6.7f;
    m_Unk354 = 9.0f;
    m_Unk358 = 1.5f;
    m_Unk35C = 40.0f;
    m_Unk360 = 0.2f;
    m_Unk364 = 9.0f;
    m_Unk368 = 1.5f;
    m_Unk36C = 55.0f;
    m_Unk370 = 0.7f;
    m_Unk374 = 10.0f;
    m_Unk378 = 0.0f;
    m_Unk37C = 5.3f;
    m_Unk380 = 0.0f;
    m_Unk384 = 0.0f;
    m_Unk388 = 30.0f;
    m_Unk38C = 30.0f;
    m_Unk390 = 1.1f;
    m_Unk394 = 5.0f;
    m_Unk398 = 25.0f;
    m_Unk39C = 28.0f;
    m_Unk3A0 = 30.0f;
    m_Unk3A8 = 1;
    m_Unk3C0 = 20.0f;
    m_Unk3C4 = 3.0f;
    m_Unk3C8 = 14.0f;
    m_Unk3CC = 6.0f;
    m_Unk3D0 = 1.0f;
    m_Unk3AC = 4.0f;
    m_Unk3B0 = 4.0f;
    m_Unk3B4 = 0.0f;
    m_Unk3B8 = 4.0f;
    m_Unk3BC = 2.0f;
    m_Unk3D4 = 9.0f;
    m_Unk3D8 = 12.0f;
    m_Unk3DC = 1.0f;
    __init2();
}

CC_PARAM_VOLLEY_Mii01_NORMAL_STAR::CC_PARAM_VOLLEY_Mii01_NORMAL_STAR() {
    m_Unk120 = 0.6f;
    m_Unk14C = 0.8f;
    m_Unk18C = 0.6f;
    m_Unk23C = 0.3f;
    m_Unk25C = 0.3f;
    m_Unk27C = 0.4f;
    m_Unk290 = 0.4f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.5f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.5f;
}

CC_PARAM_VOLLEY_Mii01_NORMAL::CC_PARAM_VOLLEY_Mii01_NORMAL() {
    m_Unk120 = 0.5f;
    m_Unk14C = 0.7f;
    m_Unk18C = 0.5f;
    m_Unk23C = 0.25f;
    m_Unk25C = 0.25f;
    m_Unk27C = 0.32f;
    m_Unk290 = 0.32f;
    m_Unk2A4 = 15.0f;
    m_Unk2A8 = 34.0f;
    m_Unk2AC = 34.0f;
    m_Unk2B0 = 0.4f;
    m_Unk2C8 = 15.0f;
    m_Unk2CC = 23.0f;
    m_Unk2D0 = 26.0f;
    m_Unk2D4 = 0.4f;
}

CC_PARAM_VOLLEY_Mii01_HARD_STAR::CC_PARAM_VOLLEY_Mii01_HARD_STAR() {}

CC_PARAM_VOLLEY_Mii01_HARD::CC_PARAM_VOLLEY_Mii01_HARD() {}

CC_PARAM_VOLLEY_Mii01_EXPERT_STAR::CC_PARAM_VOLLEY_Mii01_EXPERT_STAR() {}

CC_PARAM_VOLLEY_Mii01_EXPERT::CC_PARAM_VOLLEY_Mii01_EXPERT() {}

} // namespace cc_data_volley_Mii01

namespace cc_data_volley_default {

CHR_CORE_VOLLEY::CC_PARAM_VOLLEY m_CcParam;

} // namespace cc_data_volley_default
} // namespace msw
