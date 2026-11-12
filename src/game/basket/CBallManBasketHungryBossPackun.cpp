// msw::CBallManBasketHungryBossPackun: the ball manager of the basket party
// game against the hungry boss Piranha Plant, and msw::CNpcStringBasket, the
// strings that hang its fruit over the court.
#include "game/msw/CBallManBasketHungryBossPackun.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Util.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/BasketPrm.hpp"
#include "game/msw/CBallBasketFruit.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitBasket.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBGM.hpp"
#include "game/msw/CGameBasket.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CHBData.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CNpcString.hpp"
#include "nw4r/math/triangular.h"

namespace CBallManBasketHungryBossPackun_cpp {
// uncalled stand-in / pool-off stand-in: the original addresses every float
// constant of this file with its own lis, which this compiler does when the
// first .rodata object of the file is weak (a function-local const). The
// module link strips this function and its constant.
void UnusedPoolOff()
{
    const long k = 0;
}
}

// Set by the file's static initialiser and used by nothing in the file (see
// game/basket/CBallManBasket.cpp: most likely from a shared header).
namespace {
    float s_fAngle70 = bcs::DEG_TO_RAD(70.0f);
}

namespace {
    // The cockpit object of the bonus time telop.
    enum {
        COCKPIT_BONUS_TELOP = 0x19
    };

    // (name guessed) How the strings of the stage hand their fruit over:
    // with 0 a string drops its fruit when it leaves or sleeps.
    long GetDeliveryPattern()
    {
        switch (msw::CGame::g_GameWork.GetStage()) {
        case 0x46:
            return msw::BASKET_PRM_S70_DELIVERY_PATTERN;
        case 0x47:
            return msw::BASKET_PRM_S71_DELIVERY_PATTERN;
        case 0x48:
            return msw::BASKET_PRM_S72_DELIVERY_PATTERN;
        }
        return 0;
    }

    // (name guessed) The fruit in play that hang on no string.
    long GetLooseFruitNum()
    {
        long nNum = 0;
        for (long i = 0; i < msw::CBallMan::GetBallNum(); i++) {
            msw::CBall *pBall = msw::CBallMan::GetBallPtr(i);
            if (pBall != 0 && pBall->IsActive() &&
                static_cast<msw::CBallBasketFruit *>(pBall)->GetStringId() == 0) {
                nNum++;
            }
        }
        return nNum;
    }
}

namespace msw {

    // A string that hangs one fruit of the party game over the court
    // (size 0x218). It drops down with its fruit (APPEAR), waits turning
    // (IDLE), swings when its fruit is picked for the bonus (MOVE) and pulls
    // its fruit up again (WITHDRAWS). Member names are guesses.
    class CNpcStringBasket : public CNpcString {
        public:
            // nIndex: the wait entry of the party game's data it serves.
            CNpcStringBasket(long nIndex) : CNpcString(CChrMan::GetInstance())
            {
                m_nIndex = nIndex;
                m_nAppendIdx = -1;
                m_nFruitType = -1;
                m_nFlag = 0;
            }

            virtual int CanbeCatch() const;
            virtual void Reset();
            virtual long SendMessage(long nMsg, long nArg1, long nArg2, long nArg3, long nArg4);
            virtual void DtorStateWithdraws();
            virtual void LoopStateWithdraws();
            virtual void CtorStateWithdraws();
            virtual void DtorStateMove();
            virtual void LoopStateMove();
            virtual void CtorStateMove();
            virtual void LoopStateIdle();
            virtual void CtorStateAppear();
            virtual void CtorStateSleep();
            virtual void MainFunc();
            virtual int CanbeAttachBall() const { return GetAttachBall() == 0; }

            // Bits of m_nFlag.
            enum {
                FLAG_PICKED = 0x1 // picked for the bonus
            };

            // The fruit kind of a fruit bomb (CBallBasketFruit::KIND_BOMB).
            enum {
                FRUIT_BOMB = 5
            };

            // Not zero while it waits with its fruit.
            int IsWaitBall_() const { return GetAttachBall() == 0 ? 0 : GetState() == STATE_IDLE; }
            // Not zero when it can drop down with a new fruit.
            int CanbeAppear_() const { return !CanbeAttachBall() ? 0 : GetState() != STATE_APPEAR; }

            // Pulls the fruit up again (with bDrop it lets go of the fruit
            // first; a bomb tells the characters it is gone).
            void Withdraw_(int bDrop)
            {
                switch (GetState()) {
                case STATE_APPEAR:
                case STATE_IDLE:
                case STATE_MOVE:
                    if (bDrop) {
                        if (m_nFruitType == FRUIT_BOMB) {
                            CBall *pBall = GetAttachBall();
                            if (pBall != 0) {
                                CChrMan::AllSendMsg(static_cast<CChrDrv::MSG_ID>(0x34), pBall->GetBallNo(), 0, 0, 0);
                            }
                        }
                        DetachBall();
                    }
                    ChangeState(STATE_WITHDRAWS);
                    break;
                }
            }

