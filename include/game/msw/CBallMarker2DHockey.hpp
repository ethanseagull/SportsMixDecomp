#ifndef CBALLMARKER2DHOCKEY_HPP_
#define CBALLMARKER2DHOCKEY_HPP_

#include "bcs/FVEC3.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "game/msw/CMenuLinkBase.hpp"
#include "game/msw/CMenuLytObj.hpp"

namespace msw {

    // Size 0x34 (CCockpitHockey::CreateObjImpl allocates it): the 2D marker
    // over the puck, or over the character holding it. CMenuLinkBase at 0
    // (vptr at 0x18), CMenuLytObj at 0x1C (vptr at 0x28), own members from
    // 0x2C. Member names are guesses.
    class CBallMarker2DHockey : public CMenuLinkBase, public CMenuLytObj<bcs::G2D::CPaneCtrl, 1> {
        public:
            // One ball's marker. 0x1C bytes.
            struct SBallParameter {
                enum {
                    KIND_NUM = 3, // pictures of a marker
                    KIND_NONE = 4 // no picture chosen yet
                };

                // The picture shown, or null.
                const char *GetPaneName() const
                {
                    return m_nKind < KIND_NUM ? m_pPaneName[m_nKind] : 0;
                }

                // (name guessed) Sets or clears bit 0 of m_nFlag.
                void SetHide(int bHide)
                {
                    m_nFlag = !bHide ? (m_nFlag & ~1) : (m_nFlag | 1);
                }

                bcs::FVEC3 m_Pos;                  // 0x00, where the marker points
                const char *m_pPaneName[KIND_NUM]; // 0x0C
                // 0x18: flags, cleared as a byte by MarkerInit (bit fields:
                // the game changes them with rlwinm, a byte member would
                // take andi.)
                union {
                    unsigned char m_nFlag;
                    struct {
                        unsigned char m_Unk18 : 6;
                        unsigned char m_bUnk : 1;  // cleared by SetBallKind
                        unsigned char m_bHide : 1; // marker not shown
                    };
                };
                unsigned char m_nKind;             // 0x19, which of m_pPaneName is shown
            };

            CBallMarker2DHockey(long nId);
            virtual ~CBallMarker2DHockey();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();

            void MarkerInit();
            void SetBallKind(SBallParameter *pParam, long nKind);

        private:
            SBallParameter *m_pBallParam; // 0x2C, one per active ball
            long m_nBallNum;              // 0x30
    };
}

#endif /* !CBALLMARKER2DHOCKEY_HPP_ */
