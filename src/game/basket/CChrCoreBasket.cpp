#include "game/msw/CC_PARAM_BASKET.hpp"
#include "game/msw/CChrCoreBasket.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/FVEC2.hpp"
#include "bcs/New.hpp"

// The basketball characters' parameters (BasketF), the counterpart of
// game/boss/CC_PARAM_BOSS.cpp and game/volley/CC_PARAM_VOLLEY.cpp (the unit
// name is a guess):
//
// - CHR_CORE_BASKET::CC_PARAM_BASKET2, the second parameter block, with one
//   class per character and game level (see CC_PARAM_BASKET.hpp), and the
//   NewCcParam2_ functions that pick one;
// - CC_DATA_BASKET, the collision sizes of the three body size classes, and
//   the table CChrCoreBasket::GetPrmBodySize hands out;
// - the block GetCcParam_ returns (the same for every character);
// - a few CChrCoreBasket members that read the parameters (move speed,
//   defence speed-up, the trick move's slow camera).
//
// Field names are placeholders (see CChrCoreBasket.hpp). The order of the
// assignments is the order of the stores in the original.

// The original file was compiled without optimisation and without inlining,
// like the two parameter files named above: the level constructors,
// CChrCore::IsStarPlayer, CGame::GetGameLevel and every other inline
// function are called instead of inlined (the inline ones emitted after
// their first caller), and the switches over the game level become a
// compare tree. Ignored by other compilers.
// file-level optimization pragma (see notes/decisions.md)
#pragma optimization_level 0
#pragma dont_inline on

namespace {
    // Set by the file's static initialiser and used by nothing in the file
    // (the same angle as in game/basket/CChrCoreBasketNinja.cpp).
    float s_fAngle70 = bcs::DEG_TO_RAD(70.0f);
}

namespace msw {

CHR_CORE_BASKET::CC_PARAM_BASKET2::CC_PARAM_BASKET2() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 6.5f;
    m_fMoveSpeedKind1 = 5.6f;
    m_fMoveSpeedKind2 = 4.7f;
    m_fMoveSpeedKind3 = 5.2f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.0f;
    m_fMoveSpeedKind6 = 11.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.0f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.0f;
    m_fUnk034 = 1.0f;
    m_nDashFrame = 20;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 5;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 25;
    m_fTurnDist = 1.3f;
    m_nTrickMoveFrame = 35;
    m_fTrickMoveUnk54 = 0.15f;
    m_fTrickMoveUnk58 = 0.1f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 2;
    m_nUnk064 = 20;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 9;
    m_nUnk07C = 17;
    m_fUnk080 = 3.2f;
    m_fUnk084 = 0.1f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 38;
    m_fUnk090 = 0.9f;
    m_fUnk094 = 2.4f;
    m_nUnk098 = 9;
    m_nUnk09C = 17;
    m_fUnk0A0 = 3.2f;
    m_fUnk0A4 = 0.1f;
    m_nUnk0A8 = 0;
    m_nShotKind = 0;
    m_fShotSpeed = 10.8f;
    m_fShotCurve = 3.3f;
    m_nUnk0B8 = 22;
    m_fUnk0BC = 2.4f;
    m_nUnk0C0 = 11;
    m_fUnk0C4 = 0.055f;
    m_nUnk0C8 = 20;
    m_fUnk0CC = 0.5f;
    m_nUnk0D0 = 34;
    m_nJustShotKind = 0;
    m_fJustShotSpeed = 10.8f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 35;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 28;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.055f;
    m_nUnk0F8 = 20;
    m_fUnk0FC = 0.5f;
    m_nUnk100 = 30;
    m_nUnk104 = 0;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.0f;
    m_nDunkFrame = 53;
    m_bDunkLoop = 1;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 40;
    m_fUnk120 = 0.9f;
    m_fUnk124 = 1.0f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 85;
    m_bAlleyoopLoop = 1;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.04f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 0;
    m_fLayUpHeight = 0.7f;
    m_nLayUpJumpFrame = 32;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.075f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 38;
    m_nJumpUpperRotBackDeg = 18;
    m_nJumpUpperJumpFrame = 18;
    m_nJumpUpperHoldFrame = 17;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 18;
    m_nJumpUpperWinStart = 0;
    m_nJumpUpperWinEnd = 20;
    m_fJumpUpperHeight = 0.5f;
    m_nJumpUpperCollType = 0;
    m_fUnk1B4 = 0.0f;
    m_fJumpUpperCollHeight = 1.0f;
    m_fUnk1BC = 1.0f;
    m_nJumpUpperSwapCount = 5;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 200;
    m_nSmashCutUnk1CC = 10;
    m_nSmashCutUnk1D0 = 35;
    m_fSmashCutRadius = 2.0f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 38;
    m_nSmashCutRotBackDeg = 18;
    m_nSmashCutJumpFrame = 15;
    m_nSmashCutHoldFrame = 20;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 5;
    m_fSmashCutHeight = 0.45f;
    m_fSmashCutSpeed = 0.03f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsTrickFrame = 0.0f;
    m_fArtsTrickMotCut = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 15.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 2.0f;
    m_nUnk274 = 0;
}

void *CChrCoreBasket::NewCcParam2_(long nHeap) const {
    return new (nHeap) CHR_CORE_BASKET::CC_PARAM_BASKET2;
}

// --------------------------------------------------------------------------
// Mario

void *CChrCoreBasketMario::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Mario;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Mario_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Mario_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Mario_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Mario_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Mario_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Mario_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Mario_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Mario_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Mario {

CC_PARAM_BASKET_Mario_EASY_STAR::CC_PARAM_BASKET_Mario_EASY_STAR() {
    m_fMoveSpeedKind0 = 5.9f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 4.8f;
    m_fMoveSpeedKind3 = 4.7f;
    m_fMoveSpeedKind4 = 5.9f;
    m_fMoveSpeedKind5 = 4.1f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 61;
}

CC_PARAM_BASKET_Mario_EASY::CC_PARAM_BASKET_Mario_EASY() {
    m_fMoveSpeedKind0 = 5.2f;
    m_fMoveSpeedKind1 = 4.8f;
    m_fMoveSpeedKind2 = 4.2f;
    m_fMoveSpeedKind3 = 4.2f;
    m_fMoveSpeedKind4 = 5.2f;
    m_fMoveSpeedKind5 = 3.7f;
    m_fMoveSpeedKind6 = 8.8f;
    m_nDunkFrame = 66;
}

CC_PARAM_BASKET_Mario::CC_PARAM_BASKET_Mario() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 6.5f;
    m_fMoveSpeedKind1 = 5.6f;
    m_fMoveSpeedKind2 = 5.3f;
    m_fMoveSpeedKind3 = 5.2f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 11.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.0f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.0f;
    m_fUnk034 = 1.0f;
    m_nDashFrame = 20;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 5;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 25;
    m_fTurnDist = 1.3f;
    m_nTrickMoveFrame = 33;
    m_fTrickMoveUnk54 = 0.15f;
    m_fTrickMoveUnk58 = 0.12f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 2;
    m_nUnk064 = 20;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 9;
    m_nUnk07C = 17;
    m_fUnk080 = 3.2f;
    m_fUnk084 = 0.1f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 38;
    m_fUnk090 = 0.9f;
    m_fUnk094 = 2.4f;
    m_nUnk0A8 = 0;
    m_nShotKind = 0;
    m_fShotSpeed = 10.8f;
    m_fShotCurve = 3.3f;
    m_nUnk0B8 = 22;
    m_fUnk0BC = 2.4f;
    m_nUnk0C0 = 11;
    m_fUnk0C4 = 0.055f;
    m_nUnk0C8 = 20;
    m_fUnk0CC = 0.5f;
    m_nUnk0D0 = 34;
    m_nJustShotKind = 0;
    m_fJustShotSpeed = 15.0f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 35;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 28;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.07f;
    m_nUnk0F8 = 46;
    m_fUnk0FC = 1.5f;
    m_nUnk100 = 50;
    m_nUnk104 = 5;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.0f;
    m_nDunkFrame = 55;
    m_bDunkLoop = 1;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 40;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 120;
    m_bAlleyoopLoop = 1;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.04f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 0;
    m_fLayUpHeight = 0.7f;
    m_nLayUpJumpFrame = 32;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.075f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 38;
    m_nJumpUpperRotBackDeg = 18;
    m_nJumpUpperJumpFrame = 18;
    m_nJumpUpperHoldFrame = 17;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 18;
    m_nJumpUpperWinStart = 0;
    m_nJumpUpperWinEnd = 35;
    m_fJumpUpperHeight = 0.5f;
    m_nJumpUpperCollType = 1;
    m_fUnk1B4 = 0.5f;
    m_fJumpUpperCollHeight = 0.5f;
    m_fUnk1BC = 1.0f;
    m_nJumpUpperSwapCount = 5;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 10;
    m_nSmashCutUnk1D0 = 35;
    m_fSmashCutRadius = 3.0f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 38;
    m_nSmashCutRotBackDeg = 18;
    m_nSmashCutJumpFrame = 15;
    m_nSmashCutHoldFrame = 20;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 5;
    m_fSmashCutHeight = 0.45f;
    m_fSmashCutSpeed = 0.03f;
    m_nSmashCutJumpFrame = 15;
    m_fSmashCutSpeed = 0.03f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 15.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Mario_NORMAL_STAR::CC_PARAM_BASKET_Mario_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 6.5f;
    m_fMoveSpeedKind1 = 5.6f;
    m_fMoveSpeedKind2 = 5.3f;
    m_fMoveSpeedKind3 = 5.2f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 55;
}

CC_PARAM_BASKET_Mario_NORMAL::CC_PARAM_BASKET_Mario_NORMAL() {
    m_fMoveSpeedKind0 = 5.9f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 4.8f;
    m_fMoveSpeedKind3 = 4.7f;
    m_fMoveSpeedKind4 = 5.9f;
    m_fMoveSpeedKind5 = 4.1f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 61;
}

CC_PARAM_BASKET_Mario_HARD_STAR::CC_PARAM_BASKET_Mario_HARD_STAR() {
    m_fMoveSpeedKind0 = 6.9f;
    m_fMoveSpeedKind1 = 5.9f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.9f;
    m_fMoveSpeedKind5 = 4.9f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 52;
}

CC_PARAM_BASKET_Mario_HARD::CC_PARAM_BASKET_Mario_HARD() {}

CC_PARAM_BASKET_Mario_EXPERT_STAR::CC_PARAM_BASKET_Mario_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 6.9f;
    m_fMoveSpeedKind1 = 5.9f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.9f;
    m_fMoveSpeedKind5 = 4.9f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 52;
}

CC_PARAM_BASKET_Mario_EXPERT::CC_PARAM_BASKET_Mario_EXPERT() {}

} // namespace cc_data_basket_Mario

// --------------------------------------------------------------------------
// Luigi

void *CChrCoreBasketLuigi::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Luigi;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Luigi_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Luigi_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Luigi_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Luigi_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Luigi_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Luigi_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Luigi_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Luigi_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Luigi {

CC_PARAM_BASKET_Luigi_EASY_STAR::CC_PARAM_BASKET_Luigi_EASY_STAR() {
    m_fMoveSpeedKind0 = 6.3f;
    m_fMoveSpeedKind1 = 5.4f;
    m_fMoveSpeedKind2 = 5.0f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 6.1f;
    m_fMoveSpeedKind5 = 4.4f;
    m_fMoveSpeedKind6 = 10.8f;
    m_nDunkFrame = 69;
}

CC_PARAM_BASKET_Luigi_EASY::CC_PARAM_BASKET_Luigi_EASY() {
    m_fMoveSpeedKind0 = 5.6f;
    m_fMoveSpeedKind1 = 5.1f;
    m_fMoveSpeedKind2 = 4.5f;
    m_fMoveSpeedKind3 = 4.4f;
    m_fMoveSpeedKind4 = 5.4f;
    m_fMoveSpeedKind5 = 3.9f;
    m_fMoveSpeedKind6 = 9.6f;
    m_nDunkFrame = 76;
}

CC_PARAM_BASKET_Luigi::CC_PARAM_BASKET_Luigi() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 7.0f;
    m_fMoveSpeedKind1 = 6.0f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.8f;
    m_fMoveSpeedKind5 = 4.9f;
    m_fMoveSpeedKind6 = 12.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.0f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.0f;
    m_fUnk034 = 1.0f;
    m_nDashFrame = 15;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 15;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 30;
    m_fTurnDist = 2.8f;
    m_nTrickMoveFrame = 43;
    m_fTrickMoveUnk54 = 0.13f;
    m_fTrickMoveUnk58 = 0.1f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 4;
    m_nUnk064 = 23;
    m_fUnk068 = 2.9f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 13;
    m_nUnk07C = 17;
    m_fUnk080 = 2.85f;
    m_fUnk084 = 0.1f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 38;
    m_fUnk090 = 0.5f;
    m_fUnk094 = 2.4f;
    m_nUnk0A8 = 0;
    m_nShotKind = 0;
    m_fShotSpeed = 9.8f;
    m_fShotCurve = 3.3f;
    m_nUnk0B8 = 28;
    m_fUnk0BC = 3.0f;
    m_nUnk0C0 = 13;
    m_fUnk0C4 = 0.065f;
    m_nUnk0C8 = 30;
    m_fUnk0CC = 0.5f;
    m_nUnk0D0 = 40;
    m_nJustShotKind = 0;
    m_fJustShotSpeed = 12.5f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 32;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 32;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.0f;
    m_nUnk0F8 = 20;
    m_fUnk0FC = 0.0f;
    m_nUnk100 = 15;
    m_nUnk104 = 60;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.0f;
    m_nDunkFrame = 63;
    m_bDunkLoop = 1;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 50;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 120;
    m_bAlleyoopLoop = 1;
    m_fAlleyoopHeight = 4.2f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.04f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 35;
    m_fLayUpHeight = 0.7f;
    m_nLayUpJumpFrame = 32;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.075f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 0;
    m_nJumpUpperRotBackDeg = 0;
    m_nJumpUpperJumpFrame = 18;
    m_nJumpUpperHoldFrame = 40;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 18;
    m_nJumpUpperWinStart = 0;
    m_nJumpUpperWinEnd = 60;
    m_fJumpUpperHeight = 0.5f;
    m_nJumpUpperCollType = 0;
    m_fUnk1B4 = 0.85f;
    m_fJumpUpperCollHeight = 1.0f;
    m_fUnk1BC = 1.0f;
    m_nJumpUpperSwapCount = 5;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 15;
    m_nSmashCutUnk1D0 = 40;
    m_fSmashCutRadius = 3.0f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 0;
    m_nSmashCutRotBackDeg = 0;
    m_nSmashCutJumpFrame = 30;
    m_nSmashCutHoldFrame = 40;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 5;
    m_fSmashCutHeight = 0.45f;
    m_fSmashCutSpeed = 0.03f;
    m_nSmashCutJumpFrame = 30;
    m_fSmashCutSpeed = 0.03f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 120.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 15.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Luigi_NORMAL_STAR::CC_PARAM_BASKET_Luigi_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 7.0f;
    m_fMoveSpeedKind1 = 6.0f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.8f;
    m_fMoveSpeedKind5 = 4.9f;
    m_fMoveSpeedKind6 = 12.0f;
    m_nDunkFrame = 63;
}

CC_PARAM_BASKET_Luigi_NORMAL::CC_PARAM_BASKET_Luigi_NORMAL() {
    m_fMoveSpeedKind0 = 6.3f;
    m_fMoveSpeedKind1 = 5.4f;
    m_fMoveSpeedKind2 = 5.0f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 6.1f;
    m_fMoveSpeedKind5 = 4.4f;
    m_fMoveSpeedKind6 = 10.8f;
    m_nDunkFrame = 69;
}

CC_PARAM_BASKET_Luigi_HARD_STAR::CC_PARAM_BASKET_Luigi_HARD_STAR() {
    m_fMoveSpeedKind0 = 7.4f;
    m_fMoveSpeedKind1 = 6.4f;
    m_fMoveSpeedKind2 = 5.9f;
    m_fMoveSpeedKind3 = 5.8f;
    m_fMoveSpeedKind4 = 7.2f;
    m_fMoveSpeedKind5 = 5.2f;
    m_fMoveSpeedKind6 = 12.7f;
    m_nDunkFrame = 60;
}

CC_PARAM_BASKET_Luigi_HARD::CC_PARAM_BASKET_Luigi_HARD() {}

CC_PARAM_BASKET_Luigi_EXPERT_STAR::CC_PARAM_BASKET_Luigi_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 7.4f;
    m_fMoveSpeedKind1 = 6.4f;
    m_fMoveSpeedKind2 = 5.9f;
    m_fMoveSpeedKind3 = 5.8f;
    m_fMoveSpeedKind4 = 7.2f;
    m_fMoveSpeedKind5 = 5.2f;
    m_fMoveSpeedKind6 = 12.7f;
    m_nDunkFrame = 60;
}

CC_PARAM_BASKET_Luigi_EXPERT::CC_PARAM_BASKET_Luigi_EXPERT() {}

} // namespace cc_data_basket_Luigi

// --------------------------------------------------------------------------
// Peach

