// msw::CStageBasketHungryBossPackun72: Hungry Boss Packun stage 72 of the
// basket game, with a conveyor belt across the court.
#include "game/msw/CStageBasketHungryBossPackun72.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/CScnObj.hpp"
#include "game/msw/BasketPrm.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CStageMdl.hpp"
#include "game/msw/CStageMdlListNode.hpp"

namespace msw {

    // (names guessed) One entry of the parameter table that GetPrmTbl gives
    // the boss: the type of a tuning value, the value, and the offset of the
    // word it fills. The table ends with a zero entry.
    enum PRM_TYPE {
        PRM_TYPE_END,
        PRM_TYPE_LONG,
        PRM_TYPE_FLOAT
    };

    struct PRM_TBL {
        long m_nType;    // 0x00
        void *m_pValue;  // 0x04
        long m_nOffset;  // 0x08
    };

    static const PRM_TBL s_PrmTbl[] = {
        // For each of the three party game settings, ten tables and their
        // indices, then the belt's values.
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL0_IDX, 0x00 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL0, 0x04 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL1_IDX, 0x08 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL1, 0x0C },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL2_IDX, 0x10 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL2, 0x14 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL3_IDX, 0x18 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL3, 0x1C },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL4_IDX, 0x20 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL4, 0x24 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL5_IDX, 0x28 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL5, 0x2C },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL6_IDX, 0x30 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL6, 0x34 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL7_IDX, 0x38 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL7, 0x3C },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL8_IDX, 0x40 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL8, 0x44 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL9_IDX, 0x48 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE0_TBL9, 0x4C },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S72_CV0_INTERVAL, 0x50 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S72_CV0_INTERVAL_RND, 0x54 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S72_CV0_STOP_TIME, 0x58 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S72_CV0_SPEED, 0x5C },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_CV0_DIR, 0x60 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL0_IDX, 0x64 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL0, 0x68 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL1_IDX, 0x6C },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL1, 0x70 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL2_IDX, 0x74 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL2, 0x78 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL3_IDX, 0x7C },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL3, 0x80 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL4_IDX, 0x84 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL4, 0x88 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL5_IDX, 0x8C },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL5, 0x90 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL6_IDX, 0x94 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL6, 0x98 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL7_IDX, 0x9C },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL7, 0xA0 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL8_IDX, 0xA4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL8, 0xA8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL9_IDX, 0xAC },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE1_TBL9, 0xB0 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S72_CV1_INTERVAL, 0xB4 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S72_CV1_INTERVAL_RND, 0xB8 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S72_CV1_STOP_TIME, 0xBC },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S72_CV1_SPEED, 0xC0 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_CV1_DIR, 0xC4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL0_IDX, 0xC8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL0, 0xCC },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL1_IDX, 0xD0 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL1, 0xD4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL2_IDX, 0xD8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL2, 0xDC },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL3_IDX, 0xE0 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL3, 0xE4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL4_IDX, 0xE8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL4, 0xEC },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL5_IDX, 0xF0 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL5, 0xF4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL6_IDX, 0xF8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL6, 0xFC },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL7_IDX, 0x100 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL7, 0x104 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL8_IDX, 0x108 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL8, 0x10C },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL9_IDX, 0x110 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_STATE2_TBL9, 0x114 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S72_CV2_INTERVAL, 0x118 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S72_CV2_INTERVAL_RND, 0x11C },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S72_CV2_STOP_TIME, 0x120 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S72_CV2_SPEED, 0x124 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_CV2_DIR, 0x128 },
        { PRM_TYPE_LONG, &BASKET_PRM_S72_DELIVERY_PATTERN, 0x12C },
        { PRM_TYPE_END, 0, 0 },
    };
}

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it, so it most likely comes
// from a header they all include. (which one, and its name, are not known)
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

// (guessed helper) A stage parameter for the current party game setting
// (party game parameter 0): each parameter comes in three sets.
template <typename T>
static inline T SelectPrm(const T &rPrm0, const T &rPrm1, const T &rPrm2)
{
    if (msw::CGame::GetPartyGameParam(0) == 0) {
        return rPrm0;
    }
    if (msw::CGame::GetPartyGameParam(0) == 1) {
        return rPrm1;
    }
    return rPrm2;
}

