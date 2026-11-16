#include "bcs/CMdlObj.hpp"

namespace bcs {

    // nw4r's resource handles check the alignment of their pointer when
    // built. The project's declarations of ResTlutObj, ResMatChan,
    // ResGenMode, ResMatMisc, ResMatTevColor and ResMatIndMtxAndScale (in
    // nw4r/g3d/ResMat.hpp) do not have the check yet, so it is made here
    // before building them, with the original's file and lines.
    static inline void CheckAlign4(const void *p, int nLine)
    {
        if ((unsigned long)p & 0x3) {
            nw4r::db::Panic("g3d_resmat_ac.h", nLine, "NW4R:Failed assertion !((u32)p & 0x3)");
        }
    }

    static inline void CheckAlign32(const void *p, int nLine)
    {
        if ((unsigned long)p & 0x1F) {
            nw4r::db::Panic("g3d_resmat_ac.h", nLine, "NW4R:Failed assertion !((u32)p & 0x1f)");
        }
    }

    CMdlObj::CopiedMatAccess::CopiedMatAccess(CMdlObj *pMdl, unsigned long nMat) : m_pMdl(0), m_nMat(nMat)
    {
        if (pMdl != 0 && pMdl->GetResMdl().GetResMat(nMat).IsValid()) {
            m_pMdl = pMdl;
            m_nMat = nMat;
            if (pMdl->m_pTexObjBuf != 0) {
                m_TexObj = nw4r::g3d::ResTexObj(&pMdl->m_pTexObjBuf[nMat]);
            }
            if (pMdl->m_pTlutObjBuf != 0) {
                nw4r::g3d::ResTlutObjData *p = &pMdl->m_pTlutObjBuf[nMat];
                CheckAlign4(p, 74);
                m_TlutObj = nw4r::g3d::ResTlutObj(p);
            }
            if (pMdl->m_pTexSrtBuf != 0) {
                m_TexSrt = nw4r::g3d::ResTexSrt(&pMdl->m_pTexSrtBuf[nMat]);
            }
            if (pMdl->m_pChanBuf != 0) {
                nw4r::g3d::ResMatChanData *p = &pMdl->m_pChanBuf[nMat];
                CheckAlign4(p, 465);
                m_Chan = nw4r::g3d::ResMatChan(p);
            }
            if (pMdl->m_pGenModeBuf != 0) {
                nw4r::g3d::ResGenModeData *p = &pMdl->m_pGenModeBuf[nMat];
                CheckAlign4(p, 175);
                m_GenMode = nw4r::g3d::ResGenMode(p);
            }
            if (pMdl->m_pMatMiscBuf != 0) {
                nw4r::g3d::ResMatMiscData *p = &pMdl->m_pMatMiscBuf[nMat];
                CheckAlign4(p, 243);
                m_MatMisc = nw4r::g3d::ResMatMisc(p);
            }
            if (pMdl->m_pPixBuf != 0) {
                m_Pix = nw4r::g3d::ResMatPix(&pMdl->m_pPixBuf[nMat]);
            }
            if (pMdl->m_pTevColorBuf != 0) {
                nw4r::g3d::ResTevColorDL *p = &pMdl->m_pTevColorBuf[nMat];
                CheckAlign32(p, 378);
                m_TevColor = nw4r::g3d::ResMatTevColor(p);
            }
            if (pMdl->m_pIndMtxAndScaleBuf != 0) {
                nw4r::g3d::ResIndMtxAndScaleDL *p = &pMdl->m_pIndMtxAndScaleBuf[nMat];
                CheckAlign32(p, 413);
                m_IndMtxAndScale = nw4r::g3d::ResMatIndMtxAndScale(p);
            }
            if (pMdl->m_pTexCoordGenBuf != 0) {
                m_TexCoordGen = nw4r::g3d::ResMatTexCoordGen(&pMdl->m_pTexCoordGenBuf[nMat]);
            }
            if (pMdl->m_pTevBuf != 0) {
                m_Tev = nw4r::g3d::ResTev(&pMdl->m_pTevBuf[nMat]);
            }
        }
    }