            // Lets go of the fruit and sleeps.
            void Sleep_()
            {
                if (GetState() != STATE_SLEEP) {
                    DetachBall();
                    ChangeState(STATE_SLEEP);
                }
            }

            long m_nIndex;      // 0x1DC
            long m_nAppendIdx;  // 0x1E0, the place of the grid it hangs over; -1 for none
            long m_nFruitType;  // 0x1E4, the kind of its fruit; -1 for none
            unsigned long m_nFlag; // 0x1E8

        private:
            // Deletes the fruit when the stage does not hand it over.
            void DeleteBall_()
            {
                if (GetDeliveryPattern() == 0) {
                    CBall *pBall = GetAttachBall();
                    if (pBall != 0) {
                        DetachBall();
                        CBallMan::DelBall(pBall->GetBallNo());
                    }
                }
            }

            // Turns the string round, now and then the other way.
            void Rotate_()
            {
                if (bcs::BCS_RandMax(1000L) < 2) {
                    m_fRotDir = -m_fRotDir;
                }
                m_fRotSpeed += m_fRotDir * GetFrameRateF();
                float fMax = BASKET_PRM_HB_WAIT_STATE_ROT_MAX;
                m_fRotSpeed = m_fRotSpeed > fMax ? fMax : (m_fRotSpeed < -fMax ? -fMax : m_fRotSpeed);
                float fSpeed = bcs::DEG_TO_RAD(m_fRotSpeed);
                AddRotY_(fSpeed * GetFrameRateF());
            }

            // Brings the model's offset back to the string's place.
            bcs::FVEC3 &ReturnOffset_()
            {
                float fRate = BASKET_PRM_COMMON_WAIT_MOVE_REV_RATE;
                bcs::FVEC3 &rOfs = GetOffsetPos();
                rOfs.y *= fRate;
                if (rOfs.y >= -0.01f && rOfs.y <= 0.01f) {
                    rOfs.y = 0.0f;
                }
                return rOfs;
            }

            void AddRotY_(float fRad) { m_vRot.y = bcs::ROUND_PI(m_vRot.y + fRad); }

            static int IsZero_(float f) { return 0.0f == f; }

            // A random value between fMin and fMax.
            // (by reference: the original reads the minimum first)
            static float RandRange_(const float &rMin, const float &rMax)
            {
                float fMin = rMin;
                float fMax = rMax;
                if (fMin > fMax) {
                    fMin = fMax;
                }
                return bcs::BCS_RandRange(fMin, fMax);
            }

            // Moves the shadow towards the look of the state.
            void UpdateShadow_()
            {
                float fScale = 0.0f;
                float fA = 0.0f;
                float fR = 0.0f;
                float fG = 0.0f;
                float fB = 0.0f;

                switch (GetState()) {
                case STATE_SLEEP:
                    m_fShadowScale = fScale;
                    m_fShadowA = fScale;
                    m_fShadowR = fScale;
                    m_fShadowG = fScale;
                    m_fShadowB = fScale;
                    break;
                case STATE_MOVE:
                    if ((m_nFlag & FLAG_PICKED) && m_nFruitType != FRUIT_BOMB) {
                        fScale = BASKET_PRM_WAIT_BALL_SHADOW_SCALE_CATCH;
                        fA = BASKET_PRM_WAIT_BALL_COLOR_A_CATCH;
                        fR = BASKET_PRM_WAIT_BALL_COLOR_R_CATCH;
                        fG = BASKET_PRM_WAIT_BALL_COLOR_G_CATCH;
                        fB = BASKET_PRM_WAIT_BALL_COLOR_B_CATCH;
                        break;
                    }
                    // fall through
                case STATE_APPEAR:
                case STATE_IDLE:
                    fScale = BASKET_PRM_WAIT_BALL_SHADOW_SCALE;
                    fA = BASKET_PRM_WAIT_BALL_COLOR_A;
                    fR = BASKET_PRM_WAIT_BALL_COLOR_R;
                    fG = BASKET_PRM_WAIT_BALL_COLOR_G;
                    fB = BASKET_PRM_WAIT_BALL_COLOR_B;
                    break;
                }

                m_fShadowScale += (fScale - m_fShadowScale) * BASKET_PRM_SHADOW_SCALE_UPDATE_RATE;
                float fRate = BASKET_PRM_SHADOW_COLOR_UPDATE_RATE;
                m_fShadowA += (fA - m_fShadowA) * fRate;
                m_fShadowR += (fR - m_fShadowR) * fRate;
                m_fShadowG += (fG - m_fShadowG) * fRate;
                m_fShadowB += (fB - m_fShadowB) * fRate;
                SetShadowRate(m_fShadowA);
                SetShadowScale(m_fShadowScale);
                bcs::BCS_Color color(m_fShadowR, m_fShadowG, m_fShadowB, 0xFF);
                SetShadowColor(color);
            }

