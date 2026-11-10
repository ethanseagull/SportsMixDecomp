#ifndef CEVENTMOVE_HPP_
#define CEVENTMOVE_HPP_

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

        private:
            unsigned char m_Unk04[0x48]; // 0x04
            float m_fFrame;              // 0x4C (name guessed)
            float m_fEndFrame;           // 0x50 (name guessed)
            unsigned char m_Unk54[0xC];  // 0x54
    }; // size 0x60
}

#endif /* !CEVENTMOVE_HPP_ */
