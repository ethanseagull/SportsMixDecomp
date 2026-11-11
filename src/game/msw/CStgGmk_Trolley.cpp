// The car's header comes first: the vtables come out in the reverse order
// of the class declarations, and the original has the car's after the
// actor's.
#include "game/msw/CStgGmk_Trolley_Mdl.hpp"
#include "game/msw/CStgGmk_Trolley.hpp"
#include "game/msw/CStgGmk_Trolley_Act.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CCollUnit.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CNpcHeyho.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStgMdlObjBase.hpp"
#include "game/msw/CStgSrt.hpp"
#include "nw4r/math/types.h"
#include "revolution/mtx.h"

// The trolley of stage 17. In the original one file holds the cars
// (CStgGmk_Trolley_Mdl, from 0x802219F0), the rails
// (CStgGmk_Trolley_Act::CRailMdl), the actor (CStgGmk_Trolley_Act), the
// driver (CStgGmk_Trolley_Drv) and the gimmick (CStgGmk_Trolley), with their
// weak functions at the end of .text (to 0x80226894) and the vtables in one
// .data block (0x80489318 to 0x80489C80). Evidence: the gimmick's NewMdl
// inlines the car's constructor, the actor's constructor the rails', the
// weak NewDrv the driver's, and the getters of the gimmick the actor calls
// are emitted between the actor's functions.
//
// The train: GetDispMdlNum() cars run one behind the other along the
// rails, the first at the actor's position. Every car but the first
// carries a heyho (CNpcHeyho) that throws things; a catch by the attacking
// side takes one life, and a train out of lives breaks.

namespace {

    // The suffixes of the animations of the cars and of the rails.
    const char *s_CarAnmSuffix[] = { "", 0 };
    const char *s_RailAnmSuffix[] = { "", 0 };

    // The car models, one per type (CStgGmk_Trolley_Mdl::GetMdlType).
    const msw::CStgMdl::MDL_PARAM s_CarMdlParam[] = {
        { "s17_truck_01", 0, 0, 0, s_CarAnmSuffix, "s17_truck_01", 0, 0 },
        { "s17_truck_02", 0, 0, 0, s_CarAnmSuffix, "s17_truck_02", 0, 0 },
        { "s17_truck_03", 0, 0, 0, s_CarAnmSuffix, "s17_truck_03", 0, 0 },
        { "s17_truck_04", 0, 0, 0, s_CarAnmSuffix, "s17_truck_04", 0, 0 },
    };

    // The rails, whose animation carries the node the cars follow.
    const msw::CStgMdl::MDL_PARAM s_RailMdlParam = { "truck_line", 0, 0, 0, s_RailAnmSuffix, "truck_line", 0, 0 };

    // (layout guessed) A table of the location data (CLoadData 0x6C): the
    // number of entries at 0x08 and the entries, 20.12 fixed point, from
    // 0x10.
    struct LOC_RAW_TBL {
        unsigned char m_Unk00[8];
        long m_nNum;              // 0x08
        unsigned char m_Unk0C[4];
        long m_aData[1];          // 0x10
    };

    // (name guessed) rPos moved down by h.
    inline bcs::FVEC3 LowerPos(const bcs::FVEC3 &rPos, float h)
    {
        bcs::FVEC3 pos(rPos);
        pos.y -= h;
        return pos;
    }

    inline long LocRawTblGetNum(const LOC_RAW_TBL *pTbl)
    {
        return pTbl == 0 ? 0 : pTbl->m_nNum;
    }

    // Entry idx of the table, 0 when there is none.
    inline long LocRawTblGet(const LOC_RAW_TBL *pTbl, long idx)
    {
        if (pTbl == 0) {
            return 0;
        }
        const long *pData = pTbl->m_aData;
        if (idx < 0 || idx >= LocRawTblGetNum(pTbl)) {
            return 0;
        }
        return pData[idx];
    }
}

namespace msw {

    CStgGmk_Trolley_Mdl::CStgGmk_Trolley_Mdl(CStgGmk *pParent, long idx)
        : CStgMdl(pParent), m_nMdlIdx(idx), m_nHeyhoId(0)
    {
        long type = 2;
        if (idx == 0) {
            type = 0;
        }
        m_nMdlType = type;
    }

    CStgGmk_Trolley_Mdl::~CStgGmk_Trolley_Mdl()
    {
        DeleteHeyho();
    }

    // Makes the model of the car's type; a heyho already on the car is let
    // go of first.
    void CStgGmk_Trolley_Mdl::Create()
    {
        CNpcHeyho *pHeyho = GetHeyho();
        if (pHeyho != 0) {
            pHeyho->AttachTo(0, 0);
        }
        long type = m_nMdlType;
        type = type > 3 ? 3 : (type < 0 ? 0 : type);
        CStgMdl::Create(CGenericGimmickCreator(CLoadData::GetRes(0), &s_CarMdlParam[type]));
        SetAnm(0, 2, 1.0f, 0.0f, 0.0f, -1);
    }

    bcs::CMdlObj *CStgGmk_Trolley_Mdl::NewMdl(long heap) const
    {
        return new (heap) CStgMdlObjBase<bcs::CMdlObj>;
    }

    void CStgGmk_Trolley_Mdl::OnPostCreate()
    {
        CreateHeyho();
    }

    void CStgGmk_Trolley_Mdl::DeleteMdl()
    {
        DeleteHeyho();
        CStgMdl::DeleteMdl();
    }