void *CChrCoreBasketPeach::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Peach;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Peach_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Peach_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Peach_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Peach_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Peach_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Peach_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Peach_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Peach_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Peach {

CC_PARAM_BASKET_Peach_EASY_STAR::CC_PARAM_BASKET_Peach_EASY_STAR() {
    m_fMoveSpeedKind0 = 6.1f;
    m_fMoveSpeedKind1 = 5.3f;
    m_fMoveSpeedKind2 = 5.0f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 6.1f;
    m_fMoveSpeedKind5 = 5.2f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 88;
}

CC_PARAM_BASKET_Peach_EASY::CC_PARAM_BASKET_Peach_EASY() {
    m_fMoveSpeedKind0 = 5.4f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 4.5f;
    m_fMoveSpeedKind3 = 4.4f;
    m_fMoveSpeedKind4 = 5.4f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 8.8f;
    m_nDunkFrame = 96;
}

CC_PARAM_BASKET_Peach::CC_PARAM_BASKET_Peach() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 6.8f;
    m_fMoveSpeedKind1 = 5.9f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.8f;
    m_fMoveSpeedKind5 = 5.8f;
    m_fMoveSpeedKind6 = 11.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.0f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.0f;
    m_fUnk034 = 1.0f;
    m_nDashFrame = 20;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 5;
    m_nDashTurnDeg = 80;
    m_nTurnFrame = 20;
    m_fTurnDist = 1.0f;
    m_nTrickMoveFrame = 28;
    m_fTrickMoveUnk54 = 0.18f;
    m_fTrickMoveUnk58 = 0.1f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 2;
    m_nUnk064 = 20;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 1.1f;
    m_fUnk074 = 0.4f;
    m_nUnk078 = 9;
    m_nUnk07C = 17;
    m_fUnk080 = 2.9f;
    m_fUnk084 = 0.1f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 38;
    m_fUnk090 = 0.8f;
    m_fUnk094 = 2.7f;
    m_nUnk0A8 = 0;
    m_nShotKind = 0;
    m_fShotSpeed = 13.0f;
    m_fShotCurve = 2.5f;
    m_nUnk0B8 = 22;
    m_fUnk0BC = 2.4f;
    m_nUnk0C0 = 11;
    m_fUnk0C4 = 0.055f;
    m_nUnk0C8 = 20;
    m_fUnk0CC = 0.5f;
    m_nUnk0D0 = 34;
    m_nJustShotKind = 2;
    m_fJustShotSpeed = 6.0f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 40;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 32;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.02f;
    m_nUnk0F8 = 40;
    m_fUnk0FC = 1.1f;
    m_nUnk100 = 60;
    m_nUnk104 = 30;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.0f;
    m_nDunkFrame = 80;
    m_bDunkLoop = 0;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 60;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 120;
    m_bAlleyoopLoop = 0;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.04f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 40;
    m_fLayUpHeight = 0.7f;
    m_nLayUpJumpFrame = 32;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.05f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 48;
    m_nJumpUpperRotBackDeg = 18;
    m_nJumpUpperJumpFrame = 18;
    m_nJumpUpperHoldFrame = 17;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 18;
    m_nJumpUpperWinStart = 0;
    m_nJumpUpperWinEnd = 35;
    m_fJumpUpperHeight = 0.3f;
    m_nJumpUpperCollType = 1;
    m_fUnk1B4 = 1.0f;
    m_fJumpUpperCollHeight = 0.5f;
    m_fUnk1BC = 1.0f;
    m_nJumpUpperSwapCount = 5;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 10;
    m_nSmashCutUnk1D0 = 35;
    m_fSmashCutRadius = 3.0f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 48;
    m_nSmashCutRotBackDeg = 18;
    m_nSmashCutJumpFrame = 15;
    m_nSmashCutHoldFrame = 20;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 10;
    m_fSmashCutHeight = 0.6f;
    m_fSmashCutSpeed = 0.03f;
    m_nSmashCutJumpFrame = 15;
    m_fSmashCutSpeed = 0.03f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 3.5f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 12.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Peach_NORMAL_STAR::CC_PARAM_BASKET_Peach_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 6.8f;
    m_fMoveSpeedKind1 = 5.9f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.8f;
    m_fMoveSpeedKind5 = 5.8f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 80;
}

CC_PARAM_BASKET_Peach_NORMAL::CC_PARAM_BASKET_Peach_NORMAL() {
    m_fMoveSpeedKind0 = 6.1f;
    m_fMoveSpeedKind1 = 5.3f;
    m_fMoveSpeedKind2 = 5.0f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 6.1f;
    m_fMoveSpeedKind5 = 5.2f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 88;
}

CC_PARAM_BASKET_Peach_HARD_STAR::CC_PARAM_BASKET_Peach_HARD_STAR() {
    m_fMoveSpeedKind0 = 7.2f;
    m_fMoveSpeedKind1 = 6.3f;
    m_fMoveSpeedKind2 = 5.9f;
    m_fMoveSpeedKind3 = 5.8f;
    m_fMoveSpeedKind4 = 7.2f;
    m_fMoveSpeedKind5 = 6.1f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 76;
}

CC_PARAM_BASKET_Peach_HARD::CC_PARAM_BASKET_Peach_HARD() {}

CC_PARAM_BASKET_Peach_EXPERT_STAR::CC_PARAM_BASKET_Peach_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 7.2f;
    m_fMoveSpeedKind1 = 6.3f;
    m_fMoveSpeedKind2 = 5.9f;
    m_fMoveSpeedKind3 = 5.8f;
    m_fMoveSpeedKind4 = 7.2f;
    m_fMoveSpeedKind5 = 6.1f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 76;
}

CC_PARAM_BASKET_Peach_EXPERT::CC_PARAM_BASKET_Peach_EXPERT() {}

} // namespace cc_data_basket_Peach

// --------------------------------------------------------------------------
// Daisy

void *CChrCoreBasketDaisy::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Daisy;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Daisy_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Daisy_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Daisy_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Daisy_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Daisy_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Daisy_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Daisy_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Daisy_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Daisy {

CC_PARAM_BASKET_Daisy_EASY_STAR::CC_PARAM_BASKET_Daisy_EASY_STAR() {
    m_fMoveSpeedKind0 = 6.1f;
    m_fMoveSpeedKind1 = 5.3f;
    m_fMoveSpeedKind2 = 5.0f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 6.1f;
    m_fMoveSpeedKind5 = 5.2f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 77;
}

CC_PARAM_BASKET_Daisy_EASY::CC_PARAM_BASKET_Daisy_EASY() {
    m_fMoveSpeedKind0 = 5.4f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 4.5f;
    m_fMoveSpeedKind3 = 4.4f;
    m_fMoveSpeedKind4 = 5.4f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 8.8f;
    m_nDunkFrame = 84;
}

CC_PARAM_BASKET_Daisy::CC_PARAM_BASKET_Daisy() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 6.8f;
    m_fMoveSpeedKind1 = 5.9f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.8f;
    m_fMoveSpeedKind5 = 5.8f;
    m_fMoveSpeedKind6 = 11.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.0f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.0f;
    m_fUnk034 = 1.0f;
    m_nDashFrame = 20;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 5;
    m_nDashTurnDeg = 80;
    m_nTurnFrame = 28;
    m_fTurnDist = 1.5f;
    m_nTrickMoveFrame = 25;
    m_fTrickMoveUnk54 = 0.2f;
    m_fTrickMoveUnk58 = 0.1f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 2;
    m_nUnk064 = 20;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.4f;
    m_nUnk078 = 16;
    m_nUnk07C = 16;
    m_fUnk080 = 2.9f;
    m_fUnk084 = 0.08f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 38;
    m_fUnk090 = 1.2f;
    m_fUnk094 = 2.4f;
    m_nUnk0A8 = 30;
    m_nShotKind = 0;
    m_fShotSpeed = 12.5f;
    m_fShotCurve = 2.5f;
    m_nUnk0B8 = 22;
    m_fUnk0BC = 2.4f;
    m_nUnk0C0 = 11;
    m_fUnk0C4 = 0.055f;
    m_nUnk0C8 = 20;
    m_fUnk0CC = 0.5f;
    m_nUnk0D0 = 34;
    m_nJustShotKind = 2;
    m_fJustShotSpeed = 6.2f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 37;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 30;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.055f;
    m_nUnk0F8 = 25;
    m_fUnk0FC = 0.3f;
    m_nUnk100 = 30;
    m_nUnk104 = 56;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.0f;
    m_nDunkFrame = 70;
    m_bDunkLoop = 1;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 50;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 120;
    m_bAlleyoopLoop = 1;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.04f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 30;
    m_fLayUpHeight = 0.7f;
    m_nLayUpJumpFrame = 32;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.075f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 30;
    m_nJumpUpperRotBackDeg = 0;
    m_nJumpUpperJumpFrame = 15;
    m_nJumpUpperHoldFrame = 20;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 20;
    m_nJumpUpperWinStart = 0;
    m_nJumpUpperWinEnd = 35;
    m_fJumpUpperHeight = 0.2f;
    m_nJumpUpperCollType = 1;
    m_fUnk1B4 = 0.6f;
    m_fJumpUpperCollHeight = 0.8f;
    m_fUnk1BC = 1.3f;
    m_nJumpUpperSwapCount = 5;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 10;
    m_nSmashCutUnk1D0 = 35;
    m_fSmashCutRadius = 3.0f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 25;
    m_nSmashCutRotBackDeg = 0;
    m_nSmashCutJumpFrame = 5;
    m_nSmashCutHoldFrame = 20;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 6;
    m_fSmashCutHeight = 0.6f;
    m_fSmashCutSpeed = 0.03f;
    m_nSmashCutJumpFrame = 5;
    m_fSmashCutSpeed = 0.03f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 8.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Daisy_NORMAL_STAR::CC_PARAM_BASKET_Daisy_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 6.8f;
    m_fMoveSpeedKind1 = 5.9f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.8f;
    m_fMoveSpeedKind5 = 5.8f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 70;
}

CC_PARAM_BASKET_Daisy_NORMAL::CC_PARAM_BASKET_Daisy_NORMAL() {
    m_fMoveSpeedKind0 = 6.1f;
    m_fMoveSpeedKind1 = 5.3f;
    m_fMoveSpeedKind2 = 5.0f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 6.1f;
    m_fMoveSpeedKind5 = 5.2f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 77;
}

CC_PARAM_BASKET_Daisy_HARD_STAR::CC_PARAM_BASKET_Daisy_HARD_STAR() {
    m_fMoveSpeedKind0 = 7.2f;
    m_fMoveSpeedKind1 = 6.3f;
    m_fMoveSpeedKind2 = 5.9f;
    m_fMoveSpeedKind3 = 5.8f;
    m_fMoveSpeedKind4 = 7.2f;
    m_fMoveSpeedKind5 = 6.1f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 67;
}

CC_PARAM_BASKET_Daisy_HARD::CC_PARAM_BASKET_Daisy_HARD() {}

CC_PARAM_BASKET_Daisy_EXPERT_STAR::CC_PARAM_BASKET_Daisy_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 7.2f;
    m_fMoveSpeedKind1 = 6.3f;
    m_fMoveSpeedKind2 = 5.9f;
    m_fMoveSpeedKind3 = 5.8f;
    m_fMoveSpeedKind4 = 7.2f;
    m_fMoveSpeedKind5 = 6.1f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 67;
}

CC_PARAM_BASKET_Daisy_EXPERT::CC_PARAM_BASKET_Daisy_EXPERT() {}

} // namespace cc_data_basket_Daisy

// --------------------------------------------------------------------------
// Yoshi

void *CChrCoreBasketYoshi::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Yoshi;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Yoshi_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Yoshi_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Yoshi_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Yoshi_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Yoshi_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Yoshi_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Yoshi_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Yoshi_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Yoshi {

CC_PARAM_BASKET_Yoshi_EASY_STAR::CC_PARAM_BASKET_Yoshi_EASY_STAR() {
    m_fMoveSpeedKind0 = 5.9f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 4.8f;
    m_fMoveSpeedKind3 = 4.7f;
    m_fMoveSpeedKind4 = 5.9f;
    m_fMoveSpeedKind5 = 4.1f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 88;
}

CC_PARAM_BASKET_Yoshi_EASY::CC_PARAM_BASKET_Yoshi_EASY() {
    m_fMoveSpeedKind0 = 5.2f;
    m_fMoveSpeedKind1 = 4.8f;
    m_fMoveSpeedKind2 = 4.2f;
    m_fMoveSpeedKind3 = 4.2f;
    m_fMoveSpeedKind4 = 5.2f;
    m_fMoveSpeedKind5 = 3.7f;
    m_fMoveSpeedKind6 = 8.8f;
    m_nDunkFrame = 96;
}

CC_PARAM_BASKET_Yoshi::CC_PARAM_BASKET_Yoshi() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 6.5f;
    m_fMoveSpeedKind1 = 5.6f;
    m_fMoveSpeedKind2 = 5.3f;
    m_fMoveSpeedKind3 = 5.2f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 11.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.0f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.0f;
    m_fUnk034 = 1.0f;
    m_nDashFrame = 20;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 10;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 25;
    m_fTurnDist = 1.3f;
    m_nTrickMoveFrame = 35;
    m_fTrickMoveUnk54 = 0.16f;
    m_fTrickMoveUnk58 = 0.1f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 6;
    m_nUnk064 = 24;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 18;
    m_nUnk07C = 23;
    m_fUnk080 = 2.7f;
    m_fUnk084 = 0.07f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 52;
    m_fUnk090 = 0.9f;
    m_fUnk094 = 2.4f;
    m_nUnk0A8 = 0;
    m_nShotKind = 0;
    m_fShotSpeed = 10.8f;
    m_fShotCurve = 3.3f;
    m_nUnk0B8 = 26;
    m_fUnk0BC = 2.6f;
    m_nUnk0C0 = 11;
    m_fUnk0C4 = 0.055f;
    m_nUnk0C8 = 20;
    m_fUnk0CC = 0.5f;
    m_nUnk0D0 = 34;
    m_nJustShotKind = 0;
    m_fJustShotSpeed = 15.0f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 35;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 28;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.055f;
    m_nUnk0F8 = 20;
    m_fUnk0FC = 0.5f;
    m_nUnk100 = 30;
    m_nUnk104 = 50;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.0f;
    m_nDunkFrame = 80;
    m_bDunkLoop = 1;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 55;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 120;
    m_bAlleyoopLoop = 1;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.04f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 0;
    m_fLayUpHeight = 0.7f;
    m_nLayUpJumpFrame = 32;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.075f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 38;
    m_nJumpUpperRotBackDeg = 18;
    m_nJumpUpperJumpFrame = 33;
    m_nJumpUpperHoldFrame = 7;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 22;
    m_nJumpUpperWinStart = 3;
    m_nJumpUpperWinEnd = 45;
    m_fJumpUpperHeight = 0.8f;
    m_nJumpUpperCollType = 1;
    m_fUnk1B4 = 0.5f;
    m_fJumpUpperCollHeight = 0.5f;
    m_fUnk1BC = 1.7f;
    m_nJumpUpperSwapCount = 6;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 5;
    m_nSmashCutUnk1D0 = 35;
    m_fSmashCutRadius = 0.5f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 0;
    m_nSmashCutRotBackDeg = 0;
    m_nSmashCutJumpFrame = 0;
    m_nSmashCutHoldFrame = 12;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 0;
    m_fSmashCutHeight = 0.0f;
    m_fSmashCutSpeed = 0.0f;
    m_nSmashCutJumpFrame = 0;
    m_fSmashCutSpeed = 0.0f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 6.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 2.0f;
}

CC_PARAM_BASKET_Yoshi_NORMAL_STAR::CC_PARAM_BASKET_Yoshi_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 6.5f;
    m_fMoveSpeedKind1 = 5.6f;
    m_fMoveSpeedKind2 = 5.3f;
    m_fMoveSpeedKind3 = 5.2f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 80;
}

CC_PARAM_BASKET_Yoshi_NORMAL::CC_PARAM_BASKET_Yoshi_NORMAL() {
    m_fMoveSpeedKind0 = 5.9f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 4.8f;
    m_fMoveSpeedKind3 = 4.7f;
    m_fMoveSpeedKind4 = 5.9f;
    m_fMoveSpeedKind5 = 4.1f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 88;
}

CC_PARAM_BASKET_Yoshi_HARD_STAR::CC_PARAM_BASKET_Yoshi_HARD_STAR() {
    m_fMoveSpeedKind0 = 6.9f;
    m_fMoveSpeedKind1 = 5.9f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.9f;
    m_fMoveSpeedKind5 = 4.9f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 76;
}

CC_PARAM_BASKET_Yoshi_HARD::CC_PARAM_BASKET_Yoshi_HARD() {}

CC_PARAM_BASKET_Yoshi_EXPERT_STAR::CC_PARAM_BASKET_Yoshi_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 6.9f;
    m_fMoveSpeedKind1 = 5.9f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.9f;
    m_fMoveSpeedKind5 = 4.9f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 76;
}

CC_PARAM_BASKET_Yoshi_EXPERT::CC_PARAM_BASKET_Yoshi_EXPERT() {}

} // namespace cc_data_basket_Yoshi

// --------------------------------------------------------------------------
// Wario

void *CChrCoreBasketWario::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Wario;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Wario_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Wario_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Wario_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Wario_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Wario_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Wario_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Wario_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Wario_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Wario {

CC_PARAM_BASKET_Wario_EASY_STAR::CC_PARAM_BASKET_Wario_EASY_STAR() {
    m_fMoveSpeedKind0 = 5.0f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 4.5f;
    m_fMoveSpeedKind3 = 4.5f;
    m_fMoveSpeedKind4 = 5.0f;
    m_fMoveSpeedKind5 = 3.9f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 55;
}

CC_PARAM_BASKET_Wario_EASY::CC_PARAM_BASKET_Wario_EASY() {
    m_fMoveSpeedKind0 = 4.4f;
    m_fMoveSpeedKind1 = 4.7f;
    m_fMoveSpeedKind2 = 4.0f;
    m_fMoveSpeedKind3 = 4.0f;
    m_fMoveSpeedKind4 = 4.4f;
    m_fMoveSpeedKind5 = 3.4f;
    m_fMoveSpeedKind6 = 8.8f;
    m_nDunkFrame = 60;
}

CC_PARAM_BASKET_Wario::CC_PARAM_BASKET_Wario() {
    m_nBodySize = 2;
    m_fMoveSpeedKind0 = 5.5f;
    m_fMoveSpeedKind1 = 5.5f;
    m_fMoveSpeedKind2 = 5.0f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 5.5f;
    m_fMoveSpeedKind5 = 4.3f;
    m_fMoveSpeedKind6 = 11.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.0f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.0f;
    m_fUnk034 = 1.0f;
    m_nDashFrame = 20;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 5;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 25;
    m_fTurnDist = 1.3f;
    m_nTrickMoveFrame = 45;
    m_fTrickMoveUnk54 = 0.11f;
    m_fTrickMoveUnk58 = 0.1f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 2;
    m_nUnk064 = 20;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 12;
    m_nUnk07C = 17;
    m_fUnk080 = 3.0f;
    m_fUnk084 = 0.06f;
    m_nJumpBlockDefStart = 8;
    m_nJumpBlockDefEnd = 35;
    m_fUnk090 = 1.0f;
    m_fUnk094 = 2.4f;
    m_nUnk0A8 = 0;
    m_nShotKind = 0;
    m_fShotSpeed = 8.0f;
    m_fShotCurve = 3.3f;
    m_nUnk0B8 = 22;
    m_fUnk0BC = 2.0f;
    m_nUnk0C0 = 5;
    m_fUnk0C4 = 0.015f;
    m_nUnk0C8 = 40;
    m_fUnk0CC = 0.2f;
    m_nUnk0D0 = 28;
    m_nJustShotKind = 1;
    m_fJustShotSpeed = 20.0f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 30;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 26;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.06f;
    m_nUnk0F8 = 45;
    m_fUnk0FC = 0.8f;
    m_nUnk100 = 30;
    m_nUnk104 = 25;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.0f;
    m_nDunkFrame = 50;
    m_bDunkLoop = 1;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 40;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 120;
    m_bAlleyoopLoop = 1;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.04f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 35;
    m_fLayUpHeight = 0.4f;
    m_nLayUpJumpFrame = 35;
    m_fUnk174 = 0.6f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.15f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 38;
    m_nJumpUpperRotBackDeg = 18;
    m_nJumpUpperJumpFrame = 18;
    m_nJumpUpperHoldFrame = 17;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 18;
    m_nJumpUpperWinStart = 3;
    m_nJumpUpperWinEnd = 38;
    m_fJumpUpperHeight = 0.2f;
    m_nJumpUpperCollType = 1;
    m_fUnk1B4 = -0.1f;
    m_fJumpUpperCollHeight = 2.2f;
    m_fUnk1BC = 1.2f;
    m_nJumpUpperSwapCount = 5;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 0;
    m_nSmashCutUnk1D0 = 55;
    m_fSmashCutRadius = 1.0f;
    m_nUnk1D8 = 28;
    m_nSmashCutRotDeg = 38;
    m_nSmashCutRotBackDeg = 18;
    m_nSmashCutJumpFrame = 60;
    m_nSmashCutHoldFrame = 20;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 5;
    m_fSmashCutHeight = 0.45f;
    m_fSmashCutSpeed = 0.13f;
    m_nSmashCutJumpFrame = 60;
    m_fSmashCutSpeed = 0.13f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 3.5f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 18.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Wario_NORMAL_STAR::CC_PARAM_BASKET_Wario_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 5.5f;
    m_fMoveSpeedKind1 = 5.5f;
    m_fMoveSpeedKind2 = 5.0f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 5.5f;
    m_fMoveSpeedKind5 = 4.3f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 50;
}

CC_PARAM_BASKET_Wario_NORMAL::CC_PARAM_BASKET_Wario_NORMAL() {
    m_fMoveSpeedKind0 = 5.0f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 4.5f;
    m_fMoveSpeedKind3 = 4.5f;
    m_fMoveSpeedKind4 = 5.0f;
    m_fMoveSpeedKind5 = 3.9f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 55;
}

CC_PARAM_BASKET_Wario_HARD_STAR::CC_PARAM_BASKET_Wario_HARD_STAR() {
    m_fMoveSpeedKind0 = 5.8f;
    m_fMoveSpeedKind1 = 5.8f;
    m_fMoveSpeedKind2 = 5.3f;
    m_fMoveSpeedKind3 = 5.3f;
    m_fMoveSpeedKind4 = 5.8f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 50;
}

CC_PARAM_BASKET_Wario_HARD::CC_PARAM_BASKET_Wario_HARD() {}

CC_PARAM_BASKET_Wario_EXPERT_STAR::CC_PARAM_BASKET_Wario_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 5.8f;
    m_fMoveSpeedKind1 = 5.8f;
    m_fMoveSpeedKind2 = 5.3f;
    m_fMoveSpeedKind3 = 5.3f;
    m_fMoveSpeedKind4 = 5.8f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 50;
}