            float m_fSwingPhase;  // 0x1EC, degrees
            float m_fSwingWidth;  // 0x1F0
            float m_fSwingSpeed;  // 0x1F4, degrees per frame
            float m_fRotSpeed;    // 0x1F8, degrees per frame
            float m_fRotDir;      // 0x1FC, the change of m_fRotSpeed per frame
            float m_fShadowScale; // 0x200
            float m_fShadowA;     // 0x204
            float m_fShadowR;     // 0x208
            float m_fShadowG;     // 0x20C
            float m_fShadowB;     // 0x210
            float m_fShadowTimer; // 0x214, the shadow moves once per whole frame
    };

    CBallManBasketHungryBossPackun::CBallManBasketHungryBossPackun(bcs::CTask *pParent)
        : CBallManBasket(pParent, 32)
    {
        m_pHBData = new BA_PARTY::CHBData;
        m_ppString = 0;
        m_nPickStep = 0;
        m_nTelopStep = 0;
        m_nTurn = -1;
        m_fTelopWait = 0.0f;
    }

    CBallManBasketHungryBossPackun::~CBallManBasketHungryBossPackun()
    {
        if (m_pHBData != 0) {
            delete m_pHBData;
        }
        m_pHBData = 0;
        if (m_ppString != 0) {
            delete[] m_ppString;
        }
        m_ppString = 0;
        DeleteAllBall_();
    }

    void CBallManBasketHungryBossPackun::AllReset_()
    {
        long nNum = m_pHBData->GetWaitEntryCount();
        for (long i = 0; i < nNum; i++) {
            m_ppString[i]->Reset();
        }
        CBallManBasket::AllReset_();
        DeleteAllBall_();
        CCockpit::RemoveObj(COCKPIT_BONUS_TELOP);
        m_nTelopStep = 0;
        m_nPickStep = 0;
        m_nTurn = -1;
        m_fPickTime = 0.0f;
        m_fPickWait = 0.0f;
    }

    int CBallManBasketHungryBossPackun::CatchEnable_(const CBall *pBall) const
    {
        if (pBall == 0) {
            return 0;
        }
        return !pBall->IsActive() ? 0 : pBall->CanbeCatch() != 0;
    }

    int CBallManBasketHungryBossPackun::IsAttachString_(const CBall *pBall) const
    {
        return static_cast<const CBallBasketFruit *>(pBall)->GetStringId() != 0;
    }

    long CBallManBasketHungryBossPackun::GetFruitType_(const CBall *pBall) const
    {
        return static_cast<const CBallBasketFruit *>(pBall)->GetKind();
    }

    long CBallManBasketHungryBossPackun::GetFruitScore_(const CBall *pBall) const
    {
        return GetFruitScore_(static_cast<const CBallBasketFruit *>(pBall)->GetKind());
    }

    long CBallManBasketHungryBossPackun::GetFruitScore_(long nType) const
    {
        switch (nType) {
        case CBallBasketFruit::KIND_ORANGE:
            return BASKET_PRM_COMMON_HUNGRY_ORENGE_PT;
        case CBallBasketFruit::KIND_BLUE_APPLE:
            return BASKET_PRM_COMMON_HUNGRY_BLUE_APPLE_PT;
        case CBallBasketFruit::KIND_RED_APPLE:
            return BASKET_PRM_COMMON_HUNGRY_RED_APPLE_PT;
        case CBallBasketFruit::KIND_SUICA:
            return BASKET_PRM_COMMON_HUNGRY_SUICA_PT;
        case CBallBasketFruit::KIND_MELON:
            return 0;
        case CBallBasketFruit::KIND_BOMB:
            return 0;
        }
        return -1;
    }