    // Scales the model to the actor's car size; the heyho shows while the
    // car is in the scene, after a wait of five frames.
    void CStgGmk_Trolley_Mdl::Main(float rate)
    {
        CStgGmk_Trolley_Act *pAct = static_cast<CStgGmk_Trolley_Act *>(GetParent()->GetAct());
        bcs::CMdlObj *pMdl = GetMdlObj();
        if (pAct != 0 && pMdl != 0) {
            float depth = pAct->GetParam_MdlDepth() / GetParam_MdlDepth();
            float height = pAct->GetParam_MdlHeight() / GetParam_MdlHeight();
            pMdl->m_vScale.x = pAct->GetParam_MdlWidth() / GetParam_MdlWidth();
            pMdl->m_vScale.y = height;
            pMdl->m_vScale.z = depth;
            pMdl->CalcRootMtx();
            pMdl->CalcWorldMtx(0);
        }
        CStgMdl::Main(rate);
        if (IsSceneEntry()) {
            DispHeyho(1);
            if (m_fHeyhoWait > 0.0f) {
                m_fHeyhoWait -= rate;
            }
            float wait = m_fHeyhoWait;
            if (0.0f > wait) {
                wait = 0.0f;
            }
            m_fHeyhoWait = wait;
        } else {
            DispHeyho(0);
            m_fHeyhoWait = 5.0f;
        }
    }

    // The heyho faces the front or the back of the car at random.
    void CStgGmk_Trolley_Mdl::SetVisible(int bVisible)
    {
        CStgMdl::SetVisible(bVisible);
        CNpcHeyho *pHeyho = GetHeyho();
        if (pHeyho != 0) {
            pHeyho->SetRotY(bcs::BCS_RandF() > 0.5f ? 0.0f : 3.1415927f);
        }
    }

    // Puts a heyho on a car of type 2 or 3, on node 2 of its model.
    void CStgGmk_Trolley_Mdl::CreateHeyho()
    {
        DeleteHeyho();
        long type = GetMdlType();
        if (type != 2 && type != 3) {
            return;
        }
        bcs::CMdlObj *pMdl = GetMdlObj();
        if (pMdl != 0) {
            CNpcHeyho *pHeyho = new (0x100) CNpcHeyho(CChrMan::GetInstance());
            if (pHeyho != 0) {
                pHeyho->SetPos(bcs::FVEC3(0.0f, 0.0f, 0.0f));
                pHeyho->SetRotY(bcs::BCS_RandF() > 0.5f ? 0.0f : 3.1415927f);
                pHeyho->Create();
                pHeyho->AttachTo(pMdl, 2);
                pHeyho->Disappear();
                m_nHeyhoId = pHeyho->GetId();
            }
        }
        m_fHeyhoWait = 5.0f;
    }