CC_PARAM_BASKET_Wario_EXPERT::CC_PARAM_BASKET_Wario_EXPERT() {}

} // namespace cc_data_basket_Wario

// --------------------------------------------------------------------------
// Waluigi

void *CChrCoreBasketWaluigi::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Waluigi;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Waluigi_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Waluigi_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Waluigi_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Waluigi_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Waluigi_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Waluigi_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Waluigi_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Waluigi_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Waluigi {

CC_PARAM_BASKET_Waluigi_EASY_STAR::CC_PARAM_BASKET_Waluigi_EASY_STAR() {
    m_fMoveSpeedKind0 = 6.0f;
    m_fMoveSpeedKind1 = 5.2f;
    m_fMoveSpeedKind2 = 5.9f;
    m_fMoveSpeedKind3 = 5.9f;
    m_fMoveSpeedKind4 = 5.9f;
    m_fMoveSpeedKind5 = 5.4f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 72;
}

CC_PARAM_BASKET_Waluigi_EASY::CC_PARAM_BASKET_Waluigi_EASY() {
    m_fMoveSpeedKind0 = 5.4f;
    m_fMoveSpeedKind1 = 4.9f;
    m_fMoveSpeedKind2 = 5.2f;
    m_fMoveSpeedKind3 = 5.2f;
    m_fMoveSpeedKind4 = 5.2f;
    m_fMoveSpeedKind5 = 4.8f;
    m_fMoveSpeedKind6 = 8.8f;
    m_nDunkFrame = 78;
}

CC_PARAM_BASKET_Waluigi::CC_PARAM_BASKET_Waluigi() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 6.7f;
    m_fMoveSpeedKind1 = 5.8f;
    m_fMoveSpeedKind2 = 6.5f;
    m_fMoveSpeedKind3 = 6.5f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 6.0f;
    m_fMoveSpeedKind6 = 11.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.0f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.0f;
    m_fUnk034 = 1.0f;
    m_nDashFrame = 20;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 5;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 20;
    m_fTurnDist = 1.0f;
    m_nTrickMoveFrame = 25;
    m_fTrickMoveUnk54 = 0.19f;
    m_fTrickMoveUnk58 = 0.1f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 3;
    m_nUnk064 = 22;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 9;
    m_nUnk07C = 25;
    m_fUnk080 = 2.6f;
    m_fUnk084 = 0.08f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 38;
    m_fUnk090 = 1.3f;
    m_fUnk094 = 2.8f;
    m_nUnk0A8 = 0;
    m_nShotKind = 0;
    m_fShotSpeed = 12.0f;
    m_fShotCurve = 3.0f;
    m_nUnk0B8 = 22;
    m_fUnk0BC = 2.2f;
    m_nUnk0C0 = 15;
    m_fUnk0C4 = 0.055f;
    m_nUnk0C8 = 20;
    m_fUnk0CC = 0.5f;
    m_nUnk0D0 = 34;
    m_nJustShotKind = 2;
    m_fJustShotSpeed = 5.8f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 37;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 30;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.055f;
    m_nUnk0F8 = 35;
    m_fUnk0FC = 0.8f;
    m_nUnk100 = 40;
    m_nUnk104 = 20;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.0f;
    m_nDunkFrame = 65;
    m_bDunkLoop = 1;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 40;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 120;
    m_bAlleyoopLoop = 1;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.06f;
    m_nLayUpRotDeg = 45;
    m_fLayUpMotSpeed = 1.0f;
    m_nLayUpHoldFrame = 20;
    m_fLayUpHeight = 0.7f;
    m_nLayUpJumpFrame = 32;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.075f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 32;
    m_nJumpUpperRotBackDeg = 15;
    m_nJumpUpperJumpFrame = 18;
    m_nJumpUpperHoldFrame = 30;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 50;
    m_nJumpUpperWinStart = 0;
    m_nJumpUpperWinEnd = 50;
    m_fJumpUpperHeight = 0.0f;
    m_nJumpUpperCollType = 1;
    m_fUnk1B4 = 1.5f;
    m_fJumpUpperCollHeight = 0.5f;
    m_fUnk1BC = 1.2f;
    m_nJumpUpperSwapCount = 5;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 10;
    m_nSmashCutUnk1D0 = 35;
    m_fSmashCutRadius = 3.0f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 32;
    m_nSmashCutRotBackDeg = 15;
    m_nSmashCutJumpFrame = 15;
    m_nSmashCutHoldFrame = 25;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 14;
    m_fSmashCutHeight = 1.2f;
    m_fSmashCutSpeed = 0.03f;
    m_nSmashCutJumpFrame = 15;
    m_fSmashCutSpeed = 0.03f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 15.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Waluigi_NORMAL_STAR::CC_PARAM_BASKET_Waluigi_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 6.7f;
    m_fMoveSpeedKind1 = 5.8f;
    m_fMoveSpeedKind2 = 6.5f;
    m_fMoveSpeedKind3 = 6.5f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 6.0f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 65;
}

CC_PARAM_BASKET_Waluigi_NORMAL::CC_PARAM_BASKET_Waluigi_NORMAL() {
    m_fMoveSpeedKind0 = 6.0f;
    m_fMoveSpeedKind1 = 5.2f;
    m_fMoveSpeedKind2 = 5.9f;
    m_fMoveSpeedKind3 = 5.9f;
    m_fMoveSpeedKind4 = 5.9f;
    m_fMoveSpeedKind5 = 5.4f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 72;
}

CC_PARAM_BASKET_Waluigi_HARD_STAR::CC_PARAM_BASKET_Waluigi_HARD_STAR() {
    m_fMoveSpeedKind0 = 7.1f;
    m_fMoveSpeedKind1 = 6.1f;
    m_fMoveSpeedKind2 = 6.9f;
    m_fMoveSpeedKind3 = 6.9f;
    m_fMoveSpeedKind4 = 6.9f;
    m_fMoveSpeedKind5 = 6.4f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 62;
}

CC_PARAM_BASKET_Waluigi_HARD::CC_PARAM_BASKET_Waluigi_HARD() {}

CC_PARAM_BASKET_Waluigi_EXPERT_STAR::CC_PARAM_BASKET_Waluigi_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 7.1f;
    m_fMoveSpeedKind1 = 6.1f;
    m_fMoveSpeedKind2 = 6.9f;
    m_fMoveSpeedKind3 = 6.9f;
    m_fMoveSpeedKind4 = 6.9f;
    m_fMoveSpeedKind5 = 6.4f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 62;
}

CC_PARAM_BASKET_Waluigi_EXPERT::CC_PARAM_BASKET_Waluigi_EXPERT() {}

} // namespace cc_data_basket_Waluigi

// --------------------------------------------------------------------------
// Donkey

void *CChrCoreBasketDonkey::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Donkey;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Donkey_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Donkey_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Donkey_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Donkey_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Donkey_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Donkey_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Donkey_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Donkey_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Donkey {

CC_PARAM_BASKET_Donkey_EASY_STAR::CC_PARAM_BASKET_Donkey_EASY_STAR() {
    m_fMoveSpeedKind0 = 5.0f;
    m_fMoveSpeedKind1 = 4.5f;
    m_fMoveSpeedKind2 = 4.5f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 5.4f;
    m_fMoveSpeedKind5 = 3.9f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 55;
}

CC_PARAM_BASKET_Donkey_EASY::CC_PARAM_BASKET_Donkey_EASY() {
    m_fMoveSpeedKind0 = 4.4f;
    m_fMoveSpeedKind1 = 4.3f;
    m_fMoveSpeedKind2 = 4.0f;
    m_fMoveSpeedKind3 = 4.4f;
    m_fMoveSpeedKind4 = 4.8f;
    m_fMoveSpeedKind5 = 3.4f;
    m_fMoveSpeedKind6 = 8.8f;
    m_nDunkFrame = 60;
}

CC_PARAM_BASKET_Donkey::CC_PARAM_BASKET_Donkey() {
    m_nBodySize = 2;
    m_fMoveSpeedKind0 = 5.5f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 5.0f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.0f;
    m_fMoveSpeedKind5 = 4.3f;
    m_fMoveSpeedKind6 = 11.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.0f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.0f;
    m_fUnk034 = 1.0f;
    m_nDashFrame = 20;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 5;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 35;
    m_fTurnDist = 2.0f;
    m_nTrickMoveFrame = 45;
    m_fTrickMoveUnk54 = 0.11f;
    m_fTrickMoveUnk58 = 0.1f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 0;
    m_nUnk064 = 20;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 6;
    m_nUnk07C = 17;
    m_fUnk080 = 2.5f;
    m_fUnk084 = 0.1f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 38;
    m_fUnk090 = 1.3f;
    m_fUnk094 = 3.3f;
    m_nUnk0A8 = 0;
    m_nShotKind = 0;
    m_fShotSpeed = 8.0f;
    m_fShotCurve = 3.3f;
    m_nUnk0B8 = 22;
    m_fUnk0BC = 2.4f;
    m_nUnk0C0 = 2;
    m_fUnk0C4 = 0.055f;
    m_nUnk0C8 = 20;
    m_fUnk0CC = 0.5f;
    m_nUnk0D0 = 34;
    m_nJustShotKind = 1;
    m_fJustShotSpeed = 20.0f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 34;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 27;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.07f;
    m_nUnk0F8 = 30;
    m_fUnk0FC = 1.0f;
    m_nUnk100 = 30;
    m_nUnk104 = 20;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 3.8f;
    m_nDunkFrame = 50;
    m_bDunkLoop = 1;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 40;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 120;
    m_bAlleyoopLoop = 1;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.04f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 20;
    m_fLayUpHeight = 0.7f;
    m_nLayUpJumpFrame = 35;
    m_fUnk174 = 0.8f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.17f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 38;
    m_nJumpUpperRotBackDeg = 18;
    m_nJumpUpperJumpFrame = 18;
    m_nJumpUpperHoldFrame = 17;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 18;
    m_nJumpUpperWinStart = 0;
    m_nJumpUpperWinEnd = 35;
    m_fJumpUpperHeight = 0.4f;
    m_nJumpUpperCollType = 1;
    m_fUnk1B4 = 1.0f;
    m_fJumpUpperCollHeight = 1.0f;
    m_fUnk1BC = 1.6f;
    m_nJumpUpperSwapCount = 5;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 10;
    m_nSmashCutUnk1D0 = 35;
    m_fSmashCutRadius = 3.0f;
    m_nUnk1D8 = 36;
    m_nSmashCutRotDeg = 30;
    m_nSmashCutRotBackDeg = 18;
    m_nSmashCutJumpFrame = 5;
    m_nSmashCutHoldFrame = 40;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 5;
    m_fSmashCutHeight = 0.1f;
    m_fSmashCutSpeed = 0.03f;
    m_nSmashCutJumpFrame = 5;
    m_fSmashCutSpeed = 0.03f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 15.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Donkey_NORMAL_STAR::CC_PARAM_BASKET_Donkey_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 5.5f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 5.0f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.0f;
    m_fMoveSpeedKind5 = 4.3f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 50;
}

CC_PARAM_BASKET_Donkey_NORMAL::CC_PARAM_BASKET_Donkey_NORMAL() {
    m_fMoveSpeedKind0 = 5.0f;
    m_fMoveSpeedKind1 = 4.5f;
    m_fMoveSpeedKind2 = 4.5f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 5.4f;
    m_fMoveSpeedKind5 = 3.9f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 55;
}

CC_PARAM_BASKET_Donkey_HARD_STAR::CC_PARAM_BASKET_Donkey_HARD_STAR() {
    m_fMoveSpeedKind0 = 5.8f;
    m_fMoveSpeedKind1 = 5.3f;
    m_fMoveSpeedKind2 = 5.3f;
    m_fMoveSpeedKind3 = 5.8f;
    m_fMoveSpeedKind4 = 6.4f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 50;
}

CC_PARAM_BASKET_Donkey_HARD::CC_PARAM_BASKET_Donkey_HARD() {}

CC_PARAM_BASKET_Donkey_EXPERT_STAR::CC_PARAM_BASKET_Donkey_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 5.8f;
    m_fMoveSpeedKind1 = 5.3f;
    m_fMoveSpeedKind2 = 5.3f;
    m_fMoveSpeedKind3 = 5.8f;
    m_fMoveSpeedKind4 = 6.4f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 50;
}

CC_PARAM_BASKET_Donkey_EXPERT::CC_PARAM_BASKET_Donkey_EXPERT() {}

} // namespace cc_data_basket_Donkey

// --------------------------------------------------------------------------
// Diddy

void *CChrCoreBasketDiddy::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Diddy;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Diddy_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Diddy_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Diddy_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Diddy_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Diddy_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Diddy_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Diddy_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Diddy_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Diddy {

CC_PARAM_BASKET_Diddy_EASY_STAR::CC_PARAM_BASKET_Diddy_EASY_STAR() {
    m_fMoveSpeedKind0 = 6.8f;
    m_fMoveSpeedKind1 = 5.6f;
    m_fMoveSpeedKind2 = 5.5f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 5.0f;
    m_fMoveSpeedKind5 = 4.7f;
    m_fMoveSpeedKind6 = 10.8f;
    m_nDunkFrame = 83;
}

CC_PARAM_BASKET_Diddy_EASY::CC_PARAM_BASKET_Diddy_EASY() {
    m_fMoveSpeedKind0 = 6.0f;
    m_fMoveSpeedKind1 = 5.3f;
    m_fMoveSpeedKind2 = 4.9f;
    m_fMoveSpeedKind3 = 4.4f;
    m_fMoveSpeedKind4 = 4.4f;
    m_fMoveSpeedKind5 = 4.2f;
    m_fMoveSpeedKind6 = 9.6f;
    m_nDunkFrame = 90;
}

CC_PARAM_BASKET_Diddy::CC_PARAM_BASKET_Diddy() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 7.5f;
    m_fMoveSpeedKind1 = 6.2f;
    m_fMoveSpeedKind2 = 6.1f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 5.5f;
    m_fMoveSpeedKind5 = 5.2f;
    m_fMoveSpeedKind6 = 12.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.0f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.0f;
    m_fUnk034 = 1.0f;
    m_nDashFrame = 15;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 3;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 22;
    m_fTurnDist = 1.0f;
    m_nTrickMoveFrame = 26;
    m_fTrickMoveUnk54 = 0.17f;
    m_fTrickMoveUnk58 = 0.13f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 2;
    m_nUnk064 = 20;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 12;
    m_nUnk07C = 15;
    m_fUnk080 = 3.2f;
    m_fUnk084 = 0.12f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 38;
    m_fUnk090 = 0.9f;
    m_fUnk094 = 2.0f;
    m_nUnk0A8 = 30;
    m_nShotKind = 0;
    m_fShotSpeed = 8.0f;
    m_fShotCurve = 3.3f;
    m_nUnk0B8 = 22;
    m_fUnk0BC = 2.5f;
    m_nUnk0C0 = 3;
    m_fUnk0C4 = 0.042f;
    m_nUnk0C8 = 30;
    m_fUnk0CC = 0.3f;
    m_nUnk0D0 = 30;
    m_nJustShotKind = 0;
    m_fJustShotSpeed = 15.0f;
    m_nUnk0DC = 17;
    m_nUnk0E0 = 29;
    m_nUnk0E4 = 17;
    m_nUnk0E8 = 23;
    m_nUnk0EC = 17;
    m_nUnk0F0 = 17;
    m_fUnk0F4 = 0.045f;
    m_nUnk0F8 = 20;
    m_fUnk0FC = 1.5f;
    m_nUnk100 = 42;
    m_nUnk104 = 10;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.2f;
    m_nDunkFrame = 75;
    m_bDunkLoop = 1;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 48;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 120;
    m_bAlleyoopLoop = 1;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.04f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 7;
    m_fLayUpHeight = 0.7f;
    m_nLayUpJumpFrame = 25;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.075f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 0;
    m_nJumpUpperRotBackDeg = 0;
    m_nJumpUpperJumpFrame = 18;
    m_nJumpUpperHoldFrame = 17;
    m_nJumpUpperTurnFrame = 10;
    m_nJumpUpperFallFrame = 12;
    m_nJumpUpperWinStart = 7;
    m_nJumpUpperWinEnd = 30;
    m_fJumpUpperHeight = 0.5f;
    m_nJumpUpperCollType = 0;
    m_fUnk1B4 = 0.6f;
    m_fJumpUpperCollHeight = 1.0f;
    m_fUnk1BC = 0.8f;
    m_nJumpUpperSwapCount = 5;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 10;
    m_nSmashCutUnk1D0 = 35;
    m_fSmashCutRadius = 0.7f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 38;
    m_nSmashCutRotBackDeg = 18;
    m_nSmashCutJumpFrame = 41;
    m_nSmashCutHoldFrame = 20;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 5;
    m_fSmashCutHeight = 0.45f;
    m_fSmashCutSpeed = 0.13f;
    m_nSmashCutJumpFrame = 41;
    m_fSmashCutSpeed = 0.13f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 4.0f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 9.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Diddy_NORMAL_STAR::CC_PARAM_BASKET_Diddy_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 7.5f;
    m_fMoveSpeedKind1 = 6.2f;
    m_fMoveSpeedKind2 = 6.1f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 5.5f;
    m_fMoveSpeedKind5 = 5.2f;
    m_fMoveSpeedKind6 = 12.0f;
    m_nDunkFrame = 75;
}

