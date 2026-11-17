#include "game/msw/CTrophyObj.hpp"
#include "bcs/BCS_Color.hpp"
#include "bcs/CAnmMatClr.hpp"
#include "bcs/CAnmTexSrt.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CScnGroup.hpp"
#include "bcs/New.hpp"
#include "game/msw/EvDemo.hpp"
#include "nw4r/g3d/ResAnmClr.hpp"
#include "nw4r/g3d/ResAnmTexPat.hpp"
#include "nw4r/g3d/ResAnmTexSrt.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/g3d/ResMat.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "revolution/gx/GXTexture.h"
#include <stdio.h>
#include <string.h>

// msw::CTrophyObj, the cup model of the tournament, and the model inside it.

namespace msw {

    CTrophyInObj::~CTrophyInObj()
    {
        m_hParent = 0;
    }

    // Hidden while the cup is hidden, with the cup's alpha; draws the cup's
    // task 1 along with its own.
    void CTrophyInObj::ObjProc(unsigned long nTask, unsigned long nArg, void *pArg)
    {
        if (IsProcDisabled(nTask)) {
            return;
        }
        bcs::CScnObj *pParent = 0;
        if (m_hParent != 0) {
            pParent = GetScnObj(m_hParent);
        }
        switch (nTask) {
            case 8:
                OnFlag(FLAG_HIDE_40 | FLAG_HIDE_80);
                if (pParent != 0) {
                    SetAlphaRateF(pParent->GetAlphaRateF());
                    unsigned long flag = pParent->GetFlag();
                    if ((flag & FLAG_HIDE_40) == 0 || (flag & FLAG_HIDE_80) == 0) {
                        OffFlag(FLAG_HIDE_40 | FLAG_HIDE_80);
                    }
                }
                CEventMdlObj::ObjProc(nTask, nArg, pArg);
                break;
            case 1:
                if (pParent != 0) {
                    pParent->ObjProc(nTask, nArg, pArg);
                }
                CEventMdlObj::ObjProc(nTask, nArg, pArg);
                break;
            default:
                CEventMdlObj::ObjProc(nTask, nArg, pArg);
                break;
        }
    }

    CTrophyObj::~CTrophyObj()
    {
        DestroyWithIn();
    }

    void CTrophyObj::Destroy()
    {
        DestroyWithIn();
    }

    bcs::CScnObj *CTrophyObj::GetParentObj()
    {
        return this;
    }

    void CTrophyObj::SetCrystalColor(bcs::CMdlObj *pMdl, nw4r::g3d::ResFile file, long nGameMode, long nCup)
    {
        static const char *s_pModeName[] = { "BK", "VB", "DB", "HK", "MX", "MX" };
        const char *pMode = s_pModeName[nGameMode];
        char name[32];
        sprintf(name, "t%02d_%s", nCup - 1, pMode);

        bcs::CAnmTexSrt *pTexSrt = new bcs::CAnmTexSrt;
        if (pTexSrt->Create(pMdl, file.GetResAnmTexSrt(name), 0, 0)) {
            pMdl->AttachAnmObj(pTexSrt, bcs::CMdlObj::ANMOBJTYPE_TEXSRT);
        } else if (pTexSrt != 0) {
            delete pTexSrt;
        }

        bcs::CAnmMatClr *pMatClr = new bcs::CAnmMatClr;
        if (pMatClr->Create(pMdl, file.GetResAnmClr(name), 0, 0)) {
            pMdl->AttachAnmObj(pMatClr, bcs::CMdlObj::ANMOBJTYPE_MATCLR);
        } else if (pMatClr != 0) {
            delete pMatClr;
        }
    }

    // Whether name is pStr, whose length is len.
    static inline bool IsTexName(nw4r::g3d::ResName name, const char *pStr, unsigned long len)
    {
        bool bSame = false;
        if (len == name.GetLength() && strcmp(name.GetName(), pStr) == 0) {
            bSame = true;
        }
        return bSame;
    }

    // Binds the texture pName of the stage's environment archive to every
    // texture of that name in pMdl's materials, and gives "Mat_trophy" the
    // cup's TEV colour.
    void CTrophyObj::SetCrystalEnvTex(bcs::CMdlObj *pMdl, const char *pName, long nArc, long nStageGameMode, long nCup)
    {
        static const char *s_pEnvFileName[] = { "cenv_ba", "cenv_vo", "cenv_do", "cenv_ho", "cenv_ba", "cenv_ba" };
        static const char *s_pClrAnmName[] = { "t00", "t01", "t02", "t03" };

        if (pName == 0) {
            return;
        }
        long nIndex;
        nw4r::g3d::ResFile file = EvDemo::GetArcResDataMdl(nIndex, nArc, s_pEnvFileName[nStageGameMode], 0);
        if (!file.IsValid()) {
            return;
        }
        nw4r::g3d::ResAnmClr clr = file.GetResAnmClr(s_pClrAnmName[nCup - 1]);
        if (!clr.IsValid()) {
            return;
        }
        nw4r::g3d::ClrAnmResult result;
        clr.GetAnmResult(&result, 0, 0.0f);
        if (result.bRgbaExist & (1 << 4)) {
            pMdl->SetMatTevColor("Mat_trophy", GX_TEVREG0, bcs::BCS_Color(result.rgba[4]));
        }

        nw4r::g3d::ResTex tex = file.GetResTex(pName);
        if (!tex.IsValid()) {
            return;
        }
        void *pImage;
        unsigned short width;
        unsigned short height;
        GXTexFmt fmt;
        float minLod;
        float maxLod;
        unsigned char mipmap;
        tex.GetTexObjParam(&pImage, &width, &height, &fmt, &minLod, &maxLod, &mipmap);
        GXTexObj texObj;
        GXInitTexObj(&texObj, pImage, width, height, fmt, GX_CLAMP, GX_CLAMP, mipmap);
        GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, minLod, maxLod, 0.0f, 0, 0, GX_ANISO_1);

