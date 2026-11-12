#ifndef CEVENTMOVE_HPP_
#define CEVENTMOVE_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    class CEventModel;
    class CEventNodeBase;

    // Moves an event model (DOL class, 0x60 bytes, vptr at 0x00). Created
    // with a model it attaches itself to (the callers in TourF drop the
    // pointer); msw::CRouteMove (TourF) derives from it. The virtual
    // functions are in the order of __vt__Q23msw10CRouteMove. Only the
    // members used so far; names of the parameters guessed.
    class CEventMove {
        public:
            CEventMove(CEventModel *pModel, CEventNodeBase *pNode, int b);
            virtual ~CEventMove();
            virtual void SetFrame(float fFrame);
            virtual void UpdateFrame(float fFrame);

            // Whether the move advances with the model's frames.
            void SetUpdate(int bUpdate);

            // (name guessed) Inline in msw::CTournamentMan::IsMoveEnd: the
            // move has no end frame or has reached it.
            int IsEnd() const
            {
                if (m_fEndFrame <= 0.0f) {
                    return 1;
                }
                return m_fEndFrame <= m_fFrame;
            }

        protected:
            // (CEventModel links the move and writes its offsets; protected
            // for msw::CRouteMove of TourF)
            friend class CEventModel;

            // msw::CEventModel's SetMoveOffset, SetMoveOffsetRot and
            // SetMoveFlip write these through the model. (names guessed)
            bcs::FVEC3 m_vOffset;        // 0x04
            bcs::FVEC3 m_vOffsetRot;     // 0x10
            bcs::FVEC3 m_vPathRot;       // 0x1C: rotation of the path (only y is used; CEventModel::SetMovePathRot)
            // What the path gave at the frame set last (SetFrame), or what
            // the model had before the step (UpdateFrame).
            bcs::FVEC3 m_vPos;           // 0x28
            bcs::FVEC3 m_vRot;           // 0x34
            bcs::FVEC3 m_vScale;         // 0x40
            float m_fFrame;              // 0x4C (name guessed)
            float m_fEndFrame;           // 0x50 (name guessed) length of the node path
            CEventModel *m_pModel;       // 0x54
            CEventNodeBase *m_pNode;     // 0x58: the node path followed
            short m_nNodeId;             // 0x5C (name guessed) node of the path
            short m_nFlip;               // 0x5E (name guessed)
    }; // size 0x60
}

#endif /* !CEVENTMOVE_HPP_ */
