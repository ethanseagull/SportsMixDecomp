#ifndef MSW_CSTGGMK_WATERWORKS_DODGE_HPP_
#define MSW_CSTGGMK_WATERWORKS_DODGE_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CDefCollision.hpp"
#include "game/msw/CStgGmk.hpp"
#include "game/msw/CStgGmkAct.hpp"
#include "game/msw/CStgMdl.hpp"

namespace msw {

    // The water jets of the dodge fountain stage (stage 03): a row of them
    // is the moving centre line of msw::CStageDodge03. The model, actor,
    // driver and gimmick of one jet all live in
    // game/dodge/CStgGmk_WaterWorks_Dodge_Act.cpp; the classes are declared
    // in the reverse order of their vtables in that file's .data. Unlike
    // VolleyF's water works the jet has a tall stage (TALL_START, TALL_STAY,
    // TALL_END) and sounds instead of effects on the ball.

    class CStgGmk;
    class CStgGmk_WaterWorks_Dodge;

    // The first model of a jet (the water wall), size 0x130. Made by
    // CStgGmk_WaterWorks_Dodge::NewMdl, which inlines the constructor.
    class CStgGmk_WaterWorks_Dodge_Mdl : public CStgMdl {
        public:
            CStgGmk_WaterWorks_Dodge_Mdl(CStgGmk *pParent, long idx) : CStgMdl(pParent) { m_nMdlIdx = idx; }
            virtual ~CStgGmk_WaterWorks_Dodge_Mdl();
            virtual void Create();
            virtual void Main(float rate);
            virtual bcs::CMdlObj *NewMdl(long heap) const;

            virtual void SetMdlIdx(long idx) { m_nMdlIdx = idx; } // 0x124

        private:
            long m_nMdlIdx; // 0x12C, which entry of the model table (0 to 3)
    };

    // The actor of one jet, size 0x210: CStgGmkAct at 0x000 (vptrs at 0x008
    // and 0x014), its state queue bcs::CFsmQueue at 0x174 (vptr at 0x1D0),
    // own members from 0x1D4. As in msw::CStgGmk_WaterPillar_Act, the new
    // virtual functions are reached through the vptr at 0x008, after the
    // CFsmQueue part (__vt__Q23msw28CStgGmk_WaterWorks_Dodge_Act:
    // GetParam_Width at 0x1A4 to GetNetHeightBottom at 0x220); the overrides
    // of the bases' virtuals may be declared anywhere.
    //
    // States: 0 DEFAULT (back to the default position, then the next queued
    // state or INACTIVE), 1 STANDBY (waits for ForceStart, or counts its
    // delay down), 2 PRESTART (warning), 3 START (the water rises), 4 STAY,
    // 5 END (it falls, then STANDBY), 6 TALL_START, 7 TALL_STAY, 8 TALL_END
    // (the same for the tall jet), 9 INACTIVE (off).
    class CStgGmk_WaterWorks_Dodge_Act : public CStgGmkAct, public bcs::CFsmQueue<CStgGmk_WaterWorks_Dodge_Act> {
            // CStgGmk_WaterWorks_Dodge::Reset runs the state machine at once.
            friend class CStgGmk_WaterWorks_Dodge;

        public:
            CStgGmk_WaterWorks_Dodge_Act(CStgGmk *pParent);
            virtual ~CStgGmk_WaterWorks_Dodge_Act();

