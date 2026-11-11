#ifndef MSW_CSTGGMK_BOSSPACKUN_DODGE_HPP_
#define MSW_CSTGGMK_BOSSPACKUN_DODGE_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CStgGmk.hpp"

namespace msw {

    // INCOMPLETE: the boss Packun of DodgeF's stage 07, as a stage gimmick
    // (made by CStageDodge07::OnPostInitialize). Size 0x74: CStgGmk (0x68),
    // then the default position at 0x68. The virtual functions follow
    // __vt__Q23msw24CStgGmk_BossPackun_Dodge; the bodies are in
    // game/dodge/CStgGmk_BossPackun_Dodge_Act.cpp, not written yet. Return
    // types not visible in a body are guesses.
    class CStgGmk_BossPackun_Dodge : public CStgGmk {
        public:
            // (meaning of the second argument not known; the stage passes 0)
            CStgGmk_BossPackun_Dodge(bcs::CTask *pParent, int arg);
            virtual ~CStgGmk_BossPackun_Dodge();

            virtual void Main();
            virtual void Disp();
            virtual CStgMdl *NewMdl(long heap, unsigned long idx);
            virtual CStgAct *NewAct(long heap);
            virtual CStgDrv *NewDrv(long heap);
            virtual unsigned long GetMdlNum() const;
            virtual void OnPostCreate();
            virtual void OnSceneEntry();
            virtual void OnSceneLeave();

            virtual void SetDefaultPos(const bcs::FVEC3 &rPos);
            virtual const bcs::FVEC3 &GetDefaultPos() const { return m_DefaultPos; }
            virtual const bcs::FVEC3 &GetPos() const;
            virtual void Activate();
            virtual void Inactivate();
            virtual void Start(float frame);
            virtual void Stop();
            virtual void Reset();
            virtual bool IsHungry() const;

        private:
            bcs::FVEC3 m_DefaultPos; // 0x68
    };
}

#endif /* !MSW_CSTGGMK_BOSSPACKUN_DODGE_HPP_ */
