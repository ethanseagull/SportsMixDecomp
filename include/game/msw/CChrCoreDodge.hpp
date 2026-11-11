#ifndef MSW_CCHRCOREDODGE_HPP_
#define MSW_CCHRCOREDODGE_HPP_

#include "game/msw/CChrCore.hpp"

namespace msw {

    // INCOMPLETE: the dodgeball character core (DodgeF, size 0x930: what
    // CChrManDodge::CreateObj_ allocates for it and most characters), and one
    // class per character. Only the constructors, the destructors (declared
    // so that the vtables are not emitted from here) and the sizes are known
    // here; the padding stands for members not mapped yet.
    class CChrCoreDodge : public CChrCore {
        public:
            CChrCoreDodge();
            virtual ~CChrCoreDodge();

            // Slots 0x848..0xEA8 of __vt__Q23msw13CChrCoreDodge are not mapped
            // yet (CChrCore declares up to 0x844); placeholders keep GetLife at 0xEAC.
            virtual void Unk848();
            virtual void Unk84C();
            virtual void Unk850();
            virtual void Unk854();
            virtual void Unk858();
            virtual void Unk85C();
            virtual void Unk860();
            virtual void Unk864();
            virtual void Unk868();
            virtual void Unk86C();
            virtual void Unk870();
            virtual void Unk874();
            virtual void Unk878();
            virtual void Unk87C();
            virtual void Unk880();
            virtual void Unk884();
            virtual void Unk888();
            virtual void Unk88C();
            virtual void Unk890();
            virtual void Unk894();
            virtual void Unk898();
            virtual void Unk89C();
            virtual void Unk8A0();
            virtual void Unk8A4();
            virtual void Unk8A8();
            virtual void Unk8AC();
            virtual void Unk8B0();
            virtual void Unk8B4();
            virtual void Unk8B8();
            virtual void Unk8BC();
            virtual void Unk8C0();
            virtual void Unk8C4();
            virtual void Unk8C8();
            virtual void Unk8CC();
            virtual void Unk8D0();
            virtual void Unk8D4();
            virtual void Unk8D8();
            virtual void Unk8DC();
            virtual void Unk8E0();
            virtual void Unk8E4();
            virtual void Unk8E8();
            virtual void Unk8EC();
            virtual void Unk8F0();
            virtual void Unk8F4();
            virtual void Unk8F8();
            virtual void Unk8FC();
            virtual void Unk900();
            virtual void Unk904();
            virtual void Unk908();
            virtual void Unk90C();
            virtual void Unk910();
            virtual void Unk914();
            virtual void Unk918();
            virtual void Unk91C();
            virtual void Unk920();
            virtual void Unk924();
            virtual void Unk928();
            virtual void Unk92C();
            virtual void Unk930();
            virtual void Unk934();
            virtual void Unk938();
            virtual void Unk93C();
            virtual void Unk940();
            virtual void Unk944();
            virtual void Unk948();
            virtual void Unk94C();
            virtual void Unk950();
            virtual void Unk954();
            virtual void Unk958();
            virtual void Unk95C();
            virtual void Unk960();
            virtual void Unk964();
            virtual void Unk968();
            virtual void Unk96C();
            virtual void Unk970();
            virtual void Unk974();
            virtual void Unk978();
            virtual void Unk97C();
            virtual void Unk980();
            virtual void Unk984();
            virtual void Unk988();
            virtual void Unk98C();
            virtual void Unk990();
            virtual void Unk994();
            virtual void Unk998();
            virtual void Unk99C();
            virtual void Unk9A0();
            virtual void Unk9A4();
            virtual void Unk9A8();
            virtual void Unk9AC();
            virtual void Unk9B0();
            virtual void Unk9B4();
            virtual void Unk9B8();
            virtual void Unk9BC();
            virtual void Unk9C0();
            virtual void Unk9C4();
            virtual void Unk9C8();
            virtual void Unk9CC();
            virtual void Unk9D0();
            virtual void Unk9D4();
            virtual void Unk9D8();
            virtual void Unk9DC();
            virtual void Unk9E0();
            virtual void Unk9E4();
            virtual void Unk9E8();
            virtual void Unk9EC();
            virtual void Unk9F0();
            virtual void Unk9F4();
            virtual void Unk9F8();
            virtual void Unk9FC();
            virtual void UnkA00();
            virtual void UnkA04();
            virtual void UnkA08();
            virtual void UnkA0C();
            virtual void UnkA10();
            virtual void UnkA14();
            virtual void UnkA18();
            virtual void UnkA1C();
            virtual void UnkA20();
            virtual void UnkA24();
            virtual void UnkA28();
            virtual void UnkA2C();
            virtual void UnkA30();
            virtual void UnkA34();
            virtual void UnkA38();
            virtual void UnkA3C();
            virtual void UnkA40();
            virtual void UnkA44();
            virtual void UnkA48();
            virtual void UnkA4C();
            virtual void UnkA50();
            virtual void UnkA54();
            virtual void UnkA58();
            virtual void UnkA5C();
            virtual void UnkA60();
            virtual void UnkA64();
            virtual void UnkA68();
            virtual void UnkA6C();
            virtual void UnkA70();
            virtual void UnkA74();
            virtual void UnkA78();
            virtual void UnkA7C();
            virtual void UnkA80();
            virtual void UnkA84();
            virtual void UnkA88();
            virtual void UnkA8C();
            virtual void UnkA90();
            virtual void UnkA94();
            virtual void UnkA98();
            virtual void UnkA9C();
            virtual void UnkAA0();
            virtual void UnkAA4();
            virtual void UnkAA8();
            virtual void UnkAAC();
            virtual void UnkAB0();
            virtual void UnkAB4();
            virtual void UnkAB8();
            virtual void UnkABC();
            virtual void UnkAC0();
            virtual void UnkAC4();
            virtual void UnkAC8();
            virtual void UnkACC();
            virtual void UnkAD0();
            virtual void UnkAD4();
            virtual void UnkAD8();
            virtual void UnkADC();
            virtual void UnkAE0();
            virtual void UnkAE4();
            virtual void UnkAE8();
            virtual void UnkAEC();
            virtual void UnkAF0();
            virtual void UnkAF4();
            virtual void UnkAF8();
            virtual void UnkAFC();
            virtual void UnkB00();
            virtual void UnkB04();
            virtual void UnkB08();
            virtual void UnkB0C();
            virtual void UnkB10();
            virtual void UnkB14();
            virtual void UnkB18();
            virtual void UnkB1C();
            virtual void UnkB20();
            virtual void UnkB24();
            virtual void UnkB28();
            virtual void UnkB2C();
            virtual void UnkB30();
            virtual void UnkB34();
            virtual void UnkB38();
            virtual void UnkB3C();
            virtual void UnkB40();
            virtual void UnkB44();
            virtual void UnkB48();
            virtual void UnkB4C();
            virtual void UnkB50();
            virtual void UnkB54();
            virtual void UnkB58();
            virtual void UnkB5C();
            virtual void UnkB60();
            virtual void UnkB64();
            virtual void UnkB68();
            virtual void UnkB6C();
            virtual void UnkB70();
            virtual void UnkB74();
            virtual void UnkB78();
            virtual void UnkB7C();
            virtual void UnkB80();
            virtual void UnkB84();
            virtual void UnkB88();
            virtual void UnkB8C();
            virtual void UnkB90();
            virtual void UnkB94();
            virtual void UnkB98();
            virtual void UnkB9C();
            virtual void UnkBA0();
            virtual void UnkBA4();
            virtual void UnkBA8();
            virtual void UnkBAC();
            virtual void UnkBB0();
            virtual void UnkBB4();
            virtual void UnkBB8();
            virtual void UnkBBC();
            virtual void UnkBC0();
            virtual void UnkBC4();
            virtual void UnkBC8();
            virtual void UnkBCC();
            virtual void UnkBD0();
            virtual void UnkBD4();
            virtual void UnkBD8();
            virtual void UnkBDC();
            virtual void UnkBE0();
            virtual void UnkBE4();
            virtual void UnkBE8();
            virtual void UnkBEC();
            virtual void UnkBF0();
            virtual void UnkBF4();
            virtual void UnkBF8();
            virtual void UnkBFC();
            virtual void UnkC00();
            virtual void UnkC04();
            virtual void UnkC08();
            virtual void UnkC0C();
            virtual void UnkC10();
            virtual void UnkC14();
            virtual void UnkC18();
            virtual void UnkC1C();
            virtual void UnkC20();
            virtual void UnkC24();
            virtual void UnkC28();
            virtual void UnkC2C();
            virtual void UnkC30();
            virtual void UnkC34();
            virtual void UnkC38();
            virtual void UnkC3C();
            virtual void UnkC40();
            virtual void UnkC44();
            virtual void UnkC48();
            virtual void UnkC4C();
            virtual void UnkC50();
            virtual void UnkC54();
            virtual void UnkC58();
            virtual void UnkC5C();
            virtual void UnkC60();
            virtual void UnkC64();
            virtual void UnkC68();
            virtual void UnkC6C();
            virtual void UnkC70();
            virtual void UnkC74();
            virtual void UnkC78();
            virtual void UnkC7C();
            virtual void UnkC80();
            virtual void UnkC84();
            virtual void UnkC88();
            virtual void UnkC8C();
            virtual void UnkC90();
            virtual void UnkC94();
            virtual void UnkC98();
            virtual void UnkC9C();
            virtual void UnkCA0();
            virtual void UnkCA4();
            virtual void UnkCA8();
            virtual void UnkCAC();
            virtual void UnkCB0();
            virtual void UnkCB4();
            virtual void UnkCB8();
            virtual void UnkCBC();
            virtual void UnkCC0();
            virtual void UnkCC4();
            virtual void UnkCC8();
            virtual void UnkCCC();
            virtual void UnkCD0();
            virtual void UnkCD4();
            virtual void UnkCD8();
            virtual void UnkCDC();
            virtual void UnkCE0();
            virtual void UnkCE4();
            virtual void UnkCE8();
            virtual void UnkCEC();
            virtual void UnkCF0();
            virtual void UnkCF4();
            virtual void UnkCF8();
            virtual void UnkCFC();
            virtual void UnkD00();
            virtual void UnkD04();
            virtual void UnkD08();
            virtual void UnkD0C();
            virtual void UnkD10();
            virtual void UnkD14();
            virtual void UnkD18();
            virtual void UnkD1C();
            virtual void UnkD20();
            virtual void UnkD24();
            virtual void UnkD28();
            virtual void UnkD2C();
            virtual void UnkD30();
            virtual void UnkD34();
            virtual void UnkD38();
            virtual void UnkD3C();
            virtual void UnkD40();
            virtual void UnkD44();
            virtual void UnkD48();
            virtual void UnkD4C();
            virtual void UnkD50();
            virtual void UnkD54();
            virtual void UnkD58();
            virtual void UnkD5C();
            virtual void UnkD60();
            virtual void UnkD64();
            virtual void UnkD68();
            virtual void UnkD6C();
            virtual void UnkD70();
            virtual void UnkD74();
            virtual void UnkD78();
            virtual void UnkD7C();
            virtual void UnkD80();
            virtual void UnkD84();
            virtual void UnkD88();
            virtual void UnkD8C();
            virtual void UnkD90();
            virtual void UnkD94();
            virtual void UnkD98();
            virtual void UnkD9C();
            virtual void UnkDA0();
            virtual void UnkDA4();
            virtual void UnkDA8();
            virtual void UnkDAC();
            virtual void UnkDB0();
            virtual void UnkDB4();
            virtual void UnkDB8();
            virtual void UnkDBC();
            virtual void UnkDC0();
            virtual void UnkDC4();
            virtual void UnkDC8();
            virtual void UnkDCC();
            virtual void UnkDD0();
            virtual void UnkDD4();
            virtual void UnkDD8();
            virtual void UnkDDC();
            virtual void UnkDE0();
            virtual void UnkDE4();
            virtual void UnkDE8();
            virtual void UnkDEC();
            virtual void UnkDF0();
            virtual void UnkDF4();
            virtual void UnkDF8();
            virtual void UnkDFC();
            virtual void UnkE00();
            virtual void UnkE04();
            virtual void UnkE08();
            virtual void UnkE0C();
            virtual void UnkE10();
            virtual void UnkE14();
            virtual void UnkE18();
            virtual void UnkE1C();
            virtual void UnkE20();
            virtual void UnkE24();
            virtual void UnkE28();
            virtual void UnkE2C();
            virtual void UnkE30();
            virtual void UnkE34();
            virtual void UnkE38();
            virtual void UnkE3C();
            virtual void UnkE40();
            virtual void UnkE44();
            virtual void UnkE48();
            virtual void UnkE4C();
            virtual void UnkE50();
            virtual void UnkE54();
            virtual void UnkE58();
            virtual void UnkE5C();
            virtual void UnkE60();
            virtual void UnkE64();
            virtual void UnkE68();
            virtual void UnkE6C();
            virtual void UnkE70();
            virtual void UnkE74();
            virtual void UnkE78();
            virtual void UnkE7C();
            virtual void UnkE80();
            virtual void UnkE84();
            virtual void UnkE88();
            virtual void UnkE8C();
            virtual void UnkE90();
            virtual void UnkE94();
            virtual void UnkE98();
            virtual void UnkE9C();
            virtual void UnkEA0();
            virtual void UnkEA4();
            virtual void UnkEA8();
            // the character's lives left (CGameDodge::GetChrRestLife(m_nPlayerNo))
            virtual int GetLife() const; // 0xEAC