CC_PARAM_BASKET_Diddy_NORMAL::CC_PARAM_BASKET_Diddy_NORMAL() {
    m_fMoveSpeedKind0 = 6.8f;
    m_fMoveSpeedKind1 = 5.6f;
    m_fMoveSpeedKind2 = 5.5f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 5.0f;
    m_fMoveSpeedKind5 = 4.7f;
    m_fMoveSpeedKind6 = 10.8f;
    m_nDunkFrame = 83;
}

CC_PARAM_BASKET_Diddy_HARD_STAR::CC_PARAM_BASKET_Diddy_HARD_STAR() {
    m_fMoveSpeedKind0 = 7.5f;
    m_fMoveSpeedKind1 = 6.2f;
    m_fMoveSpeedKind2 = 6.1f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 5.5f;
    m_fMoveSpeedKind5 = 5.2f;
    m_fMoveSpeedKind6 = 12.0f;
    m_nDunkFrame = 71;
}

CC_PARAM_BASKET_Diddy_HARD::CC_PARAM_BASKET_Diddy_HARD() {}

CC_PARAM_BASKET_Diddy_EXPERT_STAR::CC_PARAM_BASKET_Diddy_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 7.5f;
    m_fMoveSpeedKind1 = 6.2f;
    m_fMoveSpeedKind2 = 6.1f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 5.5f;
    m_fMoveSpeedKind5 = 5.2f;
    m_fMoveSpeedKind6 = 12.0f;
    m_nDunkFrame = 71;
}

CC_PARAM_BASKET_Diddy_EXPERT::CC_PARAM_BASKET_Diddy_EXPERT() {}

} // namespace cc_data_basket_Diddy

// --------------------------------------------------------------------------
// Koopa

void *CChrCoreBasketKoopa::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Koopa;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Koopa_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Koopa_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Koopa_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Koopa_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Koopa_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Koopa_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Koopa_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Koopa_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Koopa {

CC_PARAM_BASKET_Koopa_EASY_STAR::CC_PARAM_BASKET_Koopa_EASY_STAR() {
    m_fMoveSpeedKind0 = 4.8f;
    m_fMoveSpeedKind1 = 4.5f;
    m_fMoveSpeedKind2 = 4.1f;
    m_fMoveSpeedKind3 = 3.9f;
    m_fMoveSpeedKind4 = 4.5f;
    m_fMoveSpeedKind5 = 3.6f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 55;
}

CC_PARAM_BASKET_Koopa_EASY::CC_PARAM_BASKET_Koopa_EASY() {
    m_fMoveSpeedKind0 = 4.2f;
    m_fMoveSpeedKind1 = 4.3f;
    m_fMoveSpeedKind2 = 3.6f;
    m_fMoveSpeedKind3 = 3.4f;
    m_fMoveSpeedKind4 = 4.0f;
    m_fMoveSpeedKind5 = 3.2f;
    m_fMoveSpeedKind6 = 8.8f;
    m_nDunkFrame = 60;
}

CC_PARAM_BASKET_Koopa::CC_PARAM_BASKET_Koopa() {
    m_nBodySize = 2;
    m_fMoveSpeedKind0 = 5.3f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 4.5f;
    m_fMoveSpeedKind3 = 4.3f;
    m_fMoveSpeedKind4 = 5.0f;
    m_fMoveSpeedKind5 = 4.0f;
    m_fMoveSpeedKind6 = 11.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.0f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.0f;
    m_fUnk034 = 1.0f;
    m_nDashFrame = 20;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 10;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 35;
    m_fTurnDist = 2.3f;
    m_nTrickMoveFrame = 50;
    m_fTrickMoveUnk54 = 0.09f;
    m_fTrickMoveUnk58 = 0.1f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 13;
    m_nUnk064 = 16;
    m_fUnk068 = 2.3f;
    m_fUnk06C = 0.03f;
    m_fUnk070 = 1.15f;
    m_fUnk074 = 0.6f;
    m_nUnk078 = 13;
    m_nUnk07C = 16;
    m_fUnk080 = 2.9f;
    m_fUnk084 = 0.05f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 47;
    m_fUnk090 = 1.3f;
    m_fUnk094 = 3.0f;
    m_nUnk0A8 = 0;
    m_nShotKind = 0;
    m_fShotSpeed = 9.0f;
    m_fShotCurve = 3.3f;
    m_nUnk0B8 = 32;
    m_fUnk0BC = 2.4f;
    m_nUnk0C0 = 11;
    m_fUnk0C4 = 0.02f;
    m_nUnk0C8 = 20;
    m_fUnk0CC = 0.25f;
    m_nUnk0D0 = 30;
    m_nJustShotKind = 1;
    m_fJustShotSpeed = 23.0f;
    m_nUnk0DC = 33;
    m_nUnk0E0 = 46;
    m_nUnk0E4 = 33;
    m_nUnk0E8 = 39;
    m_nUnk0EC = 33;
    m_nUnk0F0 = 33;
    m_fUnk0F4 = 0.2f;
    m_nUnk0F8 = 4;
    m_fUnk0FC = 1.0f;
    m_nUnk100 = 40;
    m_nUnk104 = 30;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 3.5f;
    m_nDunkFrame = 50;
    m_bDunkLoop = 0;
    m_fReboundDunkHeight = 3.5f;
    m_nReboundDunkFrame = 40;
    m_fUnk120 = 0.6f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 90;
    m_bAlleyoopLoop = 0;
    m_fAlleyoopHeight = 3.5f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.02f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 60;
    m_fLayUpHeight = 0.1f;
    m_nLayUpJumpFrame = 15;
    m_fUnk174 = 0.8f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.17f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 40;
    m_nJumpUpperRotBackDeg = 0;
    m_nJumpUpperJumpFrame = 15;
    m_nJumpUpperHoldFrame = 30;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 20;
    m_nJumpUpperWinStart = 8;
    m_nJumpUpperWinEnd = 45;
    m_fJumpUpperHeight = 0.1f;
    m_nJumpUpperCollType = 1;
    m_fUnk1B4 = 0.45f;
    m_fJumpUpperCollHeight = 1.0f;
    m_fUnk1BC = 1.5f;
    m_nJumpUpperSwapCount = 5;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 10;
    m_nSmashCutUnk1D0 = 35;
    m_fSmashCutRadius = 3.0f;
    m_nUnk1D8 = 40;
    m_nSmashCutRotDeg = 40;
    m_nSmashCutRotBackDeg = 0;
    m_nSmashCutJumpFrame = 15;
    m_nSmashCutHoldFrame = 30;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 5;
    m_fSmashCutHeight = 0.4f;
    m_fSmashCutSpeed = 0.03f;
    m_nSmashCutJumpFrame = 15;
    m_fSmashCutSpeed = 0.03f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 3.5f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 4.0f;
    m_fArtsSetbackMax = 2.0f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 15.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Koopa_NORMAL_STAR::CC_PARAM_BASKET_Koopa_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 5.3f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 4.5f;
    m_fMoveSpeedKind3 = 4.3f;
    m_fMoveSpeedKind4 = 5.0f;
    m_fMoveSpeedKind5 = 4.0f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 50;
}

CC_PARAM_BASKET_Koopa_NORMAL::CC_PARAM_BASKET_Koopa_NORMAL() {
    m_fMoveSpeedKind0 = 4.8f;
    m_fMoveSpeedKind1 = 4.5f;
    m_fMoveSpeedKind2 = 4.1f;
    m_fMoveSpeedKind3 = 3.9f;
    m_fMoveSpeedKind4 = 4.5f;
    m_fMoveSpeedKind5 = 3.6f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 55;
}

CC_PARAM_BASKET_Koopa_HARD_STAR::CC_PARAM_BASKET_Koopa_HARD_STAR() {
    m_fMoveSpeedKind0 = 5.6f;
    m_fMoveSpeedKind1 = 5.3f;
    m_fMoveSpeedKind2 = 4.8f;
    m_fMoveSpeedKind3 = 4.6f;
    m_fMoveSpeedKind4 = 5.3f;
    m_fMoveSpeedKind5 = 4.2f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 50;
}

CC_PARAM_BASKET_Koopa_HARD::CC_PARAM_BASKET_Koopa_HARD() {}

CC_PARAM_BASKET_Koopa_EXPERT_STAR::CC_PARAM_BASKET_Koopa_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 5.6f;
    m_fMoveSpeedKind1 = 5.3f;
    m_fMoveSpeedKind2 = 4.8f;
    m_fMoveSpeedKind3 = 4.6f;
    m_fMoveSpeedKind4 = 5.3f;
    m_fMoveSpeedKind5 = 4.2f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 50;
}

CC_PARAM_BASKET_Koopa_EXPERT::CC_PARAM_BASKET_Koopa_EXPERT() {}

} // namespace cc_data_basket_Koopa

// --------------------------------------------------------------------------
// KoopaJr

void *CChrCoreBasketKoopaJr::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_KoopaJr;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_KoopaJr_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_KoopaJr_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_KoopaJr_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_KoopaJr_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_KoopaJr_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_KoopaJr_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_KoopaJr_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_KoopaJr_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_KoopaJr {

CC_PARAM_BASKET_KoopaJr_EASY_STAR::CC_PARAM_BASKET_KoopaJr_EASY_STAR() {
    m_fMoveSpeedKind0 = 6.7f;
    m_fMoveSpeedKind1 = 5.8f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 4.7f;
    m_fMoveSpeedKind4 = 5.6f;
    m_fMoveSpeedKind5 = 4.1f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 64;
}

CC_PARAM_BASKET_KoopaJr_EASY::CC_PARAM_BASKET_KoopaJr_EASY() {
    m_fMoveSpeedKind0 = 5.9f;
    m_fMoveSpeedKind1 = 5.4f;
    m_fMoveSpeedKind2 = 5.0f;
    m_fMoveSpeedKind3 = 4.2f;
    m_fMoveSpeedKind4 = 5.0f;
    m_fMoveSpeedKind5 = 3.6f;
    m_fMoveSpeedKind6 = 8.8f;
    m_nDunkFrame = 70;
}

CC_PARAM_BASKET_KoopaJr::CC_PARAM_BASKET_KoopaJr() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 7.4f;
    m_fMoveSpeedKind1 = 6.4f;
    m_fMoveSpeedKind2 = 6.2f;
    m_fMoveSpeedKind3 = 5.2f;
    m_fMoveSpeedKind4 = 6.2f;
    m_fMoveSpeedKind5 = 4.5f;
    m_fMoveSpeedKind6 = 11.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.0f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.0f;
    m_fUnk034 = 1.12f;
    m_nDashFrame = 15;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 10;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 25;
    m_fTurnDist = 0.8f;
    m_nTrickMoveFrame = 28;
    m_fTrickMoveUnk54 = 0.17f;
    m_fTrickMoveUnk58 = 0.11f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 2;
    m_nUnk064 = 20;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 30;
    m_nUnk07C = 15;
    m_fUnk080 = 2.8f;
    m_fUnk084 = 0.1f;
    m_nJumpBlockDefStart = 15;
    m_nJumpBlockDefEnd = 43;
    m_fUnk090 = 1.5f;
    m_fUnk094 = 2.4f;
    m_nUnk0A8 = 0;
    m_nShotKind = 0;
    m_fShotSpeed = 6.8f;
    m_fShotCurve = 3.3f;
    m_nUnk0B8 = 22;
    m_fUnk0BC = 2.4f;
    m_nUnk0C0 = 11;
    m_fUnk0C4 = 0.045f;
    m_nUnk0C8 = 20;
    m_fUnk0CC = 0.4f;
    m_nUnk0D0 = 30;
    m_nJustShotKind = 1;
    m_fJustShotSpeed = 15.5f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 35;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 28;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.15f;
    m_nUnk0F8 = 10;
    m_fUnk0FC = 1.5f;
    m_nUnk100 = 55;
    m_nUnk104 = 26;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.0f;
    m_nDunkFrame = 58;
    m_bDunkLoop = 0;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 70;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 120;
    m_bAlleyoopLoop = 0;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.04f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 0;
    m_fLayUpHeight = 0.7f;
    m_nLayUpJumpFrame = 32;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.075f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 38;
    m_nJumpUpperRotBackDeg = 18;
    m_nJumpUpperJumpFrame = 10;
    m_nJumpUpperHoldFrame = 17;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 18;
    m_nJumpUpperWinStart = 0;
    m_nJumpUpperWinEnd = 30;
    m_fJumpUpperHeight = 0.8f;
    m_nJumpUpperCollType = 1;
    m_fUnk1B4 = 0.0f;
    m_fJumpUpperCollHeight = 1.0f;
    m_fUnk1BC = 1.7f;
    m_nJumpUpperSwapCount = 5;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 10;
    m_nSmashCutUnk1D0 = 35;
    m_fSmashCutRadius = 3.0f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 38;
    m_nSmashCutRotBackDeg = 18;
    m_nSmashCutJumpFrame = 15;
    m_nSmashCutHoldFrame = 20;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 5;
    m_fSmashCutHeight = 0.45f;
    m_fSmashCutSpeed = 0.03f;
    m_nSmashCutJumpFrame = 15;
    m_fSmashCutSpeed = 0.03f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 13.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_KoopaJr_NORMAL_STAR::CC_PARAM_BASKET_KoopaJr_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 7.4f;
    m_fMoveSpeedKind1 = 6.4f;
    m_fMoveSpeedKind2 = 6.2f;
    m_fMoveSpeedKind3 = 5.2f;
    m_fMoveSpeedKind4 = 6.2f;
    m_fMoveSpeedKind5 = 4.5f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 58;
}

CC_PARAM_BASKET_KoopaJr_NORMAL::CC_PARAM_BASKET_KoopaJr_NORMAL() {
    m_fMoveSpeedKind0 = 6.7f;
    m_fMoveSpeedKind1 = 5.8f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 4.7f;
    m_fMoveSpeedKind4 = 5.6f;
    m_fMoveSpeedKind5 = 4.1f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 64;
}

CC_PARAM_BASKET_KoopaJr_HARD_STAR::CC_PARAM_BASKET_KoopaJr_HARD_STAR() {
    m_fMoveSpeedKind0 = 7.4f;
    m_fMoveSpeedKind1 = 6.4f;
    m_fMoveSpeedKind2 = 6.2f;
    m_fMoveSpeedKind3 = 5.2f;
    m_fMoveSpeedKind4 = 6.2f;
    m_fMoveSpeedKind5 = 4.5f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 55;
}

CC_PARAM_BASKET_KoopaJr_HARD::CC_PARAM_BASKET_KoopaJr_HARD() {}

CC_PARAM_BASKET_KoopaJr_EXPERT_STAR::CC_PARAM_BASKET_KoopaJr_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 7.4f;
    m_fMoveSpeedKind1 = 6.4f;
    m_fMoveSpeedKind2 = 6.2f;
    m_fMoveSpeedKind3 = 5.2f;
    m_fMoveSpeedKind4 = 6.2f;
    m_fMoveSpeedKind5 = 4.5f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 55;
}

CC_PARAM_BASKET_KoopaJr_EXPERT::CC_PARAM_BASKET_KoopaJr_EXPERT() {}

} // namespace cc_data_basket_KoopaJr

// --------------------------------------------------------------------------
// Kinopio

void *CChrCoreBasketKinopio::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Kinopio;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Kinopio_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Kinopio_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Kinopio_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Kinopio_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Kinopio_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Kinopio_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Kinopio_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Kinopio_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Kinopio {

CC_PARAM_BASKET_Kinopio_EASY_STAR::CC_PARAM_BASKET_Kinopio_EASY_STAR() {
    m_fMoveSpeedKind0 = 6.8f;
    m_fMoveSpeedKind1 = 5.4f;
    m_fMoveSpeedKind2 = 5.4f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 5.9f;
    m_fMoveSpeedKind5 = 3.6f;
    m_fMoveSpeedKind6 = 10.8f;
    m_nDunkFrame = 72;
}

CC_PARAM_BASKET_Kinopio_EASY::CC_PARAM_BASKET_Kinopio_EASY() {
    m_fMoveSpeedKind0 = 6.0f;
    m_fMoveSpeedKind1 = 5.1f;
    m_fMoveSpeedKind2 = 4.8f;
    m_fMoveSpeedKind3 = 4.4f;
    m_fMoveSpeedKind4 = 5.2f;
    m_fMoveSpeedKind5 = 3.2f;
    m_fMoveSpeedKind6 = 9.6f;
    m_nDunkFrame = 78;
}

CC_PARAM_BASKET_Kinopio::CC_PARAM_BASKET_Kinopio() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 7.5f;
    m_fMoveSpeedKind1 = 6.0f;
    m_fMoveSpeedKind2 = 6.0f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.0f;
    m_fMoveSpeedKind6 = 12.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.0f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.0f;
    m_fUnk034 = 1.0f;
    m_nDashFrame = 15;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 2;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 18;
    m_fTurnDist = 0.9f;
    m_nTrickMoveFrame = 25;
    m_fTrickMoveUnk54 = 0.18f;
    m_fTrickMoveUnk58 = 0.12f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 1;
    m_nUnk064 = 16;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 16;
    m_nUnk07C = 10;
    m_fUnk080 = 3.0f;
    m_fUnk084 = 0.1f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 38;
    m_fUnk090 = 0.9f;
    m_fUnk094 = 2.2f;
    m_nUnk0A8 = 0;
    m_nShotKind = 0;
    m_fShotSpeed = 8.0f;
    m_fShotCurve = 3.3f;
    m_nUnk0B8 = 22;
    m_fUnk0BC = 3.0f;
    m_nUnk0C0 = 15;
    m_fUnk0C4 = 0.03f;
    m_nUnk0C8 = 30;
    m_fUnk0CC = 0.5f;
    m_nUnk0D0 = 34;
    m_nJustShotKind = 0;
    m_fJustShotSpeed = 15.0f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 36;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 29;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.055f;
    m_nUnk0F8 = 20;
    m_fUnk0FC = 0.5f;
    m_nUnk100 = 30;
    m_nUnk104 = 30;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.0f;
    m_nDunkFrame = 65;
    m_bDunkLoop = 1;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 50;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 120;
    m_bAlleyoopLoop = 1;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.04f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 0;
    m_fLayUpHeight = 0.7f;
    m_nLayUpJumpFrame = 32;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.075f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 38;
    m_nJumpUpperRotBackDeg = 18;
    m_nJumpUpperJumpFrame = 18;
    m_nJumpUpperHoldFrame = 17;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 18;
    m_nJumpUpperWinStart = 0;
    m_nJumpUpperWinEnd = 35;
    m_fJumpUpperHeight = 0.5f;
    m_nJumpUpperCollType = 0;
    m_fUnk1B4 = 0.5f;
    m_fJumpUpperCollHeight = 1.0f;
    m_fUnk1BC = 0.8f;
    m_nJumpUpperSwapCount = 5;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 10;
    m_nSmashCutUnk1D0 = 35;
    m_fSmashCutRadius = 0.5f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 38;
    m_nSmashCutRotBackDeg = 18;
    m_nSmashCutJumpFrame = 15;
    m_nSmashCutHoldFrame = 20;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 5;
    m_fSmashCutHeight = 0.45f;
    m_fSmashCutSpeed = 0.03f;
    m_nSmashCutJumpFrame = 15;
    m_fSmashCutSpeed = 0.03f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 15.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Kinopio_NORMAL_STAR::CC_PARAM_BASKET_Kinopio_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 7.5f;
    m_fMoveSpeedKind1 = 6.0f;
    m_fMoveSpeedKind2 = 6.0f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.0f;
    m_fMoveSpeedKind6 = 12.0f;
    m_nDunkFrame = 65;
}