    CMdlObj::CopiedMatAccess::CopiedMatAccess(const char *pMatName, CMdlObj *pMdl) : m_pMdl(0), m_nMat(0)
    {
        if (pMdl != 0 && pMdl->GetResMdl().GetResMat(pMatName).IsValid()) {
            m_pMdl = pMdl;
            m_nMat = pMdl->GetResMdl().GetResMat(pMatName).GetID();
            CMdlObj *p = m_pMdl;
            if (p->m_pTexObjBuf != 0) {
                m_TexObj = nw4r::g3d::ResTexObj(&p->m_pTexObjBuf[m_nMat]);
            }
            if (p->m_pTlutObjBuf != 0) {
                nw4r::g3d::ResTlutObjData *pData = &p->m_pTlutObjBuf[m_nMat];
                CheckAlign4(pData, 74);
                m_TlutObj = nw4r::g3d::ResTlutObj(pData);
            }
            if (p->m_pTexSrtBuf != 0) {
                m_TexSrt = nw4r::g3d::ResTexSrt(&p->m_pTexSrtBuf[m_nMat]);
            }
            if (p->m_pChanBuf != 0) {
                nw4r::g3d::ResMatChanData *pData = &p->m_pChanBuf[m_nMat];
                CheckAlign4(pData, 465);
                m_Chan = nw4r::g3d::ResMatChan(pData);
            }
            if (p->m_pGenModeBuf != 0) {
                nw4r::g3d::ResGenModeData *pData = &p->m_pGenModeBuf[m_nMat];
                CheckAlign4(pData, 175);
                m_GenMode = nw4r::g3d::ResGenMode(pData);
            }
            if (p->m_pMatMiscBuf != 0) {
                nw4r::g3d::ResMatMiscData *pData = &p->m_pMatMiscBuf[m_nMat];
                CheckAlign4(pData, 243);
                m_MatMisc = nw4r::g3d::ResMatMisc(pData);
            }
            if (p->m_pPixBuf != 0) {
                m_Pix = nw4r::g3d::ResMatPix(&p->m_pPixBuf[m_nMat]);
            }
            if (p->m_pTevColorBuf != 0) {
                nw4r::g3d::ResTevColorDL *pData = &p->m_pTevColorBuf[m_nMat];
                CheckAlign32(pData, 378);
                m_TevColor = nw4r::g3d::ResMatTevColor(pData);
            }
            if (p->m_pIndMtxAndScaleBuf != 0) {
                nw4r::g3d::ResIndMtxAndScaleDL *pData = &p->m_pIndMtxAndScaleBuf[m_nMat];
                CheckAlign32(pData, 413);
                m_IndMtxAndScale = nw4r::g3d::ResMatIndMtxAndScale(pData);
            }
            if (p->m_pTexCoordGenBuf != 0) {
                m_TexCoordGen = nw4r::g3d::ResMatTexCoordGen(&p->m_pTexCoordGenBuf[m_nMat]);
            }
            if (p->m_pTevBuf != 0) {
                m_Tev = nw4r::g3d::ResTev(&p->m_pTevBuf[m_nMat]);
            }
        }
    }

    nw4r::g3d::ResTexObj CMdlObj::CopiedMatAccess::GetResTexObj(int bMarkDirty)
    {
        if (m_pMdl != 0 && m_TexObj.IsValid()) {
            if (bMarkDirty) {
                m_pMdl->m_pMatBufDirtyFlag[m_nMat] |= 0x01;
            }
            return m_TexObj;
        }
        return nw4r::g3d::ResTexObj(0);
    }

    nw4r::g3d::ResTexSrt CMdlObj::CopiedMatAccess::GetResTexSrt(bool bMarkDirty)
    {
        if (m_pMdl != 0 && m_TexSrt.IsValid()) {
            if (bMarkDirty) {
                m_pMdl->m_pMatBufDirtyFlag[m_nMat] |= 0x04;
            }
            return m_TexSrt;
        }
        return nw4r::g3d::ResTexSrt(0);
    }

    nw4r::g3d::ResGenMode CMdlObj::CopiedMatAccess::GetResGenMode(bool bMarkDirty)
    {
        if (m_pMdl != 0 && m_GenMode.IsValid()) {
            if (bMarkDirty) {
                m_pMdl->m_pMatBufDirtyFlag[m_nMat] |= 0x10;
            }
            return m_GenMode;
        }
        return nw4r::g3d::ResGenMode(0);
    }

    nw4r::g3d::ResMatMisc CMdlObj::CopiedMatAccess::GetResMatMisc(bool bMarkDirty)
    {
        if (m_pMdl != 0 && m_MatMisc.IsValid()) {
            if (bMarkDirty) {
                m_pMdl->m_pMatBufDirtyFlag[m_nMat] |= 0x20;
            }
            return m_MatMisc;
        }
        return nw4r::g3d::ResMatMisc(0);
    }

    nw4r::g3d::ResMatPix CMdlObj::CopiedMatAccess::GetResMatPix(bool bMarkDirty)
    {
        if (m_pMdl != 0 && m_Pix.IsValid()) {
            if (bMarkDirty) {
                m_pMdl->m_pMatBufDirtyFlag[m_nMat] |= 0x80;
            }
            return m_Pix;
        }
        return nw4r::g3d::ResMatPix(0);
    }
}