        protected:
            unsigned char m_Unk574[0x930 - 0x574];
    };

    // Size 0x930.
    class CChrCoreDodgeMario : public CChrCoreDodge {
        public:
            CChrCoreDodgeMario();
            virtual ~CChrCoreDodgeMario();
    };

    // Size 0x968.
    class CChrCoreDodgeLuigi : public CChrCoreDodge {
        public:
            CChrCoreDodgeLuigi();
            virtual ~CChrCoreDodgeLuigi();

        protected:
            unsigned char m_Unk930[0x968 - 0x930];
    };

    // Size 0x930.
    class CChrCoreDodgePeach : public CChrCoreDodge {
        public:
            CChrCoreDodgePeach();
            virtual ~CChrCoreDodgePeach();
    };

    // Size 0x930.
    class CChrCoreDodgeDaisy : public CChrCoreDodge {
        public:
            CChrCoreDodgeDaisy();
            virtual ~CChrCoreDodgeDaisy();
    };

    // Size 0x930.
    class CChrCoreDodgeYoshi : public CChrCoreDodge {
        public:
            CChrCoreDodgeYoshi();
            virtual ~CChrCoreDodgeYoshi();
    };

    // Size 0x930.
    class CChrCoreDodgeWario : public CChrCoreDodge {
        public:
            CChrCoreDodgeWario();
            virtual ~CChrCoreDodgeWario();
    };

