#ifndef BCS_CREGEX_HPP_
#define BCS_CREGEX_HPP_

namespace bcs {

    // A regular expression matcher; the stages match model node names with
    // it (msw::STAGE::MDL_GEN_RULE). No function of it is named in the
    // game, so only what is called is known: the vptr is at 0x88, and slot
    // 0x18 gives where a submatch starts in the string last matched (BasketF
    // CStageBasket13::OnPostCreateMdl reads the number after "null_scr" from
    // there). The slots before it are placeholders that keep it in place.
    class CRegex {
        private:
            unsigned char m_Unk00[0x88]; // 0x00

        public:
            virtual void Unk08();
            virtual void Unk0C();
            virtual void Unk10();
            virtual void Unk14();
            // (name guessed) Offset of submatch n in the matched string.
            virtual long GetSubMatchPos(long n);
    };
}

#endif /* !BCS_CREGEX_HPP_ */