        unsigned long len = strlen(pName);
        nw4r::g3d::ResMdl mdl = pMdl->GetResMdl();
        unsigned long nMat = mdl.GetResMatNumEntries();
        for (unsigned long i = 0; i < nMat; i++) {
            nw4r::g3d::ResMat mat = mdl.GetResMat(i);
            if (mat.IsValid()) {
                bcs::CMdlObj::CopiedMatAccess access(pMdl, i);
                nw4r::g3d::ResTexObj texObjRes = access.GetResTexObjEx();
                unsigned long num = mat.GetNumResTexPlttInfo();
                for (unsigned long j = 0; j < num; j++) {
                    if (IsTexName(mat.GetResTexPlttInfo(j).GetTexResName(), pName, len)) {
                        *texObjRes.GetTexObj(static_cast<_GXTexMapID>(j)) = texObj;
                        texObjRes.Validate(static_cast<_GXTexMapID>(j));
                    }
                }
            }
        }
    }

    CTrophyObj *CTrophyObj::CreateTrophy(nw4r::g3d::ResFile file, long nCup, long nGameMode, int bLarge)
    {
        CTrophyObj *pTrophy = new (0) CTrophyObj;
        pTrophy->Create(bLarge ? file.GetResMdl("trophyL") : file.GetResMdl("trophy"), 0x30C, 0);
        pTrophy->OnFlag(0x2000);
        pTrophy->SetExBlendMode(1);
        pTrophy->OnFlag(0x4000);
        return pTrophy;
    }

    CTrophyInObj *CTrophyObj::CreateTrophyIn(CTrophyObj *pTrophy, nw4r::g3d::ResFile file, long nCup, long nGameMode)
    {
        static const char *s_pMdlName[] = { "m10", "m11", "m12", "m13", "m10", "m10" };

        if (nGameMode > 3) {
            return 0;
        }
        nw4r::g3d::ResMdl mdl = file.GetResMdl(s_pMdlName[nGameMode]);
        CTrophyInObj *pIn = new (0) CTrophyInObj;
        pTrophy->m_hIn = pIn->GetHandle();
        pIn->m_hParent = pTrophy->GetHandle();
        pIn->Create(mdl, 0, 0);
        pIn->OnFlag(0x2000);
        pIn->SetExBlendMode(1);
        pIn->OnFlag(0x4000);
        pIn->m_vTrans.x = 0.0f;
        pIn->m_vTrans.y = 0.0f;
        pIn->m_vTrans.z = 0.0f;
        pIn->SetRotXYZ(0.0f, 0.0f, 0.0f);
        pIn->m_vScale.x = 1.0f;
        pIn->m_vScale.y = 1.0f;
        pIn->m_vScale.z = 1.0f;
        pIn->AttachToObject(pTrophy, 0, ATTACHMODE_0);
        return pIn;
    }

    void CTrophyObj::SetDisp(int bDisp)
    {
        if (!bDisp) {
            OnFlag(FLAG_HIDE_40 | FLAG_HIDE_80);
        } else {
            OffFlag(FLAG_HIDE_40 | FLAG_HIDE_80);
        }
    }

    void CTrophyObj::_SetDrawPriority(long nPriority)
    {
        SetDrawPriorityWithIn(nPriority);
    }

    void CTrophyObj::_SetLightSelector(long nSelector)
    {
        SetLightSelectorWithIn(nSelector);
    }

    void CTrophyObj::_ResetLightSelector()
    {
        bcs::CMdlObj *pIn = GetInObj();
        if (pIn != 0) {
            pIn->ResetLightSelector();
        }
        bcs::CMdlObj::ResetLightSelector();
    }

    void CTrophyObj::_SetBaseColor(bcs::BCS_Color color)
    {
        bcs::CMdlObj *pIn = GetInObj();
        if (pIn != 0) {
            pIn->SetSilhouetteColor(color);
        }
        SetSilhouetteColor(color);
    }

    void CTrophyObj::_SetColorScale(bcs::BCS_Color color)
    {
        bcs::CMdlObj *pIn = GetInObj();
        if (pIn != 0) {
            pIn->SetSilhouetteColor(color);
        }
        SetSilhouetteColor(color);
    }

    void CTrophyObj::_PushBack(bcs::CScnGroup *pGroup)
    {
        pGroup->AppendChild(this);
    }

    void CTrophyObj::_PushBack(long nLayer)
    {
        PushBackWithIn(nLayer);
    }

    CTrophyObj *CTrophyObj::CreateModel(long nCup, long nGameMode, long nLightSel, long nLayer, const char *pEnvTex)
    {
        char name[32];
        sprintf(name, "trophy%02d", nCup);
        long nIndex;
        nw4r::g3d::ResFile file = EvDemo::GetArcResDataMdl(nIndex, 0x5C, name, 0);
        nw4r::g3d::ResFile fileIn = EvDemo::GetArcResDataMdl(nIndex, 0x5C, "trophyIn", 0);
        CTrophyObj *pTrophy = CreateTrophy(file, nCup, nGameMode, 1);
        CreateTrophyIn(pTrophy, fileIn, nCup, nGameMode);
        SetCrystalColor(pTrophy, file, nGameMode, nCup);
        SetCrystalEnvTex(pTrophy, pEnvTex, 0x5C, nGameMode, nCup);
        pTrophy->PushBackWithIn(nLayer);
        pTrophy->SetLightSelectorWithIn(nLightSel);
        pTrophy->SetDrawPriorityWithIn(0);
        return pTrophy;
    }
}