    void CStgGmk_Trolley_Mdl::DeleteHeyho()
    {
        if (m_nHeyhoId != 0) {
            bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_nHeyhoId);
            if (pTask != 0) {
                pTask->Delete();
            }
            m_nHeyhoId = 0;
        }
    }

    void CStgGmk_Trolley_Mdl::DispHeyho(int bDisp)
    {
        CNpcHeyho *pHeyho = GetHeyho();
        if (pHeyho != 0) {
            if (bDisp && m_fHeyhoWait <= 0.0f) {
                pHeyho->Appear();
            } else {
                pHeyho->Disappear();
            }
        }
    }

    CNpcHeyho *CStgGmk_Trolley_Mdl::GetHeyho()
    {
        if (m_nHeyhoId == 0) {
            return 0;
        }
        return static_cast<CNpcHeyho *>(bcs::CTaskMan::GetTask(m_nHeyhoId));
    }

    void CStgGmk_Trolley_Mdl::SetHeyhoParam(int bCoin, float distMin, float distMax)
    {
        CNpcHeyho *pHeyho = GetHeyho();
        if (pHeyho != 0) {
            pHeyho->SetThrowParam(bCoin, distMin, distMax);
        }
    }

    float CStgGmk_Trolley_Mdl::GetParam_MdlWidth() const
    {
        return 2.5f;
    }

    float CStgGmk_Trolley_Mdl::GetParam_MdlHeight() const
    {
        return 1.5f;
    }

    float CStgGmk_Trolley_Mdl::GetParam_MdlDepth() const
    {
        return 5.0f;
    }

    CStgGmk_Trolley_Act::CRailMdl::~CRailMdl()
    {
    }

    // The animation stands still: GetMtx sets its frame.
    void CStgGmk_Trolley_Act::CRailMdl::Create()
    {
        CStgMdl::Create(CGenericGimmickCreator(CLoadData::GetRes(0), &s_RailMdlParam));
        SetAnm(0, 0, 1.0f, 0.0f, 0.0f, -1);
        SetAnmSpeed(0.0f);
    }

    void CStgGmk_Trolley_Act::CRailMdl::SetNode(const char *pName)
    {
        SetNodeIdx(-1);
        bcs::CMdlObj *pMdl = GetMdlObj();
        if (pMdl != 0) {
            nw4r::g3d::ResMdl mdl = pMdl->GetResMdl();
            if (mdl.IsValid()) {
                nw4r::g3d::ResNode node = mdl.GetResNode(pName);
                if (node.IsValid()) {
                    SetNodeIdx(node.GetID());
                }
            }
        }
    }

    // The matrix of a car dist along the track, turned by rotY: the
    // animation is set to the frame of that distance and the node's matrix
    // read. False without a node, an animation or a length.
    bool CStgGmk_Trolley_Act::CRailMdl::GetMtx(bcs::FMTX34 &rMtx, float dist, float rotY)
    {
        if (!IsValidNode()) {
            return false;
        }
        float total = GetAnmTotalFrame(static_cast<CStageAnm::ANM_IDX>(0));
        if (total <= 0.0f) {
            return false;
        }
        float len = GetLength();
        if (len <= 0.0f) {
            return false;
        }
        float frame = total * (dist / len);
        float last = total - 1.0f;
        SetAnmFrame(frame > last ? last : (frame < 0.0f ? 0.0f : frame));
        SetAnmSpeed(0.0f);
        bcs::CMdlObj *pMdl = GetMdlObj();
        if (pMdl != 0) {
            float zero = 0.0f;
            pMdl->ObjProc(8, 0, &zero);
        }
        Main(1.0f);
        pMdl->GetNodeLocalMtx(GetNodeIdx(), rMtx);

        bcs::FMTX34 mtx;
        bcs::FMTX34 rot;
        PSMTXIdentity(mtx.m);
        nw4r::math::MTX34RotXYZRad(reinterpret_cast<nw4r::math::MTX34 *>(&rot), 0.0f, rotY, 0.0f);
        PSMTXConcat(mtx.m, rot.m, mtx.m);
        PSMTXConcat(mtx.m, rMtx.m, rMtx.m);
        return true;
    }

    CStgGmk_Trolley_Act::CStgGmk_Trolley_Act(CStgGmk *pParent)
        : CStgGmkAct(pParent), m_SePosFunc(this), m_bAlive(0), m_bQuake(0), m_fInvincibleTime(0.0f),
          m_nDispMdlNum(1), m_pRailMdl(0), m_nHeightTypeNum(1), m_nHeightType(0)
    {
        FsmqInit(this, 6, 4, 4);
        FsmqSetState(STATE_DEFAULT, &CStgGmk_Trolley_Act::CtorState_DEFAULT, &CStgGmk_Trolley_Act::LoopState_DEFAULT,
                     &CStgGmk_Trolley_Act::DtorState_DEFAULT);
        FsmqSetState(STATE_STANDBY, &CStgGmk_Trolley_Act::CtorState_STANDBY, &CStgGmk_Trolley_Act::LoopState_STANDBY,
                     &CStgGmk_Trolley_Act::DtorState_STANDBY);
        FsmqSetState(STATE_MOVE, &CStgGmk_Trolley_Act::CtorState_MOVE, &CStgGmk_Trolley_Act::LoopState_MOVE,
                     &CStgGmk_Trolley_Act::DtorState_MOVE);
        FsmqSetState(STATE_BREAK, &CStgGmk_Trolley_Act::CtorState_BREAK, &CStgGmk_Trolley_Act::LoopState_BREAK,
                     &CStgGmk_Trolley_Act::DtorState_BREAK);
        FsmqSetState(STATE_RESPAWN, &CStgGmk_Trolley_Act::CtorState_RESPAWN, &CStgGmk_Trolley_Act::LoopState_RESPAWN,
                     &CStgGmk_Trolley_Act::DtorState_RESPAWN);
        FsmqSetState(STATE_INACTIVE, &CStgGmk_Trolley_Act::CtorState_INACTIVE,
                     &CStgGmk_Trolley_Act::LoopState_INACTIVE, &CStgGmk_Trolley_Act::DtorState_INACTIVE);
        CStgSrt *pSrt = GetSrt();
        pSrt->ResetVelocity(GetParam_Gravity());
        m_pRailMdl = new CRailMdl(pParent);
        for (long i = 0; i < HEIGHT_TYPE_TBL_NUM; i++) {
            for (long j = 0; j < HEIGHT_TYPE_CAR_NUM; j++) {
                m_aHeightTypeTbl[i][j] = j == 0 ? 0 : 1;
            }
        }
    }

    CStgGmk_Trolley_Act::~CStgGmk_Trolley_Act()
    {
        if (m_pRailMdl != 0) {
            delete m_pRailMdl;
        }
        m_pRailMdl = 0;
    }

    void CStgGmk_Trolley_Act::OnInit()
    {
        InitColl();
        if (m_pRailMdl != 0) {
            m_pRailMdl->Create();
            m_pRailMdl->SetNode(GetParam_RailNodeName());
            m_pRailMdl->SetLength(GetParam_RailLength());
        }
        BuildHeightTypeTbl();
    }

    // Runs the states, then moves the train: along the rails while
    // GetParam_IsRailMotRelate(), otherwise by its velocity (and then the
    // distance run is measured from the spawn point). The train never goes
    // below 0.05.
    void CStgGmk_Trolley_Act::OnUpdate(float rate)
    {
        CStgSrt *pSrt = GetSrt();
        CStageMan::GetInstance();
        bcs::FVEC3 prevPos = pSrt->GetPos();
        if (m_fInvincibleTime > 0.0f) {
            m_fInvincibleTime -= rate;
        }
        FsmqExec();
        if (!GetParam_IsRailMotRelate()) {
            pSrt->Update(rate);
        } else {
            bcs::FVEC3 vel = pSrt->GetVel();
            float run = PSVECMag(reinterpret_cast<const Vec *>(&vel)) / 60.0f * rate;
            m_fRunDist += run;
            bcs::FMTX34 mtx;
            if (m_pRailMdl->GetMtx(mtx, m_fRunDist, GetParam_LocRotY())) {
                pSrt->SetPos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
                pSrt->SetFront(mtx.m[0][2], mtx.m[1][2], mtx.m[2][2]);
            }
        }
        bcs::FVEC3 pos = pSrt->GetPos();
        if (pos.y < 0.05f) {
            pos.y = 0.05f;
            pSrt->SetPos(pos);
            pSrt->ClearUnkE4();
        }
        if (!GetParam_IsRailMotRelate()) {
            bcs::FVEC3 ofs;
            bcs::FVEC3Sub(&ofs, &pos, &m_SpawnPos);
            ofs.y = 0.0f;
            m_fRunDist = PSVECMag(reinterpret_cast<const Vec *>(&ofs));
        }
        UpdateColl();
    }

    // The cars follow one another: behind the first along the rails, or
    // straight back along its direction.
    void CStgGmk_Trolley_Act::UpdateMdlMtx()
    {
        bcs::FVEC3 up(0.0f, 1.0f, 0.0f);
        CStgObj *pParent = GetParent();
        if (pParent != 0) {
            const bcs::FMTX34 &rBaseMtx = GetSrt()->GetMtx();
            long num = m_nDispMdlNum;
            float depth = GetParam_MdlDepth();
            float rotY = GetParam_LocRotY();
            for (long i = 0; i < num; i++) {
                CStgMdl *pMdl = pParent->GetMdl(i);
                if (pMdl != 0) {
                    bcs::FMTX34 mtx;
                    bcs::FMTX34Copy(&mtx, &rBaseMtx);
                    if (i > 0) {
                        if (!GetParam_IsRailMotRelate()) {
                            bcs::FVEC3 ofs(mtx.m[0][2], mtx.m[1][2], mtx.m[2][2]);
                            bcs::BCS_Vec_Normalize(ofs, ofs);
                            bcs::FVEC3Scale(&ofs, &ofs, -depth * i);
                            PSMTXTransApply(mtx.m, mtx.m, ofs.x, ofs.y, ofs.z);
                        } else {
                            m_pRailMdl->GetMtx(mtx, -depth * i + m_fRunDist, rotY);
                        }
                    }
                    pMdl->SetMtx(mtx);
                }
            }
        }
        CStgMdl *pMdl = GetMdl(0);
        if (pMdl != 0) {
            pMdl->SetMtx(GetSrt()->GetMtx());
        }
    }

    // Body units 0 and 1 cover the cars (1 to a height of 8 for the ball),
    // unit 2 catches the ball; two attack capsules run along the cars.
    void CStgGmk_Trolley_Act::InitColl()
    {
        GetParam_Width();
        GetParam_Depth();
        GetParam_Height(0);
        m_BodyColl.Create(3, 5, 3, 0);
        m_BodyColl.SetCollType(0, 2);
        m_BodyColl.SetPushRank(0, 5);
        m_BodyColl.SetPushPower(0, 6);
        m_BodyColl.SetTargetFlag(0, 4);
        m_BodyColl.SetCollType(1, 2);
        m_BodyColl.SetPushRank(1, 5);
        m_BodyColl.SetPushPower(1, 6);
        m_BodyColl.SetTargetFlag(1, 2);
        m_BodyColl.SetCollType(2, 2);
        m_BodyColl.SetTargetFlag(2, 0x14);
        RegistBodyPushCallback(m_BodyColl);
        RegistBodyDamageCallback(m_BodyColl);
        RegistBallCatchCallback(m_BodyColl);
        InitDefColl();
        m_AtkColl.Create(2, 5, 0, 0);
        for (long i = 0; i < 2; i++) {
            m_AtkColl.SetCollType(i, 3);
            m_AtkColl.SetTargetFlag(i, 2);
            m_AtkColl.SetAtkType(i, 0xF01);
            RegistAttackHitCallback(m_AtkColl);
            m_AtkColl.SetEnable(i);
        }
    }

    void CStgGmk_Trolley_Act::InitDefColl()
    {
        m_DefColl.Create(1, 5, 4, 0);
        m_DefColl.GetColl(0)->SetCollType(2);
        m_DefColl.GetColl(0)->SetShape(4);
        m_DefColl.GetColl(0)->OffFlagA0(1);
        RegistReflectHitCallback(m_DefColl);
    }

    // The collisions follow the train while it is alive and are off
    // otherwise.
    void CStgGmk_Trolley_Act::UpdateColl()
    {
        GetSrt();
        if (m_bAlive) {
            float width = GetBodyTotalWidth();
            float height = GetParam_Height(0);
            float ballHeight = 8.0f;
            float depth = GetBodyTotalDepth();
            bcs::FVEC3 pos = GetVirtualPos();
            float rotY = GetParam_LocRotY();
            float atkWidth = GetAtkTotalWidth();
            float atkHeight = GetParam_AtkHeight();
            float atkDepth = GetAtkTotalDepth();
            float defWidth = GetParam_DefWidth();
            float defHeight = GetParam_DefHeight(0);

            m_BodyColl.SetEnableAll();
            m_BodyColl.SetPos(0, LowerPos(pos, 0.5f * width));
            m_BodyColl.SetSize(0, 0.5f * width, height + width, 0.5f * depth);
            m_BodyColl.SetYRot(0, rotY);
            m_BodyColl.SetPushEnable(0);
            m_BodyColl.SetPos(1, LowerPos(pos, 0.5f * width));
            m_BodyColl.SetSize(1, 0.5f * width, ballHeight + width, 0.5f * depth);
            m_BodyColl.SetYRot(1, rotY);
            m_BodyColl.SetPushEnable(1);
            if (IsEnableCatchColl()) {
                m_BodyColl.SetPos(2, LowerPos(pos, 0.5f * width));
                m_BodyColl.SetSize(2, 0.5f * width, height + width, 0.5f * depth);
                m_BodyColl.SetYRot(2, rotY);
                m_BodyColl.SetBodyEnable(2);
                m_BodyColl.SetCatchEnable(2);
            } else {
                m_BodyColl.SetBodyDisable(2);
                m_BodyColl.SetCatchDisable(2);
            }

            // The capsules run from one end of the cars to the other, at
            // the attack height. (Built with the original compiler, the
            // paired-single add of pos2 is not stored, in the DOL as here:
            // only pos2.y is set, x and z are left as they were. Other
            // compilers give the intended pos - dir.)
            float len = 0.5f * (atkDepth - atkWidth);
            if (0.0f > len) {
                len = 0.0f;
            }
            float top = atkHeight - 0.5f * atkWidth;
            if (0.0f > top) {
                top = 0.0f;
            }
            bcs::FVEC3 dir;
            bcs::BCS_VecZ_RotY(dir, len, rotY);
            float bottom = 0.0f;
            float step = top - bottom;
            bcs::FVEC3 pos1;
            bcs::FVEC3 pos2;
            bcs::FVEC3Add(&pos1, &pos, &dir);
            bcs::FVEC3Scale(&dir, &dir, -1.0f);
            bcs::FVEC3Add(&pos2, &pos, &dir);
            for (long i = 0; i < 2; i++) {
                float y = pos.y + (i * step + bottom);
                pos2.y = y;
                pos1.y = y;
                m_AtkColl.SetRadius(i, 0.5f * atkWidth);
                m_AtkColl.SetPos(i, pos1);
                m_AtkColl.SetPos2(i, pos2);
                m_AtkColl.SetEnable(i);
            }
            UpdateDefColl(pos, defWidth, defHeight, depth, rotY);
        } else {
            m_BodyColl.SetDisableAll();
            m_AtkColl.SetDisableAll();
            m_DefColl.SetDisableAll();
        }
    }

    void CStgGmk_Trolley_Act::UpdateDefColl(const bcs::FVEC3 &rPos, float width, float height, float depth,
                                            float rotY)
    {
        if (IsEnableDefColl()) {
            m_DefColl.GetColl(0)->SetPos(LowerPos(rPos, 0.5f * height));
            m_DefColl.GetColl(0)->SetSize(0.5f * width, height + width, 0.5f * depth);
            m_DefColl.GetColl(0)->SetRotYWrapped(rotY);
            m_DefColl.GetColl(0)->OnFlag10(1);
            m_DefColl.GetColl(0)->OnFlagA0(1);
        } else {
            m_DefColl.GetColl(0)->OffFlag10(1);
            m_DefColl.GetColl(0)->OffFlagA0(1);
        }
    }

    // The middle of the train: half its length behind the first car.
    bcs::FVEC3 CStgGmk_Trolley_Act::GetCenterPos() const
    {
        const CStgSrt *pSrt = GetSrt();
        float rotY = GetParam_LocRotY();
        bcs::FVEC3 pos = pSrt->GetPos();
        bcs::FVEC3 ofs(0.0f, 0.0f, 0.0f);
        ofs.z = -0.5f * ((m_nDispMdlNum - 1) * GetParam_MdlDepth());
        bcs::BCS_Vec_RotY(ofs, ofs, rotY);
        bcs::FVEC3Add(&pos, &pos, &ofs);
        return pos;
    }

    // The centre of the train as the collisions see it. Off the rails it
    // is GetCenterPos; on the rails, the distance run less half the rail
    // and half the train, turned by the yaw. The translation of the rail
    // node half a rail along is added to the position first.
    bcs::FVEC3 CStgGmk_Trolley_Act::GetVirtualPos() const
    {
        if (!GetParam_IsRailMotRelate()) {
            return GetCenterPos();
        }
        GetSrt();
        float rotY = GetParam_LocRotY();
        bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
        bcs::FVEC3 ofs(0.0f, 0.0f, 0.0f);
        float railLen = GetParam_RailLength();
        float center = -0.5f * ((m_nDispMdlNum - 1) * GetParam_MdlDepth());
        ofs.z = center - 0.5f * railLen;
        ofs.z += m_fRunDist;
        if (GetParam_IsRailMotRelate() && m_pRailMdl != 0) {
            bcs::FMTX34 mtx;
            if (m_pRailMdl->GetMtx(mtx, 0.5f * railLen, rotY)) {
                bcs::FVEC3 trans(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
                bcs::FVEC3Add(&pos, &pos, &trans);
            }
        }
        bcs::BCS_Vec_RotY(ofs, ofs, rotY);
        bcs::FVEC3Add(&pos, &pos, &ofs);
        return pos;
    }

    float CStgGmk_Trolley_Act::GetAtkTotalWidth() const
    {
        return GetParam_AtkWidth();
    }

    // The length of the train, shortened at the ends by the gap between a
    // car and its attack volume.
    float CStgGmk_Trolley_Act::GetAtkTotalDepth() const
    {
        float gap = GetParam_MdlDepth() - GetParam_AtkDepth();
        return m_nDispMdlNum * GetParam_MdlDepth() - gap;
    }

    float CStgGmk_Trolley_Act::GetBodyTotalWidth() const
    {
        return GetParam_Width();
    }

    float CStgGmk_Trolley_Act::GetBodyTotalDepth() const
    {
        float gap = GetParam_MdlDepth() - GetParam_Depth();
        return m_nDispMdlNum * GetParam_MdlDepth() - gap;
    }

    void CStgGmk_Trolley_Act::OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        pOther->DoBodyPushCallback(pBody, 0);
    }

    unsigned long CStgGmk_Trolley_Act::OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                    const bcs::FVEC3 &rPos)
    {
        return 2;
    }

    // A catch by the attacking side takes one life (when the train has
    // lives), plays the hit, and makes the train invincible for 30 frames;
    // out of lives, it breaks.
    void CStgGmk_Trolley_Act::OnBallCatch(CCollBodyUnit *pBody, CBall *pBall)
    {
        if (FsmqGetState() != STATE_BREAK && pBall->IsOffensive() && !IsInvincible()) {
            if (m_fMaxLife > 0.0f) {
                m_fLife -= 1.0f;
            }
            bcs::FVEC3 pos = pBall->GetPos();
            long eff = GetParam_Effect(2);
            if (eff >= 0) {
                CEffectCtrl::SimpleEffect(eff, 0, &pos, 0, 3, 0, 0, 0);
            }
            long se = GetParam_SE(2);
            if (se >= 0) {
                CGameSnd::Play(se, pos, 1, -1, 0.0f, 1.0f, 0.0f);
            }
            m_fInvincibleTime = 30.0f;
            if (m_fMaxLife > 0.0f && m_fLife <= 0.0f) {
                FsmqChangeState(STATE_BREAK);
            }
            OnDamage(pBody, pBall);
        }
    }

    // A hit on a character shakes the camera.
    void CStgGmk_Trolley_Act::OnAttackHit(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody,
                                          const bcs::FVEC3 &rPos, unsigned long flag)
    {
        if (pBody != 0 && pBody->GetOwnerKind() == 1) {
            long quake = GetParam_HitCameraQuake();
            long idx = pBody->m_nOwnerIdx;
            if (quake >= 0 && idx >= 0) {
                CGame::StartCameraQuake(quake, idx, -1);
            }
        }
    }

    float CStgGmk_Trolley_Act::GetParam_MdlWidth() const
    {
        return 2.5f;
    }

    float CStgGmk_Trolley_Act::GetParam_MdlDepth() const
    {
        return 5.0f;
    }

    float CStgGmk_Trolley_Act::GetParam_MdlHeight() const
    {
        return 1.5f;
    }

    float CStgGmk_Trolley_Act::GetParam_Width() const
    {
        return 2.5f;
    }

    float CStgGmk_Trolley_Act::GetParam_Depth() const
    {
        return 5.0f;
    }

    float CStgGmk_Trolley_Act::GetParam_Height(long type) const
    {
        return 1.5f;
    }

    float CStgGmk_Trolley_Act::GetParam_AtkWidth() const
    {
        return 0.1f * GetParam_Width();
    }

    float CStgGmk_Trolley_Act::GetParam_AtkDepth() const
    {
        return 0.1f * GetParam_Depth();
    }

    float CStgGmk_Trolley_Act::GetParam_AtkHeight() const
    {
        return 0.1f * GetParam_Height(0);
    }

    float CStgGmk_Trolley_Act::GetParam_VelLimit() const
    {
        return 10.0f;
    }

    float CStgGmk_Trolley_Act::GetParam_Gravity() const
    {
        return -49.0f;
    }

    long CStgGmk_Trolley_Act::GetParam_MdlMinNum() const
    {
        return 1;
    }

    long CStgGmk_Trolley_Act::GetParam_MdlMaxNum() const
    {
        return 3;
    }

    long CStgGmk_Trolley_Act::GetParam_Effect(long type) const
    {
        return -1;
    }

    // Sound 0xD97 is the rolling (type 1).
    long CStgGmk_Trolley_Act::GetParam_SE(long type) const
    {
        switch (type) {
            case 0:
                return -1;
            case 1:
                return 0xD97;
            case 2:
            case 3:
                return -1;
        }
        return -1;
    }

    float CStgGmk_Trolley_Act::GetParam_MaxLife() const
    {
        return -1.0f;
    }

    float CStgGmk_Trolley_Act::GetParam_LocRotY() const
    {
        const CStgGmk_Trolley *pParent = static_cast<const CStgGmk_Trolley *>(GetParent());
        if (pParent == 0) {
            return 0.0f;
        }
        return pParent->GetDefaultRotY();
    }

    // How far the train runs: the gimmick's run distance (or the length
    // of the rails) plus its own length.
    float CStgGmk_Trolley_Act::GetParam_RunDist() const
    {
        const CStgGmk_Trolley *pParent = static_cast<const CStgGmk_Trolley *>(GetParent());
        if (pParent == 0) {
            return 0.0f;
        }
        float dist = !GetParam_IsRailMotRelate() ? pParent->GetRunDist() : GetParam_RailLength();
        float len = m_nDispMdlNum * GetParam_MdlDepth();
        return dist + len;
    }

    long CStgGmk_Trolley_Act::GetParam_HitCameraQuake() const
    {
        return -1;
    }

    // Puts the train at the gimmick's default position, picks the number
    // of cars and shows them.
    void CStgGmk_Trolley_Act::DoSpawn()
    {
        RebuildMdl();
        CStgSrt *pSrt = GetSrt();
        CStgGmk_Trolley *pParent = static_cast<CStgGmk_Trolley *>(GetParent());
        CStgMdl *pMdl = GetMdl(0);
        pSrt->SetGravity(GetParam_Gravity());
        pSrt->ClearUnkE4();
        m_bAlive = 1;
        if (pMdl != 0) {
            pMdl->SetVisible(1);
        }
        bcs::FVEC3 pos = pParent->GetDefaultPos();
        float rotY = GetParam_LocRotY();
        pos.y = 0.05f;
        pos.z -= 0.5f * GetParam_MdlDepth();
        bcs::BCS_Vec_RotY(pos, pos, rotY);
        pSrt->SetPos(pos);
        m_SpawnPos = pos;
        bcs::FVEC3 front;
        bcs::BCS_VecOneZ_RotY(front, rotY);
        pSrt->SetFront(front);

        long min = GetParam_MdlMinNum();
        long max = GetParam_MdlMaxNum();
        if (max > min) {
            m_nDispMdlNum = bcs::BCS_RandRange(min, max);
        } else {
            m_nDispMdlNum = min;
        }
        m_nDispMdlNum = m_nDispMdlNum > HEIGHT_TYPE_CAR_NUM ? HEIGHT_TYPE_CAR_NUM
                                                            : (m_nDispMdlNum < 1 ? 1 : m_nDispMdlNum);
        if (pParent != 0) {
            long num = pParent->GetMdlNum();
            for (long i = 1; i < num; i++) {
                CStgMdl *pCar = GetMdl(i);
                if (pCar != 0) {
                    pCar->SetVisible(i < m_nDispMdlNum);
                }
            }
        }
        m_bQuake = 0;
        m_fLife = m_fMaxLife = GetParam_MaxLife();
        m_fInvincibleTime = 0.0f;
    }

    void CStgGmk_Trolley_Act::DoInactivate()
    {
        m_bAlive = 0;
        CStgObj *pParent = GetParent();
        if (pParent != 0) {
            long num = pParent->GetMdlNum();
            for (long i = 0; i < num; i++) {
                CStgMdl *pCar = GetMdl(i);
                if (pCar != 0) {
                    pCar->SetVisible(0);
                }
            }
        }
    }

    // Gives every car its type from the height pattern in use; with a
    // chance of GetParam_CoinModeRate() the heyhos of this run throw coins.
    void CStgGmk_Trolley_Act::RebuildMdl()
    {
        const long *pType;
        SetNextHeightType();
        float rand = bcs::BCS_RandF();
        int bCoin = 0;
        if (rand > 0.0f && rand <= GetParam_CoinModeRate()) {
            bCoin = 1;
        }
        pType = m_aHeightTypeTbl[m_nHeightType];
        for (long i = 0; i < HEIGHT_TYPE_CAR_NUM; i++, pType++) {
            CStgGmk_Trolley_Mdl *pCar = static_cast<CStgGmk_Trolley_Mdl *>(GetMdl(i));
            if (pCar != 0) {
                pCar->DeleteMdl();
                long type = *pType;
                if (type == 2 && bCoin) {
                    type = 3;
                }
                pCar->SetMdlType(type);
                pCar->Create();
                pCar->SceneEntry();
                float distMin = GetParam_ThrowDistMin(bCoin);
                float distMax = GetParam_ThrowDistMax(bCoin);
                pCar->SetHeyhoParam(bCoin, distMin, distMax);
            }
        }
    }

    // Reads the car types of the height patterns from the location data.
    void CStgGmk_Trolley_Act::BuildHeightTypeTbl()
    {
        const LOC_RAW_TBL *pTbl = static_cast<const LOC_RAW_TBL *>(CLoadData::GetPtr(0x6C));
        long valid = GetParam_TrolleyPatValidNum();
        if (valid < 0) {
            m_nHeightTypeNum = 1;
        } else {
            m_nHeightTypeNum = LocRawTblGet(pTbl, valid) >> 12;
        }
        for (long i = 0; i < HEIGHT_TYPE_TBL_NUM; i++) {
            for (long j = 0; j < HEIGHT_TYPE_CAR_NUM; j++) {
                m_aHeightTypeTbl[i][j] = LocRawTblGet(pTbl, GetLocRawTblIdx(i, j)) >> 12;
            }
        }
    }

    // Picks the next height pattern: a fixed one (type 0), the next or the
    // previous (types 2 and 3) or one at random.
    void CStgGmk_Trolley_Act::SetNextHeightType()
    {
        long type = m_nHeightType;
        long num = GetHeightTypeNum();
        switch (GetParam_TrolleyPatType()) {
            case 0:
                type = GetParam_TrolleyPatNo();
                break;
            default:
                type = bcs::BCS_RandMax(num - 1);
                break;
            case 2:
                type++;
                break;
            case 3:
                type--;
                break;
        }
        m_nHeightType = (num + type % num) % num;
    }

    long CStgGmk_Trolley_Act::GetHeightTypeNum() const
    {
        return m_nHeightTypeNum;
    }

    long CStgGmk_Trolley_Act::GetHeightType(long idx) const
    {
        return m_aHeightTypeTbl[m_nHeightType][idx];
    }

    void CStgGmk_Trolley_Act::CtorState_DEFAULT()
    {
        m_bQuake = 0;
        DoInactivate();
        FsmqChangeState(STATE_INACTIVE);
        FsmqRetry();
    }

    void CStgGmk_Trolley_Act::LoopState_DEFAULT()
    {
    }

    void CStgGmk_Trolley_Act::DtorState_DEFAULT()
    {
    }

    void CStgGmk_Trolley_Act::CtorState_STANDBY()
    {
        GetSrt();
        m_fStateTimer = -1.0f;
        m_bQuake = 0;
    }

    // Waits until a time is set, then runs.
    void CStgGmk_Trolley_Act::LoopState_STANDBY()
    {
        float rate = GetFrameRateF();
        if (m_fStateTimer >= 0.0f) {
            m_fStateTimer -= rate;
            FsmqChangeState(STATE_MOVE);
            FsmqRetry();
        }
    }

    void CStgGmk_Trolley_Act::DtorState_STANDBY()
    {
    }

    // The train starts at its top speed, along its direction.
    void CStgGmk_Trolley_Act::CtorState_MOVE()
    {
        CStgSrt *pSrt = GetSrt();
        pSrt->StopAccel();
        pSrt->ClearFlagC8(3);
        pSrt->SetLocalVel(bcs::FVEC3(0.0f, 0.0f, GetParam_VelLimit()));
        m_fRunDist = 0.0f;
        m_fRunDistMax = GetParam_RunDist();
        m_bQuake = 0;
    }

    // The rolling sound and effect play while the train is on the ground
    // and moving; the camera shakes while the train is within the ground's
    // rectangle. Past the run distance the train goes.
    void CStgGmk_Trolley_Act::LoopState_MOVE()
    {
        GetFrameRateF();
        CStgSrt *pSrt = GetSrt();
        bcs::FVEC3 vel = pSrt->GetVel();
        float speed = PSVECMag(reinterpret_cast<const Vec *>(&vel));
        if (pSrt->GetPos().y <= 0.05f && speed > 0.7f) {
            if (!Eff_IsAlive(0)) {
                Eff_CreateAttach(0, GetParam_Effect(1), 0, 0UL, 0, 0, 0, 0, 3);
            }
        } else {
            Eff_Stop(0);
        }
        if (!SE_IsAlive(0)) {
            SE_PlayPos(0, GetParam_SE(1), 0.0f, 3, &m_SePosFunc);
        }

        bcs::FVEC3 pos = GetVirtualPos();
        float width = GetBodyTotalWidth();
        float depth = GetBodyTotalDepth();
        bcs::FVEC3 min(0.5f * -width, 0.0f, 0.5f * -depth);
        bcs::FVEC3 max(0.5f * width, 0.0f, 0.5f * depth);
        bcs::BCS_Vec_RotY(min, min, GetParam_LocRotY());
        bcs::BCS_Vec_RotY(max, max, GetParam_LocRotY());
        bcs::FVEC3Add(&min, &min, &pos);
        bcs::FVEC3Add(&max, &max, &pos);

        float margin = 0.0f;
        float left, right, front, back;
        CStageMan::GetInstance()->GetGroundRect(&left, &right, &front, &back);
        nw4r::math::AABB box(*reinterpret_cast<nw4r::math::VEC3 *>(&min),
                             *reinterpret_cast<nw4r::math::VEC3 *>(&max));
        nw4r::math::VEC3 groundMax(right + margin, 1.0f, back + margin);
        nw4r::math::VEC3 groundMin(left - margin, -1.0f, front - margin);
        nw4r::math::AABB ground(groundMin, groundMax);
        if (nw4r::math::IntersectionAABB(&box, &ground)) {
            if (!m_bQuake) {
                m_bQuake = 1;
                CGame::StartCameraQuake(0x4A, -1, -1);
            }
        } else if (m_bQuake) {
            m_bQuake = 0;
            CGame::StopCameraQuake(0x4A);
        }

        if (m_fRunDist > m_fRunDistMax) {
            FsmqChangeState(STATE_INACTIVE);
            FsmqRetry();
        }
    }

    void CStgGmk_Trolley_Act::DtorState_MOVE()
    {
        GetSrt()->StopRun();
        Eff_Stop(0);
        SE_Stop(0, 0.0f);
        CGame::StopCameraQuake(0x4A);
        m_bQuake = 0;
    }

    void CStgGmk_Trolley_Act::CtorState_BREAK()
    {
    }

    void CStgGmk_Trolley_Act::LoopState_BREAK()
    {
    }

    void CStgGmk_Trolley_Act::DtorState_BREAK()
    {
    }

    void CStgGmk_Trolley_Act::CtorState_RESPAWN()
    {
        DoSpawn();
        FsmqChangeState(STATE_MOVE);
        FsmqRetry();
    }

    void CStgGmk_Trolley_Act::LoopState_RESPAWN()
    {
    }

    void CStgGmk_Trolley_Act::DtorState_RESPAWN()
    {
    }

    void CStgGmk_Trolley_Act::CtorState_INACTIVE()
    {
        DoInactivate();
        m_fStateTimer = -1.0f;
    }

    // Waits until a time is set and has run out, then respawns.
    void CStgGmk_Trolley_Act::LoopState_INACTIVE()
    {
        float rate = GetFrameRateF();
        if (m_fStateTimer >= 0.0f) {
            m_fStateTimer -= rate;
            if (m_fStateTimer <= 0.0f) {
                FsmqChangeState(STATE_RESPAWN);
                FsmqRetry();
            }
        }
    }

    void CStgGmk_Trolley_Act::DtorState_INACTIVE()
    {
    }

    CStgGmk_Trolley_Drv::CStgGmk_Trolley_Drv(CStgGmk *pParent) : CStgDrv(pParent) {}

    CStgGmk_Trolley_Drv::~CStgGmk_Trolley_Drv() {}

    void CStgGmk_Trolley_Drv::Main(float rate)
    {
        CStgDrv::Main(rate);
    }

    CStgGmk_Trolley::CStgGmk_Trolley(bcs::CTask *pParent, int arg)
        : CStgGmk(pParent, 0x14), m_Unk68(arg), m_fDefaultRotY(0.0f), m_fRunDist(0.0f)
    {
        SetType(13);
        m_DefaultPos.Set(0.0f, 0.0f, 0.0f);
    }

    CStgGmk_Trolley::~CStgGmk_Trolley() {}

    CStgMdl *CStgGmk_Trolley::NewMdl(long heap, unsigned long idx)
    {
        return new (heap) CStgGmk_Trolley_Mdl(this, idx);
    }

    void CStgGmk_Trolley::OnPostCreate() {}

    void CStgGmk_Trolley::OnSceneEntry() {}

    void CStgGmk_Trolley::OnSceneLeave() {}

    void CStgGmk_Trolley::Main()
    {
        GetFrameRateF();
        CStgObj::Main();
    }

    void CStgGmk_Trolley::Respawn()
    {
        CStgGmk_Trolley_Act *pAct = GetTrolleyAct();
        if (pAct) {
            pAct->FsmqChangeState(CStgGmk_Trolley_Act::STATE_RESPAWN);
        }
    }

    int CStgGmk_Trolley::IsActive() const
    {
        const CStgGmk_Trolley_Act *pAct = GetTrolleyAct();
        if (pAct) {
            return pAct->IsAlive();
        }
        return 0;
    }

    void CStgGmk_Trolley::Inactivate()
    {
        CStgGmk_Trolley_Act *pAct = GetTrolleyAct();
        if (pAct) {
            pAct->FsmqChangeState(CStgGmk_Trolley_Act::STATE_INACTIVE);
        }
    }

    void CStgGmk_Trolley::Disp() {}
}