    // Size 0x940.
    class CChrCoreDodgeWaluigi : public CChrCoreDodge {
        public:
            CChrCoreDodgeWaluigi();
            virtual ~CChrCoreDodgeWaluigi();

        protected:
            unsigned char m_Unk930[0x940 - 0x930];
    };

    // Size 0x930.
    class CChrCoreDodgeDonkey : public CChrCoreDodge {
        public:
            CChrCoreDodgeDonkey();
            virtual ~CChrCoreDodgeDonkey();
    };

    // Size 0x930.
    class CChrCoreDodgeDiddy : public CChrCoreDodge {
        public:
            CChrCoreDodgeDiddy();
            virtual ~CChrCoreDodgeDiddy();
    };

    // Size 0x930.
    class CChrCoreDodgeKinopio : public CChrCoreDodge {
        public:
            CChrCoreDodgeKinopio();
            virtual ~CChrCoreDodgeKinopio();
    };

    // Size 0x930.
    class CChrCoreDodgeKoopa : public CChrCoreDodge {
        public:
            CChrCoreDodgeKoopa();
            virtual ~CChrCoreDodgeKoopa();
    };

    // Size 0x930.
    class CChrCoreDodgeKoopaJr : public CChrCoreDodge {
        public:
            CChrCoreDodgeKoopaJr();
            virtual ~CChrCoreDodgeKoopaJr();
    };

