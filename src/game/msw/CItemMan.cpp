// msw::CItemMan: creates and owns the items of a game, keeps the item icons
// and the per-kind extra parameters the games set.
//
// In the DOL this file goes on with msw::CItem and the item-get effect
// (msw::CEffectItemGet): the CItem constructor and CItem::GetItemRsrcFile
// read the same file-name table (.data 0x8046F928) as this constructor, the
// CItem code shares this file's float literals (.sdata2 0x805C2C08 on), and
// CItemMan's vtable sits between theirs. Only the CItemMan part is written
// so far; the original inlines CItem::GetItemRsrcFile and CItem::SetMdlAlpha
// here, which this file still calls.
#include "game/msw/CItemMan.hpp"

#include "bcs/New.hpp"
#include "game/msw/CEffectItemGet.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CItem.hpp"
#include "nw4r/g3d/ResAnmTexPat.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "revolution/gx/GXTexture.h"

namespace {

    // Max(0, x) as the setters clamp their values.
    inline float Max(float a, float b) { return a < b ? b : a; }
    inline long Max(long a, long b) { return a < b ? b : a; }
}

namespace msw {

    CItemMan::CItemMan(bcs::CTask *pParent) : bcs::CUnivTaskEx(pParent, -1), m_pIconTexBuf(0)
    {
        nw4r::ut::List_Init(&m_ItemList, 0x7C); // offset of CItem's link

        // The icon of each kind: texture "cicon" of the kind's file.
        GXTexObj *pTexObj = new (1) GXTexObj[KIND_NUM];
        m_pIconTexBuf = pTexObj;
        for (long i = 0; i < KIND_NUM; i++, pTexObj++) {
            m_pIconTex[i] = 0;
            nw4r::g3d::ResFile file = CItem::GetItemRsrcFile(i);
            if (!file.IsValid()) {
                continue;
            }
            nw4r::g3d::ResTex tex = file.GetResTex("cicon");
            if (!tex.IsValid()) {
                continue;
            }

            void *pImage;
            unsigned short width;
            unsigned short height;
            GXTexFmt fmt;
            float minLod;
            float maxLod;
            unsigned char mipmap;
            tex.GetTexObjParam(&pImage, &width, &height, &fmt, &minLod, &maxLod, &mipmap);
            GXInitTexObj(pTexObj, pImage, width, height, fmt, GX_CLAMP, GX_CLAMP, mipmap);
            GXInitTexObjLOD(pTexObj, GX_LIN_MIP_LIN, GX_LINEAR, minLod, maxLod, 0.0f, 0, 0, GX_ANISO_1);
            m_pIconTex[i] = pTexObj;
        }

        ExParamInitialize();
    }

    CItemMan::~CItemMan()
    {
        CItem *pItem = ItemGetFirst();
        while (pItem != 0) {
            CItem *pNext = ItemGetNext(pItem);
            if (pItem != 0 && !pItem->IsDelete()) {
                pItem->DoDestroy();
                pItem->Delete();
            }
            pItem = pNext;
        }
        DeleteAllChild();
        nw4r::ut::List_Init(&m_ItemList, 0x7C);

        if (m_pIconTexBuf != 0) {
            delete[] m_pIconTexBuf;
        }
        m_pIconTexBuf = 0;
        for (long i = 0; i < KIND_NUM; i++) {
            m_pIconTex[i] = 0;
        }
    }

    // Removes every item that is not in a character's hands.
    void CItemMan::AllReset()
    {
        CItem *pItem = ItemGetFirst();
        while (pItem != 0) {
            CItem *pNext = ItemGetNext(pItem);
            if (pItem != 0 && !pItem->IsDelete() && pItem->GetState() != CItem::STATE_STOCK) {
                pItem->DoDestroy();
                pItem->Delete();
            }
            pItem = pNext;
        }
    }

