#ifndef MSW_CSTGGMK_TROLLEY_HPP_
#define MSW_CSTGGMK_TROLLEY_HPP_

#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/CStgGmk.hpp"
#include "game/msw/CStgGmk_Trolley_Act.hpp"
#include "game/msw/CStgSrt.hpp"

namespace bcs {
    class CTask;
}

namespace msw {

    // The driver of the trolley. A DOL class, size 0xC (nothing added to
    // msw::CStgDrv).
    class CStgGmk_Trolley_Drv : public CStgDrv {
        public:
            CStgGmk_Trolley_Drv(CStgGmk *pParent);
            virtual ~CStgGmk_Trolley_Drv();
            virtual void Main(float rate);
    };

    // (purpose guessed from the names) The trolley of stage 17, made by
    // msw::CStage17Base through NewTrolley. A DOL class, size 0x80,
    // constructor __ct__Q23msw15CStgGmk_TrolleyFPQ23bcs5CTaski. The actor is
    // a msw::CStgGmk_Trolley_Act.
    //
    // The order of the virtual functions follows
    // __vt__Q23msw21CStgGmk_Trolley_Dodge (DodgeF). The modules emit weak
    // copies of NewDrv, the default position accessors and GetPos to
    // GetBodyDepth, in reverse order of the declarations here (so NewDrv, an
    // override with no slot of its own, is declared last). Return types not
    // visible in a body are guesses.
    class CStgGmk_Trolley : public CStgGmk {
        public:
            CStgGmk_Trolley(bcs::CTask *pParent, int);
            virtual ~CStgGmk_Trolley();
            virtual void Main();
            virtual void Disp();
            virtual CStgMdl *NewMdl(long heap, unsigned long idx);
            virtual void OnPostCreate();
            virtual void OnSceneEntry();
            virtual void OnSceneLeave();

            virtual void SetDefaultPos(const bcs::FVEC3 &rPos) { m_DefaultPos = rPos; }
            virtual const bcs::FVEC3 &GetDefaultPos() const { return m_DefaultPos; }
            virtual void SetDefaultRotY(float rot) { m_fDefaultRotY = rot; }
            virtual float GetDefaultRotY() const { return m_fDefaultRotY; }
            virtual void SetRunDist(float dist) { m_fRunDist = dist; }
            virtual float GetRunDist() const { return m_fRunDist; }
            virtual void Respawn();
            // The actor's IsAlive (an int: no conversion to bool).
            virtual int IsActive() const;
            virtual void Inactivate();
            // The position of the actor; zero without one.
            virtual bcs::FVEC3 GetPos() const
            {
                bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
                pos = GetTrolleyAct()->GetVirtualPos();
                return pos;
            }
            // The velocity of the actor; zero without one.
            virtual bcs::FVEC3 GetVel() const
            {
                bcs::FVEC3 vel(0.0f, 0.0f, 0.0f);
                vel = GetSrt()->GetVel();
                return vel;
            }
            virtual float GetWidth() const { return GetTrolleyAct()->GetAtkTotalWidth(); }
            virtual float GetDepth() const { return GetTrolleyAct()->GetAtkTotalDepth(); }
            virtual float GetBodyWidth() const { return GetTrolleyAct()->GetBodyTotalWidth(); }
            virtual float GetBodyDepth() const { return GetTrolleyAct()->GetBodyTotalDepth(); }

            // The DOL emits this one after NewDrv; the per-sport classes
            // override it with their own actors.
            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Trolley_Act(this); }
            virtual CStgDrv *NewDrv(long heap) { return new (heap) CStgGmk_Trolley_Drv(this); }

        private:
            // The actor is always a CStgGmk_Trolley_Act. (name guessed)
            const CStgGmk_Trolley_Act *GetTrolleyAct() const
            {
                return static_cast<const CStgGmk_Trolley_Act *>(GetAct());
            }
            CStgGmk_Trolley_Act *GetTrolleyAct() { return static_cast<CStgGmk_Trolley_Act *>(GetAct()); }

            int m_Unk68;             // 0x68, the second argument of the constructor
            bcs::FVEC3 m_DefaultPos; // 0x6C
            float m_fDefaultRotY;    // 0x78
            float m_fRunDist;        // 0x7C
    };
}

#endif /* !MSW_CSTGGMK_TROLLEY_HPP_ */