#define S72_CV_PRM(name)                                                                                      \
    SelectPrm(msw::BASKET_PRM_S72_CV0_##name, msw::BASKET_PRM_S72_CV1_##name, msw::BASKET_PRM_S72_CV2_##name)

const void *msw::CStageBasketHungryBossPackun72::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageBasketHungryBossPackun72::CStageBasketHungryBossPackun72(bcs::CTask *pParent)
    : CStageBasketHungryBossPackun(pParent), m_nState(STATE_RUN), m_nBeltMdlNum(0)
{
    for (int i = 0; i < BELT_NUM; i++) {
        m_apBeltMdl[i] = 0;
    }
}

msw::CStageBasketHungryBossPackun72::~CStageBasketHungryBossPackun72()
{
}

void msw::CStageBasketHungryBossPackun72::Main_(float frame)
{
    ConveyorUpdate(frame);
    CStageBasketHungryBossPackun::Main_(frame);
}

void msw::CStageBasketHungryBossPackun72::ConveyorUpdate(float frame)
{
    BELT &rBelt = m_aBelt[0];

    for (long i = 0; i < BELT_NUM; i++) {
        BELT &rCurBelt = m_aBelt[i];

        if (IsGimmickStop()) {
            rCurBelt.m_nDir = BELT::DIR_STOP;
        }

        // The speed is in units per second.
        float speed = S72_CV_PRM(SPEED) / 60.0f;
        if (rCurBelt.m_nDir == BELT::DIR_FRONT) {
            rCurBelt.m_fTargetSpeed = speed;
        } else if (rCurBelt.m_nDir == BELT::DIR_BACK) {
            rCurBelt.m_fTargetSpeed = -speed;
        } else {
            rCurBelt.m_fTargetSpeed = 0.0f;
        }

        // The belt changes speed by 1/60 a frame.
        float beltSpeed = rCurBelt.m_fSpeed;
        if (beltSpeed < rCurBelt.m_fTargetSpeed) {
            beltSpeed += 1.0f / 60.0f;
            if (rCurBelt.m_fTargetSpeed < beltSpeed) {
                beltSpeed = rCurBelt.m_fTargetSpeed;
            }
        } else if (beltSpeed > rCurBelt.m_fTargetSpeed) {
            beltSpeed -= 1.0f / 60.0f;
            if (rCurBelt.m_fTargetSpeed > beltSpeed) {
                beltSpeed = rCurBelt.m_fTargetSpeed;
            }
        }
        rCurBelt.m_fMove = beltSpeed * frame;
        SetBeltSpeed(i, beltSpeed);
    }

    if (!IsGimmickStop()) {
        switch (m_nState) {
            case STATE_RUN:
                m_fTimer -= frame;
                if (m_fTimer <= 0.0f) {
                    m_nState = STATE_STOP;
                    m_fTimer = 60.0f * S72_CV_PRM(STOP_TIME);
                    rBelt.m_nPrevDir = rBelt.m_nDir;
                    rBelt.m_nDir = BELT::DIR_STOP;
                }
                break;
            case STATE_STOP:
                m_fTimer -= frame;
                if (m_fTimer <= 0.0f) {
                    m_nState = STATE_RUN;
                    m_fTimer = 60.0f * S72_CV_PRM(INTERVAL);
                    m_fTimer += 60.0f * bcs::BCS_RandMax(S72_CV_PRM(INTERVAL_RND));

                    // 0: either way, 1: the other way from the last run,
                    // 2: back, 3: front.
                    for (long i = 0; i < BELT_NUM; i++) {
                        switch (S72_CV_PRM(DIR)) {
                            case 0:
                                m_aBelt[i].m_nDir =
                                    bcs::BCS_RandRange(0L, 1L) != 0 ? BELT::DIR_BACK : BELT::DIR_FRONT;
                                break;
                            case 1:
                                m_aBelt[i].m_nDir =
                                    m_aBelt[i].m_nPrevDir == BELT::DIR_FRONT ? BELT::DIR_BACK : BELT::DIR_FRONT;
                                break;
                            case 2:
                                m_aBelt[i].m_nDir = BELT::DIR_BACK;
                                break;
                            case 3:
                                m_aBelt[i].m_nDir = BELT::DIR_FRONT;
                                break;
                        }
                    }
                }
                break;
        }
    } else {
        m_nState = STATE_STOP;
        m_fTimer = 60.0f * S72_CV_PRM(STOP_TIME);
        rBelt.m_nDir = BELT::DIR_STOP;
    }

    // The belt's sound runs while it moves, with a stop sound at the end.
    if (m_aBelt[0].m_nDir == BELT::DIR_STOP) {
        if (SE_IsAlive(4)) {
            SE_Stop(4, 0.0f);
            CGameSnd::Play(0xDB0, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        }
    } else if (!SE_IsAlive(4)) {
        SE_Play(4, 0xDAF, 0.0f, 3);
    }
}

void msw::CStageBasketHungryBossPackun72::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageBasketHungryBossPackun::SendMessage(msg, arg1, arg2, arg3);

    if (msg == 3 || msg == 0x13) {
        // The results are not used. (kept for the calls the original makes)
        SE_IsAlive(0);
        SE_IsAlive(1);
    }
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageBasketHungryBossPackun72::GetMdlGenRule_() const
{
    // The end entry chains to the table of CStageBasketHungryBossPackun.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_ground_00bsk$", 3, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_ground_01bsk[0-9]+a$", 0xC, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_ground_01bsk[0-9]+b$", 0xC, 0, 0, CStageMan::NewMdl },
        { 0, reinterpret_cast<long>(CStageBasketHungryBossPackun::GetMdlGenRule_()) },
    };

    return tbl;
}

void msw::CStageBasketHungryBossPackun72::OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                                          CMdlPtrPool::Handle &rHandle, CArn &rArn, long n)
{
    if (pRule->m_nId != 3) {
        return;
    }

    // The belt model.
    m_apBeltMdl[m_nBeltMdlNum++] = *rHandle.Get();
    m_nBeltMdlNum = m_nBeltMdlNum > BELT_NUM ? m_nBeltMdlNum : BELT_NUM;
}