    // Size 0x930.
    class CChrCoreDodgeMoogle : public CChrCoreDodge {
        public:
            CChrCoreDodgeMoogle();
            virtual ~CChrCoreDodgeMoogle();
    };

    // Size 0x930.
    class CChrCoreDodgeSaboten : public CChrCoreDodge {
        public:
            CChrCoreDodgeSaboten();
            virtual ~CChrCoreDodgeSaboten();
    };

    // Size 0x944.
    class CChrCoreDodgeNinja : public CChrCoreDodge {
        public:
            CChrCoreDodgeNinja();
            virtual ~CChrCoreDodgeNinja();

        protected:
            unsigned char m_Unk930[0x944 - 0x930];
    };

    // Size 0x934.
    class CChrCoreDodgeSiroma : public CChrCoreDodge {
        public:
            CChrCoreDodgeSiroma();
            virtual ~CChrCoreDodgeSiroma();

        protected:
            unsigned char m_Unk930[0x934 - 0x930];
    };

    // Size 0x934.
    class CChrCoreDodgeSlime : public CChrCoreDodge {
        public:
            CChrCoreDodgeSlime();
            virtual ~CChrCoreDodgeSlime();

        protected:
            unsigned char m_Unk930[0x934 - 0x930];
    };

    // Size 0x934.
    class CChrCoreDodgeKuroma : public CChrCoreDodge {
        public:
            CChrCoreDodgeKuroma();
            virtual ~CChrCoreDodgeKuroma();

        protected:
            unsigned char m_Unk930[0x934 - 0x930];
    };

    // Size 0x930.
    class CChrCoreDodgeMii00 : public CChrCoreDodge {
        public:
            CChrCoreDodgeMii00();
            virtual ~CChrCoreDodgeMii00();
    };

    // Size 0x930.
    class CChrCoreDodgeMii01 : public CChrCoreDodge {
        public:
            CChrCoreDodgeMii01();
            virtual ~CChrCoreDodgeMii01();
    };

    // Size 0x930 (the party game "dodge or die").
    class CChrCoreDodgeDodgeOrDie : public CChrCoreDodge {
        public:
            CChrCoreDodgeDodgeOrDie();
            virtual ~CChrCoreDodgeDodgeOrDie();
    };
}

#endif /* !MSW_CCHRCOREDODGE_HPP_ */
