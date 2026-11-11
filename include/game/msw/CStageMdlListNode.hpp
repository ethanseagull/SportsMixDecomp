#ifndef MSW_CSTAGEMDLLISTNODE_HPP_
#define MSW_CSTAGEMDLLISTNODE_HPP_

#include "bcs/CListNodeBase.hpp"

namespace bcs {
    class CScnObj;
}

namespace msw {

    // (name guessed) The animation player of a stage model node (what
    // CStageMdlListNode holds at 0x18). A DOL class without a name; the
    // vptr is at 0xA8, after the data. Only the slots BasketF's
    // CStageBasket05 calls are named; the others are placeholders that keep
    // them in place.
    class CStageNodeAnm {
        private:
            unsigned char m_Unk00[0xA8]; // 0x00

        public:
            virtual void Unk08();
            virtual void Unk0C();
            virtual void Unk10();
            virtual void Unk14();
            virtual void Unk18();
            virtual void Unk1C();
            virtual void Unk20();
            // 0x24 (names guessed) Starts animation `anm`; mode 2 loops it,
            // mode 0 plays it once. CStageBasket05 passes (1.0f, 0.0f, 0.0f,
            // -1) for the rest.
            virtual void Play(long anm, long mode, float rate, float f2, float f3, long n);
            virtual void Unk28();
            virtual void Unk2C();
            virtual void Unk30();
            virtual void Unk34();
            virtual void Unk38();
            // 0x3C (names guessed) Sets the frame of animation anm (-1: all).
            // DodgeF's CStageDodge11 and the DOL's STAGE11::CStgRndAnmObj.
            virtual void SetFrame(long anm, float frame);
            virtual float GetFrame(long anm); // 0x40 (name guessed; the DOL's pinball launcher)
            virtual void Unk44();
            // 0x48 (name and return type guessed) The length in frames of
            // animation anm.
            virtual float GetFrameMax(long anm);
            // 0x4C (name guessed) Sets the playback rate of animation anm (-1: all).
            virtual void SetRate(long anm, float rate);
            virtual void Unk50();
            virtual void Unk54();
            virtual void Unk58();
            // 0x5C (name and return type guessed) Whether the animation has
            // come to its end (CStageBasket05 then starts the next one).
            virtual int IsEnd(long n) const;
    };

    // (member names guessed) A node of a stage's model list, handed to
    // CStageMan::OnPostAppendNode. Only what BasketF's stages read and
    // write is mapped; the size is not known.
    class CStageMdlListNode : public bcs::CListNodeBase {
        public:
            // Bits of m_nFlag that BasketF's CStageBasket01 sets on some
            // models (meaning not known).
            enum {
                FLAG_4 = 0x4,
                FLAG_8 = 0x8
            };

            bcs::CScnObj *GetObj() const { return m_pObj; }
            // (name guessed)
            CStageNodeAnm *GetAnm() const { return m_pAnm; }
            void OnFlag(unsigned long flag) { m_nFlag |= flag; }
            // (name guessed)
            void OffFlag(unsigned long flag) { m_nFlag &= ~flag; }
            unsigned long GetFlag() const { return m_nFlag; }

        private:
            // 0x00: the list links (bcs::CListNodeBase)
            bcs::CScnObj *m_pObj;        // 0x10, the model's scene object
            unsigned char m_Unk14[0x4];  // 0x14
            CStageNodeAnm *m_pAnm;       // 0x18, the model's animations (may be null)
            unsigned long m_nFlag;       // 0x1C
    };
}

namespace msw {

    // (name guessed) What a CStageMdl holds at 0x14: the list of its
    // CStageMdlListNode at 0x30 (HockeyF CStageHockey01::SwitchAudienceMdl_
    // and the DOL's CStageMan::SetDispByTag walk it). Nothing else mapped.
    class CStageMdlNodeList {
        public:
            bcs::CListBase &GetList() { return m_List; }

        private:
            unsigned char m_Unk00[0x30]; // 0x00
            bcs::CListBase m_List;       // 0x30
    };
}

#endif /* !MSW_CSTAGEMDLLISTNODE_HPP_ */
