// msw::CNpcBombCannon: the bomb cannon NPC (type 0xC). It sleeps until the
// game resets it, then waits a random 180 to 300 frames, fires, and waits
// again.
#include "game/msw/CNpcBombCannon.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CMdlRef.hpp"
#include "bcs/CMdlShadow.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLoadData.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace {

    enum {
        NPC_TYPE_BOMB_CANNON = 0xC,
        NPC_SLOT_NUM = 8,
        // The first model and motion resources of the NPC slots in the
        // load data (CLoadData::GetRes).
        RES_NPC_MDL = 0xB,
        RES_NPC_MOT = 0x1B
    };

    // The states (names from the symbols of their hooks).
    enum {
        STATE_SLEEP,
        STATE_IDLE,
        STATE_FIRE,
        STATE_NUM
    };

    // The messages that put the cannon back to its start (meanings not
    // known).
    enum {
        MSG_1 = 0x1,
        MSG_3 = 0x3,
        MSG_5 = 0x5,
        MSG_F = 0xF,
        MSG_12 = 0x12,
        MSG_13 = 0x13
    };
}

namespace msw {

    CNpcBombCannon::CNpcBombCannon(CChrMan *pMan)
        : CNpcBase(pMan, 0x13),
          m_Unk1D0(0),
          m_fUnk1D4(0.0f),
          m_fWait(0.0f),
          m_fUnk1DC(0.0f)
    {
    }

    CNpcBombCannon::~CNpcBombCannon() {}

    void CNpcBombCannon::Create()
    {
        InitStep();
        CreateMdlObj_();
    }

    void CNpcBombCannon::Reset()
    {
        ChangeState(STATE_IDLE);
    }

    void CNpcBombCannon::InitStep()
    {
        InitFsm(this, STATE_NUM);
        SetState(STATE_SLEEP, &CNpcBombCannon::CtorStateSleep_, &CNpcBombCannon::LoopStateSleep_,
                 &CNpcBombCannon::DtorStateSleep_);
        SetState(STATE_IDLE, &CNpcBombCannon::CtorStateIdle_, &CNpcBombCannon::LoopStateIdle_,
                 &CNpcBombCannon::DtorStateIdle_);
        SetState(STATE_FIRE, &CNpcBombCannon::CtorStateFire_, &CNpcBombCannon::LoopStateFire_,
                 &CNpcBombCannon::DtorStateFire_);
        ChangeState(STATE_SLEEP);
    }

    void CNpcBombCannon::CreateMdlObj_()
    {
        long nSlot = -1;
        for (long i = 0; i < NPC_SLOT_NUM; i++) {
            if (CGame::GetNpcType(i) == NPC_TYPE_BOMB_CANNON) {
                nSlot = i;
                break;
            }
        }
        if (nSlot < 0) {
            return;
        }

        nw4r::g3d::ResFile mdlFile = CLoadData::GetRes(nSlot + RES_NPC_MDL);
        nw4r::g3d::ResFile motFile = CLoadData::GetRes(nSlot + RES_NPC_MOT);
        nw4r::g3d::ResMdl mdl = mdlFile.GetResMdl(0UL);

        m_pMdl = new bcs::CMdlObj;
        unsigned long anmFlag = 0;
        mdlFile.HasResAnmChr(); // (tested, but adds no flag)
        if (mdlFile.HasResAnmClr()) {
            anmFlag |= 0x108;
        }
        if (mdlFile.HasResAnmTexSrt()) {
            anmFlag |= 0x204;
        }
        if (mdlFile.HasResAnmTexPat()) {
            anmFlag |= 0x3;
        }
        if (mdlFile.HasResAnmShp()) {
            anmFlag |= 0x7000;
        }
        m_pMdl->Create(mdl, anmFlag, 0);
        m_pMdl->OnFlag(0x100);
        m_pMdl->OnFlag(0x2);
        m_pMdl->SetExBlendMode(1);
        m_pMdl->OnFlag(0x2000);

        m_pAnmMot = new (0) bcs::CAnmMotion;
        m_pAnmMot->Create(m_pMdl, motFile, CC_DATA::GetNpcMdl_NodeExInfo(1), 3, 0);
        m_pMdl->AttachAnmObj(m_pAnmMot, bcs::CMdlObj::ANMOBJTYPE_CHR);

        m_pMdl->m_vTransOffset.y = -0.3f;
        m_pMdl->m_vScale.Set(0.9f, 0.9f, 0.9f);

        if (m_pMdl != 0) {
            m_pMdl->m_vTrans = m_vPos;
            m_pMdl->CalcRootMtx();
            m_pMdl->CalcWorldMatrixAll(0);
        }
        bcs::CRenderMan::PushBack(m_pMdl, 2);

        nw4r::g3d::ResMdl shadowMdl = mdlFile.GetResMdl(0UL);
        m_pShadow = new (0x100) bcs::CMdlShadow(m_pMdl, shadowMdl, 0x40, 0, 0);
        bcs::CRenderMan::PushBack(m_pShadow, 2);
        nw4r::g3d::ResMdl refMdl = mdlFile.GetResMdl(0UL);
        m_pRef = new bcs::CMdlRef(m_pMdl, refMdl, 0, 0, 0);
        bcs::CRenderMan::PushBack(m_pRef, 4);
    }

    long CNpcBombCannon::SendMessage(long nMsg, long, long, long, long)
    {
        switch (nMsg) {
        case MSG_1:
        case MSG_3:
        case MSG_5:
        case MSG_F:
        case MSG_12:
        case MSG_13:
            Reset();
            break;
        }
        return 0;
    }

    void CNpcBombCannon::MainFunc()
    {
        ExecState();

        if (m_pMdl != 0) {
            m_pMdl->m_vTrans = m_vPos;
            m_pMdl->SetRotXYZ(m_vRot.x, m_vRot.y, m_vRot.z);
            m_pMdl->UpdateFrame(GetFrameRateF());
            if (m_pMdl != 0) {
                m_pMdl->m_vTrans = m_vPos;
                m_pMdl->CalcRootMtx();
                m_pMdl->CalcWorldMatrixAll(0);
            }
        }

        if (m_pMdl != 0) {
            m_pMdl->OffFlag(0xC0);
        }
        if (m_pRef != 0) {
            m_pRef->OffFlag(0xC0);
        }
        if (m_pShadow != 0) {
            m_pShadow->OffFlag(0xC0);
        }
    }

    void CNpcBombCannon::CtorStateSleep() {}

    void CNpcBombCannon::LoopStateSleep() {}

    void CNpcBombCannon::DtorStateSleep() {}

    void CNpcBombCannon::CtorStateIdle()
    {
        AnimPlay(0, 1, 8.0f, 0.0f);
        m_fWait = bcs::BCS_RandRange(180.0f, 300.0f);
    }

    // Waits out the time (not while the game is stopped), then fires.
    void CNpcBombCannon::LoopStateIdle()
    {
        if (CGame::g_GameWork.m_nFlag18 & 0x4306) {
            return;
        }
        m_fWait -= GetFrameRateF();
        if (m_fWait <= 0.0f) {
            ChangeState(STATE_FIRE);
        }
    }

    void CNpcBombCannon::DtorStateIdle() {}

    void CNpcBombCannon::CtorStateFire()
    {
        AnimPlay(1, 0, 0.0f, 0.0f);
    }

    void CNpcBombCannon::LoopStateFire()
    {
        if (IsAnimEnd()) {
            ChangeState(STATE_IDLE);
        }
    }

    void CNpcBombCannon::DtorStateFire() {}
}
