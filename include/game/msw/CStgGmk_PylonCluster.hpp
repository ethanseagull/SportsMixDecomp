#ifndef MSW_CSTGGMK_PYLONCLUSTER_HPP_
#define MSW_CSTGGMK_PYLONCLUSTER_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CStgGmkClusterImpl.hpp"
#include "game/msw/CStgGmk_Pylon.hpp"

namespace msw {

    // The pylons of stage 04: GetMaxNum() msw::CStgGmk_Pylon gimmicks set out
    // after one of several location patterns. A DOL class, size 0xA1C:
    // CStgGmkClusterImpl at 0x00 (vptr at 0x10), own members from 0x14.
    // Constructor __ct__Q23msw20CStgGmk_PylonClusterFPQ23bcs5CTask. Each game
    // module derives its own class from it (CStgGmk_PylonCluster_Dodge).
    //
    // The order of the virtual functions is that of
    // __vt__Q23msw26CStgGmk_PylonCluster_Dodge (DodgeF). DodgeF emits weak
    // copies of GetMaxNum, GetCurPattern, SetCurPattern, SetCurPatternAuto,
    // GetLocInfoNum, GetLocInfoPosNum, GetLocInfo(long) and
    // SetLocInfo(long, const LOC_INFO &), so those are inline in the
    // original (bodies from DodgeF's game/dodge/CStgGmk_Pylon_Act_Dodge.cpp). Return types
    // not visible in a body are guesses.
    class CStgGmk_PylonCluster : public CStgGmkClusterImpl {
        public:
            // (member names guessed) Where one pylon stands. Size 0x10.
            struct LOC_INFO {
                long m_nType;     // 0x00
                bcs::FVEC3 m_Pos; // 0x04
            };

            CStgGmk_PylonCluster(bcs::CTask *pParent);
            virtual ~CStgGmk_PylonCluster();
            virtual long GetMaxNum() const { return PYLON_MAX; }
            virtual void OnPostCreateGmk();
            virtual void OnUpdate(float rate);

            virtual void Reset();
            virtual void InactivateAll();
            virtual long GetCurPattern() const { return m_nCurPattern; }
            virtual void SetCurPattern(long pattern) { m_nCurPattern = pattern; }
            virtual void SetCurPatternAuto() { SetCurPattern(SelectPattern()); }
            virtual long SelectPattern() const;
            virtual long GetParam_LocPattern() const;
            virtual void LoadLocInfoTbl();
            virtual long GetLocInfoNum() const { return m_nLocInfoNum; }
            virtual long GetLocInfoPosNum() const { return PYLON_MAX; }
            virtual const LOC_INFO &GetLocInfo(long pattern, long idx) const;
            virtual void SetLocInfo(long pattern, long idx, const LOC_INFO &rInfo);
            // Position idx of the current pattern.
            virtual const LOC_INFO &GetLocInfo(long idx) const { return GetLocInfo(GetCurPattern(), idx); }
            virtual void SetLocInfo(long idx, const LOC_INFO &rInfo) { SetLocInfo(GetCurPattern(), idx, rInfo); }
            virtual void ApplyLocInfo(CStgGmk_Pylon *pPylon, const LOC_INFO &rInfo) const;
            virtual long GetLocValidNumIdx() const;
            virtual long GetLocRawTblIdx(long, long) const;

            // (name guessed) Pylon idx, through the const GetGmk_ (slot 0x38),
            // as CStageDodge04::GetPylon reads it.
            const CStgGmk_Pylon *GetPylon(long idx) const
            {
                return static_cast<const CStgGmk_Pylon *>(GetGmk_(idx));
            }

        private:
            enum {
                PYLON_MAX = 16,
                // (guess) 0x9F0 bytes of LOC_INFO after the first one: 160
                // in all, maybe ten patterns of PYLON_MAX.
                LOC_INFO_NUM = 160
            };

            LOC_INFO m_aLocInfo[LOC_INFO_NUM]; // 0x14
            long m_nCurPattern;                // 0xA14
            long m_nLocInfoNum;                // 0xA18 (name guessed)
    };
}

#endif /* !MSW_CSTGGMK_PYLONCLUSTER_HPP_ */
