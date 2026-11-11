#include "game/msw/CStgGmk_WaterPillar.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "revolution/mtx.h"
#include <math.h>

// The cluster of water pillars: GetParam_LocNum() pillars stand on an arc
// in front of the guarded goal, cut into three zones (0 to 2). While a
// character of the guarding team holds the ball, the pillars of the zone it
// stands in stop and those of the others run. Part of the same original
// file as CStgGmk_WaterPillar_Mdl.cpp (see there).

namespace {

    // (name guessed) rVec scaled to length len (zero stays zero).
    inline void SetLength(bcs::FVEC3 &rVec, float len)
    {
        bcs::BCS_Vec_Normalize(rVec, rVec);
        bcs::FVEC3Scale(&rVec, &rVec, len);
    }
}

namespace msw {

    CStgGmk_WaterPillarCluster::CStgGmk_WaterPillarCluster(bcs::CTask *pParent)
        : CStgGmkCluster<CStgGmk_WaterPillar>(pParent), m_nGuardGoalIdx(0), m_CenterBasePos(0.0f, 0.0f, 0.0f),
          m_Vec(0.0f, 0.0f, 1.0f), m_nZone(-1), m_fStopTimer(0.0f)
    {
    }

    CStgGmk_WaterPillarCluster::~CStgGmk_WaterPillarCluster()
    {
    }

    // The first GetParam_LocNum() pillars reset, the others go out.
    void CStgGmk_WaterPillarCluster::Reset()
    {
        long num = GetTblSize();
        long locNum = GetParam_LocNum();
        for (long i = 0; i < num; i++) {
            CStgGmk_WaterPillar *pPillar = GetPillar(i);
            if (pPillar != 0) {
                if (i < locNum) {
                    pPillar->Reset();
                } else {
                    pPillar->Inactivate();
                }
            }
        }
        m_nZone = -1;
        m_fStopTimer = 0.0f;
        SE_StopAll();
    }

