#ifndef CMENULINKBASE_HPP_
#define CMENULINKBASE_HPP_

namespace msw {

    // Size 0x1C. Base of every menu class made by newMenuClass<T>(long).
    // The class has no vtable symbol of its own in the game, so every virtual
    // is pure or inline. Member names are guesses.
    class CMenuLinkBase {
        public:
            CMenuLinkBase(long nId)
                : m_Unk08(0)
                , m_nId(nId)
                , m_Unk10(0)
                , m_Unk14(0)
            {
            }

            unsigned long m_Unk00; // 0x00, not set by the constructor
            unsigned long m_Unk04; // 0x04, not set by the constructor
            unsigned long m_Unk08; // 0x08
            long m_nId;            // 0x0C, the constructor argument
            unsigned long m_Unk10; // 0x10
            unsigned long m_Unk14; // 0x14

            // vptr at 0x18
            virtual ~CMenuLinkBase() {}
            virtual void Main() = 0;
            virtual long SendEvent(long nEvent, void *pParam) = 0;
            virtual void Disp() = 0;
            virtual int Set() = 0;     // return type guessed
            virtual void Remove() = 0;
            virtual void SetVisible(int bVisible) = 0;
    };
}

#endif /* !CMENULINKBASE_HPP_ */
