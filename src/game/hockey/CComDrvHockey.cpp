// msw::CComDrvHockey, the computer driver of the hockey game: constructor,
// destructor, the keeper information and CComManHockey's queries. OnInit,
// OnMain, MethdSkate and MethdRotateY are in another file (they follow this
// file's weak functions in the original).
#include "game/msw/CComDrvHockey.hpp"
#include "game/msw/CChrCore.hpp"

namespace msw {

CComDrvHockey::CComDrvHockey(long nArg) : CComDrv(nArg) {
}

CComDrvHockey::~CComDrvHockey() {
}

void CComDrvHockey::ClearAll() {
    CComDrv::ClearAll();
    m_EmKeeper.m_pChr = 0;
    m_MyKeeper.m_pChr = 0;
    m_EmKeeper.m_nPlayerNo = -1;
    m_MyKeeper.m_nPlayerNo = -1;
    m_EmKeeper.m_Pos.Set(0.0f, 0.0f, 0.0f);
    m_MyKeeper.m_Pos.Set(0.0f, 0.0f, 0.0f);
    m_EmKeeper.m_WorldPos.Set(0.0f, 0.0f, 0.0f);
    m_MyKeeper.m_WorldPos.Set(0.0f, 0.0f, 0.0f);
}

void CComDrvHockey::UpdateInfo() {
    CComDrv::UpdateInfo();
    if (m_EmKeeper.m_pChr != 0) {
        m_EmKeeper.m_Pos = CComMan::GetChrPos(m_EmKeeper.m_pChr);
        m_EmKeeper.m_WorldPos = CComMan::GetChrWorldPos(m_EmKeeper.m_pChr);
    }
    if (m_MyKeeper.m_pChr != 0) {
        m_MyKeeper.m_Pos = CComMan::GetChrPos(m_MyKeeper.m_pChr);
        m_MyKeeper.m_WorldPos = CComMan::GetChrWorldPos(m_MyKeeper.m_pChr);
    }
}

CComDrvHockey *CComDrvHockey::GetComDrv(const CChrCore *pChr) const {
    if (pChr->DriverGetType() != 3) {
        return 0;
    }
    return static_cast<CComDrvHockey *>(pChr->DriverGetCurrent());
}

long CComDrvHockey::GetChrTypeID() const {
    return m_pChr->GetChrTypeID();
}

float CComDrvHockey::GetElapsedTime() const {
    CComManHockey *pMan = CComManHockey::GetInstance();
    if (pMan == 0) {
        return 0.0f;
    }
    return pMan->GetElapsedTime();
}

void CComDrvHockey::InitMyTeamKeeperCore() {
    CChrCore *pChr = CComManHockey::GetKeeperCore(m_nMyTeam, 0);
    m_MyKeeper.m_pChr = pChr;
    m_MyKeeper.m_nPlayerNo = -1;
    if (pChr != 0) {
        m_MyKeeper.m_nPlayerNo = pChr->GetPlayerNo();
    }
}

CChrCore *CComDrvHockey::GetMyTeamKeeperCore() const {
    return m_MyKeeper.m_pChr;
}

void CComDrvHockey::InitEmTeamKeeperCore() {
    CChrCore *pChr = CComManHockey::GetKeeperCore(m_nMyTeam, 1);
    m_EmKeeper.m_pChr = pChr;
    m_EmKeeper.m_nPlayerNo = -1;
    if (pChr != 0) {
        m_EmKeeper.m_nPlayerNo = pChr->GetPlayerNo();
    }
}

CChrCore *CComDrvHockey::GetEmTeamKeeperCore() const {
    return m_EmKeeper.m_pChr;
}

int CComDrvHockey::CanReachTarget(long nArg0, int nArg1, const bcs::FVEC3 &rFrom,
                                  const bcs::FVEC3 &rTo, float fArg) {
    CComManHockey *pMan = CComManHockey::GetInstance();
    if (pMan == 0) {
        return 0;
    }
    return pMan->CanReachTarget(nArg0, nArg1, rFrom, rTo, fArg) != 0;
}

int CComDrvHockey::isExistPillar() {
    CComManHockey *pMan = CComManHockey::GetInstance();
    if (pMan == 0) {
        return 0;
    }
    return pMan->isExistPillar();
}

int CComDrvHockey::GetMapStatus(short nX, short nZ, CComManHockey::MAP_STATUS_DATA &rData) {
    CComManHockey *pMan = CComManHockey::GetInstance();
    if (pMan == 0) {
        return 0;
    }
    return pMan->GetMapStatus(nX, nZ, rData);
}

int CComDrvHockey::GetPartyMapStatus(short nX, short nZ, unsigned char &rArg0,
                                     unsigned char &rArg1, char &rArg2) {
    CComManHockey *pMan = CComManHockey::GetInstance();
    if (pMan == 0) {
        return 0;
    }
    return pMan->GetPartyMapStatus(nX, nZ, rArg0, rArg1, rArg2);
}

} // namespace msw
