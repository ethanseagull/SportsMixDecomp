#ifndef CLOCKCURSORDODGE_HPP_
#define CLOCKCURSORDODGE_HPP_

#include "bcs/FVEC3.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x214 (CCockpitDodge::CreateObjImpl allocates it). The dodge
    // game's lock-on cursors: three per player, each following the
    // character it is locked on (DodgeF game/dodge/CLockCursorDodge.cpp).
    // Member names are guesses.
    class CLockCursorDodge : public CCockpitLayoutBase {
        public:
            enum {
                PLAYER_MAX = 4, // players with cursors
                CURSOR_MAX = 3  // cursors of one player
            };

            // One cursor. 0x24 bytes.
            struct SCursorParameter {
                // (no symbol; name guessed) The state CursorInit leaves.
                void Init(long nPlayer)
                {
                    m_Pos.x = 0.0f;
                    m_Pos.y = 0.0f;
                    m_Pos.z = 0.0f;
                    m_pPaneName = 0;
                    m_pAnimPaneName = 0;
                    m_nBlendAnim = 0xFF;
                    m_nPlayer = nPlayer;
                    m_nState = 0;
                    Reset();
                }

                // (no symbol; name guessed) Drops the target.
                void Reset()
                {
                    m_nFlags = 0;
                    m_nTarget = 0xFF;
                    m_nAnim = 0xFF;
                    m_fRotZ = 0.0f;
                    m_nLockFrame = 0;
                }

                // (no symbol; name guessed) Whether a character is locked on.
                int IsTarget() const
                {
                    return m_nTarget < 0xFF;
                }

                // (no symbol; name guessed) Whether m_Pos was set this frame.
                int IsPosValid() const
                {
                    return m_Flag.bPosValid;
                }

                // (no symbol; name guessed) Turns the cursor's pane by fRotZ
                // around the screen axis.
                void SetRotZ(float fRotZ, bcs::G2D::CPaneCtrl *pPaneCtrl)
                {
                    if (pPaneCtrl == 0) {
                        return;
                    }

                    bcs::G2D::CPaneObj *pPaneObj = pPaneCtrl->GetChildPaneObj(m_pPaneName);

                    if (pPaneObj == 0) {
                        return;
                    }

                    nw4r::math::VEC3 rot(0.0f, 0.0f, fRotZ);
                    pPaneObj->GetPane()->SetRotate(rot);
                }

                // Plays animation nAnim on the cursor's pane, then nNext
                // when it is not negative. Inline (the local static `speed`
                // is all the original keeps of it); defined in
                // CLockCursorDodge.cpp.
                inline void PlayAnimation(long nAnim, long nNext, bcs::G2D::CPaneCtrl *pPaneCtrl);

                // (no symbol; name guessed) Whether animation nAnim of the
                // cursor's first child pane has ended.
                int IsAnimEnd(long nAnim, bcs::G2D::CPaneCtrl *pPaneCtrl)
                {
                    if (pPaneCtrl == 0) {
                        return 0;
                    }

                    bcs::G2D::CPaneObj *pPaneObj = pPaneCtrl->GetChildPaneObj(m_pAnimPaneName);

                    if (pPaneObj == 0) {
                        return 0;
                    }
                    return pPaneObj->IsAnimEnd(nAnim);
                }

                bcs::FVEC3 m_Pos;            // 0x00, where the target is
                const char *m_pPaneName;     // 0x0C, the cursor's pane
                const char *m_pAnimPaneName; // 0x10, its first child pane
                // 0x14: bit 1 (0x2) m_Pos is valid this frame, bit 2 (0x4)
                // another cursor is on the same character.
                union {
                    unsigned char m_nFlags;
                    struct {
                        unsigned char : 5;
                        unsigned char bShared : 1;
                        unsigned char bPosValid : 1;
                        unsigned char : 1;
                    } m_Flag;
                };
                unsigned char m_nTarget;     // 0x15, the character, 0xFF for none
                unsigned char m_nAnim;       // 0x16, the animation playing
                unsigned char m_nBlendAnim;  // 0x17, 0xFF (never set otherwise)
                float m_fRotZ;               // 0x18
                long m_nLockFrame;           // 0x1C, frames locked on
                unsigned char m_nPlayer;     // 0x20
                unsigned char m_nState;      // 0x21: 0 free, 3 starting, 2 locked, 1 ending
            };

            CLockCursorDodge(long nId);
            virtual ~CLockCursorDodge();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            void CursorInit();
            long GetTargetPosition();

        private:
            SCursorParameter m_Cursor[PLAYER_MAX][CURSOR_MAX]; // 0x60
            long m_nPlayerNum;                                 // 0x210, 4 from CursorInit
    };
}

#endif /* !CLOCKCURSORDODGE_HPP_ */
