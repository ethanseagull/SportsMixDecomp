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

    // (purpose guessed from the names) The balls of a sport's stage 12: a
    // DOL class (game/msw/CPinballMan.cpp), size 0x54, data first and the
    // vptr at 0x50. It makes the pinball gimmicks (NewGmk, given by each
    // sport's subclass), runs them and the launcher model ("mecha"). Layout
    // read from the DOL constructor (0x801E587C); the order of the virtual
    // functions is that of __vt__Q23msw16CPinballManDodge (DodgeF).
    //
    // DodgeF has weak copies of MechaCancel, MechaReset, SetMechaMdl, both
    // GetGmk and GetDefaultPos (game/dodge/CStgGmk_Pinball_Act_Dodge.cpp, in
    // reverse order of the declarations), so those are inline.
    class CPinballMan {
        public:
            // (name guessed) The launcher model ("mecha"), at 0x1C: a
            // msw::CStgSeSlot with more data, 0x34 bytes, vtable
            // lbl_80484FD8 in the DOL (no class name; CPinballMan's
            // constructor builds it inline). Its new slots follow
            // CStgSeSlot's (the last of which, SE_GetDefaultFunc, is at
            // 0xA0). Only the slots up to Cancel are declared; the names of
            // those without a known caller are placeholders.
            class CMecha : public CStgSeSlot {
                public:
                    virtual void Unk_A4();                          // 0xA4
                    virtual void SetMdl(CStageMdlListNode *pNode);  // 0xA8, CPinballMan::SetMechaMdl
                    virtual void Unk_AC();                          // 0xAC
                    virtual void Main(float frame);                 // 0xB0, CPinballMan::MechaMain
                    virtual void Reset();                           // 0xB4, CPinballMan::MechaReset
                    virtual void Cancel();                          // 0xB8, CPinballMan::MechaCancel

                private:
                    unsigned char m_Unk0C[0x28]; // 0x0C
            };

            CPinballMan();

        private:
            CStgGmk_Pinball **m_ppGmk;   // 0x00, GetMaxNum gimmicks (GetGmk reads it)
            unsigned long m_Unk04;       // 0x04
            float m_Unk08;               // 0x08
            float m_Unk0C;               // 0x0C
            bcs::FVEC3 m_vDefaultPos;    // 0x10, GetDefaultPos
            CMecha m_Mecha;              // 0x1C, the launcher (its vptr at 0x24)

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
            virtual const bcs::FVEC3 &GetDefaultPos() const { return m_vDefaultPos; }
            // Null before CreateGmk.
            virtual CStgGmk_Pinball *GetGmk(long idx)
            {
                return m_ppGmk != 0 ? m_ppGmk[idx] : 0;
            }
            virtual const CStgGmk_Pinball *GetGmk(long idx) const
            {
                return m_ppGmk != 0 ? m_ppGmk[idx] : 0;
            }
            virtual void InactivateAll();
            virtual void SetMechaMdl(CStageMdlListNode *pNode) { m_Mecha.SetMdl(pNode); }
            virtual void MechaReset() { m_Mecha.Reset(); }
            virtual void MechaCancel() { m_Mecha.Cancel(); }
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