    void CItemMan::SendMessage(long nMsg, long, long, long)
    {
        int bHide = 0;
        if (nMsg == 0xB) {
            bHide = 1;
        }
        if (!bHide) {
            return;
        }

        CItem *pNext;
        for (CItem *pItem = ItemGetFirst(); pItem != 0; pItem = pNext) {
            pNext = ItemGetNext(pItem);
            if (pItem == 0 || pItem->IsDelete()) {
                continue;
            }
            switch (pItem->GetState()) {
            case CItem::STATE_STOCK:
                pItem->SetMdlAlpha(0.0f);
                break;
            case CItem::STATE_DRIFT:
                pItem->SetMdlAlpha(0.0f);
                pItem->DoDestroy();
                break;
            }
        }
    }

    void CItemMan::EffectItemGetCreate(long kind, const bcs::FVEC3 &pos, float f)
    {
        new (0x100) CEffectItemGet(CGame::GetTempParent(), pos, kind, f);
    }

    void CItemMan::Main()
    {
        MainFunc();
    }

    // Calls pFunc for each item that can be picked up at rPos.
    void CItemMan::ItemPickup(const bcs::FVEC3 &rPos, float fRadius, float fHeight,
                              void (*pFunc)(const CItem *, void *), void *pArg)
    {
        if (pFunc == 0) {
            return;
        }
        CItem *pNext;
        for (CItem *pItem = ItemGetFirst(); pItem != 0; pItem = pNext) {
            pNext = ItemGetNext(pItem);
            if (pItem->CheckPickup(rPos, fRadius, fHeight)) {
                pFunc(pItem, pArg);
            }
        }
    }

    // Calls pFunc for each item that pBody touches.
    void CItemMan::ItemPickup(CCollBodyUnit *pBody, void (*pFunc)(const CItem *, void *), void *pArg)
    {
        if (pFunc == 0 || pBody == 0) {
            return;
        }
        CItem *pNext;
        for (CItem *pItem = ItemGetFirst(); pItem != 0; pItem = pNext) {
            pNext = ItemGetNext(pItem);
            if (pItem->CheckPickup(pBody)) {
                pFunc(pItem, pArg);
            }
        }
    }

    long CItemMan::ItemGetNum()
    {
        return nw4r::ut::List_GetSize(&m_ItemList);
    }

    long CItemMan::ThrowItemGetNum()
    {
        long num = 0;
        for (CItem *pItem = ItemGetFirst(); pItem != 0; pItem = ItemGetNext(pItem)) {
            if (pItem->GetState() == CItem::STATE_THROW) {
                num++;
            }
        }
        return num;
    }

    CItem *CItemMan::ThrowItemGetFirst()
    {
        for (CItem *pItem = ItemGetFirst(); pItem != 0; pItem = ItemGetNext(pItem)) {
            if (pItem->GetState() == CItem::STATE_THROW) {
                return pItem;
            }
        }
        return 0;
    }

    CItem *CItemMan::ThrowItemGetNext(CItem *pItem)
    {
        if (pItem == 0) {
            return 0;
        }
        for (pItem = ItemGetNext(pItem); pItem != 0; pItem = ItemGetNext(pItem)) {
            if (pItem->GetState() == CItem::STATE_THROW) {
                return pItem;
            }
        }
        return 0;
    }

    void *CItemMan::GetIconTexObj(long nKind, int)
    {
        return m_pIconTex[nKind];
    }

    void CItemMan::ExParamInitialize()
    {
        for (long i = 0; i < KIND_NUM; i++) {
            EX_PARAM &r = m_ExParam[i];
            r.nFlag = 0;
            r.fLife = 600.0f;
            r.fUnk08 = 600.0f;
            r.fDispScale = 1.0f;
            r.fThrowSpeed = 1.0f;
            r.fTurnSpeed = 0.0f;
            r.fEffectFrame = 0.0f;
            r.fDropCoinRate = 0.0f;
            r.nDropCoinMin = 0;
            r.nDropCoinMax = 0;
            r.nReboundNum = 1;
            r.fPThrowDist = 1.0f;
            r.fPThrowHeight = 1.0f;
            r.fPThrowFrame = 30.0f;
            r.fExplodeRange = 1.0f;
        }
        ExParamInitialize_();
    }