CC_PARAM_BASKET_Kinopio_NORMAL::CC_PARAM_BASKET_Kinopio_NORMAL() {
    m_fMoveSpeedKind0 = 6.8f;
    m_fMoveSpeedKind1 = 5.4f;
    m_fMoveSpeedKind2 = 5.4f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 5.9f;
    m_fMoveSpeedKind5 = 3.6f;
    m_fMoveSpeedKind6 = 10.8f;
    m_nDunkFrame = 72;
}

CC_PARAM_BASKET_Kinopio_HARD_STAR::CC_PARAM_BASKET_Kinopio_HARD_STAR() {
    m_fMoveSpeedKind0 = 7.5f;
    m_fMoveSpeedKind1 = 6.0f;
    m_fMoveSpeedKind2 = 6.0f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.0f;
    m_fMoveSpeedKind6 = 12.0f;
    m_nDunkFrame = 62;
}

CC_PARAM_BASKET_Kinopio_HARD::CC_PARAM_BASKET_Kinopio_HARD() {}

CC_PARAM_BASKET_Kinopio_EXPERT_STAR::CC_PARAM_BASKET_Kinopio_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 7.5f;
    m_fMoveSpeedKind1 = 6.0f;
    m_fMoveSpeedKind2 = 6.0f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.0f;
    m_fMoveSpeedKind6 = 12.0f;
    m_nDunkFrame = 62;
}

CC_PARAM_BASKET_Kinopio_EXPERT::CC_PARAM_BASKET_Kinopio_EXPERT() {}

} // namespace cc_data_basket_Kinopio

// --------------------------------------------------------------------------
// Moogle

void *CChrCoreBasketMoogle::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Moogle;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Moogle_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Moogle_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Moogle_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Moogle_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Moogle_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Moogle_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Moogle_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Moogle_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Moogle {

CC_PARAM_BASKET_Moogle_EASY_STAR::CC_PARAM_BASKET_Moogle_EASY_STAR() {
    m_fMoveSpeedKind0 = 5.6f;
    m_fMoveSpeedKind1 = 4.8f;
    m_fMoveSpeedKind2 = 4.5f;
    m_fMoveSpeedKind3 = 4.4f;
    m_fMoveSpeedKind4 = 5.6f;
    m_fMoveSpeedKind5 = 3.9f;
    m_fMoveSpeedKind6 = 9.6f;
    m_nDunkFrame = 99;
}

CC_PARAM_BASKET_Moogle_EASY::CC_PARAM_BASKET_Moogle_EASY() {
    m_fMoveSpeedKind0 = 5.0f;
    m_fMoveSpeedKind1 = 4.5f;
    m_fMoveSpeedKind2 = 4.0f;
    m_fMoveSpeedKind3 = 3.9f;
    m_fMoveSpeedKind4 = 5.0f;
    m_fMoveSpeedKind5 = 3.4f;
    m_fMoveSpeedKind6 = 8.6f;
    m_nDunkFrame = 108;
}

CC_PARAM_BASKET_Moogle::CC_PARAM_BASKET_Moogle() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 6.2f;
    m_fMoveSpeedKind1 = 5.3f;
    m_fMoveSpeedKind2 = 5.0f;
    m_fMoveSpeedKind3 = 4.9f;
    m_fMoveSpeedKind4 = 6.2f;
    m_fMoveSpeedKind5 = 4.3f;
    m_fMoveSpeedKind6 = 10.7f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.15f;
    m_fUnk028 = 1.2f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.1f;
    m_fUnk034 = 1.0f;
    m_nDashFrame = 15;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 5;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 28;
    m_fTurnDist = 1.5f;
    m_nTrickMoveFrame = 43;
    m_fTrickMoveUnk54 = 0.1f;
    m_fTrickMoveUnk58 = 0.12f;
    m_nTrickMoveRotDeg = 50;
    m_nUnk060 = 15;
    m_nUnk064 = 20;
    m_fUnk068 = 2.4f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 18;
    m_nUnk07C = 34;
    m_fUnk080 = 3.2f;
    m_fUnk084 = 0.1f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 65;
    m_fUnk090 = 0.9f;
    m_fUnk094 = 2.4f;
    m_nUnk0A8 = 0;
    m_nShotKind = 0;
    m_fShotSpeed = 6.8f;
    m_fShotCurve = 4.2f;
    m_nUnk0B8 = 34;
    m_fUnk0BC = 2.4f;
    m_nUnk0C0 = 18;
    m_fUnk0C4 = 0.035f;
    m_nUnk0C8 = 45;
    m_fUnk0CC = 0.5f;
    m_nUnk0D0 = 49;
    m_nJustShotKind = 2;
    m_fJustShotSpeed = 5.0f;
    m_nUnk0DC = 34;
    m_nUnk0E0 = 52;
    m_nUnk0E4 = 34;
    m_nUnk0E8 = 44;
    m_nUnk0EC = 34;
    m_nUnk0F0 = 34;
    m_fUnk0F4 = 0.055f;
    m_nUnk0F8 = 20;
    m_fUnk0FC = 0.6f;
    m_nUnk100 = 40;
    m_nUnk104 = 40;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.0f;
    m_nDunkFrame = 90;
    m_bDunkLoop = 1;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 80;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 150;
    m_bAlleyoopLoop = 1;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.04f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.0f;
    m_nLayUpHoldFrame = 45;
    m_fLayUpHeight = 0.9f;
    m_nLayUpJumpFrame = 41;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.05f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 0;
    m_nJumpUpperRotBackDeg = 0;
    m_nJumpUpperJumpFrame = 18;
    m_nJumpUpperHoldFrame = 39;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 36;
    m_nJumpUpperWinStart = 6;
    m_nJumpUpperWinEnd = 63;
    m_fJumpUpperHeight = 0.5f;
    m_nJumpUpperCollType = 0;
    m_fUnk1B4 = 0.5f;
    m_fJumpUpperCollHeight = 1.0f;
    m_fUnk1BC = 1.0f;
    m_nJumpUpperSwapCount = 5;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 0;
    m_nSmashCutUnk1D0 = 30;
    m_fSmashCutRadius = 0.8f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 38;
    m_nSmashCutRotBackDeg = 18;
    m_nSmashCutJumpFrame = 45;
    m_nSmashCutHoldFrame = 20;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 5;
    m_fSmashCutHeight = 0.45f;
    m_fSmashCutSpeed = 0.07f;
    m_nSmashCutJumpFrame = 45;
    m_fSmashCutSpeed = 0.07f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 15.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Moogle_NORMAL_STAR::CC_PARAM_BASKET_Moogle_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 6.2f;
    m_fMoveSpeedKind1 = 5.3f;
    m_fMoveSpeedKind2 = 5.0f;
    m_fMoveSpeedKind3 = 4.9f;
    m_fMoveSpeedKind4 = 6.2f;
    m_fMoveSpeedKind5 = 4.3f;
    m_fMoveSpeedKind6 = 10.7f;
    m_nDunkFrame = 90;
}

CC_PARAM_BASKET_Moogle_NORMAL::CC_PARAM_BASKET_Moogle_NORMAL() {
    m_fMoveSpeedKind0 = 5.6f;
    m_fMoveSpeedKind1 = 4.8f;
    m_fMoveSpeedKind2 = 4.5f;
    m_fMoveSpeedKind3 = 4.4f;
    m_fMoveSpeedKind4 = 5.6f;
    m_fMoveSpeedKind5 = 3.9f;
    m_fMoveSpeedKind6 = 9.6f;
    m_nDunkFrame = 99;
}

CC_PARAM_BASKET_Moogle_HARD_STAR::CC_PARAM_BASKET_Moogle_HARD_STAR() {
    m_fMoveSpeedKind0 = 6.6f;
    m_fMoveSpeedKind1 = 5.6f;
    m_fMoveSpeedKind2 = 5.3f;
    m_fMoveSpeedKind3 = 5.2f;
    m_fMoveSpeedKind4 = 6.6f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 11.3f;
    m_nDunkFrame = 86;
}

CC_PARAM_BASKET_Moogle_HARD::CC_PARAM_BASKET_Moogle_HARD() {}

CC_PARAM_BASKET_Moogle_EXPERT_STAR::CC_PARAM_BASKET_Moogle_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 6.6f;
    m_fMoveSpeedKind1 = 5.6f;
    m_fMoveSpeedKind2 = 5.3f;
    m_fMoveSpeedKind3 = 5.2f;
    m_fMoveSpeedKind4 = 6.6f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 11.3f;
    m_nDunkFrame = 86;
}

CC_PARAM_BASKET_Moogle_EXPERT::CC_PARAM_BASKET_Moogle_EXPERT() {}

} // namespace cc_data_basket_Moogle

// --------------------------------------------------------------------------
// Saboten

void *CChrCoreBasketSaboten::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Saboten;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Saboten_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Saboten_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Saboten_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Saboten_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Saboten_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Saboten_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Saboten_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Saboten_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Saboten {

CC_PARAM_BASKET_Saboten_EASY_STAR::CC_PARAM_BASKET_Saboten_EASY_STAR() {
    m_fMoveSpeedKind0 = 10.4f;
    m_fMoveSpeedKind1 = 8.1f;
    m_fMoveSpeedKind2 = 7.7f;
    m_fMoveSpeedKind3 = 7.7f;
    m_fMoveSpeedKind4 = 8.1f;
    m_fMoveSpeedKind5 = 6.1f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 88;
}

CC_PARAM_BASKET_Saboten_EASY::CC_PARAM_BASKET_Saboten_EASY() {
    m_fMoveSpeedKind0 = 9.2f;
    m_fMoveSpeedKind1 = 7.7f;
    m_fMoveSpeedKind2 = 6.8f;
    m_fMoveSpeedKind3 = 6.8f;
    m_fMoveSpeedKind4 = 7.2f;
    m_fMoveSpeedKind5 = 5.4f;
    m_fMoveSpeedKind6 = 8.8f;
    m_nDunkFrame = 96;
}

CC_PARAM_BASKET_Saboten::CC_PARAM_BASKET_Saboten() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 11.5f;
    m_fMoveSpeedKind1 = 9.0f;
    m_fMoveSpeedKind2 = 8.5f;
    m_fMoveSpeedKind3 = 8.5f;
    m_fMoveSpeedKind4 = 9.0f;
    m_fMoveSpeedKind5 = 6.8f;
    m_fMoveSpeedKind6 = 11.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.9f;
    m_fUnk028 = 1.75f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.35f;
    m_fUnk034 = 1.85f;
    m_nDashFrame = 20;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 5;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 30;
    m_fTurnDist = 2.6f;
    m_nTrickMoveFrame = 25;
    m_fTrickMoveUnk54 = 0.09f;
    m_fTrickMoveUnk58 = 0.02f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 0;
    m_nUnk064 = 20;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 15;
    m_nUnk07C = 12;
    m_fUnk080 = 2.9f;
    m_fUnk084 = 0.1f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 38;
    m_fUnk090 = 0.9f;
    m_fUnk094 = 2.4f;
    m_nUnk0A8 = 20;
    m_nShotKind = 0;
    m_fShotSpeed = 6.0f;
    m_fShotCurve = 3.3f;
    m_nUnk0B8 = 22;
    m_fUnk0BC = 2.4f;
    m_nUnk0C0 = 11;
    m_fUnk0C4 = 0.055f;
    m_nUnk0C8 = 20;
    m_fUnk0CC = 0.5f;
    m_nUnk0D0 = 34;
    m_nJustShotKind = 0;
    m_fJustShotSpeed = 10.0f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 34;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 27;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.02f;
    m_nUnk0F8 = 20;
    m_fUnk0FC = 0.1f;
    m_nUnk100 = 20;
    m_nUnk104 = 55;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.0f;
    m_nDunkFrame = 80;
    m_bDunkLoop = 1;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 40;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 120;
    m_bAlleyoopLoop = 1;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.06f;
    m_nLayUpRotDeg = 45;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 20;
    m_fLayUpHeight = 0.5f;
    m_nLayUpJumpFrame = 32;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.075f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 38;
    m_nJumpUpperRotBackDeg = 18;
    m_nJumpUpperJumpFrame = 18;
    m_nJumpUpperHoldFrame = 17;
    m_nJumpUpperTurnFrame = 20;
    m_nJumpUpperFallFrame = 18;
    m_nJumpUpperWinStart = 0;
    m_nJumpUpperWinEnd = 30;
    m_fJumpUpperHeight = 0.1f;
    m_nJumpUpperCollType = 0;
    m_fUnk1B4 = 0.7f;
    m_fJumpUpperCollHeight = 1.0f;
    m_fUnk1BC = 1.4f;
    m_nJumpUpperSwapCount = 5;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 0;
    m_nSmashCutUnk1D0 = 35;
    m_fSmashCutRadius = 3.0f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 36;
    m_nSmashCutRotBackDeg = 18;
    m_nSmashCutJumpFrame = 15;
    m_nSmashCutHoldFrame = 22;
    m_nSmashCutTurnFrame = 20;
    m_nSmashCutFallFrame = 15;
    m_fSmashCutHeight = 0.45f;
    m_fSmashCutSpeed = 0.03f;
    m_nSmashCutJumpFrame = 15;
    m_fSmashCutSpeed = 0.03f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 15.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Saboten_NORMAL_STAR::CC_PARAM_BASKET_Saboten_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 11.5f;
    m_fMoveSpeedKind1 = 9.0f;
    m_fMoveSpeedKind2 = 8.5f;
    m_fMoveSpeedKind3 = 8.5f;
    m_fMoveSpeedKind4 = 9.0f;
    m_fMoveSpeedKind5 = 6.8f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 80;
}

CC_PARAM_BASKET_Saboten_NORMAL::CC_PARAM_BASKET_Saboten_NORMAL() {
    m_fMoveSpeedKind0 = 10.4f;
    m_fMoveSpeedKind1 = 8.1f;
    m_fMoveSpeedKind2 = 7.7f;
    m_fMoveSpeedKind3 = 7.7f;
    m_fMoveSpeedKind4 = 8.1f;
    m_fMoveSpeedKind5 = 6.1f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 88;
}

CC_PARAM_BASKET_Saboten_HARD_STAR::CC_PARAM_BASKET_Saboten_HARD_STAR() {
    m_fMoveSpeedKind0 = 11.5f;
    m_fMoveSpeedKind1 = 9.0f;
    m_fMoveSpeedKind2 = 8.5f;
    m_fMoveSpeedKind3 = 8.5f;
    m_fMoveSpeedKind4 = 9.0f;
    m_fMoveSpeedKind5 = 6.8f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 76;
}

CC_PARAM_BASKET_Saboten_HARD::CC_PARAM_BASKET_Saboten_HARD() {}

CC_PARAM_BASKET_Saboten_EXPERT_STAR::CC_PARAM_BASKET_Saboten_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 11.5f;
    m_fMoveSpeedKind1 = 9.0f;
    m_fMoveSpeedKind2 = 8.5f;
    m_fMoveSpeedKind3 = 8.5f;
    m_fMoveSpeedKind4 = 9.0f;
    m_fMoveSpeedKind5 = 6.8f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 76;
}

CC_PARAM_BASKET_Saboten_EXPERT::CC_PARAM_BASKET_Saboten_EXPERT() {}

} // namespace cc_data_basket_Saboten

// --------------------------------------------------------------------------
// Ninja

void *CChrCoreBasketNinja::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Ninja;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Ninja_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Ninja_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Ninja_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Ninja_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Ninja_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Ninja_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Ninja_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Ninja_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Ninja {

CC_PARAM_BASKET_Ninja_EASY_STAR::CC_PARAM_BASKET_Ninja_EASY_STAR() {
    m_fMoveSpeedKind0 = 7.2f;
    m_fMoveSpeedKind1 = 5.9f;
    m_fMoveSpeedKind2 = 5.9f;
    m_fMoveSpeedKind3 = 4.8f;
    m_fMoveSpeedKind4 = 5.9f;
    m_fMoveSpeedKind5 = 4.2f;
    m_fMoveSpeedKind6 = 10.8f;
    m_nDunkFrame = 58;
}

CC_PARAM_BASKET_Ninja_EASY::CC_PARAM_BASKET_Ninja_EASY() {
    m_fMoveSpeedKind0 = 6.4f;
    m_fMoveSpeedKind1 = 5.5f;
    m_fMoveSpeedKind2 = 5.2f;
    m_fMoveSpeedKind3 = 4.2f;
    m_fMoveSpeedKind4 = 5.2f;
    m_fMoveSpeedKind5 = 3.8f;
    m_fMoveSpeedKind6 = 9.6f;
    m_nDunkFrame = 64;
}

CC_PARAM_BASKET_Ninja::CC_PARAM_BASKET_Ninja() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 8.0f;
    m_fMoveSpeedKind1 = 6.5f;
    m_fMoveSpeedKind2 = 6.5f;
    m_fMoveSpeedKind3 = 5.3f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.7f;
    m_fMoveSpeedKind6 = 12.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.25f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.1f;
    m_fUnk034 = 1.1f;
    m_nDashFrame = 12;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 0;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 10;
    m_fTurnDist = 1.0f;
    m_nTrickMoveFrame = 26;
    m_fTrickMoveUnk54 = 0.18f;
    m_fTrickMoveUnk58 = 0.1f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 0;
    m_nUnk064 = 18;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 6;
    m_nUnk07C = 15;
    m_fUnk080 = 3.2f;
    m_fUnk084 = 0.08f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 38;
    m_fUnk090 = 0.9f;
    m_fUnk094 = 2.4f;
    m_nUnk0A8 = 20;
    m_nShotKind = 0;
    m_fShotSpeed = 12.0f;
    m_fShotCurve = 3.3f;
    m_nUnk0B8 = 22;
    m_fUnk0BC = 3.0f;
    m_nUnk0C0 = 6;
    m_fUnk0C4 = 0.0f;
    m_nUnk0C8 = 20;
    m_fUnk0CC = 0.5f;
    m_nUnk0D0 = 34;
    m_nJustShotKind = 0;
    m_fJustShotSpeed = 18.0f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 34;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 27;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.025f;
    m_nUnk0F8 = 30;
    m_fUnk0FC = 0.5f;
    m_nUnk100 = 30;
    m_nUnk104 = 49;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.0f;
    m_nDunkFrame = 53;
    m_bDunkLoop = 1;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 40;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 110;
    m_bAlleyoopLoop = 1;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 80;
    m_nUnk13C = 30;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.02f;
    m_fUnk148 = -0.1f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.04f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 8;
    m_fLayUpHeight = 0.7f;
    m_nLayUpJumpFrame = 32;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.075f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 38;
    m_nJumpUpperRotBackDeg = 0;
    m_nJumpUpperJumpFrame = 20;
    m_nJumpUpperHoldFrame = 20;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 25;
    m_nJumpUpperWinStart = 5;
    m_nJumpUpperWinEnd = 35;
    m_fJumpUpperHeight = 0.8f;
    m_nJumpUpperCollType = 1;
    m_fUnk1B4 = 0.35f;
    m_fJumpUpperCollHeight = 0.5f;
    m_fUnk1BC = 1.0f;
    m_nJumpUpperSwapCount = 5;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 10;
    m_nSmashCutUnk1D0 = 35;
    m_fSmashCutRadius = 3.0f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 38;
    m_nSmashCutRotBackDeg = 18;
    m_nSmashCutJumpFrame = 15;
    m_nSmashCutHoldFrame = 20;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 5;
    m_fSmashCutHeight = 0.45f;
    m_fSmashCutSpeed = 0.03f;
    m_nSmashCutJumpFrame = 15;
    m_fSmashCutSpeed = 0.03f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 15.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Ninja_NORMAL_STAR::CC_PARAM_BASKET_Ninja_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 8.0f;
    m_fMoveSpeedKind1 = 6.5f;
    m_fMoveSpeedKind2 = 6.5f;
    m_fMoveSpeedKind3 = 5.3f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.7f;
    m_fMoveSpeedKind6 = 12.0f;
    m_nDunkFrame = 53;
}

