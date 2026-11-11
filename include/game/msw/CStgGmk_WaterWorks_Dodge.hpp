#ifndef MSW_CSTGGMK_WATERWORKS_DODGE_HPP_
#define MSW_CSTGGMK_WATERWORKS_DODGE_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CStgGmk.hpp"
#include "game/msw/CStgGmkAct.hpp"

namespace msw {

    // The water jets of the dodge fountain stage (stage 03): a row of them
    // is the moving centre line of msw::CStageDodge03. The gimmick and its
    // actor live in game/dodge/CStgGmk_WaterWorks_Dodge_Act.cpp; only what
    // CStageDodge03.cpp needs is declared so far (the slots up to the ones
    // it calls, with the overrides and members left out).

    // The actor of one jet, size 0x210: CStgGmkAct at 0x000 (vptr at
    // 0x008), its state queue bcs::CFsmQueue at 0x174, own members from
    // 0x1D4. As in msw::CStgGmk_WaterPillar_Act, the new virtual functions
    // are reached through the vptr at 0x008, after the CFsmQueue part
    // (__vt__Q23msw28CStgGmk_WaterWorks_Dodge_Act: GetParam_Width at
    // 0x1A4 to GetNetHeightBottom at 0x220). Return types are guesses.
    class CStgGmk_WaterWorks_Dodge_Act : public CStgGmkAct, public bcs::CFsmQueue<CStgGmk_WaterWorks_Dodge_Act> {
        public:
            CStgGmk_WaterWorks_Dodge_Act(CStgGmk *pParent);
            virtual ~CStgGmk_WaterWorks_Dodge_Act();

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
            virtual void SetParam_StayTime(float time);           // 0x1D0
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
            virtual long SE_GetSlotNum() const;                   // 0x1F8
            virtual void AnimPlay(long mdl, unsigned long anm, float speed, float frame, float blend,
                                  unsigned long flag);            // 0x1FC
            virtual bool IsAnimEnd();                             // 0x200
            virtual void SetVisibleAll(int bVisible);             // 0x204
            virtual void Mdl_FadeIn(long mdl, float frame, float alpha);  // 0x208
            virtual void Mdl_FadeOut(long mdl, float frame, float alpha); // 0x20C
            virtual void WaterSuspend();                          // 0x210
            virtual void WaterResume();                           // 0x214
            virtual bool IsWaterSuspend() const;                  // 0x218
            virtual float GetNetHeightTop() const;                // 0x21C
            virtual float GetNetHeightBottom() const;             // 0x220

        private:
            unsigned char m_Unk1D4[0x30]; // 0x1D4 (not mapped)
            float m_fHeight;              // 0x204, set by SetParam_Height
            unsigned char m_Unk208[0x8];  // 0x208 (not mapped)
    };

    // One jet, size 0x74: msw::CStgGmk (0x68) and its default position.
    // Made by msw::CStageDodge03::CWaterInfo::Init. The order of the new
    // virtual functions is that of __vt__Q23msw24CStgGmk_WaterWorks_Dodge
    // (SetDefaultPos at 0xB0 to SetParam_StayTime at 0xE0); SetDefaultPos
    // and SetParam_Height are inline (CStageDodge03.cpp emits them).
    class CStgGmk_WaterWorks_Dodge : public CStgGmk {
        public:
            // (meaning of the second argument not known; CWaterInfo passes 0)
            CStgGmk_WaterWorks_Dodge(bcs::CTask *pParent, int arg);
            virtual ~CStgGmk_WaterWorks_Dodge();

            virtual void Main();
            virtual void Disp();
            virtual CStgMdl *NewMdl(long heap, unsigned long idx);
            virtual CStgAct *NewAct(long heap);
            virtual CStgDrv *NewDrv(long heap);
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

        private:
            bcs::FVEC3 m_vDefaultPos; // 0x68
    };
}

#endif /* !MSW_CSTGGMK_WATERWORKS_DODGE_HPP_ */
