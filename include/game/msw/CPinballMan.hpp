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
    // 0x1C: a sound slot (CStgSeSlot, vptr at 0x08) plus 0x28 bytes. It runs
    // the launcher model of the stage (a model list node with an
    // animation): it idles, plays the entrance event, and shoots a ball out
    // of the node "tekkyu_exit_pos_null", carrying the ball's model on that
    // node until the shot animation lets it go. Its functions follow
    // CPinballMan's in the DOL (0x801E63D4 to 0x801E73F0). Names guessed.
    class CPinballMecha : public CStgSeSlot {
        public:
            // (names guessed)
            enum STATE {
                STATE_NONE,
                STATE_IDLE,
                STATE_EVENT,
                STATE_SHOOT_LEFT,  // the angle is above pi
                STATE_SHOOT_RIGHT
            };

            enum {
                QUAKE_NUM = 2
            };

            CPinballMecha() : m_pNode(0), m_nExitNode(0), m_pGmk(0) { ClearQuake(); }
            virtual ~CPinballMecha() {}
            virtual long SE_GetSlotNum() const { return 2; }

            virtual void Init() { SE_Init(); }                         // 0xA4
            virtual void SetMdl(CStageMdlListNode *pNode);             // 0xA8
            virtual bool IsValid() const { return m_pNode != 0; }      // 0xAC
            virtual void Main(float frame);                            // 0xB0
            virtual void Reset();                                      // 0xB4
            virtual void Cancel() { Reset(); }                         // 0xB8
            virtual bool IsReady() const                               // 0xBC
            {
                bool bReady = false;
                if (IsValid() && m_nState == STATE_IDLE) {
                    bReady = true;
                }
                return bReady;
            }
            virtual void SetIdle();                                    // 0xC0
            virtual void StartEvent(CStgGmk_Pinball *pGmk);            // 0xC4
            virtual void Shoot(CStgGmk_Pinball *pGmk, float angle);    // 0xC8
            virtual CStgGmk_Pinball *GetGmk() { return m_pGmk; }       // 0xCC
            virtual const CStgGmk_Pinball *GetGmk() const { return m_pGmk; } // 0xD0
            virtual void UpdateIdle(float frame);                      // 0xD4
            virtual void UpdateEvent(float frame);                     // 0xD8
            virtual void UpdateShootLeft(float frame);                 // 0xDC
            virtual void UpdateShootRight(float frame);                // 0xE0
            virtual void ResetIdleCount();                             // 0xE4
            virtual void UpdateGmkPos();                               // 0xE8
            virtual void ClearQuake();                                 // 0xEC
            virtual void AddQuake(float frame);                        // 0xF0
            virtual void UpdateQuake();                                // 0xF4

        private:
            CStageMdlListNode *m_pNode;     // 0x0C, the launcher model
            unsigned long m_nExitNode;      // 0x10, the node the balls leave from
            long m_nState;                  // 0x14
            long m_nStep;                   // 0x18, step within the state
            CStgGmk_Pinball *m_pGmk;        // 0x1C, the ball being shot
            float m_fAngle;                 // 0x20, the angle it is shot at
            long m_nIdleCount;              // 0x24, idle loops before the next fidget
            float m_afQuake[QUAKE_NUM];     // 0x28, animation frames that shake the camera (-1: none)
            long m_nQuakeNum;               // 0x30
    };

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
            // (name guessed) The launcher; its functions are reached through
            // its vtable.
            CPinballMecha *GetMecha() { return &m_Mecha; }

            CStgGmk_Pinball **m_ppGmk;   // 0x00, GetMaxNum gimmicks (GetGmk reads it)
            long m_nGmkNum;              // 0x04, GetMaxNum at Init
            float m_fShotTime;           // 0x08, frames to the next shot (names guessed)
            float m_fUnk0C;              // 0x0C, -1 (not read in the DOL)
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
            virtual CStgGmk_Pinball *NewGmk(bcs::CTask *pParent, int arg) = 0;
            virtual long GetParam_BallNum() const;
            virtual float GetParam_InitialInterval() const;
            virtual float GetParam_Interval() const;
            virtual float GetParam_ShotAngle() const;
            virtual long GetParam_ShotSideRate() const;
    };
}

#endif /* !MSW_CPINBALLMAN_HPP_ */