    void CBallManBasketHungryBossPackun::MainFunc_()
    {
        BA_PARTY::CHBData *pData = m_pHBData;
        if (pData == 0) {
            return;
        }

        // Runs while the game plays (game state 5).
        if (m_nPickStep == 0) {
            if (CGame::m_pInstance->GetState() != 5) {
                return;
            }
            m_nTurn = 0;
            m_fPickTime = 0.0f;
            m_fPickWait = 0.0f;
            m_nPickStep++;
        } else if (CGame::m_pInstance->GetState() != 5) {
            // (three times in the original too)
            CCockpit::SetAnimEndClose(COCKPIT_BONUS_TELOP, 1);
            CCockpit::SetAnimEndClose(COCKPIT_BONUS_TELOP, 1);
            CCockpit::SetAnimEndClose(COCKPIT_BONUS_TELOP, 1);
            m_nTelopStep = 0;
            m_nPickStep = 0;
            return;
        }

        UpDataWaitBall();

        switch (m_nPickStep) {
        case 0:
            return;

        case 1: {
            // Waits, then picks the fruit of a combination of the data.
            m_fPickWait -= GetFrameRateF();
            if (m_fPickWait > 0.0f) {
                break;
            }
            m_fPickWait = 0.0f;
            if (!IsNowBonusTime_() && GetLooseFruitNum() >= 3) {
                break;
            }

            long nEntry = pData->GetWaitEntryCount();
            int bBusy = 0;
            for (long i = 0; i < nEntry; i++) {
                CNpcStringBasket *pString = m_ppString[i];
                if (pString->GetState() != CNpcString::STATE_IDLE && pString->m_nFruitType != -1) {
                    bBusy = 1;
                    break;
                }
            }
            if (bBusy) {
                break;
            }

            long nLoose = GetLooseFruitNum();
            long aTurnRate[3];
            aTurnRate[0] = pData->GetTurnRateData(m_nTurn, 0);
            aTurnRate[1] = pData->GetTurnRateData(m_nTurn, 1);
            aTurnRate[2] = pData->GetTurnRateData(m_nTurn, 2);
            switch (nLoose) {
            case 1:
                aTurnRate[2] = 0;
                break;
            case 2:
                aTurnRate[2] = 0;
                aTurnRate[1] = 0;
                break;
            }
            long nGroup = pData->GetGroupEntryData(m_nTurn, bcs::BCS_LotProbability(aTurnRate, 3));

            long nCombiNum = pData->GetGroupCombiCount(nGroup);
            long aCombiRate[16];
            for (long i = 0; i < nCombiNum; i++) {
                aCombiRate[i] = pData->GetGroupCombiRate(nGroup, i);
            }
            long nCombi = bcs::BCS_LotProbability(aCombiRate, nCombiNum);
            m_fPickTime = pData->GetGroupCombiTime(nGroup, nCombi);

            long nBallNum = pData->GetGroupBallCount(nGroup);
            long aKind[16];
            for (long i = 0; i < nBallNum; i++) {
                aKind[i] = pData->GetGroupCombiBallKind(nGroup, nCombi, i);
            }

            // The original asks for the turn count here and drops the
            // answer (a test whose result goes unused).
            IsNowBonusTime_();
            for (long j = 0; j < nBallNum; j++) {
                long nKind = aKind[j];
                long aLot[16];
                for (long i = 0; i < nEntry; i++) {
                    aLot[i] = 0;
                    CNpcStringBasket *pString = m_ppString[i];
                    if (nKind == pString->m_nFruitType && pString->IsWaitBall_() &&
                        !(pString->m_nFlag & CNpcStringBasket::FLAG_PICKED)) {
                        aLot[i] = 10;
                    }
                }
                m_ppString[bcs::BCS_LotProbability(aLot, nEntry)]->m_nFlag |= CNpcStringBasket::FLAG_PICKED;
            }

            long nWaitMin = pData->GetGroupCombiWaitMin(nGroup, nCombi);
            long nWait = bcs::BCS_RandRange(nWaitMin, pData->GetGroupCombiWaitMax(nGroup, nCombi));
            m_nPickStep = 2;
            m_fPickWait = nWait;
            break;
        }

        case 2: {
            // The pick runs until its time is over and its fruit are taken.
            int bEnd = 0;
            m_fPickTime -= GetFrameRateF();
            if (m_fPickTime <= 0.0f) {
                m_fPickTime = 0.0f;
                bEnd = 1;
            }

            long nEntry = pData->GetWaitEntryCount();
            long nLoose = GetLooseFruitNum();
            if (!IsNowBonusTime_() && nLoose >= 3) {
                bEnd = 0;
            } else if ((IsNowBonusTime_() || nLoose == 0) && !bEnd) {
                bEnd = 1;
                for (long i = 0; i < nEntry; i++) {
                    CNpcStringBasket *pString = m_ppString[i];
                    if (pString->GetState() == CNpcString::STATE_MOVE ||
                        (pString->m_nFlag & CNpcStringBasket::FLAG_PICKED)) {
                        bEnd = 0;
                    }
                }
            }
            if (!bEnd) {
                break;
            }

            for (long i = 0; i < nEntry; i++) {
                CNpcStringBasket *pString = m_ppString[i];
                if (pString->GetState() == CNpcString::STATE_MOVE && pString->GetAttachBall() != 0) {
                    pString->m_nFlag &= ~CNpcStringBasket::FLAG_PICKED;
                }
            }
            m_nPickStep = 1;
            break;
        }
        }

        UpDateBonusTelop();
    }

