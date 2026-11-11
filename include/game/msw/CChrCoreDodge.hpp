#ifndef MSW_CCHRCOREDODGE_HPP_
#define MSW_CCHRCOREDODGE_HPP_

#include "game/msw/CChrCore.hpp"

namespace bcs {
    class CScnObj;
}

namespace msw {

    namespace CHR_CORE_DODGE {
        // (contents not known) A dodgeball character's parameters (NewCcParam_).
        struct CC_PARAM_DODGE;
    }

    // INCOMPLETE: the dodgeball character core (DodgeF, size 0x930: what
    // CChrManDodge::CreateObj_ allocates for it and most characters), and one
    // class per character. Only the constructors, the destructors (declared
    // so that the vtables are not emitted from here) and the sizes are known
    // here; the padding stands for members not mapped yet.
    class CChrCoreDodge : public CChrCore {
        public:
            // The work of the air trick / arts shot states, passed to the
            // SubStateArtsShot* virtuals (see below; only part is mapped).
            struct STATE_WORK_AIR_TRICK_CMN;
            // (contents not known) The work of the diving catch state.
            struct STATE_WORK_DIVING_CATCH;

            CChrCoreDodge();
            virtual ~CChrCoreDodge();
            virtual void Reset_();                                     // 0x384
            virtual void CtorStateAirTrick();                          // 0x584
            virtual void LoopStateAirTrick();                          // 0x588
            virtual void DtorStateAirTrick();                          // 0x58C

