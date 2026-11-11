// msw::CStageNetVolley: the net across the volley court, with its actor
// (which carries the models along) and its models. The net model plays a
// hit motion for the spot of the net the ball touched (a grid of 5 by 3
// cells per side), then goes back to its idle motion.
#include "game/msw/CStageNetVolley.hpp"

#include <stdio.h>

#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/New.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CStgMdlObjBase.hpp"
#include "game/msw/CStgSrt.hpp"

namespace {

    // The suffixes of the net's motion names ("hit_pos_" and a cell number;
    // "99" is the idle motion) and of the goal's ("", "_m01").
    const char *s_NetAnmSuffixTbl[] = {
        "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
        "10", "11", "12", "13", "14", "15", "16", "99", 0,
    };
    const char *s_GoalAnmSuffixTbl[] = { "", "_m01", 0 };

    // How each motion of the net is played (the CGoalMdl::ANM_PARAM layout;
    // the table ends with id -1). Field meanings guessed from the values.
    struct ANM_PARAM {
        long nId;     // index into s_NetAnmSuffixTbl
        long nLoop;
        float fSpeed;
        float fFrame;
        float fBlend;
        long nFlag;
    };

    const ANM_PARAM s_NetAnmParamTbl[] = {
        { 0, 2, 1.0f, 0.0f, 0.0f, -1 },
        { 16, 2, 1.0f, 0.0f, 0.0f, -1 },
        { 0, 2, 1.0f, 30.0f, 0.0f, -1 },
        { 16, 2, 1.0f, 10.0f, 0.0f, -1 },
        { 1, 0, 1.0f, 5.0f, 0.0f, -1 },
        { 2, 0, 1.0f, 5.0f, 0.0f, -1 },
        { 3, 0, 1.0f, 5.0f, 0.0f, -1 },
        { 4, 0, 1.0f, 5.0f, 0.0f, -1 },
        { 5, 0, 1.0f, 5.0f, 0.0f, -1 },
        { 6, 0, 1.0f, 5.0f, 0.0f, -1 },
        { 7, 0, 1.0f, 5.0f, 0.0f, -1 },
        { 8, 0, 1.0f, 5.0f, 0.0f, -1 },
        { 9, 0, 1.0f, 5.0f, 0.0f, -1 },
        { 10, 0, 1.0f, 5.0f, 0.0f, -1 },
        { 11, 0, 1.0f, 5.0f, 0.0f, -1 },
        { 12, 0, 1.0f, 5.0f, 0.0f, -1 },
        { 13, 0, 1.0f, 5.0f, 0.0f, -1 },
        { 14, 0, 1.0f, 5.0f, 0.0f, -1 },
        { 15, 0, 1.0f, 5.0f, 0.0f, -1 },
        { 1, 1, 1.0f, 5.0f, 0.0f, -1 },
        { 2, 1, 1.0f, 5.0f, 0.0f, -1 },
        { 3, 1, 1.0f, 5.0f, 0.0f, -1 },
        { 4, 1, 1.0f, 5.0f, 0.0f, -1 },
        { 5, 1, 1.0f, 5.0f, 0.0f, -1 },
        { 6, 1, 1.0f, 5.0f, 0.0f, -1 },
        { 7, 1, 1.0f, 5.0f, 0.0f, -1 },
        { 8, 1, 1.0f, 5.0f, 0.0f, -1 },
        { 9, 1, 1.0f, 5.0f, 0.0f, -1 },
        { 10, 1, 1.0f, 5.0f, 0.0f, -1 },
        { 11, 1, 1.0f, 5.0f, 0.0f, -1 },
        { 12, 1, 1.0f, 5.0f, 0.0f, -1 },
        { 13, 1, 1.0f, 5.0f, 0.0f, -1 },
        { 14, 1, 1.0f, 5.0f, 0.0f, -1 },
        { 15, 1, 1.0f, 5.0f, 0.0f, -1 },
        { -1, 0, 0.0f, 0.0f, 0.0f, 0 },
    };

    // The per-node motion settings of the net model, by stage. The last
    // field's meaning is not known (the kind 8 nodes carry values in it).

