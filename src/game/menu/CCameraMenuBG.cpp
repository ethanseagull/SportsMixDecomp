// msw::CCameraAnimMenu and msw::CCameraMenuBG, the camera of the menus' 3D
// background.
#include "game/msw/CCameraMenuBG.hpp"

#include "bcs/EF/CResource.hpp"
#include "game/msw/CCameraAnimMenu.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "nw4r/ut/List.hpp"

// Defined before the destructor: the compiler then registers the
// destructor for it (it leaves out a call it knows to be empty).
msw::CCameraMenuBG msw::CCameraMenuBG::s_Instance;

// (name guessed) The null check the original repeats before setting the
// task's delete flag.
static inline void DeleteTask(bcs::CTask *pTask)
{
    if (pTask != 0) {
        pTask->Delete();
    }
}

msw::CCameraAnimMenu::~CCameraAnimMenu()
{
}

msw::CCameraMenuBG::~CCameraMenuBG()
{
}

void msw::CCameraMenuBG::Initialize(long view, nw4r::g3d::ResFile file)
{
    m_nView = view;

    CCameraAnimMenu *pCamera = new CCameraAnimMenu(view, 0x80, 0);
    if (pCamera != 0) {
        pCamera->SetData(file);
    }
    m_pCamera = pCamera;

    ClearList();
    m_nCurrent = -1;
    m_nVariant = 0;
}

void msw::CCameraMenuBG::Finalize()
{
    ClearList();
    if (m_pCamera != 0) {
        DeleteTask(m_pCamera);
        m_pCamera = 0;
    }
}

void msw::CCameraMenuBG::MainLoop()
{
    CCameraAnimMenu *pCamera = m_pCamera;
    if (pCamera == 0) {
        return;
    }

    if (!pCamera->IsPlaying() || pCamera->IsLooped() || pCamera->IsEnd()) {
        CCameraMenuAnimInfoLink *pLink = static_cast<CCameraMenuAnimInfoLink *>(nw4r::ut::List_GetNext(&m_List, 0));
        if (pLink != 0) {
            nw4r::ut::List_Remove(&m_List, pLink);
        }
        if (pLink != 0) {
            PlayQueued(pLink->m_nAnim, pLink->m_fRate, pLink->m_fFrame);
            delete pLink;
        }
    }
}

nw4r::g3d::ResFile msw::CCameraMenuBG::GetResFileFromEfResource(const char *pName)
{
    return bcs::EF::CResource::GetInstance()->GetG3DResFile(pName);
}

int msw::CCameraMenuBG::SetAnimTable(const AnimInfo *pTable, long num, long nVariant)
{
    m_pAnimTable = pTable;
    m_nAnimNum = num;
    m_nVariant = nVariant;
    return 0;
}

long msw::CCameraMenuBG::Play(long nAnim, float rate, float frame)
{
    ClearList();
    return PlayImpl(nAnim, rate, frame);
}

long msw::CCameraMenuBG::PlayImpl(long nAnim, float rate, float frame)
{
    CCameraAnimMenu *pCamera = m_pCamera;
    if (pCamera == 0) {
        return -1;
    }
    if (nAnim < 0 || nAnim >= m_nAnimNum) {
        return -2;
    }

    const AnimInfo *pInfo = &m_pAnimTable[nAnim];
    unsigned long flag = 0;
    if (pInfo->bFlag1) {
        flag |= 1;
    }
    if (pInfo->bFlag2) {
        flag |= 2;
    }

    long nAdjust = CLytCtrl::IsLocationAdjust() != 0;
    pCamera->SetActive(m_nView);
    pCamera->SetResAnmScn(pInfo->pName[m_nVariant][nAdjust]);
    pCamera->SetResAnmScnCamera(pInfo->nCamera);
    pCamera->Play(flag, rate, frame);
    if (rate < 0.0f) {
        // Played backwards: start from the end.
        pCamera->SetFrame(frame + pCamera->GetEndFrame());
    }
    m_nCurrent = nAnim;
    return 0;
}

int msw::CCameraMenuBG::IsEndAnim()
{
    CCameraAnimMenu *pCamera = m_pCamera;
    if (pCamera->IsEnd()) {
        return 1;
    }
    return !pCamera->IsPlaying();
}

void msw::CCameraMenuBG::PushBack(long nAnim, float rate, float frame)
{
    nw4r::ut::List_Append(&m_List, new CCameraMenuAnimInfoLink(nAnim, rate, frame));
}

void msw::CCameraMenuBG::ClearList()
{
    CCameraMenuAnimInfoLink *pNext;
    CCameraMenuAnimInfoLink *pLink = static_cast<CCameraMenuAnimInfoLink *>(nw4r::ut::List_GetNext(&m_List, 0));
    while (pLink != 0) {
        pNext = static_cast<CCameraMenuAnimInfoLink *>(nw4r::ut::List_GetNext(&m_List, pLink));
        nw4r::ut::List_Remove(&m_List, pLink);
        delete pLink;
        pLink = pNext;
    }
}

int msw::CCameraMenuBG::PushBackWithoutReady(long nAnim, float rate, float frame)
{
    if (!IsReady(nAnim)) {
        PushBack(nAnim, rate, frame);
        return 1;
    }
    return 0;
}

float msw::CCameraMenuBG::GetSpeed()
{
    float speed = 0.0f;
    CCameraAnimMenu *pCamera = m_pCamera;
    if (pCamera != 0) {
        speed = pCamera->GetRate();
    }
    return speed;
}

void msw::CCameraMenuBG::SetSpeed(float speed)
{
    CCameraAnimMenu *pCamera = m_pCamera;
    if (pCamera != 0) {
        pCamera->SetRate(speed);
    }
}
