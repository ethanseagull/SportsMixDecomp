#ifndef MSW_CSTAGESCROLL_HPP_
#define MSW_CSTAGESCROLL_HPP_

#include "nw4r/g3d/ResFile.hpp"

namespace msw {

    class CArn;
    class CStageMan;

    // (names guessed) The scrolling background of a stage, a DOL class held
    // as a member (HockeyF's CStageHockey05 at 0x218). Size 0x87C, with the
    // vptr at 0x878 after the data; the constructor and destructor are its
    // only named functions. The constructor stores the stage at 0x00 and
    // builds 13 entries of 0x24 bytes at 0x0C, two lists at 0x1E0 and 0x208
    // and 16 entries of 0x60 bytes at 0x270; 0x870 and 0x874 are the scene
    // objects whose X and Y angles GetRotX and GetRotY return.
    //
    // Vtable at 0x80474908. The slots before the destructor are not named
    // in the DOL; they are placeholders that keep the known ones in place.
    class CStageScroll {
        public:
            CStageScroll(CStageMan *pStage);

        private:
            CStageMan *m_pStage;          // 0x00
            unsigned char m_Unk04[0x874]; // 0x04

        public:
            virtual void Unk08();
            virtual void Unk0C();
            virtual void Unk10();
            virtual void Unk14();
            virtual void Unk18();
            virtual void Unk1C();
            virtual void Unk20();
            virtual void Unk24();
            virtual void Unk28();
            virtual void Unk2C();
            virtual void Unk30();
            virtual void Unk34();
            virtual void Unk38();
            virtual void Unk3C();
            virtual void Unk40();
            virtual void Unk44();
            virtual void Unk48();
            virtual void Unk4C();
            virtual void Unk50();
            virtual void Unk54();
            virtual void Unk58();
            virtual void Unk5C();
            virtual void Unk60();
            virtual void Unk64();
            virtual void Unk68();
            virtual void Unk6C();
            virtual void Unk70();
            virtual void Unk74();
            virtual void Unk78();
            virtual void Unk7C();
            virtual void Unk80();
            virtual void Unk84();
            virtual void Unk88();
            virtual ~CStageScroll();                                                    // 0x8C
            // 0x90, fn_8016EB5C: one frame of the scroll.
            virtual void Main(float frame);
            virtual void Unk94();
            // 0x98, fn_8016C304: builds scroll entry no from a stage model.
            virtual void CreateMdl(long no, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            // 0x9C, fn_8016C914
            virtual void OnSceneEntry();
            // 0xA0, fn_8016DF98: the X angle of the object at 0x870 (0 without one).
            virtual float GetRotX() const;
            // 0xA4, fn_8016DFB4: the Y angle of the object at 0x874.
            virtual float GetRotY() const;
    };
}

#endif /* !MSW_CSTAGESCROLL_HPP_ */
