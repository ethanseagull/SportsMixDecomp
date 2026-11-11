#ifndef MSW_CSTAGEDODGE03_HPP_
#define MSW_CSTAGEDODGE03_HPP_

#include "game/msw/CStageManDodge.hpp"
#include "game/msw/CStgGmk_WaterWorks_Dodge.hpp"

namespace msw {

    // Dodge stage 03, the fountain stage (DodgeF), made by
    // CStageFactoryDodge. Size 0x2CC: CStageManDodge (0x244), three rows of
    // water jets, then the centre line's state. A row of jets is the centre
    // line between the two courts; every CENTER_CYCLE seconds it moves to
    // another of the places ORIFICE_SPACE apart (the next row gushes up
    // there first), and now and then the court has no centre line at all.
    //
    // Overrides are declared in the order of their vtable slots, then the
    // new slots of __vt__Q23msw13CStageDodge03 (IsNextNoBorder at 0x448 to
    // Water_GetPrevSlot at 0x498).
    class CStageDodge03 : public CStageManDodge {
        public:
            // A centre line number meaning "no centre line".
            enum { NO_BORDER = -10000 };

            // One row of jets: up to six msw::CStgGmk_WaterWorks_Dodge put
            // across the court on the line of a centre number. Size 0x24,
            // the data first, vptr at 0x20.
            class CWaterInfo {
                private:
                    CStageDodge03 *m_pStage;                  // 0x00
                    CStgGmk_WaterWorks_Dodge *m_apWater[6];   // 0x04
                    long m_nCenterNo;                         // 0x1C, the line the row stands on

                public:
                    CWaterInfo();
                    virtual ~CWaterInfo();
                    // Makes the jets.
                    virtual void Init(CStageDodge03 *pStage);
                    virtual void Update(float frame);
                    virtual long GetCurCenterNo() const { return m_nCenterNo; }
                    // Moves the row to line `centerNo` and lets it gush up
                    // after `time` frames. (the last argument is not used)
                    virtual void PreStart(long centerNo, float time, int);
                    // Moves the row to line `centerNo` and starts it at once.
                    // (the last argument is not used)
                    virtual void Start(long centerNo, int);
                    virtual void Stop();
                    virtual void TallStart();
                    virtual void Reset();
                    virtual bool IsSwooshTall() const;
                    // Puts WATER_NUM jets evenly across the court on the
                    // row's line; the others take no part.
                    virtual void UpdatePos_();
            };

            CStageDodge03(bcs::CTask *pParent);
            virtual ~CStageDodge03();

            virtual void Disp3D(long);
            virtual float GetTeamCourtFront(long team) const;
            virtual float GetTeamCourtBack(long team) const;
            virtual float GetTeamOuterFieldFront(long team, long n) const;
            virtual float GetTeamOuterFieldBack(long team, long n) const;
            virtual bool IsNoBorder() const { return m_nCenterNo == NO_BORDER; }
            virtual bool CanbeComeback() const;
            virtual bool CanbeJump(const bcs::FVEC3 &rPos) const;
            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual bool OnPreCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                        nw4r::g3d::ResFile file, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnPostInitialize();
            virtual void OnArtsEventStart();
            virtual void OnArtsEventEnd();
            virtual void OnArtsShootStart();
            virtual void OnArtsShootEnd();
            virtual unsigned long OnCalcSum() const;
            virtual void OnGimmickReset();
            virtual void OnGimmickStop();
            virtual void OnGimmickPause();
            virtual void OnGimmickResume();
            virtual const void *GetPrmTbl() const;
            virtual void OnEntranceEventGmkStart();
            virtual void OnEntranceEventGmkEnd();
            virtual bool IsEnemyZoneLimited() const { return false; }
            virtual void OnSoundEvent(long event);

            virtual bool IsNextNoBorder() const { return m_nNextCenterNo == NO_BORDER || m_bNoBorderMode != 0; }
            // The z of centre line `centerNo`, kept inside the court.
            virtual float CalcCourtCenterZ_(long centerNo) const;
            virtual void UpdateCourtCenter_(float frame);
            virtual void SelectNextCourtCenter_();
            virtual void CommitCourtCenter_();
            virtual float GetCurCourtCenter_() const { return m_fCourtCenter; }
            virtual void SetCenterNo(long centerNo, float time);
            virtual void ResetCenter();
            virtual void SetNoBorder(float time);
            virtual void UpdateWaterPattern_(float frame);
            virtual void UpdateWaterPos_(float frame);
            virtual void Water_PreStart(long centerNo, float time);
            virtual void Water_Start(long centerNo);
            virtual void Water_StopAll();
            virtual void Water_TallStartAll();
            virtual void Water_ResetAll();
            virtual bool Water_IsSwooshAll() const;
            virtual void Water_NextPattern();
            virtual long Water_GetCurSlot() const { return m_nCurSlot; }
            virtual long Water_GetNextSlot() const { return (m_nCurSlot + 1) % 3; }
            virtual long Water_GetPrevSlot() const { return (m_nCurSlot - 1) % 3; }

        private:
            CWaterInfo m_aWater[3]; // 0x244, used in turn
            long m_nCurSlot;        // 0x2B0, the row of m_aWater that is the centre line
            float m_fCourtCenter;   // 0x2B4, z of the centre line (0 when there is none)
            long m_nCenterNo;       // 0x2B8, the centre line, or NO_BORDER
            long m_nNextCenterNo;   // 0x2BC, the line after it, or NO_BORDER
            float m_fTimer;         // 0x2C0, frames until the line moves
            long m_nBorderlessCnt;  // 0x2C4, moves since the last time without a line
            // 0x2C8, set by message 0x27 (arg1 2): the line stays away
            // except during a special shot. (meaning guessed)
            int m_bNoBorderMode;
    };
}

#endif /* !MSW_CSTAGEDODGE03_HPP_ */
