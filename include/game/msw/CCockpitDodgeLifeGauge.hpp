#ifndef CCOCKPITDODGELIFEGAUGE_HPP_
#define CCOCKPITDODGELIFEGAUGE_HPP_

#include "game/msw/CCockpitDodgeLayoutBase.hpp"

namespace bcs {
    namespace G2D {
        class CPaneCtrl;
    }
}

namespace msw {
    class CChrCore;

    // Size 0xC4 (CCockpitDodge::CreateObjImpl allocates it). The dodge
    // game's life gauges, one per character of each team, with their face
    // icons, player marks and stars (DodgeF game/dodge/CCockpitDodgeLifeGauge.cpp).
    // It overrides the gauge slides of CCockpitDodgeLayoutBase. Return
    // types and argument names are guesses; nIdx is a team colour
    // (CGame::GetTeamColorIdx, 0 or 1), nNo the character's number in the
    // team. A life is drawn as up to six marks; past six the marks change
    // colour (one "bar" per six points).
    class CCockpitDodgeLifeGauge : public CCockpitDodgeLayoutBase {
        public:
            CCockpitDodgeLifeGauge(long nId);
            virtual ~CCockpitDodgeLifeGauge();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            virtual int GaugeOut();
            virtual int GaugeIn();
            virtual int GaugeOutWait();
            virtual int GaugeInWait();

            // Plays animation nAnim on character nNo's gauge; -1 for a bad
            // character.
            long LifeAnim(long nIdx, long nNo, long nAnim);
            // The two life bars of character nNo, each a ratio from 0 to 1.
            void SetLifeValue(long nIdx, long nNo, float fRate, float fRate2);
            // (empty)
            void SetLifeValueText(long nIdx, long nNo, unsigned short nLife);
            // The life marks of character nNo for nLife points, and nGuard
            // guard marks (0 to 6).
            void SetLifeValue(long nIdx, long nNo, long nLife, long nGuard);
            // From nPrevLife to nLife points and from nPrevGuard to nGuard
            // guard marks, with the break animations of the lost marks.
            void AddLifeValue(long nIdx, long nNo, long nPrevLife, long nLife, long nPrevGuard, long nGuard);
            void _AddLifeValue(long nIdx, long nNo, long nPrevLife, long nLife, long nPrevGuard, long nGuard);
            // nKind 0 to 3: a hit animation of the gauge; kinds 1 and 2 also
            // animate the marks from nFrom up to nTo.
            void SetLifeAnim(long nIdx, long nNo, long nKind, long nTo, long nFrom);
            // The break effect of mark nPiece.
            long SetBreakPieceAnim(long nIdx, long nNo, long nPiece, long nBar);
            // Plays nAnim on mark nPiece, then nNext if not negative.
            long SetLifePieceAnim(long nIdx, long nNo, long nPiece, long nBar, long nAnim, long nNext);
            void SetLifeDisp(int bDisp);
            // (does nothing)
            long ChangeChargeGauge(long nIdx, long nNo, float fRate, float fRate2);
            // The face icons of every character in the game.
            void SetFaceIcon();
            void SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl);
            // Puts pChr's face, its player or COM mark and its star (shown
            // for a star player) on gauge nNo of team nTeam.
            void SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl, long nTeam, long nNo, const CChrCore *pChr);

        private:
            // (inline, defined in the .cpp) Plays slide animation nAnim on
            // both teams' gauges.
            void PlayAnimGauge(long nAnim);

            // (meaning not known; nothing in DodgeF reads it) One per
            // character slot, cleared by the constructor.
            struct GaugeState {
                GaugeState() : m_fValue(0.0f), m_nUnk4(0), m_nUnk5(0), m_nUnk6(0), m_nUnk7(0) {}

                float m_fValue;          // 0x00
                unsigned char m_nUnk4;   // 0x04
                unsigned char m_nUnk5;   // 0x05
                unsigned char m_nUnk6;   // 0x06
                unsigned char m_nUnk7;   // 0x07
            };

            unsigned char m_nChrNum;        // 0x60, characters per team (at least 2)
            GaugeState m_GaugeState[2][6];  // 0x64, by team colour
    };
}

#endif /* !CCOCKPITDODGELIFEGAUGE_HPP_ */
