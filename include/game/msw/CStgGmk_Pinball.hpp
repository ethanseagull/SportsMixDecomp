#ifndef MSW_CSTGGMK_PINBALL_HPP_
#define MSW_CSTGGMK_PINBALL_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CStgGmk.hpp"

namespace msw {

    // One ball of a sport's stage 12, the gimmick that CPinballMan makes. A
    // DOL class, size 0x78 (CPinballManDodge::NewGmk allocates
    // CStgGmk_Pinball_Dodge at 0x78; it sets only the vtable). Only the new
    // virtual functions are declared, in the order of
    // __vt__Q23msw21CStgGmk_Pinball_Dodge (DodgeF), so that IsActive is at
    // 0xBC; the overrides of CStgGmk's are not listed yet. DodgeF emits weak
    // copies of GetRadius, GetVel, GetPos, GetDefaultPos and SetDefaultPos,
    // so those are inline in the original; their bodies are not written yet.
    // Return types not visible in a body are guesses.
    class CStgGmk_Pinball : public CStgGmk {
        public:
            CStgGmk_Pinball(bcs::CTask *pParent, int arg);
            virtual ~CStgGmk_Pinball();

            virtual void SetDefaultPos(const bcs::FVEC3 &rPos);
            virtual const bcs::FVEC3 &GetDefaultPos() const;
            virtual void Respawn(float frame);
            virtual bool IsActive() const;
            virtual void Inactivate();
            virtual void Sleep(float frame);
            virtual void Standby();
            virtual void SetAccel(const bcs::FVEC3 &rAccel);
            virtual bcs::FVEC3 GetPos() const;
            virtual bcs::FVEC3 GetVel() const;
            virtual float GetRadius() const;

        private:
            unsigned char m_Unk68[0x10]; // 0x68
    };
}

#endif /* !MSW_CSTGGMK_PINBALL_HPP_ */