    void CBallManBasketHungryBossPackun::UpDataWaitBall()
    {
        if (m_ppString == 0) {
            return;
        }

        BA_PARTY::CHBData *pData = m_pHBData;
        long nTurn = CheckNowTurn();
        long nEntry = pData->GetWaitEntryCount();

        // The end of the bonus time: the strings leave and the next turn
        // starts.
        if (IsNowBonusTime_()) {
            m_fBonusWait -= GetFrameRateF();
            if (m_fBonusWait <= 0.0f) {
                m_fBonusWait = 0.0f;
                long nNum = m_pHBData->GetWaitEntryCount();
                long nPattern = GetDeliveryPattern();
                for (long i = 0; i < nNum; i++) {
                    m_ppString[i]->Withdraw_(nPattern);
                }
                m_nTurn = CheckNowTurn();
                CChrMan::NpcSendMessage(2, 1, 0, 0, 0);
                CChrMan::AllSendMsg(static_cast<CChrDrv::MSG_ID>(0x36), 0, 0, 0, 0);
                CGame::SendMessage(0x3B, 0, 0, 0);
            }
        }

        // The last turn is reached: the bonus time starts.
        if (m_nTurn != nTurn && m_nTurn != pData->GetTurnCount()) {
            CChrMan::NpcSendMessage(2, 0, 0, 0, 0);
            CChrMan::AllSendMsg(static_cast<CChrDrv::MSG_ID>(0x35), 0, 0, 0, 0);
            CGame::SendMessage(0x3A, 0, 0, 0);
            CGameBGM::SetBgmState(5);
            m_fBonusWait = 60.0f * BASKET_PRM_COMMON_BONUS_SECOND;
            m_nTurn = pData->GetTurnCount();
            long nPattern = GetDeliveryPattern();
            for (long i = 0; i < nEntry; i++) {
                m_ppString[i]->Withdraw_(nPattern);
            }
            m_fPickTime = 0.0f;
            m_fPickWait = 0.0f;
            m_nPickStep = 1;
        }

        // Sleeping strings get the fruit of the turn and come down.
        for (long i = 0; i < nEntry; i++) {
            CNpcStringBasket *pString = m_ppString[i];
            if (pString->GetState() == CNpcString::STATE_SLEEP && pString->CanbeAppear_()) {
                long nIndex = pString->m_nIndex;
                CBallManBasketHungryBossPackun *pMan =
                    static_cast<CBallManBasketHungryBossPackun *>(CBallMan::GetInstance());
                pString->m_nFruitType = pMan->m_pHBData->GetWaitBallEntryData(pMan->m_nTurn, nIndex);
                if (pString->m_nFruitType == -1) {
                    pString->Sleep_();
                } else {
                    pString->ChangeState(CNpcString::STATE_APPEAR);
                }
            }
        }
    }

    long CBallManBasketHungryBossPackun::CheckNowTurn() const
    {
        BA_PARTY::CHBData *pData = m_pHBData;
        long nNum = pData->GetTimeZoneCount() - 1;
        long *pRate = (long *)__alloca(nNum * sizeof(long));
        for (long i = 0; i < nNum; i++) {
            pRate[i] = pData->GetTimeRate(i);
        }

        // The rates are percentages of the game time.
        float fMaxTime = CGame::g_GameWork.m_fUnk20;
        float fTime = fMaxTime - CGame::g_GameWork.m_fGameTime;
        long nSum = 0;
        long nTurn = 0;
        for (long i = 0; i < nNum; i++) {
            nSum += pRate[i];
            if (!(fTime > fMaxTime * (nSum / 100.0f))) {
                nTurn = i;
                break;
            }
        }
        return nTurn;
    }