            virtual void UpdateMdlMtx();
            virtual void OnInit();
            virtual void OnUpdate(float rate);
            virtual void OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
            virtual unsigned long OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos);
            virtual void OnBallCatch(CCollBodyUnit *pBody, CBall *pBall);
            virtual unsigned long OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                               bcs::FVEC3 &rVel);

            // An override of CStgEffectSlot's, emitted with the inline
            // virtuals at the end of the file.
            virtual long Eff_GetSlotNum() const { return 2; }

            virtual float GetParam_Width() const;                // 0x1A4
            virtual float GetParam_Height() const;               // 0x1A8
            virtual float GetParam_WaitTime() const;             // 0x1AC
            virtual float GetParam_StayTime() const;             // 0x1B0
            virtual float GetParam_TallTime() const;             // 0x1B4
            virtual float GetParam_PrestartTime(long type) const; // 0x1B8
            virtual float GetParam_Interval() const;             // 0x1BC
            virtual long GetParam_Effect(long type) const;       // 0x1C0
            virtual float GetParam_MoveResist() const;           // 0x1C4
            virtual float GetParam_WaterSpeed() const;           // 0x1C8
            virtual void SetParam_Height(float height) { m_fHeight = height; } // 0x1CC
            // Inline, defined in the .cpp (a weak copy there only).
            virtual void SetParam_StayTime(float time);           // 0x1D0
            // Starts the jet: after delay frames, or at once when delay is
            // not above zero; type is passed on to the states (0: the
            // normal rise with the warning, else the wall goes up at once).
            virtual void ForceStart(float time, int arg);         // 0x1D4
            virtual void ForceStop();                             // 0x1D8
            virtual void ForceTallStart();                        // 0x1DC
            virtual bool IsActive() const;                        // 0x1E0
            virtual void ForceReset();                            // 0x1E4
            virtual bool IsSwoosh() const;                        // 0x1E8
            virtual bool IsSwoosh2() const;                       // 0x1EC
            virtual bool IsSwooshTall() const;                    // 0x1F0
            virtual void UpdateColl();                            // 0x1F4
            // An override of CStgSeSlot's (a second base of CStgAct), so it
            // also gets a slot of its own here.
            virtual long SE_GetSlotNum() const { return 1; }      // 0x1F8
            virtual void AnimPlay(long mdl, unsigned long anm, float speed, float frame, float blend,
                                  unsigned long flag);            // 0x1FC
            virtual bool IsAnimEnd();                             // 0x200
            virtual void SetVisibleAll(int bVisible);             // 0x204
            virtual void Mdl_FadeIn(long mdl, float frame, float alpha);  // 0x208
            virtual void Mdl_FadeOut(long mdl, float frame, float alpha); // 0x20C
            virtual void WaterSuspend();                          // 0x210
            virtual void WaterResume();                           // 0x214
            virtual int IsWaterSuspend() const;                   // 0x218
            virtual float GetNetHeightTop() const;                // 0x21C
            virtual float GetNetHeightBottom() const;             // 0x220

        private:
            // The work memory of a state (8 bytes per queue entry).
            struct WORK {
                long nDelay; // frames before PRESTART (STANDBY)
                long nType;  // ForceStart's type
            };

            // The splash sound when a ball meets the water (at most once
            // in 15 frames).
            void PlaySplash(const bcs::FVEC3 &rPos);

            void CtorState_DEFAULT();
            void LoopState_DEFAULT();
            void DtorState_DEFAULT();
            void CtorState_STANDBY();
            void LoopState_STANDBY();
            void DtorState_STANDBY();
            void CtorState_PRESTART();
            void LoopState_PRESTART();
            void DtorState_PRESTART();
            void CtorState_START();
            void LoopState_START();
            void DtorState_START();
            void CtorState_STAY();
            void LoopState_STAY();
            void DtorState_STAY();
            void CtorState_END();
            void LoopState_END();
            void DtorState_END();
            void CtorState_TALL_START();
            void LoopState_TALL_START();
            void DtorState_TALL_START();
            void CtorState_TALL_STAY();
            void LoopState_TALL_STAY();
            void DtorState_TALL_STAY();
            void CtorState_TALL_END();
            void LoopState_TALL_END();
            void DtorState_TALL_END();
            void CtorState_INACTIVE();
            void LoopState_INACTIVE();
            void DtorState_INACTIVE();

            CBodyCollision m_BodyColl;   // 0x1D4
            CDefCollision m_DefColl;     // 0x1E0
            float m_fTimer;              // 0x1EC, frames spent in the state (counts up)
            // 0x1F0, frames the state lasts; below zero it lasts until the
            // animation ends or a ForceStart with a delay sets it.
            float m_fTime;
            float m_fCollTimer;          // 0x1F4, frames left of the rise (START) or fall (END)
            float m_fCollTime;           // 0x1F8, the whole rise or fall (GetParam_WaitTime)
            int m_bCollEnable;           // 0x1FC
            int m_bSuspend;              // 0x200, see WaterSuspend
            float m_fHeight;             // 0x204, set by SetParam_Height (not read in the file)
            float m_fStayTime;           // 0x208, GetParam_StayTime
            float m_fSplashWait;         // 0x20C, frames before the next splash sound
    };

    // The driver, size 0xC (nothing added to msw::CStgDrv). Made by
    // CStgGmk_WaterWorks_Dodge::NewDrv; the constructor is inline.
    class CStgGmk_WaterWorks_Dodge_Drv : public CStgDrv {
        public:
            CStgGmk_WaterWorks_Dodge_Drv(CStgGmk *pParent) : CStgDrv(pParent) {}
            virtual ~CStgGmk_WaterWorks_Dodge_Drv();
            virtual void Main(float rate);
    };

    // One jet, size 0x74: msw::CStgGmk (0x68) and its default position.
    // Made by msw::CStageDodge03::CWaterInfo::Init. The order of the new
    // virtual functions is that of __vt__Q23msw24CStgGmk_WaterWorks_Dodge
    // (SetDefaultPos at 0xB0 to SetParam_StayTime at 0xE0); SetDefaultPos
    // and SetParam_Height are inline (CStageDodge03.cpp emits them).
    // GetMdlNum, SetParam_StayTime, NewAct and NewDrv are weak in the
    // original's game/dodge/CStgGmk_WaterWorks_Dodge_Act.cpp only: inline
    // functions defined there, declared here without bodies; they come out
    // at the end of the file in reverse declaration order (NewDrv first),
    // so NewAct and NewDrv are declared last.
    class CStgGmk_WaterWorks_Dodge : public CStgGmk {
        public:
            // (meaning of the second argument not known; CWaterInfo passes 0)
            CStgGmk_WaterWorks_Dodge(bcs::CTask *pParent, int arg);
            virtual ~CStgGmk_WaterWorks_Dodge();

            virtual void Main();
            virtual void Disp();
            // The water wall and three plain models (only 0, 2 and 3 are
            // made).
            virtual CStgMdl *NewMdl(long heap, unsigned long idx);
            virtual unsigned long GetMdlNum() const;
            virtual void OnPostCreate();
            virtual void OnSceneEntry();
            virtual void OnSceneLeave();

            virtual void SetDefaultPos(const bcs::FVEC3 &rPos) { m_vDefaultPos = rPos; }
            virtual const bcs::FVEC3 &GetDefaultPos() const { return m_vDefaultPos; }
            // The jet takes part (it is placed on the centre line) or not.
            virtual void Activate();
            virtual void Inactivate();
            virtual void Start(float time, int arg);
            virtual void Stop();
            virtual void TallStart();
            virtual void Reset();
            virtual bool IsSwoosh() const;
            virtual bool IsSwoosh2() const;
            virtual bool IsSwooshTall() const;
            virtual void SetParam_Height(float height)
            {
                CStgGmk_WaterWorks_Dodge_Act *pAct = static_cast<CStgGmk_WaterWorks_Dodge_Act *>(GetAct());
                if (pAct != 0) {
                    pAct->SetParam_Height(height);
                }
            }
            virtual void SetParam_StayTime(float time);

            virtual CStgAct *NewAct(long heap);
            virtual CStgDrv *NewDrv(long heap);

        private:
            // (name guessed) The actor is always a
            // CStgGmk_WaterWorks_Dodge_Act.
            CStgGmk_WaterWorks_Dodge_Act *GetWaterAct()
            {
                return static_cast<CStgGmk_WaterWorks_Dodge_Act *>(GetAct());
            }
            const CStgGmk_WaterWorks_Dodge_Act *GetWaterAct() const
            {
                return static_cast<const CStgGmk_WaterWorks_Dodge_Act *>(GetAct());
            }

            bcs::FVEC3 m_vDefaultPos; // 0x68
    };
}

#endif /* !MSW_CSTGGMK_WATERWORKS_DODGE_HPP_ */
