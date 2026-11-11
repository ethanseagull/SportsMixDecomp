#ifndef MSW_CPINBALLMAN_HPP_
#define MSW_CPINBALLMAN_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CStgSeSlot.hpp"

namespace bcs {
    class CTask;
}

namespace msw {

    class CStageMdlListNode;
    class CStgGmk_Pinball;

    // (added for VolleyF's CStgGmk_Pinball_Act_Volley.cpp; no name in the
    // DOL, vtable lbl_80484FD8) The launcher ("mecha") of CPinballMan, at
    // 0x1C: a sound slot (CStgSeSlot, vptr at 0x08) plus 0x28 bytes. Only
    // the first new virtual functions are declared, up to the ones
    // CPinballMan's inline functions call (its new slots follow
    // CStgSeSlot's, the last of which, SE_GetDefaultFunc, is at 0xA0); names
    // guessed, placeholders where no caller is known.
    class CPinballMecha : public CStgSeSlot {
        public:
            virtual void UnkA4();                          // 0xA4
            virtual void SetMdl(CStageMdlListNode *pNode); // 0xA8, CPinballMan::SetMechaMdl
            virtual void UnkAC();                          // 0xAC
            virtual void Main(float frame);                // 0xB0, CPinballMan::MechaMain
            virtual void Reset();                          // 0xB4, CPinballMan::MechaReset
            virtual void Cancel();                         // 0xB8, CPinballMan::MechaCancel

        private:
            unsigned char m_Unk0C[0x28]; // 0x0C
    };

    // (purpose guessed from the names) The balls of a sport's stage 12: a
    // DOL class (game/msw/CPinballMan.cpp), size 0x54, data first and the
    // vptr at 0x50. It makes the pinball gimmicks (NewGmk, given by each
    // sport's subclass), runs them and the launcher model ("mecha"). Layout
    // read from the DOL constructor (0x801E587C); the order of the virtual
    // functions is that of __vt__Q23msw16CPinballManDodge (DodgeF).
    //
    // DodgeF and VolleyF have weak copies of MechaCancel, MechaReset,
    // SetMechaMdl, both GetGmk and GetDefaultPos, so those are inline in the
    // original. They are defined out of line instead, in the units that have
    // the copies (game/dodge/CStgGmk_Pinball_Act_Dodge.cpp,
    // game/volley/CStgGmk_Pinball_Act_Volley.cpp), so that the stage units
    // that include this header do not change.
    class CPinballMan {
        public:
            CPinballMan();

        private:
            // (name guessed) The launcher; its functions are reached through
            // its vtable.
            CPinballMecha *GetMecha() { return &m_Mecha; }

            CStgGmk_Pinball **m_ppGmk;   // 0x00, GetMaxNum gimmicks (GetGmk reads it)
            unsigned long m_Unk04;       // 0x04
            float m_Unk08;               // 0x08
            float m_Unk0C;               // 0x0C
            bcs::FVEC3 m_vDefaultPos;    // 0x10, GetDefaultPos
            CPinballMecha m_Mecha;       // 0x1C, the launcher (vptr at 0x24)

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