    // Starts the pillars of zone and plays the sound once at their centre,
    // delayed by the pillars' prestart time.
    void CStgGmk_WaterPillarCluster::StartZone(long zone)
    {
        long num = GetTblSize();
        long locNum = GetParam_LocNum(); // not used
        bcs::FVEC3 center(0.0f, 0.0f, 0.0f);
        float delay = 1.0f;
        long count = 0;
        for (long i = 0; i < num; i++) {
            if (IsEqualZone(i, zone)) {
                CStgGmk_WaterPillar *pPillar = GetPillar(i);
                if (pPillar != 0) {
                    pPillar->Start();
                    bcs::FVEC3 pos = pPillar->GetPos();
                    bcs::FVEC3Add(&center, &center, &pos);
                    count++;
                    delay = static_cast<CStgGmk_WaterPillar_Act *>(pPillar->GetAct())->GetParam_PrestartTime();
                }
            }
        }
        m_nZone = zone;
        if (count > 0) {
            bcs::FVEC3Scale(&center, &center, 1.0f / count);
            SE_DelayPlayPosEx(0, delay, 0xD12, center, 3, 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    void CStgGmk_WaterPillarCluster::StopAll()
    {
        long num = GetTblSize();
        long locNum = GetParam_LocNum(); // not used
        for (long i = 0; i < num; i++) {
            CStgGmk_WaterPillar *pPillar = GetPillar(i);
            if (pPillar != 0) {
                pPillar->Stop();
            }
        }
        SE_Stop(0, 0.0f);
    }

    // Whether pillar idx belongs to zone: the first GetParam_NumPerGroup()
    // pillars are zone 0, as many in the middle zone 1, the last ones zone 2.
    bool CStgGmk_WaterPillarCluster::IsEqualZone(long idx, long zone) const
    {
        long locNum = GetParam_LocNum();
        long divNum = GetParam_DivNum(); // not used
        long perGroup = GetParam_NumPerGroup();
        long mid = locNum / 2;
        switch (zone) {
            case 0:
                if (idx < perGroup) {
                    return true;
                }
                break;
            case 1:
                if (idx >= mid - perGroup / 2 && idx < mid + perGroup / 2 + perGroup % 2) {
                    return true;
                }
                break;
            case 2:
                if (idx >= locNum - perGroup) {
                    return true;
                }
                break;
        }
        return false;
    }

    void CStgGmk_WaterPillarCluster::OnPostCreateGmk()
    {
        CStgGmkCluster<CStgGmk_WaterPillar>::OnPostCreateGmk();
        SE_Init();
    }

    void CStgGmk_WaterPillarCluster::OnUpdate(float rate)
    {
        ApplyLocate();
        const CChrCore *pChr = GetOnBallChr();
        if (IsReady() && pChr != 0) {
            long zone = -1;
            if (pChr != 0) {
                zone = GetZoneByPos(pChr->GetPos(), m_nZone, GetParam_ZoneMargin());
            }
            if (m_nZone != zone || !IsSwoosh()) {
                StopAll();
                if (m_nZone >= 0) {
                    m_fStopTimer = GetParam_StopTimer();
                    m_nZone = -1;
                }
            }
            if (m_nZone < 0) {
                m_fStopTimer -= rate;
                if (m_fStopTimer <= 0.0f) {
                    if (zone >= 0) {
                        StartZone(zone);
                    }
                    m_fStopTimer = 0.0f;
                }
            }
        }
        SE_Update(rate);
    }

    // The centre of the arc: GetParam_LocOffsetZ() along the goal's
    // direction from the base position.
    bcs::FVEC3 CStgGmk_WaterPillarCluster::GetCenterPos() const
    {
        bcs::FVEC3 pos = GetCenterBasePos();
        bcs::FVEC3 ofs;
        bcs::BCS_Vec_Normalize(ofs, GetVec());
        bcs::FVEC3Scale(&ofs, &ofs, GetParam_LocOffsetZ());
        bcs::FVEC3Add(&pos, &pos, &ofs);
        return pos;
    }

    // Sets the pillars out on a half circle of radius GetParam_LocRadius()
    // around the centre, from -90 to 90 degrees off the goal's direction.
    void CStgGmk_WaterPillarCluster::ApplyLocate()
    {
        bcs::FVEC3 center = GetCenterBasePos();
        const bcs::FVEC3 &rVec = GetVec();
        float radius = GetParam_LocRadius();
        long num = GetTblSize();
        long locNum = GetParam_LocNum();
        bcs::FVEC3 ofs;
        bcs::BCS_Vec_Normalize(ofs, rVec);
        bcs::FVEC3Scale(&ofs, &ofs, GetParam_LocOffsetZ());
        bcs::FVEC3Add(&center, &center, &ofs);
        center.y = 0.0f;
        for (long i = 0; i < num; i++) {
            CStgGmk_WaterPillar *pPillar = GetPillar(i);
            if (pPillar != 0) {
                if (i < locNum) {
                    float angle = 0.0f;
                    if (locNum > 1) {
                        angle = -1.5707964f + (float)i / (float)(locNum - 1) * 3.1415927f;
                    }
                    bcs::FVEC3 pos;
                    bcs::BCS_Vec_RotY(pos, rVec, angle);
                    bcs::FVEC3Scale(&pos, &pos, radius);
                    bcs::FVEC3Add(&pos, &pos, &center);
                    pPillar->SetPos(pos);
                } else {
                    pPillar->Stop();
                }
            }
        }
    }

    // The apex of the zones: the centre moved back along the goal's
    // direction so that GetParam_ZoneAngle() spans GetParam_ZoneWidth().
    bcs::FVEC3 CStgGmk_WaterPillarCluster::GetZoneCenter() const
    {
        bcs::FVEC3 pos = GetCenterPos();
        const bcs::FVEC3 &rVec = GetVec();
        float angle = GetParam_ZoneAngle();
        float width = GetParam_ZoneWidth();
        if (angle > 0.0f) {
            bcs::FVEC3 ofs;
            bcs::FVEC3Scale(&ofs, &rVec, -(width / (float)tan(0.5f * angle)));
            bcs::FVEC3Add(&pos, &pos, &ofs);
        }
        pos.y = 0.0f;
        return pos;
    }

    // The zone rPos is in: 1 within half the zone angle of the goal's
    // direction, 0 on one side, 2 on the other. With a current zone and a
    // margin, rPos stays in curZone while a point margin to either side of
    // it is still there.
    long CStgGmk_WaterPillarCluster::GetZoneByPos(const bcs::FVEC3 &rPos, long curZone, float margin) const
    {
        bcs::FVEC3 center = GetZoneCenter();
        const bcs::FVEC3 &rVec = GetVec();
        bcs::FVEC3 dir;
        dir.x = rPos.x;
        dir.y = center.y;
        dir.z = rPos.z;
        bcs::FVEC3Sub(&dir, &dir, &center);
        float rot = bcs::BCS_Vec_GetRot(rVec, dir);
        float zoneAngle = GetParam_ZoneAngle();
        bcs::FVEC3 cross;
        PSVECCrossProduct((const Vec *)&rVec, (const Vec *)&dir, (Vec *)&cross);
        if (cross.y < 0.0f) {
            rot *= -1.0f;
        }

        long zone;
        if (fabsf(rot) < 0.5f * zoneAngle) {
            zone = 1;
        } else if (rot < 0.0f) {
            zone = 0;
        } else {
            zone = 2;
        }

        if (curZone >= 0 && margin > 0.0f) {
            bcs::FVEC3 side0;
            bcs::FVEC3 side1;
            switch (curZone) {
                case 0:
                    bcs::BCS_Vec_RotY(side0, rVec, -0.5f * zoneAngle - 1.5707964f);
                    SetLength(side0, margin);
                    bcs::BCS_Vec_RotY(side1, rVec, -0.5f * zoneAngle + 1.5707964f);
                    SetLength(side1, margin);
                    break;
                case 1:
                    bcs::BCS_Vec_RotY(side0, rVec, -0.5f * zoneAngle - 1.5707964f);
                    SetLength(side0, margin);
                    bcs::BCS_Vec_RotY(side1, rVec, 0.5f * zoneAngle + 1.5707964f);
                    SetLength(side1, margin);
                    break;
                case 2:
                    bcs::BCS_Vec_RotY(side0, rVec, 0.5f * zoneAngle + 1.5707964f);
                    SetLength(side0, margin);
                    bcs::BCS_Vec_RotY(side1, rVec, 0.5f * zoneAngle - 1.5707964f);
                    SetLength(side1, margin);
                    break;
            }

            bcs::FVEC3 pos;
            bcs::FVEC3Add(&pos, &rPos, &side0);
            if (curZone == GetZoneByPos(pos, -1, 0.0f)) {
                return curZone;
            }
            bcs::FVEC3Add(&pos, &rPos, &side1);
            if (curZone == GetZoneByPos(pos, -1, 0.0f)) {
                return curZone;
            }
        }
        return zone;
    }

    const CChrCore *CStgGmk_WaterPillarCluster::GetOnBallChr() const
    {
        const CChrCore *pChr;
        long num = CChrMan::GetChrCoreNum();
        long team = CGame::GetTeamIdxByGoalIdx(GetGuardGoalIdx());
        for (long i = 0; i < num; i++) {
            pChr = CChrMan::GetChrCore(i);
            if (pChr != 0 && pChr->IsEnable() && (pChr->GetStatusFlag() & 1) && team == pChr->GetTeamNo()) {
                return pChr;
            }
        }
        return 0;
    }

    bool CStgGmk_WaterPillarCluster::IsSwoosh() const
    {
        long num = GetTblSize();
        for (long i = 0; i < num; i++) {
            const CStgGmk_WaterPillar *pPillar = GetPillar(i);
            if (pPillar != 0 && pPillar->IsSwoosh()) {
                return true;
            }
        }
        return false;
    }
}