    // Stage 2.
    const bcs::CAnmObjMotBase::NODE_EX_INFO s_NodeExInfo02[] = {
        { 0, 3, 0, 0 }, { 1, 1, 0, 0 }, { 2, 1, 1, 0 }, { 3, 1, 1, 0 },
        { 4, 1, 1, 0 }, { 5, 1, 1, 0 }, { 6, 1, 1, 0 }, { 7, 1, 1, 0 },
        { 8, 1, 1, 0 }, { 9, 1, 1, 0 }, { 10, 3, 0, 0 },
        { 11, 8, 1, 0xD8A0 }, { 12, 8, 1, 0xE884 }, { 13, 8, 1, 0x9824 },
        { 14, 8, 1, 0xD8A0 }, { 15, 8, 1, 0xE884 }, { 16, 8, 1, 0x9824 },
        { 17, 8, 1, 0xD8A0 }, { 18, 8, 1, 0xE884 }, { 19, 8, 1, 0x9824 },
        { 20, 8, 1, 0xD8A0 }, { 21, 8, 1, 0xE884 }, { 22, 8, 1, 0x9824 },
        { 23, 8, 1, 0xD8A0 }, { 24, 8, 1, 0xE884 }, { 25, 8, 1, 0x9824 },
        { 26, 1, 1, 0 }, { 27, 1, 1, 0 }, { 28, 1, 1, 0 }, { 29, 1, 1, 0 },
        { 30, 1, 1, 0 }, { 31, 1, 1, 0 }, { 32, 1, 1, 0 }, { 33, 1, 1, 0 },
        { 34, 1, 1, 0 },
    };

    // Stage 5.
    const bcs::CAnmObjMotBase::NODE_EX_INFO s_NodeExInfo05[] = {
        { 0, 3, 0, 0 }, { 1, 1, 1, 0 }, { 2, 6, 1, 0 }, { 3, 7, 1, 0 },
        { 4, 7, 1, 0 }, { 5, 1, 1, 0 }, { 6, 6, 1, 0 }, { 7, 7, 1, 0 },
        { 8, 7, 1, 0 }, { 9, 1, 1, 0 }, { 10, 6, 1, 0 }, { 11, 7, 1, 0 },
        { 12, 7, 1, 0 }, { 13, 1, 1, 0 }, { 14, 6, 1, 0 }, { 15, 7, 1, 0 },
        { 16, 7, 1, 0 }, { 17, 1, 1, 0 }, { 18, 6, 1, 0 }, { 19, 7, 1, 0 },
        { 20, 7, 1, 0 }, { 21, 1, 1, 0 }, { 22, 6, 1, 0 }, { 23, 7, 1, 0 },
        { 24, 7, 1, 0 }, { 25, 1, 1, 0 }, { 26, 6, 1, 0 }, { 27, 7, 1, 0 },
        { 28, 7, 1, 0 },
    };

    // Stage 6.
    const bcs::CAnmObjMotBase::NODE_EX_INFO s_NodeExInfo06[] = {
        { 0, 3, 0, 0 }, { 1, 3, 0, 0 },
        { 2, 8, 1, 0xD8A0 }, { 3, 8, 1, 0xE884 }, { 4, 8, 1, 0x9824 },
        { 5, 8, 1, 0xD8A0 }, { 6, 8, 1, 0xE884 }, { 7, 8, 1, 0x9824 },
        { 8, 8, 1, 0xD8A0 }, { 9, 8, 1, 0xE884 }, { 10, 8, 1, 0x9824 },
        { 11, 8, 1, 0xD8A0 }, { 12, 8, 1, 0xE884 }, { 13, 8, 1, 0x9824 },
        { 14, 8, 1, 0xD8A0 }, { 15, 8, 1, 0xE884 }, { 16, 8, 1, 0x9824 },
        { 17, 8, 1, 0x08A0 }, { 18, 1, 1, 0x08A0 }, { 19, 1, 1, 0x08A0 },
        { 20, 1, 1, 0 }, { 21, 1, 1, 0x08A0 },
    };