    void CBallManBasketHungryBossPackun::GetAppendPos_(long &rIdx, bcs::FVEC3 &rPos) const
    {
        // A checkerboard of the places of the grid: every other one of a row
        // can take a fruit, the other ones of the next row.
        long nAreaNum = BASKET_PRM_COMMON_APPEND_AREA_X_NUM * BASKET_PRM_COMMON_APPEND_AREA_Z_NUM;
        unsigned long nMask = 0;
        for (long x = 0; x < BASKET_PRM_COMMON_APPEND_AREA_X_NUM; x += 2) {
            nMask |= 1 << x;
        }

        long aLot[128];
        long *pLot = aLot;
        for (long z = 0; z < BASKET_PRM_COMMON_APPEND_AREA_Z_NUM; z++) {
            for (long x = 0; x < BASKET_PRM_COMMON_APPEND_AREA_X_NUM; x++) {
                if (nMask & (1 << x)) {
                    *pLot = 10;
                } else {
                    *pLot = 0;
                }
                pLot++;
            }
            nMask = ~nMask;
        }

        // Not over another string.
        BA_PARTY::CHBData *pData = m_pHBData;
        for (long i = 0; i < pData->GetWaitEntryCount(); i++) {
            long nIdx = m_ppString[i]->m_nAppendIdx;
            if (nIdx != -1) {
                aLot[nIdx] = 0;
            }
        }

        long nIdx = bcs::BCS_LotProbability(aLot, nAreaNum);
        rIdx = nIdx;

        long nZ = nIdx / BASKET_PRM_COMMON_APPEND_AREA_X_NUM;
        long nX = nIdx - nZ * BASKET_PRM_COMMON_APPEND_AREA_X_NUM;
        float fSizeX = BASKET_PRM_COMMON_APPEND_AREA_SIZE_X;
        float fSizeZ = BASKET_PRM_COMMON_APPEND_AREA_SIZE_Z;
        // The centre of the place, the grid centred on x = 0.
        float fHalfX = fSizeX * 0.5f;
        float fHalfZ = fSizeZ * 0.5f;
        bcs::FVEC3 base(0.0f, 0.0f, BASKET_PRM_COMMON_APPEND_AREA_POS_Z);
        bcs::FVEC3 pos(nX * fSizeX + fHalfX - (fSizeX * BASKET_PRM_COMMON_APPEND_AREA_X_NUM) * 0.5f,
                       BASKET_PRM_COMMON_APPEND_POS_Y,
                       base.z + (nZ * fSizeZ + fHalfZ - (fSizeZ * BASKET_PRM_COMMON_APPEND_AREA_Z_NUM) * 0.5f));
        rPos = pos;
    }

    void CBallManBasketHungryBossPackun::Create_()
    {
        DeleteAllBall_();
        m_pHBData->SetData(static_cast<const unsigned char *>(CLoadData::GetPtr(0x6D)));
        long nNum = m_pHBData->GetWaitEntryCount();
        m_ppString = new CNpcStringBasket *[nNum];
        for (long i = 0; i < nNum; i++) {
            m_ppString[i] = new CNpcStringBasket(i);
            m_ppString[i]->Create();
        }
    }

    int CBallManBasketHungryBossPackun::IsNowBonusTime_() const
    {
        return m_pHBData->GetTurnCount() == m_nTurn;
    }

    float CBallManBasketHungryBossPackun::GetBombExplodeR()
    {
        return BASKET_PRM_HB_BOMB_EXPLODE_R;
    }

    void CBallManBasketHungryBossPackun::UpDateBonusTelop()
    {
        switch (m_nTelopStep) {
        case 0:
            // Shows the telop STEP1 frames before the bonus time.
            switch (m_nTurn) {
            case 0:
            case 1: {
                BA_PARTY::CHBData *pData = m_pHBData;
                long nNum = pData->GetTimeZoneCount() - 1;
                long *pRate = (long *)__alloca(nNum * sizeof(long));
                for (long i = 0; i < nNum; i++) {
                    pRate[i] = pData->GetTimeRate(i);
                }
                float fMaxTime = CGame::g_GameWork.m_fUnk20;
                float fTime = fMaxTime - CGame::g_GameWork.m_fGameTime;
                float fSum = 0.0f;
                for (long i = 0; i < m_nTurn; i++) {
                    fSum += pRate[i];
                }
                if (fTime > fMaxTime * ((fSum + pRate[m_nTurn]) / 100.0f) - BASKET_PRM_COMMON_BONUS_TELOP_STEP1_FRM) {
                    m_nTelopStep = 1;
                }
                break;
            }
            }
            break;

        case 1:
            static_cast<CGameBasket *>(CGame::m_pInstance)->CameraQuake_(0x15, 0);
            CCockpit::SetAnimEndClose(COCKPIT_BONUS_TELOP, 0);
            CCockpitBasket::CreateObj(COCKPIT_BONUS_TELOP, 0);
            CCockpit::SetAnimEndClose(COCKPIT_BONUS_TELOP, 1);
            m_fTelopWait = BASKET_PRM_COMMON_BONUS_TELOP_STEP1_FRM;
            CGameSnd::Play(0xDD5, (const bcs::FVEC3 *)0, 1, -1, 0.0f, 1.0f, 0.0f);
            m_nTelopStep++;
            break;

        case 2:
            m_fTelopWait -= GetFrameRateF();
            if (m_fTelopWait > 0.0) {
                break;
            }
            CCockpit::SetAnimEndClose(COCKPIT_BONUS_TELOP, 1);
            m_fTelopWait = 60.0f * BASKET_PRM_COMMON_BONUS_TELOP_STEP2_MINUTES;
            m_nTelopStep++;
            // fall through
        case 3:
            m_fTelopWait -= GetFrameRateF();
            if (m_fTelopWait > 0.0) {
                break;
            }
            CCockpit::SetAnimEndClose(COCKPIT_BONUS_TELOP, 1);
            m_nTelopStep = 0;
            break;
        }
    }

