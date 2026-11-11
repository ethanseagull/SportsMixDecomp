#ifndef MSW_CSTGGMK_PINBALL_HPP_
#define MSW_CSTGGMK_PINBALL_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CStgGmk.hpp"
#include "game/msw/CStgGmk_Pinball_Act.hpp"
#include "game/msw/CStgSrt.hpp"

namespace bcs {
    class CTask;
}

namespace msw {

    // The driver of a pinball. A DOL class, size 0xC (nothing added to
    // msw::CStgDrv); constructor __ct__Q23msw19CStgGmk_Pinball_DrvFPQ23msw7CStgGmk.
    class CStgGmk_Pinball_Drv : public CStgDrv {
        public:
            CStgGmk_Pinball_Drv(CStgGmk *pParent);
            virtual ~CStgGmk_Pinball_Drv();
    };

    // One ball of a sport's stage 12, the gimmick that CPinballMan makes. A
    // DOL class, size 0x78 (CPinballManDodge::NewGmk allocates
    // CStgGmk_Pinball_Dodge at 0x78; it sets only the vtable). The actor is
    // a msw::CStgGmk_Pinball_Act. The order of the virtual functions follows
    // __vt__Q23msw21CStgGmk_Pinball_Dodge (DodgeF), so that IsActive is at
    // 0xBC; the overrides of CStgGmk's follow __vt__Q23msw22CStgGmk_Pinball_Volley.
    // DodgeF's game/dodge/CStgGmk_Pinball_Act_Dodge.cpp and VolleyF emit weak
    // copies of GetRadius, GetVel, GetPos, GetDefaultPos and SetDefaultPos
    // (reverse order of the declarations), so those are inline. Return
    // types not visible in a body are guesses.
    class CStgGmk_Pinball : public CStgGmk {
        public:
            CStgGmk_Pinball(bcs::CTask *pParent, int arg);
            virtual ~CStgGmk_Pinball();
            // (DOL overrides, listed from __vt__Q23msw22CStgGmk_Pinball_Volley)
            virtual void Main();
            virtual void Disp();
            virtual CStgMdl *NewMdl(long heap, unsigned long idx);
            virtual void OnPostCreate();
            virtual void OnSceneEntry();
            virtual void OnSceneLeave();

            virtual void SetDefaultPos(const bcs::FVEC3 &rPos) { m_DefaultPos = rPos; }
            virtual const bcs::FVEC3 &GetDefaultPos() const { return m_DefaultPos; }
            virtual void Respawn(float frame);
            virtual bool IsActive() const;
            virtual void Inactivate();
            virtual void Sleep(float frame);
            virtual void Standby();
            virtual void SetAccel(const bcs::FVEC3 &rAccel);
            // The position of the actor; zero without one.
            virtual bcs::FVEC3 GetPos() const
            {
                bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
                pos = GetSrt()->GetPos();
                return pos;
            }
            // The velocity of the actor; zero without one.
            virtual bcs::FVEC3 GetVel() const
            {
                bcs::FVEC3 vel(0.0f, 0.0f, 0.0f);
                vel = GetSrt()->GetVel();
                return vel;
            }
            virtual float GetRadius() const { return GetPinballAct()->GetParam_CollRadius(); }

        private:
            // The actor is always a CStgGmk_Pinball_Act. (name guessed)
            const CStgGmk_Pinball_Act *GetPinballAct() const
            {
                return static_cast<const CStgGmk_Pinball_Act *>(GetAct());
            }

            int m_Unk68;                 // 0x68, the second argument of the constructor
            bcs::FVEC3 m_DefaultPos;     // 0x6C (GetDefaultPos)
    };
}

#endif /* !MSW_CSTGGMK_PINBALL_HPP_ */
