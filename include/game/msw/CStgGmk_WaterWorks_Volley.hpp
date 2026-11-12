#ifndef MSW_CSTGGMK_WATERWORKS_VOLLEY_HPP_
#define MSW_CSTGGMK_WATERWORKS_VOLLEY_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CDefCollision.hpp"
#include "game/msw/CStgGmk.hpp"
#include "game/msw/CStgGmkAct.hpp"
#include "game/msw/CStgMdl.hpp"

namespace msw {

    // The water jets of volley stage 03 (msw::CStageVolley03 makes twelve
    // and lines them up along the net): the model, actor, driver and
    // gimmick of one jet. All of them live in
    // game/volley/CStgGmk_WaterWorks_Volley_Act.cpp. The classes are
    // declared in the reverse order of their vtables in that file's .data.
    // Unlike the other modes' water works the jet has no tall stage, and
    // IsSwoosh3 instead.

    class CStgGmk;

    // The first model of a jet (the water wall), size 0x130. Made by
    // CStgGmk_WaterWorks_Volley::NewMdl, which inlines the constructor; the
    // three other models are msw::CStgGenericGmkMdl.
    class CStgGmk_WaterWorks_Volley_Mdl : public CStgMdl {
        public:
            CStgGmk_WaterWorks_Volley_Mdl(CStgGmk *pParent, long idx) : CStgMdl(pParent) { m_nMdlIdx = idx; }
            virtual ~CStgGmk_WaterWorks_Volley_Mdl();
            virtual void Create();
            virtual void Main(float rate);
            virtual bcs::CMdlObj *NewMdl(long heap) const;

            virtual void SetMdlIdx(long idx) { m_nMdlIdx = idx; } // 0x124

        private:
            long m_nMdlIdx; // 0x12C, which entry of the model table (0 to 3)
    };

    // The actor of one jet, size 0x214: CStgGmkAct at 0x000 (vptrs at 0x008
    // and 0x014), its state queue bcs::CFsmQueue at 0x174 (vptr at 0x1D0),
    // own members from 0x1D4. The new virtual functions follow
    // __vt__Q23msw29CStgGmk_WaterWorks_Volley_Act (GetParam_Width at 0x1A4
    // to GetNetHeightBottom at 0x214); the overrides of the bases' virtuals
    // may be declared anywhere.
    //
    // States: 0 DEFAULT (back to the default position, then INACTIVE), 1
    // STANDBY (waits for ForceStart, or counts its delay down), 2 PRESTART
    // (warning), 3 START (the water rises), 4 STAY (it stands for StayTime
    // frames), 5 END (it falls, then STANDBY), 6 INACTIVE (off).
    class CStgGmk_WaterWorks_Volley_Act : public CStgGmkAct, public bcs::CFsmQueue<CStgGmk_WaterWorks_Volley_Act> {
        public:
            CStgGmk_WaterWorks_Volley_Act(CStgGmk *pParent);
            virtual ~CStgGmk_WaterWorks_Volley_Act();