    // Stage 11.
    const bcs::CAnmObjMotBase::NODE_EX_INFO s_NodeExInfo11[] = {
        { 0, 3, 0, 0 }, { 1, 8, 1, 0x0800 }, { 2, 1, 1, 0 }, { 3, 6, 1, 0 },
        { 4, 7, 1, 0 }, { 5, 7, 1, 0 }, { 6, 7, 1, 0 }, { 7, 1, 1, 0 },
        { 8, 6, 1, 0 }, { 9, 7, 1, 0 }, { 10, 7, 1, 0 }, { 11, 7, 1, 0 },
        { 12, 1, 1, 0 }, { 13, 6, 1, 0 }, { 14, 7, 1, 0 }, { 15, 7, 1, 0 },
        { 16, 7, 1, 0 }, { 17, 1, 1, 0 }, { 18, 6, 1, 0 }, { 19, 7, 1, 0 },
        { 20, 7, 1, 0 }, { 21, 7, 1, 0 }, { 22, 1, 1, 0 }, { 23, 6, 1, 0 },
        { 24, 7, 1, 0 }, { 25, 7, 1, 0 }, { 26, 7, 1, 0 },
    };

    // The node settings by stage; the table ends with stage -1.
    struct NODE_EX_INFO_TBL {
        long nStage;
        const bcs::CAnmObjMotBase::NODE_EX_INFO *pInfo;
    };

    const NODE_EX_INFO_TBL s_NodeExInfoTbl[] = {
        { 2, s_NodeExInfo02 },
        { 5, s_NodeExInfo05 },
        { 6, s_NodeExInfo06 },
        { 11, s_NodeExInfo11 },
        { -1, 0 },
    };
}

namespace msw {

    CStageNetVolley::CStageNetVolley(bcs::CTask *pParent)
        : CStageNet(pParent, MDL_NUM)
    {
        m_bIdle = 1;
    }

    CStageNetVolley::~CStageNetVolley()
    {
    }

    // Back to the idle motion when a hit motion ends.
    void CStageNetVolley::Main()
    {
        CStageNet::Main();
        CStgMdl *pMdl = GetMdl(MDL_NET);
        if (pMdl != 0 && pMdl->IsAnmEnd(1)) {
            pMdl->SetAnmEx(GetIdleMotID(), 1);
            m_bIdle = 1;
        }
    }

    long CStageNetVolley::GetIdleMotID() const
    {
        return 0;
    }

    // The net is 14 wide and hangs from height 3 down to 1 (relative to the
    // net's position); the hit spot picks one of 5 by 3 cells (clamped to
    // 0 to 5 across and 0 to 3 down) and the side the ball came from one of
    // two sets of hit motions (from 4 on).
    void CStageNetVolley::Hit(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rVel)
    {
        // static-order swap: declared in the order of their .rodata bytes;
        // the original's @LOCAL@ names show width, top, bottom.
        static const float bottom = 1.0f;
        static const float top = 3.0f;
        static const float width = 14.0f;

        int nCol = 5;
        int nRow = 3;
        CStgMdl *pMdl = GetMdl(MDL_NET);
        if (pMdl != 0) {
            bcs::FVEC3 ofs;
            bcs::FVEC3Sub(&ofs, &rPos, &GetSrt()->GetPos());
            float u = (ofs.x + width * 0.5f) / width;
            float v = (ofs.y - top) / (bottom - top);
            int col = (int)(nCol * u);
            int row = (int)(nRow * v);
            col = col > nCol ? nCol : (col < 0 ? 0 : col);
            row = row > nRow ? nRow : (row < 0 ? 0 : row);
            int cell = col + row * nCol;
            int base = rVel.z < 0.0f ? 4 : nCol * nRow + 4;
            pMdl->SetAnmEx(base + cell, 1);
            m_bIdle = 0;
        }
    }

    void CStageNetVolley::Reset()
    {
        CStgMdl *pMdl = GetMdl(MDL_NET);
        if (pMdl != 0) {
            pMdl->SetAnm(0, 1, 1.0f, 0.0f, 0.0f, -1);
        }
        m_bIdle = 1;
    }