            // Slots up to 0xC98 are declared by CChrCore; 0xC9C..0xEA8 of
            // __vt__Q23msw13CChrCoreDodge are not mapped yet. Placeholders keep GetLife at 0xEAC.
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
            virtual CHR_CORE_DODGE::CC_PARAM_DODGE *NewCcParam_(long nHeap) const; // 0xCDC
            virtual const CHR_CORE_DODGE::CC_PARAM_DODGE *GetCcParam_() const; // 0xCE0
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
            virtual void OnDivingCatchStart_(STATE_WORK_DIVING_CATCH *); // 0xD58
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
            virtual void SubStateArtsShotCalcRotParam_(ROT_LERP1 &, float, float); // 0xD88
            virtual void UnkD8C();
            virtual void UnkD90();
            virtual void UnkD94();
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xD98
            virtual void SubStateArtsShotReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xD9C
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA0
            virtual void UnkDA4();
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA8
            virtual void UnkDAC();
            virtual void SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK_CMN *); // 0xDB0
            virtual void UnkDB4();
            virtual void UnkDB8();
            virtual void UnkDBC();
            virtual void SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDC0
            virtual void SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK_CMN *); // 0xDC4
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
            virtual int CanbeLockTarget() const; // 0xE10
            virtual int CanbeSwitchLockTarget() const; // 0xE14
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
            // 0xEB0..0x119C: not mapped yet (__vt__Q23msw13CChrCoreDodge and every
            // character's vtable are 0x11A0 bytes).
            virtual void UnkEB0();
            virtual void UnkEB4();
            virtual void UnkEB8();
            virtual void UnkEBC();
            virtual void UnkEC0();
            virtual void UnkEC4();
            virtual void UnkEC8();
            virtual void UnkECC();
            virtual void UnkED0();
            virtual void UnkED4();
            virtual void UnkED8();
            virtual void UnkEDC();
            virtual void UnkEE0();
            virtual void UnkEE4();
            virtual void UnkEE8();
            virtual void UnkEEC();
            virtual void UnkEF0();
            virtual void UnkEF4();
            virtual void UnkEF8();
            virtual void UnkEFC();
            virtual void UnkF00();
            virtual void UnkF04();
            virtual void UnkF08();
            virtual void UnkF0C();
            virtual void UnkF10();
            virtual void UnkF14();
            virtual void UnkF18();
            virtual void UnkF1C();
            virtual void UnkF20();
            virtual void UnkF24();
            virtual void UnkF28();
            virtual void UnkF2C();
            virtual void UnkF30();
            virtual void UnkF34();
            virtual void UnkF38();
            virtual void UnkF3C();
            virtual void UnkF40();
            virtual void UnkF44();
            virtual void UnkF48();
            virtual void UnkF4C();
            virtual void UnkF50();
            virtual void UnkF54();
            virtual void UnkF58();
            virtual void UnkF5C();
            virtual void UnkF60();
            virtual void UnkF64();
            virtual void UnkF68();
            virtual void UnkF6C();
            virtual void UnkF70();
            virtual void UnkF74();
            virtual void UnkF78();
            virtual void UnkF7C();
            virtual void UnkF80();
            virtual void UnkF84();
            virtual void UnkF88();
            virtual void UnkF8C();
            virtual void UnkF90();
            virtual void UnkF94();
            virtual void UnkF98();
            virtual void UnkF9C();
            virtual void UnkFA0();
            virtual void UnkFA4();
            virtual void UnkFA8();
            virtual void UnkFAC();
            virtual void UnkFB0();
            virtual void UnkFB4();
            virtual void UnkFB8();
            virtual void UnkFBC();
            virtual void UnkFC0();
            virtual void UnkFC4();
            virtual void UnkFC8();
            virtual void UnkFCC();
            virtual void UnkFD0();
            virtual void UnkFD4();
            virtual void UnkFD8();
            virtual void UnkFDC();
            virtual void UnkFE0();
            virtual void UnkFE4();
            virtual void UnkFE8();
            virtual void UnkFEC();
            virtual void UnkFF0();
            virtual void UnkFF4();
            virtual void UnkFF8();
            virtual void UnkFFC();
            virtual void Unk1000();
            virtual void Unk1004();
            virtual void Unk1008();
            virtual void Unk100C();
            virtual void Unk1010();
            virtual void Unk1014();
            virtual void Unk1018();
            virtual void Unk101C();
            virtual void Unk1020();
            virtual void Unk1024();
            virtual void Unk1028();
            virtual void Unk102C();
            virtual void Unk1030();
            virtual void Unk1034();
            virtual void Unk1038();
            virtual void Unk103C();
            virtual void Unk1040();
            virtual void Unk1044();
            virtual void Unk1048();
            virtual void Unk104C();
            virtual void Unk1050();
            virtual void Unk1054();
            virtual void Unk1058();
            virtual void Unk105C();
            virtual void Unk1060();
            virtual void Unk1064();
            virtual void Unk1068();
            virtual void Unk106C();
            virtual void Unk1070();
            virtual void Unk1074();
            virtual void Unk1078();
            virtual void Unk107C();
            virtual void Unk1080();
            virtual void Unk1084();
            virtual void Unk1088();
            virtual void Unk108C();
            virtual void Unk1090();
            virtual void Unk1094();
            virtual void Unk1098();
            virtual void Unk109C();
            virtual void Unk10A0();
            virtual void Unk10A4();
            virtual void Unk10A8();
            virtual void Unk10AC();
            virtual void Unk10B0();
            virtual void Unk10B4();
            virtual void Unk10B8();
            virtual void Unk10BC();
            virtual void Unk10C0();
            virtual void Unk10C4();
            virtual void Unk10C8();
            virtual void Unk10CC();
            virtual void Unk10D0();
            virtual void Unk10D4();
            virtual void Unk10D8();
            virtual void Unk10DC();
            virtual void Unk10E0();
            virtual void Unk10E4();
            virtual void Unk10E8();
            virtual void Unk10EC();
            virtual void Unk10F0();
            virtual void Unk10F4();
            virtual void Unk10F8();
            virtual void Unk10FC();
            virtual void Unk1100();
            virtual void Unk1104();
            virtual void Unk1108();
            virtual void Unk110C();
            virtual void Unk1110();
            virtual void Unk1114();
            virtual void Unk1118();
            virtual void Unk111C();
            virtual void Unk1120();
            virtual void Unk1124();
            virtual void Unk1128();
            virtual void Unk112C();
            virtual void Unk1130();
            virtual void Unk1134();
            virtual void Unk1138();
            virtual void Unk113C();
            virtual void Unk1140();
            virtual void Unk1144();
            virtual void Unk1148();
            virtual void Unk114C();
            virtual void Unk1150();
            virtual void Unk1154();
            virtual void Unk1158();
            virtual void Unk115C();
            virtual void Unk1160();
            virtual void Unk1164();
            virtual void Unk1168();
            virtual void Unk116C();
            virtual void Unk1170();
            virtual void Unk1174();
            virtual void Unk1178();
            virtual void Unk117C();
            virtual void Unk1180();
            virtual void Unk1184();
            virtual void Unk1188();
            virtual void Unk118C();
            virtual void Unk1190();
            virtual void Unk1194();
            virtual void Unk1198();
            virtual void Unk119C();

        protected:
            unsigned char m_Unk574[0x930 - 0x574];
    };

    // Size 0x930.
    class CChrCoreDodgeMario : public CChrCoreDodge {
        public:
            CChrCoreDodgeMario();
            virtual ~CChrCoreDodgeMario();
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual CHR_CORE_DODGE::CC_PARAM_DODGE *NewCcParam_(long nHeap) const; // 0xCDC
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xD98
            virtual void SubStateArtsShotReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xD9C
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA0
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA8
    };

    // (only what Luigi's vacuum uses is mapped)
    struct CChrCoreDodge::STATE_WORK_AIR_TRICK_CMN {
        unsigned char m_Unk00[0xB0];
        bcs::FVEC3 m_TargetPos;        // 0xB0, where the trick turns to
        unsigned char m_UnkBC[0xFC - 0xBC];
        CChrCore::ROT_LERP1 m_RotLerp; // 0xFC, the turn (SubStateArtsShotCalcRotParam_)
        float m_fRotY;                 // 0x10C, the yaw the trick turns to
        unsigned char m_Unk110[0x124 - 0x110];
        long m_nArtsLevel;             // 0x124
    };

    // Size 0x968.
    class CChrCoreDodgeLuigi : public CChrCoreDodge {
        public:
            CChrCoreDodgeLuigi();
            virtual ~CChrCoreDodgeLuigi();
            virtual void Reset_();                                     // 0x384
            virtual void CtorStateAirTrick();                          // 0x584
            virtual void LoopStateAirTrick();                          // 0x588
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual CHR_CORE_DODGE::CC_PARAM_DODGE *NewCcParam_(long nHeap) const; // 0xCDC
            virtual void OnDivingCatchStart_(STATE_WORK_DIVING_CATCH *); // 0xD58
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xD98
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA0
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA8
            virtual void SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK_CMN *); // 0xDB0
            virtual void SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDC0
            virtual void SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK_CMN *); // 0xDC4
            virtual int CanbeSwitchLockTarget() const;                 // 0xE14

            // (name from its destructor; what it does is not known beyond
            // that) A model and an attached effect that the destructor
            // releases. Nothing in DodgeF creates them: the code that did was
            // most likely stripped by the link (its constants are still in
            // .rodata).
            class CIntakeAir {
                private:
                    bcs::CScnObj *m_pMdl;        // 0x00
                    long m_nUnk04;               // 0x04
                    long m_nUnk08;               // 0x08
                    unsigned char m_Unk0C[0x28 - 0x0C];
                    unsigned long m_hEffect;     // 0x28, CEffectCtrl attach handle

                public:
                    // (the vptr follows the members, at 0x2C)
                    CIntakeAir()
                    {
                        m_pMdl = 0;
                        m_nUnk04 = -1;
                        m_nUnk08 = 0;
                        m_hEffect = 0;
                    }
                    virtual ~CIntakeAir();
            };

        protected:
            // (inline helper, name guessed) Deletes the vacuum task, if any.
            void DeleteVacuum_();

            CIntakeAir m_IntakeAir;      // 0x930
            unsigned long m_hVacuum;     // 0x960, task id of the vacuum, 0 for none
            int m_bVacuum;               // 0x964, the vacuum pulls
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
