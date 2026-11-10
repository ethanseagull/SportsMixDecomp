#include "game/msw/CEvDemoRes.hpp"

namespace msw {

    namespace {
        // Shadow sizes, indexed by the low three bits of the byte at 0x16
        // (-1: no shadow). (name guessed)
        const short s_ShadowSize[] = { -1, 32, 64, 128, 256, 512 };
    }

    short CEvDemoRes::_CHR_INFO::GetShadowSize() const
    {
        return s_ShadowSize[nDispInfo & 7];
    }

    int CEvDemoRes::_CHR_INFO::IsDisp() const
    {
        return (nDispInfo >> 7) & 1;
    }

    int CEvDemoRes::_CHR_INFO::IsBind() const
    {
        return m_Flag.bBind;
    }

    void CEvDemoRes::_CHR_INFO::SetBind(int bBind)
    {
        if (bBind) {
            m_Flag.bBind = 1;
        } else {
            m_Flag.bBind = 0;
        }
    }

    int CEvDemoRes::_CHR_INFO::IsCreate() const
    {
        return m_Flag.bCreate;
    }

    void CEvDemoRes::_CHR_INFO::SetCreate(int bCreate)
    {
        if (bCreate) {
            m_Flag.bCreate = 1;
        } else {
            m_Flag.bCreate = 0;
        }
    }

    void CEvDemoRes::_CHR_INFO::SetRelease(int bRelease)
    {
        if (bRelease) {
            m_Flag.bRelease = 1;
        } else {
            m_Flag.bRelease = 0;
        }
    }

    void CEvDemoRes::_CHR_INFO::SetTeam(long nTeam, long nNo)
    {
        this->nTeam[0] = nTeam;
        this->nTeam[1] = nNo;
    }

    void CEvDemoRes::_CHR_INFO::SetDispInfo(int bDisp, long nDrawLayer, long nLightSelector, long nDispInfo)
    {
        this->nDrawLayer = nDrawLayer;
        this->nLightSelector = nLightSelector;
        this->nDispInfo = nDispInfo;
        if (bDisp) {
            this->nDispInfo |= 0x80;
        }
    }

    void CEvDemoRes::_CHR_INFO::SetChrInit(long nChr, long nColor, void *pMii, long nMotSel)
    {
        nId = (unsigned char)nChr | (unsigned char)nColor << 8;
        pInit = pMii;
        this->nMotSel = nMotSel;
        nKind = 0;
    }

    void CEvDemoRes::_CHR_INFO::SetObjInit(long nObj, void *pInit, long nMotSel)
    {
        nId = nObj;
        this->pInit = pInit;
        this->nMotSel = nMotSel;
        nKind = 2;
    }
}
