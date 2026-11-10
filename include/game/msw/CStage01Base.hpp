#ifndef MSW_CSTAGE01BASE_HPP_
#define MSW_CSTAGE01BASE_HPP_

namespace msw {

    class CStageMan;

    // (purpose guessed from the name) The extra part of a sport's stage 01,
    // a second base class next to the sport's stage manager (BasketF's
    // msw::CStageBasket01 has it at 0x218). A DOL class, size 8: the stage
    // it belongs to at 0x00 (data declared first), vptr at 0x04. Its
    // destructor and Stage01_Main (empty) are in the DOL.
    class CStage01Base {
        public:
            CStage01Base(CStageMan *pStage);

        private:
            CStageMan *m_pStage; // 0x00

        public:
            virtual ~CStage01Base();
            // Called by the stage's Main_ every frame.
            virtual void Stage01_Main(float);
    };
}

#endif /* !MSW_CSTAGE01BASE_HPP_ */