void msw::CStageBasketHungryBossPackun72::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                                           CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile,
                                                           CArn &rArn, long n)
{
    if (pRule->m_nId == 0xC && pNode->GetObj() != 0) {
        pNode->GetObj()->SetDrawPriority(0xC9, 0xC9);
    }
    CStageBasketHungryBossPackun::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
}

void msw::CStageBasketHungryBossPackun72::OnTurnInit()
{
    m_aBelt[0].m_nDir = BELT::DIR_STOP;
    m_aBelt[0].m_fSpeed = 0.0f;
}

void msw::CStageBasketHungryBossPackun72::OnGimmickReset()
{
    if (!(CGame::g_GameWork.m_nFlag18 & 8)) {
        for (long i = 0; i < BELT_NUM; i++) {
            m_aBelt[i].m_nDir = BELT::DIR_STOP;
            m_aBelt[i].m_fSpeed = 0.0f;
            SetBeltSpeed(i, 0.0f);
        }
    }
    m_fTimer = 0.0f;
    SE_Stop(4, 0.0f);
}

long msw::CStageBasketHungryBossPackun72::GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    return 2;
}

// (guessed helper) The flow direction of a belt along z: belt 0 runs the
// other way from its animation (see SetBeltSpeed). The original multiplies by
// the 1.0f, so the sign came from something like this.
static inline float GetBeltDir(long i)
{
    return i == 0 ? 1.0f : -1.0f;
}

void msw::CStageBasketHungryBossPackun72::GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos,
                                                       FLOW_TYPE type) const
{
    switch (type) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6:
            // The area flags are not used. (kept for the call the original
            // makes)
            GetAreaFlagByPos(rPos, 3);
            rFlow.Set(0.0f, 0.0f, GetBeltDir(0) * m_aBelt[0].m_fMove);
            break;
        default:
            rFlow.Set(0.0f, 0.0f, 0.0f);
            break;
    }
}

void msw::CStageBasketHungryBossPackun72::SetBeltSpeed(long i, float speed)
{
    m_aBelt[i].m_fSpeed = speed;

    CStageMdl *pMdl = m_apBeltMdl[i];
    if (pMdl != 0) {
        if (i == 0) {
            speed = -speed;
        }
        // The animation plays at its normal rate at 1/45 units a frame.
        pMdl->SetAnmRate(speed / (1.0f / 45.0f));
    }
}

void msw::CStageBasketHungryBossPackun72::OnArtsShootStart()
{
    m_aBelt[0].m_nDir = BELT::DIR_STOP;
}

void msw::CStageBasketHungryBossPackun72::OnArtsShootEnd()
{
}

void msw::CStageBasketHungryBossPackun72::OnSoundEvent(long event)
{
    if (event == 0) {
        if (!SE_IsAlive(0)) {
            SE_Play(0, 0xDAE, 0.0f, 3);
        }
        if (!SE_IsAlive(1)) {
            SE_Play(1, 0xDB2, 0.0f, 3);
        }
        if (!SE_IsAlive(2)) {
            SE_Play(0, 0xDB3, 0.0f, 3);
        }
    }
}

long msw::CStageBasketHungryBossPackun72::SE_GetSlotNum() const
{
    return 5;
}