CC_PARAM_BASKET_Ninja_NORMAL::CC_PARAM_BASKET_Ninja_NORMAL() {
    m_fMoveSpeedKind0 = 7.2f;
    m_fMoveSpeedKind1 = 5.9f;
    m_fMoveSpeedKind2 = 5.9f;
    m_fMoveSpeedKind3 = 4.8f;
    m_fMoveSpeedKind4 = 5.9f;
    m_fMoveSpeedKind5 = 4.2f;
    m_fMoveSpeedKind6 = 10.8f;
    m_nDunkFrame = 58;
}

CC_PARAM_BASKET_Ninja_HARD_STAR::CC_PARAM_BASKET_Ninja_HARD_STAR() {
    m_fMoveSpeedKind0 = 8.0f;
    m_fMoveSpeedKind1 = 6.5f;
    m_fMoveSpeedKind2 = 6.5f;
    m_fMoveSpeedKind3 = 5.3f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.7f;
    m_fMoveSpeedKind6 = 12.0f;
    m_nDunkFrame = 50;
}

CC_PARAM_BASKET_Ninja_HARD::CC_PARAM_BASKET_Ninja_HARD() {}

CC_PARAM_BASKET_Ninja_EXPERT_STAR::CC_PARAM_BASKET_Ninja_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 8.5f;
    m_fMoveSpeedKind1 = 6.9f;
    m_fMoveSpeedKind2 = 6.9f;
    m_fMoveSpeedKind3 = 5.6f;
    m_fMoveSpeedKind4 = 6.9f;
    m_fMoveSpeedKind5 = 5.0f;
    m_fMoveSpeedKind6 = 12.7f;
    m_nDunkFrame = 50;
}

CC_PARAM_BASKET_Ninja_EXPERT::CC_PARAM_BASKET_Ninja_EXPERT() {}

} // namespace cc_data_basket_Ninja

// --------------------------------------------------------------------------
// Siroma

void *CChrCoreBasketSiroma::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Siroma;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Siroma_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Siroma_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Siroma_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Siroma_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Siroma_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Siroma_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Siroma_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Siroma_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Siroma {

CC_PARAM_BASKET_Siroma_EASY_STAR::CC_PARAM_BASKET_Siroma_EASY_STAR() {
    m_fMoveSpeedKind0 = 6.3f;
    m_fMoveSpeedKind1 = 5.4f;
    m_fMoveSpeedKind2 = 5.4f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 6.1f;
    m_fMoveSpeedKind5 = 5.2f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 72;
}

CC_PARAM_BASKET_Siroma_EASY::CC_PARAM_BASKET_Siroma_EASY() {
    m_fMoveSpeedKind0 = 5.6f;
    m_fMoveSpeedKind1 = 5.1f;
    m_fMoveSpeedKind2 = 4.8f;
    m_fMoveSpeedKind3 = 4.4f;
    m_fMoveSpeedKind4 = 5.4f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 8.8f;
    m_nDunkFrame = 78;
}

CC_PARAM_BASKET_Siroma::CC_PARAM_BASKET_Siroma() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 7.0f;
    m_fMoveSpeedKind1 = 6.0f;
    m_fMoveSpeedKind2 = 6.0f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.8f;
    m_fMoveSpeedKind5 = 5.8f;
    m_fMoveSpeedKind6 = 11.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.0f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.0f;
    m_fUnk034 = 1.0f;
    m_nDashFrame = 20;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 5;
    m_nDashTurnDeg = 80;
    m_nTurnFrame = 20;
    m_fTurnDist = 1.0f;
    m_nTrickMoveFrame = 32;
    m_fTrickMoveUnk54 = 0.17f;
    m_fTrickMoveUnk58 = 0.1f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 2;
    m_nUnk064 = 20;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 9;
    m_nUnk07C = 17;
    m_fUnk080 = 2.8f;
    m_fUnk084 = 0.06f;
    m_nJumpBlockDefStart = 7;
    m_nJumpBlockDefEnd = 38;
    m_fUnk090 = 1.5f;
    m_fUnk094 = 2.5f;
    m_nUnk0A8 = 0;
    m_nShotKind = 0;
    m_fShotSpeed = 14.0f;
    m_fShotCurve = 3.0f;
    m_nUnk0B8 = 22;
    m_fUnk0BC = 2.4f;
    m_nUnk0C0 = 11;
    m_fUnk0C4 = 0.055f;
    m_nUnk0C8 = 20;
    m_fUnk0CC = 0.5f;
    m_nUnk0D0 = 34;
    m_nJustShotKind = 2;
    m_fJustShotSpeed = 6.5f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 36;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 29;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.055f;
    m_nUnk0F8 = 20;
    m_fUnk0FC = 0.5f;
    m_nUnk100 = 50;
    m_nUnk104 = 30;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.0f;
    m_nDunkFrame = 65;
    m_bDunkLoop = 0;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 40;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 120;
    m_bAlleyoopLoop = 0;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.04f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 40;
    m_fLayUpHeight = 0.7f;
    m_nLayUpJumpFrame = 32;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.075f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 15;
    m_nJumpUpperRotBackDeg = 10;
    m_nJumpUpperJumpFrame = 18;
    m_nJumpUpperHoldFrame = 17;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 18;
    m_nJumpUpperWinStart = 3;
    m_nJumpUpperWinEnd = 35;
    m_fJumpUpperHeight = 0.4f;
    m_nJumpUpperCollType = 1;
    m_fUnk1B4 = 0.8f;
    m_fJumpUpperCollHeight = 0.5f;
    m_fUnk1BC = 1.5f;
    m_nJumpUpperSwapCount = 6;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 10;
    m_nSmashCutUnk1D0 = 33;
    m_fSmashCutRadius = 3.0f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 38;
    m_nSmashCutRotBackDeg = 18;
    m_nSmashCutJumpFrame = 15;
    m_nSmashCutHoldFrame = 20;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 5;
    m_fSmashCutHeight = 0.45f;
    m_fSmashCutSpeed = 0.03f;
    m_nSmashCutJumpFrame = 15;
    m_fSmashCutSpeed = 0.03f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 120.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 15.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Siroma_NORMAL_STAR::CC_PARAM_BASKET_Siroma_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 7.0f;
    m_fMoveSpeedKind1 = 6.0f;
    m_fMoveSpeedKind2 = 6.0f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.8f;
    m_fMoveSpeedKind5 = 5.8f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 65;
}

CC_PARAM_BASKET_Siroma_NORMAL::CC_PARAM_BASKET_Siroma_NORMAL() {
    m_fMoveSpeedKind0 = 6.3f;
    m_fMoveSpeedKind1 = 5.4f;
    m_fMoveSpeedKind2 = 5.4f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 6.1f;
    m_fMoveSpeedKind5 = 5.2f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 72;
}

CC_PARAM_BASKET_Siroma_HARD_STAR::CC_PARAM_BASKET_Siroma_HARD_STAR() {
    m_fMoveSpeedKind0 = 7.4f;
    m_fMoveSpeedKind1 = 6.4f;
    m_fMoveSpeedKind2 = 6.4f;
    m_fMoveSpeedKind3 = 5.8f;
    m_fMoveSpeedKind4 = 7.2f;
    m_fMoveSpeedKind5 = 6.1f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 62;
}

CC_PARAM_BASKET_Siroma_HARD::CC_PARAM_BASKET_Siroma_HARD() {}

CC_PARAM_BASKET_Siroma_EXPERT_STAR::CC_PARAM_BASKET_Siroma_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 7.4f;
    m_fMoveSpeedKind1 = 6.4f;
    m_fMoveSpeedKind2 = 6.4f;
    m_fMoveSpeedKind3 = 5.8f;
    m_fMoveSpeedKind4 = 7.2f;
    m_fMoveSpeedKind5 = 6.1f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 62;
}

CC_PARAM_BASKET_Siroma_EXPERT::CC_PARAM_BASKET_Siroma_EXPERT() {}

} // namespace cc_data_basket_Siroma

// --------------------------------------------------------------------------
// Slime

void *CChrCoreBasketSlime::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Slime;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Slime_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Slime_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Slime_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Slime_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Slime_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Slime_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Slime_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Slime_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Slime {

CC_PARAM_BASKET_Slime_EASY_STAR::CC_PARAM_BASKET_Slime_EASY_STAR() {
    m_fMoveSpeedKind0 = 4.5f;
    m_fMoveSpeedKind1 = 4.5f;
    m_fMoveSpeedKind2 = 4.1f;
    m_fMoveSpeedKind3 = 3.6f;
    m_fMoveSpeedKind4 = 4.5f;
    m_fMoveSpeedKind5 = 3.6f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 61;
}

CC_PARAM_BASKET_Slime_EASY::CC_PARAM_BASKET_Slime_EASY() {
    m_fMoveSpeedKind0 = 4.0f;
    m_fMoveSpeedKind1 = 4.3f;
    m_fMoveSpeedKind2 = 3.6f;
    m_fMoveSpeedKind3 = 3.2f;
    m_fMoveSpeedKind4 = 4.0f;
    m_fMoveSpeedKind5 = 3.2f;
    m_fMoveSpeedKind6 = 8.8f;
    m_nDunkFrame = 66;
}

CC_PARAM_BASKET_Slime::CC_PARAM_BASKET_Slime() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 5.0f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 4.5f;
    m_fMoveSpeedKind3 = 4.0f;
    m_fMoveSpeedKind4 = 5.0f;
    m_fMoveSpeedKind5 = 4.0f;
    m_fMoveSpeedKind6 = 11.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.1f;
    m_fUnk028 = 0.9f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.1f;
    m_fUnk034 = 1.1f;
    m_nDashFrame = 20;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 7;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 21;
    m_fTurnDist = 0.8f;
    m_nTrickMoveFrame = 47;
    m_fTrickMoveUnk54 = 0.13f;
    m_fTrickMoveUnk58 = 0.11f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 2;
    m_nUnk064 = 20;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 12;
    m_nUnk07C = 17;
    m_fUnk080 = 3.2f;
    m_fUnk084 = 0.11f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 38;
    m_fUnk090 = 0.9f;
    m_fUnk094 = 2.0f;
    m_nUnk0A8 = 60;
    m_nShotKind = 0;
    m_fShotSpeed = 12.0f;
    m_fShotCurve = 3.3f;
    m_nUnk0B8 = 22;
    m_fUnk0BC = 2.4f;
    m_nUnk0C0 = 11;
    m_fUnk0C4 = 0.055f;
    m_nUnk0C8 = 20;
    m_fUnk0CC = 0.5f;
    m_nUnk0D0 = 34;
    m_nJustShotKind = 1;
    m_fJustShotSpeed = 18.0f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 35;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 28;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.0f;
    m_nUnk0F8 = 20;
    m_fUnk0FC = 0.0f;
    m_nUnk100 = 20;
    m_nUnk104 = 100;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 3.75f;
    m_nDunkFrame = 55;
    m_bDunkLoop = 0;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 40;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 120;
    m_bAlleyoopLoop = 0;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.07f;
    m_nLayUpRotDeg = 40;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 0;
    m_fLayUpHeight = 0.7f;
    m_nLayUpJumpFrame = 32;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 2;
    m_nUnk180 = 10;
    m_fUnk184 = 0.075f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 40;
    m_nJumpUpperRotBackDeg = 18;
    m_nJumpUpperJumpFrame = 15;
    m_nJumpUpperHoldFrame = 50;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 20;
    m_nJumpUpperWinStart = 5;
    m_nJumpUpperWinEnd = 55;
    m_fJumpUpperHeight = 0.5f;
    m_nJumpUpperCollType = 1;
    m_fUnk1B4 = 0.0f;
    m_fJumpUpperCollHeight = 0.5f;
    m_fUnk1BC = 1.35f;
    m_nJumpUpperSwapCount = 9;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 10;
    m_nSmashCutUnk1D0 = 35;
    m_fSmashCutRadius = 3.0f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 35;
    m_nSmashCutRotBackDeg = 18;
    m_nSmashCutJumpFrame = 15;
    m_nSmashCutHoldFrame = 40;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 5;
    m_fSmashCutHeight = 0.45f;
    m_fSmashCutSpeed = 0.03f;
    m_nSmashCutJumpFrame = 15;
    m_fSmashCutSpeed = 0.03f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 90.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 12.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Slime_NORMAL_STAR::CC_PARAM_BASKET_Slime_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 5.0f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 4.5f;
    m_fMoveSpeedKind3 = 4.0f;
    m_fMoveSpeedKind4 = 5.0f;
    m_fMoveSpeedKind5 = 4.0f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 55;
}

CC_PARAM_BASKET_Slime_NORMAL::CC_PARAM_BASKET_Slime_NORMAL() {
    m_fMoveSpeedKind0 = 4.5f;
    m_fMoveSpeedKind1 = 4.5f;
    m_fMoveSpeedKind2 = 4.1f;
    m_fMoveSpeedKind3 = 3.6f;
    m_fMoveSpeedKind4 = 4.5f;
    m_fMoveSpeedKind5 = 3.6f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 61;
}

CC_PARAM_BASKET_Slime_HARD_STAR::CC_PARAM_BASKET_Slime_HARD_STAR() {
    m_fMoveSpeedKind0 = 5.3f;
    m_fMoveSpeedKind1 = 5.3f;
    m_fMoveSpeedKind2 = 4.8f;
    m_fMoveSpeedKind3 = 4.2f;
    m_fMoveSpeedKind4 = 5.3f;
    m_fMoveSpeedKind5 = 4.2f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 52;
}

CC_PARAM_BASKET_Slime_HARD::CC_PARAM_BASKET_Slime_HARD() {}

CC_PARAM_BASKET_Slime_EXPERT_STAR::CC_PARAM_BASKET_Slime_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 5.3f;
    m_fMoveSpeedKind1 = 5.3f;
    m_fMoveSpeedKind2 = 4.8f;
    m_fMoveSpeedKind3 = 4.2f;
    m_fMoveSpeedKind4 = 5.3f;
    m_fMoveSpeedKind5 = 4.2f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 52;
}

CC_PARAM_BASKET_Slime_EXPERT::CC_PARAM_BASKET_Slime_EXPERT() {}

} // namespace cc_data_basket_Slime

// --------------------------------------------------------------------------
// Kuroma

void *CChrCoreBasketKuroma::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Kuroma;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Kuroma_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Kuroma_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Kuroma_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Kuroma_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Kuroma_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Kuroma_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Kuroma_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Kuroma_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Kuroma {

CC_PARAM_BASKET_Kuroma_EASY_STAR::CC_PARAM_BASKET_Kuroma_EASY_STAR() {
    m_fMoveSpeedKind0 = 5.7f;
    m_fMoveSpeedKind1 = 5.3f;
    m_fMoveSpeedKind2 = 5.0f;
    m_fMoveSpeedKind3 = 4.5f;
    m_fMoveSpeedKind4 = 5.9f;
    m_fMoveSpeedKind5 = 4.1f;
    m_fMoveSpeedKind6 = 10.8f;
    m_nDunkFrame = 121;
}

CC_PARAM_BASKET_Kuroma_EASY::CC_PARAM_BASKET_Kuroma_EASY() {
    m_fMoveSpeedKind0 = 5.0f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 4.5f;
    m_fMoveSpeedKind3 = 4.0f;
    m_fMoveSpeedKind4 = 5.2f;
    m_fMoveSpeedKind5 = 3.7f;
    m_fMoveSpeedKind6 = 9.6f;
    m_nDunkFrame = 132;
}

CC_PARAM_BASKET_Kuroma::CC_PARAM_BASKET_Kuroma() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 6.3f;
    m_fMoveSpeedKind1 = 5.9f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 12.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.0f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.0f;
    m_fUnk034 = 1.0f;
    m_nDashFrame = 20;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 5;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 27;
    m_fTurnDist = 1.2f;
    m_nTrickMoveFrame = 40;
    m_fTrickMoveUnk54 = 0.17f;
    m_fTrickMoveUnk58 = 0.11f;
    m_nTrickMoveRotDeg = 60;
    m_nUnk060 = 2;
    m_nUnk064 = 20;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 9;
    m_nUnk07C = 37;
    m_fUnk080 = 3.0f;
    m_fUnk084 = 0.1f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 47;
    m_fUnk090 = 1.1f;
    m_fUnk094 = 2.4f;
    m_nUnk0A8 = 0;
    m_nShotKind = 0;
    m_fShotSpeed = 8.0f;
    m_fShotCurve = 3.3f;
    m_nUnk0B8 = 24;
    m_fUnk0BC = 2.4f;
    m_nUnk0C0 = 6;
    m_fUnk0C4 = 0.055f;
    m_nUnk0C8 = 20;
    m_fUnk0CC = 0.5f;
    m_nUnk0D0 = 34;
    m_nJustShotKind = 1;
    m_fJustShotSpeed = 17.0f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 35;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 28;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.055f;
    m_nUnk0F8 = 20;
    m_fUnk0FC = 0.5f;
    m_nUnk100 = 30;
    m_nUnk104 = 60;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.0f;
    m_nDunkFrame = 110;
    m_bDunkLoop = 1;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 40;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 120;
    m_bAlleyoopLoop = 1;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.0f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.0f;
    m_nLayUpHoldFrame = 50;
    m_fLayUpHeight = 0.7f;
    m_nLayUpJumpFrame = 32;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.075f;
    m_nSwatItBlend = 0;
    m_nJumpUpperRotDeg = 0;
    m_nJumpUpperRotBackDeg = 0;
    m_nJumpUpperJumpFrame = 18;
    m_nJumpUpperHoldFrame = 40;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 18;
    m_nJumpUpperWinStart = 13;
    m_nJumpUpperWinEnd = 55;
    m_fJumpUpperHeight = 0.2f;
    m_nJumpUpperCollType = 1;
    m_fUnk1B4 = 0.5f;
    m_fJumpUpperCollHeight = 0.9f;
    m_fUnk1BC = 1.2f;
    m_nJumpUpperSwapCount = 7;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 10;
    m_nSmashCutUnk1D0 = 35;
    m_fSmashCutRadius = 3.0f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 38;
    m_nSmashCutRotBackDeg = 18;
    m_nSmashCutJumpFrame = 15;
    m_nSmashCutHoldFrame = 20;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 5;
    m_fSmashCutHeight = 0.45f;
    m_fSmashCutSpeed = 0.03f;
    m_nSmashCutJumpFrame = 15;
    m_fSmashCutSpeed = 0.03f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 12.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Kuroma_NORMAL_STAR::CC_PARAM_BASKET_Kuroma_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 6.3f;
    m_fMoveSpeedKind1 = 5.9f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 5.0f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 12.0f;
    m_nDunkFrame = 110;
}