    int CBallManBasketHungryBossPackun::GetShootRateBase_() const
    {
        return BASKET_PRM_PARTY_SHOOT_RATE_BASE;
    }

    int CBallManBasketHungryBossPackun::GetShootRate0_() const
    {
        return BASKET_PRM_PARTY_SHOOT_RATE0;
    }

    int CBallManBasketHungryBossPackun::GetShootRate1_() const
    {
        return BASKET_PRM_PARTY_SHOOT_RATE1;
    }

    int CBallManBasketHungryBossPackun::GetShootRate2_() const
    {
        return BASKET_PRM_PARTY_SHOOT_RATE2;
    }

    int CBallManBasketHungryBossPackun::GetShootRate3_() const
    {
        return BASKET_PRM_PARTY_SHOOT_RATE3;
    }

    float CBallManBasketHungryBossPackun::GetShootArea1_() const
    {
        return BASKET_PRM_PARTY_SHOOT_AREA1;
    }

    float CBallManBasketHungryBossPackun::GetShootArea2_() const
    {
        return BASKET_PRM_PARTY_SHOOT_AREA2;
    }

    float CBallManBasketHungryBossPackun::GetShootArea3_() const
    {
        return BASKET_PRM_PARTY_SHOOT_AREA3;
    }

    int CBallManBasketHungryBossPackun::GetAirBallRate_() const
    {
        return BASKET_PRM_PARTY_AIR_BALL_RATE;
    }

    int CNpcStringBasket::CanbeCatch() const
    {
        return GetAttachBall() == 0 ? 0 : GetState() == STATE_MOVE;
    }

    void CNpcStringBasket::Reset()
    {
        m_nAppendIdx = -1;
        m_nFruitType = -1;
        m_nFlag = 0;
        m_fRotSpeed = 0.0f;
        m_fRotDir = 0.0f;
        m_vPos = bcs::FVEC3(0.0f, 8.0f, 0.0f);
        SetOffsetPos(bcs::FVEC3(0.0f, 0.0f, 0.0f));
        UpdateShadow_();
        Sleep_();
        m_fShadowScale = 0.0f;
        m_fShadowA = 0.0f;
        m_fShadowR = 0.0f;
        m_fShadowG = 0.0f;
        m_fShadowB = 0.0f;
        m_fShadowTimer = 0.0f;
    }

    long CNpcStringBasket::SendMessage(long nMsg, long nArg1, long nArg2, long nArg3, long nArg4)
    {
        switch (nMsg) {
        case 3:
        case 5:
        case 0xF:
        case 0x12:
        case 0x13:
            Reset();
            break;
        case 0x24:
            // (meaning guessed) Ball nArg3 is taken: a string holding it
            // lets go.
            if (nArg1 == 0) {
                CBall *pBall = GetAttachBall();
                if (pBall != 0 && nArg3 == pBall->GetBallNo()) {
                    Withdraw_(1);
                }
            }
            break;
        }
        return CNpcString::SendMessage(nMsg, nArg1, nArg2, nArg3, nArg4);
    }

    void CNpcStringBasket::DtorStateWithdraws()
    {
        bcs::FVEC3 &rOfs = GetOffsetPos();
        rOfs.y = 0.0f;
    }

    void CNpcStringBasket::LoopStateWithdraws()
    {
        ReturnOffset_();
        CNpcString::LoopStateWithdraws();
    }

    void CNpcStringBasket::CtorStateWithdraws()
    {
        m_nFlag &= ~FLAG_PICKED;
        CGameSnd::Play(0xDD4, m_vPos, 1, -1, 0.0f, 1.0f, 0.0f);
        CNpcString::CtorStateWithdraws();
    }

    void CNpcStringBasket::DtorStateMove()
    {
    }

