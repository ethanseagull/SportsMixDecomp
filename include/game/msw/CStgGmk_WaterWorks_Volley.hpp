#ifndef MSW_CSTGGMK_WATERWORKS_VOLLEY_HPP_
#define MSW_CSTGGMK_WATERWORKS_VOLLEY_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CStgGmk.hpp"

namespace msw {

    // INCOMPLETE: one water jet of volley stage 03 (msw::CStageVolley03
    // makes twelve and lines them up along the net). Size 0x74 (from the
    // allocation there). The class and its actor are in
    // game/volley/CStgGmk_WaterWorks_Volley_Act.cpp; only what
    // CStageVolley03.cpp needs is declared: most overrides are left out and
    // the new virtual functions follow __vt__Q23msw25CStgGmk_WaterWorks_Volley
    // (SetDefaultPos at 0xB0 to GetParam_Width at 0xE0). Unlike the other
    // modes' water works it has no tall stage, and IsSwoosh3 instead.
    class CStgGmk_WaterWorks_Volley : public CStgGmk {
        public:
            // (meaning of the second argument not known; CStageVolley03
            // passes 0)
            CStgGmk_WaterWorks_Volley(bcs::CTask *pParent, int arg);
            virtual ~CStgGmk_WaterWorks_Volley();

            virtual CStgMdl *NewMdl(long heap, unsigned long idx);
            virtual CStgAct *NewAct(long heap);
            virtual CStgDrv *NewDrv(long heap);

            virtual void SetDefaultPos(const bcs::FVEC3 &rPos);        // 0xB0
            virtual const bcs::FVEC3 &GetDefaultPos() const;           // 0xB4
            // The jet takes part (it is placed along the net) or not.
            virtual void Activate();                                   // 0xB8
            virtual void Inactivate();                                 // 0xBC
            // (meaning of the arguments guessed) Starts the jet after
            // `delay` frames (below zero: at once).
            virtual void Start(float delay, int bDelayed);             // 0xC0
            virtual void Stop();                                       // 0xC4
            virtual void Reset();                                      // 0xC8
            // (meanings guessed) Three phases of the gush, as the stage
            // uses them: running at all, risen (counted for the sound
            // position), and wide enough to block a ball.
            virtual bool IsSwoosh() const;                             // 0xCC
            virtual bool IsSwoosh2() const;                            // 0xD0
            virtual bool IsSwoosh3() const;                            // 0xD4
            virtual void SetParam_Height(float height);                // 0xD8
            virtual void SetParam_StayTime(float time);                // 0xDC
            virtual float GetParam_Width() const;                      // 0xE0

        private:
            unsigned char m_Unk68[0xC]; // 0x68 (not mapped), size 0x74
    };
}

#endif /* !MSW_CSTGGMK_WATERWORKS_VOLLEY_HPP_ */