CC_PARAM_BASKET_Kuroma_NORMAL::CC_PARAM_BASKET_Kuroma_NORMAL() {
    m_fMoveSpeedKind0 = 5.7f;
    m_fMoveSpeedKind1 = 5.3f;
    m_fMoveSpeedKind2 = 5.0f;
    m_fMoveSpeedKind3 = 4.5f;
    m_fMoveSpeedKind4 = 5.9f;
    m_fMoveSpeedKind5 = 4.1f;
    m_fMoveSpeedKind6 = 10.8f;
    m_nDunkFrame = 121;
}

CC_PARAM_BASKET_Kuroma_HARD_STAR::CC_PARAM_BASKET_Kuroma_HARD_STAR() {
    m_fMoveSpeedKind0 = 6.7f;
    m_fMoveSpeedKind1 = 6.3f;
    m_fMoveSpeedKind2 = 5.9f;
    m_fMoveSpeedKind3 = 5.3f;
    m_fMoveSpeedKind4 = 6.9f;
    m_fMoveSpeedKind5 = 4.9f;
    m_fMoveSpeedKind6 = 12.7f;
    m_nDunkFrame = 105;
}

CC_PARAM_BASKET_Kuroma_HARD::CC_PARAM_BASKET_Kuroma_HARD() {}

CC_PARAM_BASKET_Kuroma_EXPERT_STAR::CC_PARAM_BASKET_Kuroma_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 6.7f;
    m_fMoveSpeedKind1 = 6.3f;
    m_fMoveSpeedKind2 = 5.9f;
    m_fMoveSpeedKind3 = 5.3f;
    m_fMoveSpeedKind4 = 6.9f;
    m_fMoveSpeedKind5 = 4.9f;
    m_fMoveSpeedKind6 = 12.7f;
    m_nDunkFrame = 105;
}

CC_PARAM_BASKET_Kuroma_EXPERT::CC_PARAM_BASKET_Kuroma_EXPERT() {}

} // namespace cc_data_basket_Kuroma

// --------------------------------------------------------------------------
// Mii00

void *CChrCoreBasketMii00::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Mii00;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Mii00_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Mii00_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Mii00_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Mii00_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Mii00_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Mii00_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Mii00_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Mii00_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Mii00 {

CC_PARAM_BASKET_Mii00_EASY_STAR::CC_PARAM_BASKET_Mii00_EASY_STAR() {
    m_fMoveSpeedKind0 = 5.9f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 4.8f;
    m_fMoveSpeedKind3 = 4.7f;
    m_fMoveSpeedKind4 = 5.9f;
    m_fMoveSpeedKind5 = 4.1f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 61;
}

CC_PARAM_BASKET_Mii00_EASY::CC_PARAM_BASKET_Mii00_EASY() {
    m_fMoveSpeedKind0 = 5.2f;
    m_fMoveSpeedKind1 = 4.8f;
    m_fMoveSpeedKind2 = 4.2f;
    m_fMoveSpeedKind3 = 4.2f;
    m_fMoveSpeedKind4 = 5.2f;
    m_fMoveSpeedKind5 = 3.7f;
    m_fMoveSpeedKind6 = 8.8f;
    m_nDunkFrame = 66;
}

CC_PARAM_BASKET_Mii00::CC_PARAM_BASKET_Mii00() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 6.5f;
    m_fMoveSpeedKind1 = 5.6f;
    m_fMoveSpeedKind2 = 5.3f;
    m_fMoveSpeedKind3 = 5.2f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 11.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.0f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.0f;
    m_fUnk034 = 1.0f;
    m_nDashFrame = 20;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 5;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 25;
    m_fTurnDist = 1.3f;
    m_nTrickMoveFrame = 34;
    m_fTrickMoveUnk54 = 0.16f;
    m_fTrickMoveUnk58 = 0.1f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 2;
    m_nUnk064 = 20;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 9;
    m_nUnk07C = 17;
    m_fUnk080 = 3.2f;
    m_fUnk084 = 0.1f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 38;
    m_fUnk090 = 0.9f;
    m_fUnk094 = 2.4f;
    m_nUnk0A8 = 0;
    m_nShotKind = 0;
    m_fShotSpeed = 10.8f;
    m_fShotCurve = 3.3f;
    m_nUnk0B8 = 22;
    m_fUnk0BC = 2.4f;
    m_nUnk0C0 = 11;
    m_fUnk0C4 = 0.055f;
    m_nUnk0C8 = 20;
    m_fUnk0CC = 0.5f;
    m_nUnk0D0 = 34;
    m_nJustShotKind = 0;
    m_fJustShotSpeed = 15.0f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 35;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 28;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.04f;
    m_nUnk0F8 = 46;
    m_fUnk0FC = 1.5f;
    m_nUnk100 = 38;
    m_nUnk104 = 8;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.0f;
    m_nDunkFrame = 55;
    m_bDunkLoop = 1;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 40;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 120;
    m_bAlleyoopLoop = 1;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.04f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 0;
    m_fLayUpHeight = 0.7f;
    m_nLayUpJumpFrame = 32;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.075f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 38;
    m_nJumpUpperRotBackDeg = 18;
    m_nJumpUpperJumpFrame = 18;
    m_nJumpUpperHoldFrame = 17;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 18;
    m_nJumpUpperWinStart = 0;
    m_nJumpUpperWinEnd = 35;
    m_fJumpUpperHeight = 0.3f;
    m_nJumpUpperCollType = 0;
    m_fUnk1B4 = 0.7f;
    m_fJumpUpperCollHeight = 1.0f;
    m_fUnk1BC = 1.0f;
    m_nJumpUpperSwapCount = 5;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 10;
    m_nSmashCutUnk1D0 = 35;
    m_fSmashCutRadius = 3.0f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 38;
    m_nSmashCutRotBackDeg = 18;
    m_nSmashCutJumpFrame = 15;
    m_nSmashCutHoldFrame = 20;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 5;
    m_fSmashCutHeight = 0.45f;
    m_fSmashCutSpeed = 0.03f;
    m_nSmashCutJumpFrame = 15;
    m_fSmashCutSpeed = 0.03f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 15.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Mii00_NORMAL_STAR::CC_PARAM_BASKET_Mii00_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 6.5f;
    m_fMoveSpeedKind1 = 5.6f;
    m_fMoveSpeedKind2 = 5.3f;
    m_fMoveSpeedKind3 = 5.2f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 55;
}

CC_PARAM_BASKET_Mii00_NORMAL::CC_PARAM_BASKET_Mii00_NORMAL() {
    m_fMoveSpeedKind0 = 5.9f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 4.8f;
    m_fMoveSpeedKind3 = 4.7f;
    m_fMoveSpeedKind4 = 5.9f;
    m_fMoveSpeedKind5 = 4.1f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 61;
}

CC_PARAM_BASKET_Mii00_HARD_STAR::CC_PARAM_BASKET_Mii00_HARD_STAR() {
    m_fMoveSpeedKind0 = 6.9f;
    m_fMoveSpeedKind1 = 5.9f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.9f;
    m_fMoveSpeedKind5 = 4.9f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 52;
}

CC_PARAM_BASKET_Mii00_HARD::CC_PARAM_BASKET_Mii00_HARD() {}

CC_PARAM_BASKET_Mii00_EXPERT_STAR::CC_PARAM_BASKET_Mii00_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 6.9f;
    m_fMoveSpeedKind1 = 5.9f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.9f;
    m_fMoveSpeedKind5 = 4.9f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 52;
}

CC_PARAM_BASKET_Mii00_EXPERT::CC_PARAM_BASKET_Mii00_EXPERT() {}

} // namespace cc_data_basket_Mii00

// --------------------------------------------------------------------------
// Mii01

void *CChrCoreBasketMii01::NewCcParam2_(long nHeap) const {
    using namespace cc_data_basket_Mii01;

    if (IsStarPlayer()) {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Mii01_EASY_STAR;
            case 1: return new (nHeap) CC_PARAM_BASKET_Mii01_NORMAL_STAR;
            case 2: return new (nHeap) CC_PARAM_BASKET_Mii01_HARD_STAR;
            case 3: return new (nHeap) CC_PARAM_BASKET_Mii01_EXPERT_STAR;
        }
    } else {
        switch (CGame::GetGameLevel()) {
            case 0: return new (nHeap) CC_PARAM_BASKET_Mii01_EASY;
            case 1: return new (nHeap) CC_PARAM_BASKET_Mii01_NORMAL;
            case 2: return new (nHeap) CC_PARAM_BASKET_Mii01_HARD;
            case 3: return new (nHeap) CC_PARAM_BASKET_Mii01_EXPERT;
        }
    }
    return 0;
}

namespace cc_data_basket_Mii01 {

CC_PARAM_BASKET_Mii01_EASY_STAR::CC_PARAM_BASKET_Mii01_EASY_STAR() {
    m_fMoveSpeedKind0 = 5.9f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 4.8f;
    m_fMoveSpeedKind3 = 4.7f;
    m_fMoveSpeedKind4 = 5.9f;
    m_fMoveSpeedKind5 = 4.1f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 61;
}

CC_PARAM_BASKET_Mii01_EASY::CC_PARAM_BASKET_Mii01_EASY() {
    m_fMoveSpeedKind0 = 5.2f;
    m_fMoveSpeedKind1 = 4.8f;
    m_fMoveSpeedKind2 = 4.2f;
    m_fMoveSpeedKind3 = 4.2f;
    m_fMoveSpeedKind4 = 5.2f;
    m_fMoveSpeedKind5 = 3.7f;
    m_fMoveSpeedKind6 = 8.8f;
    m_nDunkFrame = 66;
}

CC_PARAM_BASKET_Mii01::CC_PARAM_BASKET_Mii01() {
    m_nBodySize = 1;
    m_fMoveSpeedKind0 = 6.5f;
    m_fMoveSpeedKind1 = 5.6f;
    m_fMoveSpeedKind2 = 5.3f;
    m_fMoveSpeedKind3 = 5.2f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 11.0f;
    m_fUnk020 = 0.125f;
    m_fUnk024 = 1.0f;
    m_fUnk028 = 1.0f;
    m_fUnk02C = 1.0f;
    m_fUnk030 = 1.0f;
    m_fUnk034 = 1.0f;
    m_nDashFrame = 20;
    m_nDashEffFrame = 20;
    m_nDashTurnFrame = 5;
    m_nDashTurnDeg = 100;
    m_nTurnFrame = 25;
    m_fTurnDist = 1.3f;
    m_nTrickMoveFrame = 34;
    m_fTrickMoveUnk54 = 0.16f;
    m_fTrickMoveUnk58 = 0.1f;
    m_nTrickMoveRotDeg = 39;
    m_nUnk060 = 2;
    m_nUnk064 = 20;
    m_fUnk068 = 2.6f;
    m_fUnk06C = 0.05f;
    m_fUnk070 = 0.55f;
    m_fUnk074 = 0.5f;
    m_nUnk078 = 9;
    m_nUnk07C = 17;
    m_fUnk080 = 3.2f;
    m_fUnk084 = 0.1f;
    m_nJumpBlockDefStart = 0;
    m_nJumpBlockDefEnd = 38;
    m_fUnk090 = 0.9f;
    m_fUnk094 = 2.4f;
    m_nUnk0A8 = 0;
    m_nShotKind = 0;
    m_fShotSpeed = 10.8f;
    m_fShotCurve = 3.3f;
    m_nUnk0B8 = 22;
    m_fUnk0BC = 2.4f;
    m_nUnk0C0 = 11;
    m_fUnk0C4 = 0.055f;
    m_nUnk0C8 = 20;
    m_fUnk0CC = 0.5f;
    m_nUnk0D0 = 34;
    m_nJustShotKind = 0;
    m_fJustShotSpeed = 15.0f;
    m_nUnk0DC = 22;
    m_nUnk0E0 = 35;
    m_nUnk0E4 = 22;
    m_nUnk0E8 = 28;
    m_nUnk0EC = 22;
    m_nUnk0F0 = 22;
    m_fUnk0F4 = 0.03f;
    m_nUnk0F8 = 51;
    m_fUnk0FC = 1.0f;
    m_nUnk100 = 43;
    m_nUnk104 = 8;
    m_fUnk108 = 1.0f;
    m_fDunkHeight = 4.0f;
    m_nDunkFrame = 55;
    m_bDunkLoop = 1;
    m_fReboundDunkHeight = 4.0f;
    m_nReboundDunkFrame = 40;
    m_fUnk120 = 0.5f;
    m_fUnk124 = 0.5f;
    m_nUnk128 = 0;
    m_nAlleyoopFrame = 120;
    m_bAlleyoopLoop = 1;
    m_fAlleyoopHeight = 4.0f;
    m_nUnk138 = 50;
    m_nUnk13C = 50;
    m_fUnk140 = 4.0f;
    m_fUnk144 = 0.01f;
    m_fUnk148 = -0.01f;
    m_fUnk14C = 2.0f;
    m_fUnk150 = 3.5f;
    m_fUnk154 = 3.25f;
    m_fUnk158 = 3.0f;
    m_fLayUpSpeed = 0.04f;
    m_nLayUpRotDeg = 25;
    m_fLayUpMotSpeed = 1.1f;
    m_nLayUpHoldFrame = 0;
    m_fLayUpHeight = 0.7f;
    m_nLayUpJumpFrame = 32;
    m_fUnk174 = 0.4f;
    m_fUnk178 = 0.0f;
    m_nUnk17C = 3;
    m_nUnk180 = 15;
    m_fUnk184 = 0.075f;
    m_nSwatItBlend = 4;
    m_nJumpUpperRotDeg = 38;
    m_nJumpUpperRotBackDeg = 18;
    m_nJumpUpperJumpFrame = 18;
    m_nJumpUpperHoldFrame = 17;
    m_nJumpUpperTurnFrame = 0;
    m_nJumpUpperFallFrame = 18;
    m_nJumpUpperWinStart = 0;
    m_nJumpUpperWinEnd = 35;
    m_fJumpUpperHeight = 0.3f;
    m_nJumpUpperCollType = 0;
    m_fUnk1B4 = 0.7f;
    m_fJumpUpperCollHeight = 1.0f;
    m_fUnk1BC = 1.0f;
    m_nJumpUpperSwapCount = 5;
    m_nJumpUpperTurnRate = 0;
    m_nUnk1C8 = 150;
    m_nSmashCutUnk1CC = 10;
    m_nSmashCutUnk1D0 = 35;
    m_fSmashCutRadius = 3.0f;
    m_nUnk1D8 = 0;
    m_nSmashCutRotDeg = 38;
    m_nSmashCutRotBackDeg = 18;
    m_nSmashCutJumpFrame = 15;
    m_nSmashCutHoldFrame = 20;
    m_nSmashCutTurnFrame = 0;
    m_nSmashCutFallFrame = 5;
    m_fSmashCutHeight = 0.45f;
    m_fSmashCutSpeed = 0.03f;
    m_nSmashCutJumpFrame = 15;
    m_fSmashCutSpeed = 0.03f;
    m_fUnk1FC = 0.0f;
    m_fUnk200 = 3.5f;
    m_fUnk204 = 2.4f;
    m_fUnk208 = 1.0f;
    m_fUnk20C = 1.4f;
    m_fArtsJumpHeight = 4.0f;
    m_fArtsJumpTopFrame = 0.0f;
    m_fArtsSetbackMin = 6.0f;
    m_fArtsSetbackMax = 2.5f;
    m_fArtsSetbackRotDeg = 45.0f;
    m_fArtsSetbackRotDist = 1.5f;
    m_fArtsMoveEndFrame = 0.0f;
    m_fArtsWaitFrame = 30.0f;
    m_fArtsFallFrame = 30.0f;
    m_fArtsFallHeight = 0.5f;
    m_fArtsBallUnk240 = 15.0f;
    m_fArtsHitbackFrame = 20.0f;
    m_fArtsHitbackDist = 3.0f;
    m_fArtsHitbackJump0 = 14.0f;
    m_fArtsHitbackJump1 = 6.0f;
    m_fArtsHitbackJump2 = 1.0f;
    m_fArtsTrickBlend = 4.0f;
    m_fArtsLoopBlend = 4.0f;
    m_fArtsShotBlend = 0.0f;
    m_fArtsShotBlendNoLoop = 4.0f;
    m_fArtsLandBlend = 2.0f;
    m_nArtsShotReflectDisableFrame = 10;
    m_fArtsShotReflectDisableArea = 0.0f;
}

CC_PARAM_BASKET_Mii01_NORMAL_STAR::CC_PARAM_BASKET_Mii01_NORMAL_STAR() {
    m_fMoveSpeedKind0 = 6.5f;
    m_fMoveSpeedKind1 = 5.6f;
    m_fMoveSpeedKind2 = 5.3f;
    m_fMoveSpeedKind3 = 5.2f;
    m_fMoveSpeedKind4 = 6.5f;
    m_fMoveSpeedKind5 = 4.6f;
    m_fMoveSpeedKind6 = 11.0f;
    m_nDunkFrame = 55;
}

CC_PARAM_BASKET_Mii01_NORMAL::CC_PARAM_BASKET_Mii01_NORMAL() {
    m_fMoveSpeedKind0 = 5.9f;
    m_fMoveSpeedKind1 = 5.0f;
    m_fMoveSpeedKind2 = 4.8f;
    m_fMoveSpeedKind3 = 4.7f;
    m_fMoveSpeedKind4 = 5.9f;
    m_fMoveSpeedKind5 = 4.1f;
    m_fMoveSpeedKind6 = 9.9f;
    m_nDunkFrame = 61;
}

CC_PARAM_BASKET_Mii01_HARD_STAR::CC_PARAM_BASKET_Mii01_HARD_STAR() {
    m_fMoveSpeedKind0 = 6.9f;
    m_fMoveSpeedKind1 = 5.9f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.9f;
    m_fMoveSpeedKind5 = 4.9f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 52;
}

CC_PARAM_BASKET_Mii01_HARD::CC_PARAM_BASKET_Mii01_HARD() {}

CC_PARAM_BASKET_Mii01_EXPERT_STAR::CC_PARAM_BASKET_Mii01_EXPERT_STAR() {
    m_fMoveSpeedKind0 = 6.9f;
    m_fMoveSpeedKind1 = 5.9f;
    m_fMoveSpeedKind2 = 5.6f;
    m_fMoveSpeedKind3 = 5.5f;
    m_fMoveSpeedKind4 = 6.9f;
    m_fMoveSpeedKind5 = 4.9f;
    m_fMoveSpeedKind6 = 11.7f;
    m_nDunkFrame = 52;
}

CC_PARAM_BASKET_Mii01_EXPERT::CC_PARAM_BASKET_Mii01_EXPERT() {}

} // namespace cc_data_basket_Mii01

