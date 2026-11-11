#ifndef MSW_CSTGGMK_PINBALL_HPP_
#define MSW_CSTGGMK_PINBALL_HPP_

#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/CStgGmk.hpp"
#include "game/msw/CStgGmk_Pinball_Act.hpp"
#include "game/msw/CStgSrt.hpp"

namespace msw {

    // (added for VolleyF) The driver of a pinball gimmick, 0xC bytes. A DOL
    // class.
    class CStgGmk_Pinball_Drv : public CStgDrv {
        public:
            CStgGmk_Pinball_Drv(CStgGmk *pParent);
            virtual ~CStgGmk_Pinball_Drv();
            virtual void Main(float rate);
    };

    // One ball of a sport's stage 12, the gimmick that CPinballMan makes. A
    // DOL class, size 0x78 (CPinballManDodge::NewGmk allocates
    // CStgGmk_Pinball_Dodge at 0x78; it sets only the vtable). Only the new
    // virtual functions are declared, in the order of
    // __vt__Q23msw21CStgGmk_Pinball_Dodge (DodgeF), so that IsActive is at
    // 0xBC; the overrides of CStgGmk's are not listed yet. DodgeF and VolleyF
    // emit weak copies of GetRadius, GetVel, GetPos, GetDefaultPos and
    // SetDefaultPos, so those are inline.
    // Return types not visible in a body are guesses.
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
            // The actor's IsAlive (an int: no conversion to bool).
            virtual int IsActive() const;
            virtual void Inactivate();
            virtual void Sleep(float frame);
            virtual void Standby();
            virtual void SetAccel(const bcs::FVEC3 &rAccel);
            // The position of the actor.
            virtual bcs::FVEC3 GetPos() const
            {
                bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
                pos = GetSrt()->GetPos();
                return pos;
            }
            // The velocity of the actor.
            virtual bcs::FVEC3 GetVel() const
            {
                bcs::FVEC3 vel(0.0f, 0.0f, 0.0f);
                vel = GetSrt()->GetVel();
                return vel;
            }
            virtual float GetRadius() const { return GetPinballAct()->GetParam_CollRadius(); }

            // (DOL) The DOL emits both as weak copies; the per-sport
            // classes override them.
            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Pinball_Act(this); }
            virtual CStgDrv *NewDrv(long heap) { return new (heap) CStgGmk_Pinball_Drv(this); }

        private:
            // The actor is always a CStgGmk_Pinball_Act. (name guessed)
            const CStgGmk_Pinball_Act *GetPinballAct() const
            {
                return static_cast<const CStgGmk_Pinball_Act *>(GetAct());
            }
            CStgGmk_Pinball_Act *GetPinballAct() { return static_cast<CStgGmk_Pinball_Act *>(GetAct()); }

            int m_Unk68;                 // 0x68, the second argument of the constructor
            bcs::FVEC3 m_DefaultPos;     // 0x6C (GetDefaultPos)
    };
}

#endif /* !MSW_CSTGGMK_PINBALL_HPP_ */
