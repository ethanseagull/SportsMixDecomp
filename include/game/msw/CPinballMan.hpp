#ifndef MSW_CPINBALLMAN_HPP_
#define MSW_CPINBALLMAN_HPP_

#include "bcs/FVEC3.hpp"

namespace bcs {
    class CTask;
}

namespace msw {

    class CStageMdlListNode;
    class CStgGmk_Pinball;

    // (purpose guessed from the names) The balls of a sport's stage 12: a
    // DOL class (game/msw/CPinballMan.cpp), size 0x54, data first and the
    // vptr at 0x50. It makes the pinball gimmicks (NewGmk, given by each
    // sport's subclass), runs them and the launcher model ("mecha"). Layout
    // read from the DOL constructor (0x801E587C); the order of the virtual
    // functions is that of __vt__Q23msw16CPinballManDodge (DodgeF).
    //
    // DodgeF has weak copies of MechaCancel, MechaReset, SetMechaMdl, both
    // GetGmk and GetDefaultPos, so those are inline in the original; their
    // bodies are not written here yet.
    class CPinballMan {
        public:
            CPinballMan();

        private:
            CStgGmk_Pinball **m_ppGmk;   // 0x00, GetMaxNum gimmicks (GetGmk reads it)
            unsigned long m_Unk04;       // 0x04
            float m_Unk08;               // 0x08
            float m_Unk0C;               // 0x0C
            bcs::FVEC3 m_vDefaultPos;    // 0x10, GetDefaultPos
            unsigned char m_Unk1C[0x34]; // 0x1C, the launcher (an object with its vptr at 0x24)

        public:
            virtual ~CPinballMan();
            virtual void Init();
            virtual void CreateGmk(bcs::CTask *pParent, int arg);
            virtual void DeleteGmk();
            virtual void Main(float frame);
            virtual void MechaMain(float frame);
            virtual void Reset();
            virtual long GetMaxNum() const;
            virtual long GetActiveNum() const;
            virtual void SetDefaultPos(const bcs::FVEC3 &rPos);
            virtual const bcs::FVEC3 &GetDefaultPos() const;
            virtual CStgGmk_Pinball *GetGmk(long idx);
            virtual const CStgGmk_Pinball *GetGmk(long idx) const;
            virtual void InactivateAll();
            virtual void SetMechaMdl(CStageMdlListNode *pNode);
            virtual void MechaReset();
            virtual void MechaCancel();
            // (purpose guessed) The launcher's part of the entrance event.
            virtual void MechaEvent00();
            virtual float GetShotAngle() const;
            virtual long GetWinTeam() const;
            virtual CStgGmk_Pinball *NewGmk(bcs::CTask *pParent, int arg);
            virtual long GetParam_BallNum() const;
            virtual float GetParam_InitialInterval() const;
            virtual float GetParam_Interval() const;
            virtual float GetParam_ShotAngle() const;
            virtual long GetParam_ShotSideRate() const;
    };
}

#endif /* !MSW_CPINBALLMAN_HPP_ */