    // Every model hangs on the frame; the low-detail ones go to layer 4.
    void CStageNetVolley::OnPostCreate()
    {
        CStgMdl *pFrame = GetMdl(MDL_FRAME);
        for (unsigned long i = 0; i < MDL_NUM; i++) {
            CStgMdl *pMdl = GetMdl(i);
            if (pMdl != 0) {
                unsigned short layer = 2;
                if (i >= MDL_NET_LO && i <= MDL_GOAL_LO_02) {
                    layer = 4;
                }
                pMdl->SetLayer(layer);
                if (pFrame != 0) {
                    pMdl->AttachToMdl(pFrame, 0, bcs::CScnObj::AttachMode(0));
                }
            }
        }
        Reset();
    }

    CStageNetActVolley::~CStageNetActVolley()
    {
    }

    void CStageNetActVolley::OnUpdate(float rate)
    {
    }

    void CStageNetActVolley::UpdateMdlMtx()
    {
        for (int i = 0; i < CStageNetVolley::MDL_NUM; i++) {
            CStgMdl *pMdl = GetParent()->GetMdl(i);
            if (pMdl != 0) {
                pMdl->SetMtx(GetSrt()->GetMtx());
            }
        }
    }

    CStageNetMdlVolley::~CStageNetMdlVolley()
    {
    }

    // The model names are numbered by stage. The net (model 1) plays the
    // hit_pos_ motions; goal part 2 has the "", "_m01" motions of its own
    // model name.
    void CStageNetMdlVolley::Create()
    {
        if (!CLoadData::GetRes(1).IsValid()) {
            return;
        }

        const char *mdlNameTbl[] = {
            0,
            "s%02d_goalnet_01",
            "s%02d_goal_01",
            "s%02d_goal_02",
            "s%02d_goalnet_lo_01",
            "s%02d_goal_lo_01",
            "s%02d_goal_lo_02",
        };
        const char *shadowNameTbl[CStageNetVolley::MDL_NUM] = { 0 };
        const char *refNameTbl[CStageNetVolley::MDL_NUM] = { 0 };
        MDL_PARAM param;

        char anmName[0x40];
        snprintf(anmName, sizeof(anmName) - 1, "hit_pos_");

        unsigned long idx = GetIndex();
        char mdlName[0x80];
        char shadowName[0x80];
        char refName[0x80];

        if (mdlNameTbl[idx] != 0) {
            sprintf(mdlName, mdlNameTbl[idx], CGame::g_GameWork.GetStage());
            param.pMdlName = mdlName;
        } else {
            param.pMdlName = 0;
        }
        if (shadowNameTbl[idx] != 0) {
            sprintf(shadowName, shadowNameTbl[idx], CGame::g_GameWork.GetStage());
            param.pShadowName = shadowName;
        } else {
            param.pShadowName = 0;
        }
        if (refNameTbl[idx] != 0) {
            sprintf(refName, refNameTbl[idx], CGame::g_GameWork.GetStage());
            param.pRefName = refName;
        } else {
            param.pRefName = 0;
        }
        param.unk0C = 4;
        param.ppAnmSuffix = idx == CStageNetVolley::MDL_NET       ? s_NetAnmSuffixTbl
                            : idx == CStageNetVolley::MDL_GOAL_01 ? s_GoalAnmSuffixTbl
                                                                  : 0;
        param.pAnmName = idx == CStageNetVolley::MDL_NET       ? anmName
                         : idx == CStageNetVolley::MDL_GOAL_01 ? mdlName
                                                               : 0;
        param.unk18 = idx == CStageNetVolley::MDL_NET
                          ? (unsigned long)FindNodeExInfoTbl(CGame::g_GameWork.GetStage())
                          : 0;
        param.pAnmParam = idx == CStageNetVolley::MDL_NET ? s_NetAnmParamTbl : 0;
        CStgMdl::Create(1, &param);
    }

    const bcs::CAnmObjMotBase::NODE_EX_INFO *CStageNetMdlVolley::FindNodeExInfoTbl(long stage) const
    {
        const NODE_EX_INFO_TBL *p = s_NodeExInfoTbl;
        for (; p->nStage >= 0; p++) {
            if (p->nStage == stage) {
                break;
            }
        }
        return p->pInfo;
    }
}
