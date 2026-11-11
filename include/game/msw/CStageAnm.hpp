#ifndef MSW_CSTAGEANM_HPP_
#define MSW_CSTAGEANM_HPP_

namespace msw {

    // The animations of a stage model; msw::CStgMdl keeps a pointer to one
    // at 0x14 (GetAnmObj) and forwards its animation getters to it. Nothing
    // of it has a name in the DOL except the type below, which msw::CStgMdl's
    // animation getters take
    // (GetAnmFrame__Q23msw7CStgMdlCFQ33msw9CStageAnm7ANM_IDX). The vptr is at
    // 0xA8 (the data members come before the first virtual function); the
    // slots are those the inline CStgMdl getters call (BasketF
    // game/basket/CStgGmk_WaterWorks_Basket.cpp). All names and the slots
    // without a caller are guesses.
    class CStageAnm {
        public:
            // Which of the animations playing at once is meant. Values not
            // known.
            enum ANM_IDX {
            };

        private:
            unsigned char m_Unk00[0xA8]; // 0x00

        public:
            virtual ~CStageAnm();                                              // 0x08
            virtual void VFunc0C();                                            // 0x0C
            virtual void VFunc10();                                            // 0x10
            virtual void VFunc14();                                            // 0x14
            virtual void VFunc18();                                            // 0x18
            virtual void VFunc1C();                                            // 0x1C
            virtual void VFunc20();                                            // 0x20
            virtual void VFunc24();                                            // 0x24
            virtual void VFunc28();                                            // 0x28
            virtual unsigned long GetCurAnmId(ANM_IDX idx) const;              // 0x2C
            virtual void VFunc30();                                            // 0x30
            virtual void VFunc34();                                            // 0x34
            virtual bool IsAnmExist(unsigned long anm, ANM_IDX idx) const;     // 0x38
            virtual void SetFrame(long idx, float frame);                      // 0x3C, idx -1: all
            virtual float GetFrame(ANM_IDX idx) const;                         // 0x40
            virtual void VFunc44();                                            // 0x44
            virtual float GetTotalFrame(ANM_IDX idx) const;                    // 0x48
    };
}

#endif /* !MSW_CSTAGEANM_HPP_ */
