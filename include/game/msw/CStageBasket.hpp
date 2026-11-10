#ifndef MSW_CSTAGEBASKET_HPP_
#define MSW_CSTAGEBASKET_HPP_

#include "game/msw/CStageBasket01.hpp"
#include "game/msw/CStageBasket17.hpp"
#include "game/msw/CStageManBasket.hpp"

namespace msw {

    // INCOMPLETE: the basket stages (BasketF) that CStageFactoryBasket::Create
    // makes, declared only as far as Create needs them: the constructor and
    // the size (from the allocation there). Their members are not mapped;
    // each unit that writes one of these classes should move it to a header
    // of its own and map the padding.

    // The destructors are declared so that they are the key functions: this
    // header must not make a unit emit the vtables.

    // Stages 01 and 17 have their own headers.

    class CStageBasket02 : public CStageManBasket {
        public:
            CStageBasket02(bcs::CTask *pParent);
            virtual ~CStageBasket02();

        private:
            unsigned char m_Unk218[0x34]; // size 0x24C
    };

    class CStageBasket03 : public CStageManBasket {
        public:
            CStageBasket03(bcs::CTask *pParent);
            virtual ~CStageBasket03();

        private:
            unsigned char m_Unk218[0xA0]; // size 0x2B8
    };

    class CStageBasket05 : public CStageManBasket {
        public:
            CStageBasket05(bcs::CTask *pParent);
            virtual ~CStageBasket05();

        private:
            unsigned char m_Unk218[0x84]; // size 0x29C
    };

    class CStageBasket06 : public CStageManBasket {
        public:
            CStageBasket06(bcs::CTask *pParent);
            virtual ~CStageBasket06();

        private:
            unsigned char m_Unk218[0x30]; // size 0x248
    };

    class CStageBasket07 : public CStageManBasket {
        public:
            CStageBasket07(bcs::CTask *pParent);
            virtual ~CStageBasket07();

        private:
            unsigned char m_Unk218[0x20]; // size 0x238
    };

    class CStageBasket09 : public CStageManBasket {
        public:
            CStageBasket09(bcs::CTask *pParent);
            virtual ~CStageBasket09();

        private:
            unsigned char m_Unk218[0x24]; // size 0x23C
    };

    // Made with -1 (meaning not known).
    class CStageBasket10 : public CStageManBasket {
        public:
            CStageBasket10(bcs::CTask *pParent, long);
            virtual ~CStageBasket10();

        private:
            unsigned char m_Unk218[0x464]; // size 0x67C
    };

    class CStageBasket11 : public CStageManBasket {
        public:
            CStageBasket11(bcs::CTask *pParent);
            virtual ~CStageBasket11();

        private:
            unsigned char m_Unk218[0x43C]; // size 0x654
    };

    class CStageBasket13 : public CStageManBasket {
        public:
            CStageBasket13(bcs::CTask *pParent);
            virtual ~CStageBasket13();

        private:
            unsigned char m_Unk218[0x54]; // size 0x26C
    };

    class CStageBasket15 : public CStageManBasket {
        public:
            CStageBasket15(bcs::CTask *pParent);
            virtual ~CStageBasket15();

        private:
            unsigned char m_Unk218[0x8C]; // size 0x2A4
    };

    class CStageBasket16 : public CStageManBasket {
        public:
            CStageBasket16(bcs::CTask *pParent);
            virtual ~CStageBasket16();

        private:
            unsigned char m_Unk218[0x1C]; // size 0x234
    };

    // Stage 31: BasketF's own CStageManTournament (its constructor is in
    // BasketF). Assumed to derive from CStageManBasket, since Create returns
    // it as one; TourF may have a class of the same name.
    class CStageManTournament : public CStageManBasket {
        public:
            CStageManTournament(bcs::CTask *pParent);
            virtual ~CStageManTournament();

        private:
            unsigned char m_Unk218[0x4]; // size 0x21C
    };

    // The Hungry Boss Packun stages (70 to 72).
    class CStageBasketHungryBossPackun70 : public CStageManBasket {
        public:
            CStageBasketHungryBossPackun70(bcs::CTask *pParent);
            virtual ~CStageBasketHungryBossPackun70();

        private:
            unsigned char m_Unk218[0x24]; // size 0x23C
    };

    class CStageBasketHungryBossPackun71 : public CStageManBasket {
        public:
            CStageBasketHungryBossPackun71(bcs::CTask *pParent);
            virtual ~CStageBasketHungryBossPackun71();

        private:
            unsigned char m_Unk218[0x6C]; // size 0x284
    };

    class CStageBasketHungryBossPackun72 : public CStageManBasket {
        public:
            CStageBasketHungryBossPackun72(bcs::CTask *pParent);
            virtual ~CStageBasketHungryBossPackun72();

        private:
            unsigned char m_Unk218[0x54]; // size 0x26C
    };
}

#endif /* !MSW_CSTAGEBASKET_HPP_ */
