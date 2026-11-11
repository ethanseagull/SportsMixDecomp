#ifndef MSW_CSTAGE10BASE_HPP_
#define MSW_CSTAGE10BASE_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    // (purpose guessed from the names) Stage 10, the panel stage: the board
    // of score and bonus panels the players run over. A DOL class used as a
    // second base by the sports' stages (BasketF's CStageBasket10). Only the
    // nested classes needed so far are declared; the members of
    // CStage10Base itself are not mapped yet.
    class CStage10Base {
        public:
            // One panel of the board. A DOL class with its data first and
            // the vptr at 0x0C. Only the virtuals up to the ones
            // CTelopDispBase calls are declared; the DOL's vtable
            // (lbl_80472F20) has more after them. The UnkXX names are slots
            // whose functions have no name in the DOL.
            class CPanelInfoBase {
                private:
                    unsigned long m_Unk00; // 0x00
                    unsigned long m_Unk04; // 0x04
                    unsigned long m_Unk08; // 0x08

                public:
                    virtual ~CPanelInfoBase();                     // 0x08
                    virtual void Unk0C();                          // 0x0C
                    virtual void Unk10();                          // 0x10
                    virtual void Unk14();                          // 0x14
                    virtual void Unk18();                          // 0x18
                    virtual void Unk1C();                          // 0x1C (Create)
                    virtual void Unk20();                          // 0x20 (Main)
                    virtual void Unk24();                          // 0x24
                    virtual void Unk28();                          // 0x28
                    virtual void Unk2C();                          // 0x2C
                    virtual void Unk30();                          // 0x30
                    virtual void Unk34();                          // 0x34
                    virtual void Unk38();                          // 0x38
                    virtual void Unk3C();                          // 0x3C
                    virtual void Unk40();                          // 0x40
                    virtual void Unk44();                          // 0x44
                    virtual void Unk48();                          // 0x48
                    virtual void Unk4C();                          // 0x4C (IsRide)
                    virtual void Unk50();                          // 0x50 (IsRideAnyChr)
                    virtual void Unk54();                          // 0x54 (IsOverlap)
                    virtual void Unk58();                          // 0x58
                    virtual void Unk5C();                          // 0x5C (Lock)
                    virtual void Unk60();                          // 0x60
                    virtual void Unk64();                          // 0x64 (SetScored)
                    virtual void Unk68();                          // 0x68
                    virtual void Unk6C();                          // 0x6C (SetFailed)
                    virtual void Unk70();                          // 0x70 (Terminate)
                    virtual void Unk74();                          // 0x74
                    virtual void Unk78();                          // 0x78 (GetPanelScore)
                    // (return types guessed) The bonus kind (1 and 2 seen)
                    // and its rate; CTelopDispBase::FindKind maps the pair
                    // to a telop.
                    virtual long GetPanelBonusType() const;        // 0x7C
                    virtual long GetPanelBonusRate() const;        // 0x80
            };

            // (purpose guessed) A telop shown over a goal when a bonus
            // panel is scored; it removes itself when its time runs out.
            // A DOL class; the derived classes give the display (Open).
            class CTelopDispBase : public bcs::CUnivTaskEx {
                public:
                    CTelopDispBase(bcs::CTask *pParent);
                    virtual ~CTelopDispBase();
                    virtual void Main();
                    // Starts the telop for team nTeam's target goal with the
                    // telop kind of the panel.
                    virtual void Init(long nTeam, const CPanelInfoBase *pPanel);
                    virtual void Erase();
                    // (name guessed) Where the telop is shown, relative to
                    // the goal.
                    virtual bcs::FVEC3 GetDispOffset() const;
                    // (name guessed) How long the telop stays (frames).
                    virtual float GetDispTime() const { return 90.0f; }
                    // The telop kind for a panel's bonus; -1 for none.
                    virtual long FindKind(const CPanelInfoBase *pPanel) const;
                    // (name guessed) Pure (null in the DOL's vtable).
                    virtual void Open(long nTeam, const bcs::FVEC3 &rPos, long nKind) = 0;

                private:
                    int m_bActive;     // 0x34 (name guessed) set by Init
                    float m_fTime;     // 0x38 (name guessed) frames left
            };
    };
}

#endif /* !MSW_CSTAGE10BASE_HPP_ */