            virtual void UpdateMdlMtx();
            virtual void OnInit();
            virtual void OnUpdate(float rate);
            virtual void OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
            virtual unsigned long OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos);
            virtual void OnBallCatch(CCollBodyUnit *pBody, CBall *pBall);
            virtual unsigned long OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                               bcs::FVEC3 &rVel);
            virtual unsigned long OnCalcSum() const;

            virtual float GetParam_Width() const;                        // 0x1A4
            virtual float GetParam_Height() const;                       // 0x1A8
            virtual float GetParam_WaitTime() const;                     // 0x1AC
            virtual float GetParam_StayTime() const;                     // 0x1B0
            virtual float GetParam_PrestartTime(long type) const;        // 0x1B4
            virtual float GetParam_Interval() const;                     // 0x1B8
            virtual long GetParam_Effect(long type) const;               // 0x1BC
            virtual float GetParam_MoveResist() const;                   // 0x1C0
            virtual float GetParam_WaterSpeed() const;                   // 0x1C4
            // SetParam_Height and SetParam_StayTime are inline, defined in
            // the .cpp (see CStgGmk_WaterWorks_Volley below).
            virtual void SetParam_Height(float height);                  // 0x1C8
            virtual void SetParam_StayTime(float time);                  // 0x1CC
            // Starts the jet: after delay frames, or at once when delay is
            // not above zero; type is passed on to the states (0: the
            // normal rise with the water wall, else the wall stays down).
            virtual void ForceStart(float delay, int type);              // 0x1D0
            virtual void ForceStop();                                    // 0x1D4
            virtual bool IsActive() const;                               // 0x1D8
            virtual void ForceReset();                                   // 0x1DC
            virtual bool IsSwoosh() const;                               // 0x1E0
            virtual bool IsSwoosh2() const;                              // 0x1E4
            virtual bool IsSwoosh3() const;                              // 0x1E8
            virtual void UpdateColl();                                   // 0x1EC
            virtual void AnimPlay(long anm, unsigned long sub, float speed, float frame, float blend,
                                  unsigned long flag);                   // 0x1F0
            virtual bool IsAnimEnd();                                    // 0x1F4
            virtual void SetVisibleAll(int bVisible);                    // 0x1F8
            virtual void Mdl_FadeIn(long mdl, float time, float alpha);  // 0x1FC
            virtual void Mdl_FadeOut(long mdl, float time, float alpha); // 0x200
            virtual void WaterSuspend();                                 // 0x204
            virtual void WaterResume();                                  // 0x208
            virtual int IsWaterSuspend() const;                          // 0x20C
            virtual float GetNetHeightTop() const;                       // 0x210
            virtual float GetNetHeightBottom() const;                    // 0x214

            // An override of CStgEffectSlot's, emitted with the inline
            // virtuals at the end of the file.
            virtual long Eff_GetSlotNum() const { return 2; }

        private:
            // The work memory of a state (8 bytes per queue entry).
            struct WORK {
                long nDelay; // frames before PRESTART (STANDBY)
                long nType;  // ForceStart's type
            };

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
            void CtorState_INACTIVE();
            void LoopState_INACTIVE();
            void DtorState_INACTIVE();

            CBodyCollision m_BodyColl;   // 0x1D4
            CDefCollision m_DefColl;     // 0x1E0
            float m_fTimer;              // 0x1EC, frames left in the state
            unsigned long m_Unk1F0;      // 0x1F0 (not used)
            float m_fCollTimer;          // 0x1F4, frames left of the rise (START) or fall (END)
            float m_fCollTime;           // 0x1F8, the whole rise or fall (GetParam_WaitTime)
            int m_bCollEnable;           // 0x1FC, see UpdateColl
            int m_bSuspend;              // 0x200, see WaterSuspend
            unsigned long m_ahEffect[2]; // 0x204, attached effects erased by the destructor
            float m_fHeight;             // 0x20C, SetParam_Height (not read in the file)
            float m_fStayTime;           // 0x210, GetParam_StayTime
    };

    // The driver, size 0xC (nothing added to msw::CStgDrv). Made by
    // CStgGmk_WaterWorks_Volley::NewDrv; the constructor is inline.
    class CStgGmk_WaterWorks_Volley_Drv : public CStgDrv {
        public:
            CStgGmk_WaterWorks_Volley_Drv(CStgGmk *pParent) : CStgDrv(pParent) {}
            virtual ~CStgGmk_WaterWorks_Volley_Drv();
            virtual void Main(float rate);
    };

    // One water jet, size 0x74 (from the allocation in CStageVolley03).
    // The new virtual functions follow __vt__Q23msw25CStgGmk_WaterWorks_Volley
    // (SetDefaultPos at 0xB0 to GetParam_Width at 0xE0). GetMdlNum, the
    // default position, the parameter setters, GetParam_Width, NewAct and
    // NewDrv are weak in the original's
    // game/volley/CStgGmk_WaterWorks_Volley_Act.cpp but not in
    // CStageVolley03's file, which calls them: they are inline functions
    // defined in the .cpp, declared here without bodies. They come out at
    // the end of the file in reverse declaration order (NewDrv first), so
    // NewAct and NewDrv are declared last.
    class CStgGmk_WaterWorks_Volley : public CStgGmk {
        public:
            // (meaning of the second argument not known; CStageVolley03
            // passes 0)
            CStgGmk_WaterWorks_Volley(bcs::CTask *pParent, int arg);
            virtual ~CStgGmk_WaterWorks_Volley();

            virtual void Main();
            virtual void Disp();
            virtual CStgMdl *NewMdl(long heap, unsigned long idx);
            // The water wall, two drop models and the spray.
            virtual unsigned long GetMdlNum() const;
            virtual void OnPostCreate();
            virtual void OnSceneEntry();
            virtual void OnSceneLeave();

            virtual void SetDefaultPos(const bcs::FVEC3 &rPos);        // 0xB0
            virtual const bcs::FVEC3 &GetDefaultPos() const;           // 0xB4
            // The jet takes part (it is placed along the net) or not.
            virtual void Activate();                                   // 0xB8
            virtual void Inactivate();                                 // 0xBC
            // Starts the jet after `delay` frames (not above zero: at
            // once); see CStgGmk_WaterWorks_Volley_Act::ForceStart.
            virtual void Start(float delay, int bDelayed);             // 0xC0
            virtual void Stop();                                       // 0xC4
            virtual void Reset();                                      // 0xC8
            // Three phases of the gush, as the stage uses them: running at
            // all (PRESTART to END), risen (PRESTART to STAY, counted for
            // the sound position), and standing (START and STAY, wide
            // enough to block a ball).
            virtual bool IsSwoosh() const;                             // 0xCC
            virtual bool IsSwoosh2() const;                            // 0xD0
            virtual bool IsSwoosh3() const;                            // 0xD4
            virtual void SetParam_Height(float height);                // 0xD8
            virtual void SetParam_StayTime(float time);                // 0xDC
            // -1 without an actor.
            virtual float GetParam_Width() const;                      // 0xE0

            virtual CStgAct *NewAct(long heap);
            virtual CStgDrv *NewDrv(long heap);

        private:
            // (name guessed) The actor is always a
            // CStgGmk_WaterWorks_Volley_Act.
            CStgGmk_WaterWorks_Volley_Act *GetWaterAct()
            {
                return static_cast<CStgGmk_WaterWorks_Volley_Act *>(GetAct());
            }
            const CStgGmk_WaterWorks_Volley_Act *GetWaterAct() const
            {
                return static_cast<const CStgGmk_WaterWorks_Volley_Act *>(GetAct());
            }

            bcs::FVEC3 m_DefaultPos; // 0x68, size 0x74
    };
}

#endif /* !MSW_CSTGGMK_WATERWORKS_VOLLEY_HPP_ */