    void CItemMan::ExParamSet_Life(long nKind, float f)
    {
        EX_PARAM &r = m_ExParam[nKind];
        r.fLife = Max(0.0f, f);
        r.nFlag |= 0x1;
    }

    void CItemMan::ExParamSet_DispScale(long nKind, float f)
    {
        EX_PARAM &r = m_ExParam[nKind];
        r.fDispScale = Max(0.0f, f);
        r.nFlag |= 0x2;
    }

    void CItemMan::ExParamSet_ThrowSpeed(long nKind, float f)
    {
        EX_PARAM &r = m_ExParam[nKind];
        r.fThrowSpeed = Max(0.0f, f);
        r.nFlag |= 0x4;
    }

    void CItemMan::ExParamSet_TurnSpeed(long nKind, float f)
    {
        EX_PARAM &r = m_ExParam[nKind];
        r.fTurnSpeed = Max(0.0f, f);
        r.nFlag |= 0x8;
    }

    void CItemMan::ExParamSet_EffectFrame(long nKind, float f)
    {
        EX_PARAM &r = m_ExParam[nKind];
        r.fEffectFrame = Max(0.0f, f);
        r.nFlag |= 0x10;
    }

    void CItemMan::ExParamSet_DropCoinRate(long nKind, float f)
    {
        EX_PARAM &r = m_ExParam[nKind];
        r.fDropCoinRate = Max(0.0f, f);
        r.nFlag |= 0x20;
    }

    void CItemMan::ExParamSet_DropCoinMin(long nKind, long n)
    {
        EX_PARAM &r = m_ExParam[nKind];
        r.nDropCoinMin = Max(0L, n);
        r.nFlag |= 0x40;
    }

    void CItemMan::ExParamSet_DropCoinMax(long nKind, long n)
    {
        EX_PARAM &r = m_ExParam[nKind];
        r.nDropCoinMax = Max(0L, n);
        r.nFlag |= 0x80;
    }

    void CItemMan::ExParamSet_ReboundNum(long nKind, long n)
    {
        EX_PARAM &r = m_ExParam[nKind];
        r.nReboundNum = Max(0L, n);
        r.nFlag |= 0x100;
    }

    void CItemMan::ExParamSet_PThrowDist(long nKind, float f)
    {
        EX_PARAM &r = m_ExParam[nKind];
        r.fPThrowDist = Max(0.0f, f);
        r.nFlag |= 0x200;
    }

    void CItemMan::ExParamSet_PThrowHeight(long nKind, float f)
    {
        EX_PARAM &r = m_ExParam[nKind];
        r.fPThrowHeight = Max(0.0f, f);
        r.nFlag |= 0x400;
    }

    void CItemMan::ExParamSet_PThrowFrame(long nKind, float f)
    {
        EX_PARAM &r = m_ExParam[nKind];
        r.fPThrowFrame = Max(1.0f, f);
        r.nFlag |= 0x800;
    }

    void CItemMan::ExParamSet_ExplodeRange(long nKind, float f)
    {
        EX_PARAM &r = m_ExParam[nKind];
        r.fExplodeRange = Max(0.0f, f);
        r.nFlag |= 0x1000;
    }

    // An item of the given kind, held by pChr from the start.
    CItem *CItemMan::CreateStockItem(long kind, CChrCore *pChr)
    {
        if (pChr == 0) {
            return 0;
        }
        CItem *pItem = CreateItem(kind, bcs::FVEC3(0.0f, 0.0f, 0.0f));
        if (pItem != 0) {
            pItem->SetStock(pChr);
        }
        return pItem;
    }

    // An item of the given kind drifting to pos.
    CItem *CItemMan::CreateDriftItem(long kind, const bcs::FVEC3 &pos, float a, float b)
    {
        CItem *pItem = CreateItem(kind, bcs::FVEC3(0.0f, 0.0f, 0.0f));
        if (pItem != 0) {
            pItem->DoDrift(pos, a, b);
        }
        return pItem;
    }
}
