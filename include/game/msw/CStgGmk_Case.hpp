#ifndef MSW_CSTGGMK_CASE_HPP_
#define MSW_CSTGGMK_CASE_HPP_

#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/CStgGmk.hpp"
#include "game/msw/CStgGmk_Case_Act.hpp"
#include "game/msw/CStgSrt.hpp"

namespace bcs {
    class CTask;
}

namespace msw {

    // The driver of a case. A DOL class, size 0xC (nothing added to
    // msw::CStgDrv).
    class CStgGmk_Case_Drv : public CStgDrv {
        public:
            CStgGmk_Case_Drv(CStgGmk *pParent);
            virtual ~CStgGmk_Case_Drv();
    };

    // (purpose guessed from the names) One case (crate) of the ship stage
    // (stage 15), made by msw::CStage15Base through NewCase. A DOL class,
    // size 0x78, constructor __ct__Q23msw12CStgGmk_CaseFPQ23bcs5CTaski. The
    // actor is a msw::CStgGmk_Case_Act.
    //
    // The order of the virtual functions follows
    // __vt__Q23msw18CStgGmk_Case_Dodge (DodgeF). The modules emit weak copies
    // of NewDrv, the default position accessors and GetPos to GetDepth, in
    // reverse order of the declarations here (so NewDrv, an override with no
    // slot of its own, is declared last). Return types not visible in a body
    // are guesses.
    class CStgGmk_Case : public CStgGmk {
        public:
            CStgGmk_Case(bcs::CTask *pParent, int);
            virtual ~CStgGmk_Case();
            virtual void Main();
            virtual void Disp();
            virtual CStgMdl *NewMdl(long heap, unsigned long idx);
            virtual void OnPostCreate();
            virtual void OnSceneEntry();
            virtual void OnSceneLeave();

            virtual void SetDefaultPos(const bcs::FVEC3 &rPos) { m_DefaultPos = rPos; }
            virtual const bcs::FVEC3 &GetDefaultPos() const { return m_DefaultPos; }
            virtual void Respawn();
            virtual bool IsActive() const;
            virtual void Inactivate();
            virtual void Sleep();
            virtual bool IsLanding() const;
            virtual void SetAccel(const bcs::FVEC3 &rAccel);
            virtual void ForceBreak(float delay);
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
                // TODO: the velocity is the FVEC3 at 0xCC of msw::CStgSrt
                // (inside its m_UnkC8 padding). Replace this by an accessor
                // of CStgSrt (GetVel) once that header can be edited.
                vel = *reinterpret_cast<const bcs::FVEC3 *>(reinterpret_cast<const char *>(GetSrt()) + 0xCC);
                return vel;
            }
            virtual float GetWidth() const { return GetCaseAct()->GetParam_Width(); }
            virtual float GetDepth() const { return GetCaseAct()->GetParam_Depth(); }

            virtual CStgDrv *NewDrv(long heap) { return new (heap) CStgGmk_Case_Drv(this); }

        private:
            // The actor is always a CStgGmk_Case_Act. (name guessed)
            const CStgGmk_Case_Act *GetCaseAct() const
            {
                return static_cast<const CStgGmk_Case_Act *>(GetAct());
            }

            unsigned long m_Unk68;   // 0x68
            bcs::FVEC3 m_DefaultPos; // 0x6C
    };
}

#endif /* !MSW_CSTGGMK_CASE_HPP_ */
