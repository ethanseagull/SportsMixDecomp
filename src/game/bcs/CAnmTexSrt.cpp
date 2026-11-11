#include "bcs/CAnmTexSrt.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/New.hpp"

namespace bcs {

    void CAnmTexSrt::Release()
    {
        UnbindAll();
    }

    CAnmTexSrt::CAnmTexSrt() : m_pResult(0), m_nResultNum(0)
    {
    }

    CAnmTexSrt::~CAnmTexSrt()
    {
        Destroy();
    }

    int CAnmTexSrt::Create(const CMdlObj *pMdl, nw4r::g3d::ResAnmTexSrt res, int bAllocResult, long nHeap)
    {
        nw4r::g3d::ResMdl mdl = pMdl->GetResMdl();
        if (!res.IsValid() || !mdl.IsValid()) {
            return 0;
        }

        m_Res = res;
        if (bAllocResult != 0 && m_Res.GetNumMaterial() > 0) {
            m_nResultNum = m_Res.GetNumMaterial();
            m_pResult = new (nHeap | 0x100) nw4r::g3d::TexSrtAnmResult[m_nResultNum];
        }

        m_nNum = mdl.GetResMatNumEntries();
        m_pTable = new (nHeap | 0x100) unsigned short[m_nNum];
        UnbindAll();
        InitFrame();
        Bind(mdl);
        return 1;
    }

    void CAnmTexSrt::SetFrame(float frame)
    {
        m_Frame.SetFrame(frame);
        CalcResult();
    }

    void CAnmTexSrt::UpdateFrame(float fStep)
    {
        m_Frame.SetFrame(m_Frame.m_fRate * fStep + m_Frame.m_fFrame);
        CalcResult();
    }

    int CAnmTexSrt::Bind(nw4r::g3d::ResMdl mdl)
    {
        int num = m_Res.GetNumMaterial();
        int bBound = 0;
        for (unsigned short i = 0; i < num; i++) {
            nw4r::g3d::ResMat mat = mdl.GetResMat(m_Res.GetMatAnm(i)->GetName());
            if (mat.IsValid()) {
                m_pTable[mat.GetID()] = i;
                bBound = 1;
            }
        }
        OnFlag(ANMFLAG_4);
        return bBound;
    }

    int CAnmTexSrt::ChangeRes(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmTexSrt res)
    {
        UnbindAll();
        if (!res.IsValid() || !mdl.IsValid()) {
            return 0;
        }
        if (m_pResult != 0 && m_nResultNum < res.GetNumMaterial()) {
            return 0;
        }
        if (m_nNum < mdl.GetResMatNumEntries()) {
            return 0;
        }

        m_Res = res;
        InitFrame();
        return Bind(mdl);
    }

    const nw4r::g3d::TexSrtAnmResult *CAnmTexSrt::GetResult(nw4r::g3d::TexSrtAnmResult *pResult, unsigned long idx)
    {
        unsigned short id = m_pTable[idx];
        if (id & 0xC000) {
            pResult->flags = 0;
            return pResult;
        }
        if (m_pResult != 0) {
            return &m_pResult[id];
        }
        m_Res.GetAnmResult(pResult, id, m_Frame.m_fFrame);
        return pResult;
    }

    void CAnmTexSrt::ObjProc(unsigned long nTask, unsigned long, void *pInfo)
    {
        switch (nTask) {
            case 8:
                UpdateFrame(*static_cast<const float *>(pInfo));
                break;
            case 0x10003:
                SetParent(0);
                break;
            case 0x10002:
                SetParent(static_cast<CG3dObj *>(pInfo));
                break;
        }
    }
}