// --------------------------------------------------------------------------
// The collision sizes. Middle (M) values first; a small (S) body is the
// middle one scaled by getBodyScaleS (1, so the same); large (L) values.

float CC_DATA_BASKET::getCcBodyBaseXSizeM() { return 0.5f; }
float CC_DATA_BASKET::getCcBodyBaseZSizeM() { return 0.5f; }
float CC_DATA_BASKET::getCcBodyCatchXSizeM() { return 0.7f; }
float CC_DATA_BASKET::getCcBodyCatchZSizeM() { return 0.7f; }
float CC_DATA_BASKET::getCcBodyDBXSizeM() { return 0.7f; }
float CC_DATA_BASKET::getCcBodyDBZSizeM() { return 0.7f; }
float CC_DATA_BASKET::getCcBodyBWXSizeM() { return 1.2f; }
float CC_DATA_BASKET::getCcBodyBWZSizeM() { return 0.5f; }
float CC_DATA_BASKET::getCcBodyLieXSizeM() { return 0.5f; }
float CC_DATA_BASKET::getCcBodyLieZSizeM() { return 0.8f; }
float CC_DATA_BASKET::getCcBodyDBPosShiftM() { return 0.5f; }
float CC_DATA_BASKET::getCcBodyCatch2XSizeM() { return 0.8f; }
float CC_DATA_BASKET::getCcBodyCatch2ZSizeM() { return 0.7f; }
float CC_DATA_BASKET::getCcBodyCatch2OffsetM() { return 0.2f; }

float CC_DATA_BASKET::getBodyScaleS() { return 1.0f; }

float CC_DATA_BASKET::getCcBodyBaseXSizeS() { return getCcBodyBaseXSizeM() * getBodyScaleS(); }
float CC_DATA_BASKET::getCcBodyBaseZSizeS() { return getCcBodyBaseZSizeM() * getBodyScaleS(); }
float CC_DATA_BASKET::getCcBodyCatchXSizeS() { return getCcBodyCatchXSizeM() * getBodyScaleS(); }
float CC_DATA_BASKET::getCcBodyCatchZSizeS() { return getCcBodyCatchZSizeM() * getBodyScaleS(); }
float CC_DATA_BASKET::getCcBodyDBXSizeS() { return getCcBodyDBXSizeM() * getBodyScaleS(); }
float CC_DATA_BASKET::getCcBodyDBZSizeS() { return getCcBodyDBZSizeM() * getBodyScaleS(); }
float CC_DATA_BASKET::getCcBodyBWXSizeS() { return getCcBodyBWXSizeM() * getBodyScaleS(); }
float CC_DATA_BASKET::getCcBodyBWZSizeS() { return getCcBodyBWZSizeM() * getBodyScaleS(); }
float CC_DATA_BASKET::getCcBodyLieXSizeS() { return getCcBodyLieXSizeM() * getBodyScaleS(); }
float CC_DATA_BASKET::getCcBodyLieZSizeS() { return getCcBodyLieZSizeM() * getBodyScaleS(); }
float CC_DATA_BASKET::getCcBodyDBPosShiftS() { return getCcBodyDBPosShiftM() * getBodyScaleS(); }
float CC_DATA_BASKET::getCcBodyCatch2XSizeS() { return getCcBodyCatch2XSizeM() * getBodyScaleS(); }
float CC_DATA_BASKET::getCcBodyCatch2ZSizeS() { return getCcBodyCatch2ZSizeM() * getBodyScaleS(); }
float CC_DATA_BASKET::getCcBodyCatch2OffsetS() { return getCcBodyCatch2OffsetM() * getBodyScaleS(); }

float CC_DATA_BASKET::getCcBodyBaseXSizeL() { return 0.9f; }
float CC_DATA_BASKET::getCcBodyBaseZSizeL() { return 0.9f; }
float CC_DATA_BASKET::getCcBodyCatchXSizeL() { return 0.9f; }
float CC_DATA_BASKET::getCcBodyCatchZSizeL() { return 0.9f; }
float CC_DATA_BASKET::getCcBodyDBXSizeL() { return 0.9f; }
float CC_DATA_BASKET::getCcBodyDBZSizeL() { return 0.9f; }
float CC_DATA_BASKET::getCcBodyBWXSizeL() { return 1.2f; }
float CC_DATA_BASKET::getCcBodyBWZSizeL() { return 0.5f; }
float CC_DATA_BASKET::getCcBodyLieXSizeL() { return 0.7f; }
float CC_DATA_BASKET::getCcBodyLieZSizeL() { return 0.9f; }
float CC_DATA_BASKET::getCcBodyDBPosShiftL() { return 0.5f; }
float CC_DATA_BASKET::getCcBodyCatch2XSizeL() { return 0.8f; }
float CC_DATA_BASKET::getCcBodyCatch2ZSizeL() { return 0.7f; }
float CC_DATA_BASKET::getCcBodyCatch2OffsetL() { return 0.2f; }

// --------------------------------------------------------------------------
// GetCcParam_

namespace {
    // (field names guessed) What GetCcParam_ returns for every character.
    // Other units read its start as the jump parameters
    // (CC_DATA::CHR_PARAM::JUMP_PARAM: 16 frames, 3.0); the rest is typed
    // after its bytes only.
    struct CC_PARAM_BASKET {
        float m_fJumpFrame;      // 0x00
        float m_fJumpUnk04;      // 0x04
        float m_fUnk08;          // 0x08
        unsigned char m_nUnk0C;  // 0x0C
        float m_fUnk10;          // 0x10
        long m_nUnk14;           // 0x14
        short m_nUnk18;          // 0x18
    };

    const CC_PARAM_BASKET s_CcParam = { 16.0f, 3.0f, 0.0f, 2, 0.0f, 4, 2 };
}

const void *CChrCoreBasket::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketMario::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketLuigi::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketPeach::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketDaisy::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketYoshi::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketWario::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketWaluigi::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketDonkey::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketDiddy::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketKinopio::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketKoopa::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketKoopaJr::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketMoogle::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketSaboten::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketNinja::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketSiroma::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketSlime::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketKuroma::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketMii00::GetCcParam_() const { return &s_CcParam; }
const void *CChrCoreBasketMii01::GetCcParam_() const { return &s_CcParam; }

// --------------------------------------------------------------------------
// CChrCoreBasket

// (field names guessed) The collision sizes of one body size class, each
// FVEC2 an X and a Z size.
struct CChrCore::PRM_BODY_SIZE {
    bcs::FVEC2 m_BaseSize;     // 0x00
    bcs::FVEC2 m_CatchSize;    // 0x08
    bcs::FVEC2 m_DBSize;       // 0x10
    bcs::FVEC2 m_BWSize;       // 0x18
    bcs::FVEC2 m_LieSize;      // 0x20
    float m_fDBPosShift;       // 0x28
    float m_fCatch2XSize;      // 0x2C
    float m_fCatch2ZSize;      // 0x30
    float m_fCatch2Offset;     // 0x34
};

namespace {
    // Small, middle and large (CC_PARAM_BASKET2::m_nBodySize). Filled by
    // the static initialiser.
    CChrCore::PRM_BODY_SIZE s_aBodySize[3] = {
        {
            bcs::FVEC2(CC_DATA_BASKET::getCcBodyBaseXSizeS(), CC_DATA_BASKET::getCcBodyBaseZSizeS()),
            bcs::FVEC2(CC_DATA_BASKET::getCcBodyCatchXSizeS(), CC_DATA_BASKET::getCcBodyCatchZSizeS()),
            bcs::FVEC2(CC_DATA_BASKET::getCcBodyDBXSizeS(), CC_DATA_BASKET::getCcBodyDBZSizeS()),
            bcs::FVEC2(CC_DATA_BASKET::getCcBodyBWXSizeS(), CC_DATA_BASKET::getCcBodyBWZSizeS()),
            bcs::FVEC2(CC_DATA_BASKET::getCcBodyLieXSizeS(), CC_DATA_BASKET::getCcBodyLieZSizeS()),
            CC_DATA_BASKET::getCcBodyDBPosShiftS(),
            CC_DATA_BASKET::getCcBodyCatch2XSizeS(),
            CC_DATA_BASKET::getCcBodyCatch2ZSizeS(),
            CC_DATA_BASKET::getCcBodyCatch2OffsetS(),
        },
        {
            bcs::FVEC2(CC_DATA_BASKET::getCcBodyBaseXSizeM(), CC_DATA_BASKET::getCcBodyBaseZSizeM()),
            bcs::FVEC2(CC_DATA_BASKET::getCcBodyCatchXSizeM(), CC_DATA_BASKET::getCcBodyCatchZSizeM()),
            bcs::FVEC2(CC_DATA_BASKET::getCcBodyDBXSizeM(), CC_DATA_BASKET::getCcBodyDBZSizeM()),
            bcs::FVEC2(CC_DATA_BASKET::getCcBodyBWXSizeM(), CC_DATA_BASKET::getCcBodyBWZSizeM()),
            bcs::FVEC2(CC_DATA_BASKET::getCcBodyLieXSizeM(), CC_DATA_BASKET::getCcBodyLieZSizeM()),
            CC_DATA_BASKET::getCcBodyDBPosShiftM(),
            CC_DATA_BASKET::getCcBodyCatch2XSizeM(),
            CC_DATA_BASKET::getCcBodyCatch2ZSizeM(),
            CC_DATA_BASKET::getCcBodyCatch2OffsetM(),
        },
        {
            bcs::FVEC2(CC_DATA_BASKET::getCcBodyBaseXSizeL(), CC_DATA_BASKET::getCcBodyBaseZSizeL()),
            bcs::FVEC2(CC_DATA_BASKET::getCcBodyCatchXSizeL(), CC_DATA_BASKET::getCcBodyCatchZSizeL()),
            bcs::FVEC2(CC_DATA_BASKET::getCcBodyDBXSizeL(), CC_DATA_BASKET::getCcBodyDBZSizeL()),
            bcs::FVEC2(CC_DATA_BASKET::getCcBodyBWXSizeL(), CC_DATA_BASKET::getCcBodyBWZSizeL()),
            bcs::FVEC2(CC_DATA_BASKET::getCcBodyLieXSizeL(), CC_DATA_BASKET::getCcBodyLieZSizeL()),
            CC_DATA_BASKET::getCcBodyDBPosShiftL(),
            CC_DATA_BASKET::getCcBodyCatch2XSizeL(),
            CC_DATA_BASKET::getCcBodyCatch2ZSizeL(),
            CC_DATA_BASKET::getCcBodyCatch2OffsetL(),
        },
    };

    // In the original bcs::FVEC3 derives from nw4r::math::VEC3 and the
    // distances below go through VEC3's operator-. Here FVEC3 is a separate
    // struct of the same layout, so the operands are reinterpreted (as in
    // game/basket/CChrCoreBasketNinja.cpp).
    typedef const nw4r::math::VEC3 &VEC3_CREF;
}

const CChrCore::PRM_BODY_SIZE *CChrCoreBasket::GetPrmBodySize_(long nSize) const
{
    return &s_aBodySize[nSize];
}

const CChrCore::PRM_BODY_SIZE *CChrCoreBasket::GetPrmBodySize() const
{
    return GetPrmBodySize_(GetBodySize());
}

float CChrCoreBasket::GetDefenseSpeedUpRate_() const
{
    return m_fDefenseSpeedUpRate;
}

// Flag 0x200 of m_nBasketFlag: this character guards the ball holder of
// the other team (nearer than the holder to the goal the holder attacks,
// or close to that goal). Flag 0x400, with the speed-up: it is farther
// from that goal than the holder and catches up.
void CChrCoreBasket::UpDateDefenseSpeedUpRate()
{
    const CChrCore *pHolder = CChrMan::Util_SearchHoldBallChr(0);
    int bNone;

    m_fDefenseSpeedUpRate = 0.0f;
    m_nBasketFlag &= ~0x400;
    bNone = 0;
    if (pHolder == 0) {
        bNone = 1;
    } else if (pHolder->GetTeamNo() == GetTeamNo()) {
        bNone = 1;
    }
    if (bNone) {
        m_nBasketFlag &= ~0x200;
        return;
    }

    const bcs::FVEC3 &rGoal = pHolder->GetTargetGoalPos();
    if (m_nBasketFlag & 0x200) {
        float fMyDist = bcs::BCS_Vec_GetLenXZ(bcs::FVEC3((VEC3_CREF)rGoal - (VEC3_CREF)GetPos()));
        if (fMyDist > bcs::BCS_Vec_GetLenXZ(bcs::FVEC3((VEC3_CREF)rGoal - (VEC3_CREF)pHolder->GetPos()))) {
            m_nBasketFlag &= ~0x200;
        }
    } else if (3.0f > bcs::BCS_Vec_GetLenXZ(
                          bcs::FVEC3((VEC3_CREF)pHolder->GetTargetGoalPos() - (VEC3_CREF)GetPos()))) {
        m_nBasketFlag |= 0x200;
    } else {
        float fHolderDist =
            bcs::BCS_Vec_GetLenXZ(bcs::FVEC3((VEC3_CREF)rGoal - (VEC3_CREF)pHolder->GetPos())) - 1.0f;
        if (fHolderDist < 0.0f) {
            fHolderDist = 0.0f;
        }
        if (bcs::BCS_Vec_GetLenXZ(bcs::FVEC3((VEC3_CREF)rGoal - (VEC3_CREF)GetPos())) < fHolderDist) {
            m_nBasketFlag |= 0x200;
        } else {
            m_nBasketFlag |= 0x400;
            m_fDefenseSpeedUpRate = 0.3f;
        }
    }
}

float CChrCoreBasket::GetCollBodySizeY() const
{
    return 1.8f;
}

long CChrCoreBasket::GetBodySize() const
{
    return GetCcParam2_()->m_nBodySize;
}

int CChrCoreBasket::IsJustShootTypeCurve() const
{
    return GetCcParam2_()->m_nJustShotKind == 2;
}

float CChrCoreBasket::GetJustShootCurveValue() const
{
    return GetCcParam2_()->m_fShotCurve;
}

// The speed of move kind `kind` per frame, with the condition's factor and
// the defence speed-up.
float CChrCoreBasket::GetMoveSpeedKind(MOVE_SPEED_KIND kind) const
{
    float fSpeed = 0.0f;

    switch (kind) {
        case 0:
            fSpeed = GetCcParam2_()->m_fMoveSpeedKind0;
            break;
        case 1:
            fSpeed = GetCcParam2_()->m_fMoveSpeedKind1;
            break;
        case 2:
            fSpeed = GetCcParam2_()->m_fMoveSpeedKind2;
            break;
        case 3:
            fSpeed = GetCcParam2_()->m_fMoveSpeedKind3;
            break;
        case 4:
            fSpeed = GetCcParam2_()->m_fMoveSpeedKind4;
            break;
        case 5:
            fSpeed = GetCcParam2_()->m_fMoveSpeedKind5;
            break;
        case 6:
            fSpeed = GetCcParam2_()->m_fMoveSpeedKind6;
            break;
        default:
            return 0.0f;
    }
    fSpeed = CalcConditionMoveSpeedRate_() * ((1.0f / 60.0f) * fSpeed);
    return fSpeed * GetDefenseSpeedUpRate_() + fSpeed;
}

float CChrCoreBasket::GetArtsShotRefelectDisableFrame() const
{
    return GetCcParam2_()->m_nArtsShotReflectDisableFrame;
}

float CChrCoreBasket::GetArtsShotRefelectDisableArea() const
{
    return GetCcParam2_()->m_fArtsShotReflectDisableArea;
}

// (meaning of the conditions not known)
float CChrCoreBasket::CalcConditionMoveSpeedRate_() const
{
    switch (GetCondition()) {
        case 1:
            return 1.6f;
        case 5:
            return 0.0f;
        case 4:
            return 1.3f;
        case 6:
            return 0.5f;
        case 7:
            return 0.6f;
        case 8:
            return 0.6f;
        case 9:
            return 1.0f;
        case 10:
            return 0.8f;
        default:
            if (IsStateSmallBody()) {
                return 0.8f;
            }
            return 1.0f;
    }
}

float CChrCoreBasket::GetStateWorkTrickMoveRemainderFrame() const
{
    if (GetState() != STATE_TRICK_MOVE) {
        return 0.0f;
    }
    return FsmqGetWork<const STATE_WORK_TRICK_MOVE>()->m_fRemainderFrame;
}

// Asks once per target character, while the move is young: against a
// block cut 81 times in 101, against a running character, when the move
// goes round it on the side it turns from (within 40 degrees), once in 101.
int CChrCoreBasket::CheckStateWorkTrickMoveSlow(float fRotY, const CChrCore *pTarget)
{
    if (pTarget == 0) {
        return 0;
    }
    if (IsPadCtrl() == 0) {
        return 0;
    }
    if (GetState() != STATE_TRICK_MOVE) {
        return 0;
    }
    STATE_WORK_TRICK_MOVE *pWork = FsmqGetWork<STATE_WORK_TRICK_MOVE>();
    if (pWork->m_fFrame >= 30.0f) {
        return 0;
    }
    unsigned long nBit = 1 << pTarget->GetChrIdx();
    switch (pTarget->GetState()) {
        case STATE_BLOCK_CUT:
            if (pWork->m_nSlowBlockCutBit & nBit) {
                return 0;
            }
            pWork->m_nSlowBlockCutBit |= nBit;
            if (bcs::BCS_RandMax(100L) > 80) {
                return 0;
            }
            break;
        case STATE_RUN: {
            float fRot = bcs::ROUND_PI(fRotY - pTarget->GetRotY());
            if (fRot > 3.1415927f) {
                fRot -= 6.2831855f;
            }
            if (fRot > 0.0f) {
                fRot = -1.5707964f;
            } else {
                fRot = 1.5707964f;
            }
            fRot = bcs::ROUND_PI(fRot + pTarget->GetRotY());
            fRot = bcs::ROUND_PI(pTarget->GetRotY_ToPosXZ(GetPos()) - fRot);
            if (fRot > 3.1415927f) {
                fRot -= 6.2831855f;
            }
            if (bcs::BCS_Abs(fRot) > bcs::DEG_TO_RAD(40.0f)) {
                return 0;
            }
            if (pWork->m_nSlowRunBit & nBit) {
                return 0;
            }
            pWork->m_nSlowRunBit |= nBit;
            if (bcs::BCS_RandMax(100L) > 0) {
                return 0;
            }
        } break;
        default:
            return 0;
    }
    return 1;
}

} // namespace msw
