#include "game/msw/CButtonEntry.hpp"

#include <string.h>

#include "bcs/G2D/CLytResource.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "nw4r/lyt/ResourceAccessor.hpp"

msw::CButtonEntry::CButtonEntry()
{
    for (int i = 0; i < NAME_MAX; i++) {
        m_pName[i] = 0;
    }
    m_nNameNum = 0;
}

msw::CButtonEntry::~CButtonEntry()
{
    RemoveList();
}

inline long msw::CButtonEntry::EntryName(const char *pName)
{
    if (strlen(pName) == 0) {
        return 0;
    }

    for (long i = 0; i < m_nNameNum; i++) {
        if (strcmp(m_pName[i], pName) == 0) {
            return i;
        }
    }
    m_pName[m_nNameNum] = pName;
    return m_nNameNum++;
}

void msw::CButtonEntry::ArchiveBuildLoader(const char *pName)
{
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return;
    }

    Header *pData = static_cast<Header *>(
        pLytCtrl->NewArcResourceAccessor("menu/bin/General_pack.bin", "arc")->GetResource(0, pName, 0));
    m_pData = pData;
    unsigned long nOffset = sizeof(Header);
    for (int i = 0; i < pData->m_nButtonNum; i++) {
        Button *pButton = reinterpret_cast<Button *>(reinterpret_cast<char *>(m_pData) + nOffset);
        pButton->m_nNameId = EntryName(pButton->m_Name);
        pButton->m_nName2Id = EntryName(pButton->m_Name2);
        nOffset += sizeof(Button);
    }
}

void msw::CButtonEntry::RemoveList()
{
    if (strlen(m_ArchiveName) != 0) {
        bcs::G2D::CLytResource *pResource = bcs::G2D::CLytResource::GetInstance();
        if (pResource != 0) {
            pResource->Cleanup(m_ArchiveName);
        }
    }

    for (int i = 0; i < NAME_MAX; i++) {
        m_pName[i] = 0;
    }
    m_nNameNum = 0;
}

void msw::CButtonEntry::SetUpAnim(bcs::G2D::CPaneCtrl *pPaneCtrl)
{
    Header *pData = m_pData;
    unsigned long nOffset = pData->m_nAnimOffset;
    for (int i = 0; i < pData->m_nAnimNum; i++) {
        m_pAnimName[i] = reinterpret_cast<const char *>(m_pData) + nOffset;
        nOffset += ANIM_NAME_SIZE;
    }
    pPaneCtrl->BuildAnim(0, m_pAnimName, pData->m_nAnimNum);
}

int msw::CButtonEntry::SetUpButton(CMenuPartButton *pButton, long nIndex, int bUnk)
{
    Button *pEntry = GetButton(nIndex);
    if (pEntry->m_nNextBtnKindId[0] != 0x8000) {
        pButton->m_nBtnKindId = pEntry->m_nNameId;
        pButton->m_Unk16 = pEntry->m_nName2Id;
        pButton->m_nCurrentBtnKindId = pEntry->m_nNameId;
        pButton->m_nAnim[0] = pEntry->m_nAnim[0];
        pButton->m_nAnim[1] = pEntry->m_nAnim[1];
        pButton->m_nAnim[2] = pEntry->m_nAnim[2];
        pButton->m_nAnim[3] = pEntry->m_nAnim[3];
        pButton->m_nAnim[4] = pEntry->m_nAnim[4];
        pButton->m_nAnim[5] = pEntry->m_nAnim[1];
        pButton->m_nNextBtnKindId[0] = pEntry->m_nNextBtnKindId[0];
        pButton->m_nNextBtnKindId[1] = pEntry->m_nNextBtnKindId[1];
        pButton->m_nNextBtnKindId[2] = pEntry->m_nNextBtnKindId[2];
        pButton->m_nNextBtnKindId[3] = pEntry->m_nNextBtnKindId[3];
        pButton->m_nNextBtnKindId[4] = pEntry->m_nNextBtnKindId[4];
        pButton->m_nSeCode[0] = ButtonEntrySETable[pEntry->m_nSe[0]];
        pButton->m_nSeCode[1] = ButtonEntrySETable[pEntry->m_nSe[1]];
        pButton->m_nSeCode[2] = 1;
        pButton->m_nFlags40 = bUnk ? (pButton->m_nFlags40 | 0x0004) : (pButton->m_nFlags40 & ~0x0004);
        pButton->m_nFlags40 |= 0x0020;
        return pEntry->m_nNextBtnKindId[0] & 0x7FFF;
    }
    return 0;
}

void msw::CButtonEntry::ResetButtonID(CMenuPartButton *pButton)
{
    pButton->m_nNextBtnKindId[0] = 0xFFFF;
    pButton->m_nNextBtnKindId[1] = 0xFFFF;
    pButton->m_nNextBtnKindId[2] = 0xFFFF;
    pButton->m_nNextBtnKindId[3] = 0xFFFF;
    pButton->m_nNextBtnKindId[4] = 0xFFFF;
}

void msw::CButtonEntry::ReFineButtonID(CMenuPartButton *pButton, unsigned short nId0, unsigned short nId1,
                                       unsigned short nId2, unsigned short nId4, unsigned short nId3)
{
    pButton->m_nNextBtnKindId[0] = nId0;
    pButton->m_nNextBtnKindId[1] = nId1;
    pButton->m_nNextBtnKindId[2] = nId2;
    pButton->m_nNextBtnKindId[3] = nId3;
    pButton->m_nNextBtnKindId[4] = nId4;
}