    void CNpcStringBasket::LoopStateMove()
    {
        Rotate_();
        if (m_nFruitType == FRUIT_BOMB) {
            // A bomb is not handed out: it goes up again.
            Withdraw_(1);
        } else if (m_nFlag & FLAG_PICKED) {
            // Picked: the fruit swings up and down.
            m_fSwingPhase += m_fSwingSpeed * GetFrameRateF();
            if (m_fSwingPhase >= 180.0f) {
                m_fSwingPhase -= 180.0f;
            }
            float fSin = nw4r::math::SinRad(bcs::DEG_TO_RAD(m_fSwingPhase));
            SetOffsetPos(bcs::FVEC3(0.0f, -m_fSwingWidth * fSin, 0.0f));
        } else {
            bcs::FVEC3 &rOfs = ReturnOffset_();
            if (IsZero_(rOfs.y) && GetState() == STATE_MOVE) {
                ChangeState(STATE_IDLE);
            }
        }
    }

    void CNpcStringBasket::CtorStateMove()
    {
        m_fSwingPhase = 0.0f;
        m_fSwingWidth = RandRange_(BASKET_PRM_COMMON_WAIT_MOVE_VALUE_MIN, BASKET_PRM_COMMON_WAIT_MOVE_VALUE_MAX);
        m_fSwingSpeed = RandRange_(BASKET_PRM_COMMON_WAIT_MOVE_SPEED_MIN, BASKET_PRM_COMMON_WAIT_MOVE_SPEED_MAX);
    }

    void CNpcStringBasket::LoopStateIdle()
    {
        Rotate_();
        if ((m_nFlag & FLAG_PICKED) && IsAnimLoop() && IsWaitBall_()) {
            ChangeState(STATE_MOVE);
        }
    }

    void CNpcStringBasket::CtorStateAppear()
    {
        DeleteBall_();

        long nKind = m_nFruitType;
        long nBallNo = CBallManBasket::GetInstance()->GetUseEnableIndex_();
        CBall *pBall;
        CBallBasketFruit *pFruit = new (0) CBallBasketFruit(CBallMan::GetInstance(), nBallNo, nKind);
        CBallBasketFruit *pAttach = pFruit;
        pFruit->Create(nKind, 0x40);
        pBall = CBallMan::AddBall(pFruit);
        if (pBall != 0) {
            pBall->Reset();
            pBall->SetRotSpeed(bcs::DEG_TO_RAD(5.0f));
        } else {
            pFruit->Delete();
            pAttach = 0;
        }
        AttachBall(pAttach);

        float fOfsY = 0.0f;
        switch (m_nFruitType) {
        case CBallBasketFruit::KIND_ORANGE:
            fOfsY = BASKET_PRM_HB_WAIT_OFFSET_Y_OR;
            break;
        case CBallBasketFruit::KIND_BLUE_APPLE:
            fOfsY = BASKET_PRM_HB_WAIT_OFFSET_Y_BA;
            break;
        case CBallBasketFruit::KIND_RED_APPLE:
            fOfsY = BASKET_PRM_HB_WAIT_OFFSET_Y_RA;
            break;
        case CBallBasketFruit::KIND_SUICA:
            fOfsY = BASKET_PRM_HB_WAIT_OFFSET_Y_SU;
            break;
        case CBallBasketFruit::KIND_BOMB:
            fOfsY = BASKET_PRM_HB_WAIT_OFFSET_Y_BM;
            break;
        }
        pAttach->AddAttachPos(bcs::FVEC3(0.0f, fOfsY, 0.0f));

        static_cast<CBallManBasketHungryBossPackun *>(CBallMan::GetInstance())->GetAppendPos_(m_nAppendIdx, m_vPos);
        pAttach->SetStringId(GetId());
        SetOffsetPos(bcs::FVEC3(0.0f, 0.0f, 0.0f));
        m_nFlag &= ~FLAG_PICKED;
        m_vRot.y = bcs::ROUND_PI(bcs::BCS_RandMax(6.2831855f));
        m_fRotDir = BASKET_PRM_HB_WAIT_STATE_ROT_VALUE;
        CGameSnd::Play(0xDD3, m_vPos, 1, -1, 0.0f, 1.0f, 0.0f);
        AnimPlay(1, 0, 0.0f, 0.0f);
    }

    void CNpcStringBasket::CtorStateSleep()
    {
        DeleteBall_();
        CNpcString::CtorStateSleep();
    }

    void CNpcStringBasket::MainFunc()
    {
        CNpcString::MainFunc();
        m_fShadowTimer += GetFrameRateF();
        while (m_fShadowTimer >= 1.0f) {
            m_fShadowTimer -= 1.0f;
            UpdateShadow_();
        }
    }
}
