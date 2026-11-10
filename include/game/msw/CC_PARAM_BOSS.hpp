#ifndef MSW_CC_PARAM_BOSS_HPP_
#define MSW_CC_PARAM_BOSS_HPP_

namespace msw {

    // Parameter block of a boss-game character core (BossF). Every
    // character has a cc_data_boss_<Name>::CC_PARAM_BOSS_<Name> derived from
    // it, whose constructor overrides some of the values, and one class per
    // level (EASY ... EXPERT) derived from that, each with a _STAR class
    // derived from it. Field names are placeholders; types come from the
    // access widths in CC_PARAM_BOSS::CC_PARAM_BOSS(). The integer fields
    // only ever get 0, 1 or small counts, so their signedness is a guess.
    namespace CHR_CORE_BOSS {
        class CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS();

                unsigned long m_Unk00; // 0x0
                unsigned long m_Unk04; // 0x4
                unsigned long m_Unk08; // 0x8
                float m_Unk0C; // 0xC
                float m_Unk10; // 0x10
                float m_Unk14; // 0x14
                float m_Unk18; // 0x18
                float m_Unk1C; // 0x1C
                float m_Unk20; // 0x20
                float m_Unk24; // 0x24
                float m_Unk28; // 0x28
                float m_Unk2C; // 0x2C
                float m_Unk30; // 0x30
                float m_Unk34; // 0x34
                float m_Unk38; // 0x38
                float m_Unk3C; // 0x3C
                float m_Unk40; // 0x40
                float m_Unk44; // 0x44
                float m_Unk48; // 0x48
                float m_Unk4C; // 0x4C
                float m_Unk50; // 0x50
                float m_Unk54; // 0x54
                float m_Unk58; // 0x58
                float m_Unk5C; // 0x5C
                float m_Unk60; // 0x60
                float m_Unk64; // 0x64
                float m_Unk68; // 0x68
                float m_Unk6C; // 0x6C
                float m_Unk70; // 0x70
                float m_Unk74; // 0x74
                float m_Unk78; // 0x78
                float m_Unk7C; // 0x7C
                float m_Unk80; // 0x80
                float m_Unk84; // 0x84
                unsigned long m_Unk88; // 0x88
                unsigned long m_Unk8C; // 0x8C
                float m_Unk90; // 0x90
                float m_Unk94; // 0x94
                float m_Unk98; // 0x98
                float m_Unk9C; // 0x9C
                float m_UnkA0; // 0xA0
                float m_UnkA4; // 0xA4
                float m_UnkA8; // 0xA8
                float m_UnkAC; // 0xAC
                float m_UnkB0; // 0xB0
                float m_UnkB4; // 0xB4
                float m_UnkB8; // 0xB8
                float m_UnkBC; // 0xBC
                float m_UnkC0; // 0xC0
                float m_UnkC4; // 0xC4
                float m_UnkC8; // 0xC8
                float m_UnkCC; // 0xCC
                float m_UnkD0; // 0xD0
                float m_UnkD4; // 0xD4
                float m_UnkD8; // 0xD8
                float m_UnkDC; // 0xDC
                float m_UnkE0; // 0xE0
                float m_UnkE4; // 0xE4
                float m_UnkE8; // 0xE8
                float m_UnkEC; // 0xEC
                float m_UnkF0; // 0xF0
                float m_UnkF4; // 0xF4
                float m_UnkF8; // 0xF8
                float m_UnkFC; // 0xFC
                float m_Unk100; // 0x100
                float m_Unk104; // 0x104
                float m_Unk108; // 0x108
                float m_Unk10C; // 0x10C
                float m_Unk110; // 0x110
                float m_Unk114; // 0x114
                float m_Unk118; // 0x118
                float m_Unk11C; // 0x11C
                float m_Unk120; // 0x120
                float m_Unk124; // 0x124
                float m_Unk128; // 0x128
                float m_Unk12C; // 0x12C
                float m_Unk130; // 0x130
                float m_Unk134; // 0x134
                float m_Unk138; // 0x138
                float m_Unk13C; // 0x13C
                float m_Unk140; // 0x140
                float m_Unk144; // 0x144
                float m_Unk148; // 0x148
                float m_Unk14C; // 0x14C
                float m_Unk150; // 0x150
                float m_Unk154; // 0x154
                float m_Unk158; // 0x158
                float m_Unk15C; // 0x15C
                float m_Unk160; // 0x160
                float m_Unk164; // 0x164
                float m_Unk168; // 0x168
                float m_Unk16C; // 0x16C
                float m_Unk170; // 0x170
                unsigned long m_Unk174; // 0x174
                float m_Unk178; // 0x178
                float m_Unk17C; // 0x17C
                float m_Unk180; // 0x180
                float m_Unk184; // 0x184
                float m_Unk188; // 0x188
                float m_Unk18C; // 0x18C
                float m_Unk190; // 0x190
                float m_Unk194; // 0x194
                float m_Unk198; // 0x198
                float m_Unk19C; // 0x19C
                float m_Unk1A0; // 0x1A0
                float m_Unk1A4; // 0x1A4
                float m_Unk1A8; // 0x1A8
                float m_Unk1AC; // 0x1AC
                float m_Unk1B0; // 0x1B0
                unsigned long m_Unk1B4; // 0x1B4
                float m_Unk1B8; // 0x1B8
                float m_Unk1BC; // 0x1BC
                unsigned long m_Unk1C0; // 0x1C0
                float m_Unk1C4; // 0x1C4
                float m_Unk1C8; // 0x1C8
                float m_Unk1CC; // 0x1CC
                unsigned long m_Unk1D0; // 0x1D0
                unsigned long m_Unk1D4; // 0x1D4
                float m_Unk1D8; // 0x1D8
                unsigned long m_Unk1DC; // 0x1DC
                float m_Unk1E0; // 0x1E0
                float m_Unk1E4; // 0x1E4
                unsigned long m_Unk1E8; // 0x1E8
                unsigned long m_Unk1EC; // 0x1EC
                unsigned long m_Unk1F0; // 0x1F0
                float m_Unk1F4; // 0x1F4
                unsigned long m_Unk1F8; // 0x1F8
                unsigned long m_Unk1FC; // 0x1FC
                float m_Unk200; // 0x200
                float m_Unk204; // 0x204
                float m_Unk208; // 0x208
                float m_Unk20C; // 0x20C
                float m_Unk210; // 0x210
                float m_Unk214; // 0x214
                float m_Unk218; // 0x218
                float m_Unk21C; // 0x21C
                float m_Unk220; // 0x220
                float m_Unk224; // 0x224
                float m_Unk228; // 0x228
                float m_Unk22C; // 0x22C
                float m_Unk230; // 0x230
                unsigned long m_Unk234; // 0x234
        }; // size 0x238
    }

    // Default parameter block, constructed by the static initialiser at the
    // end of the parameter file.
    namespace cc_data_boss_default {
        extern CHR_CORE_BOSS::CC_PARAM_BOSS m_CcParam;
    }

    // One namespace per character: the character's own values, and one class
    // per game level, each with a _STAR variant for the star player. The
    // level classes add nothing in the boss mode. (__init2 is empty; what it
    // is for is not known.)
    namespace cc_data_boss_Mario {
        class CC_PARAM_BOSS_Mario : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Mario();
                void __init2();
        };
        class CC_PARAM_BOSS_Mario_EASY : public CC_PARAM_BOSS_Mario {
            public: CC_PARAM_BOSS_Mario_EASY();
        };
        class CC_PARAM_BOSS_Mario_EASY_STAR : public CC_PARAM_BOSS_Mario_EASY {
            public: CC_PARAM_BOSS_Mario_EASY_STAR();
        };
        class CC_PARAM_BOSS_Mario_NORMAL : public CC_PARAM_BOSS_Mario {
            public: CC_PARAM_BOSS_Mario_NORMAL();
        };
        class CC_PARAM_BOSS_Mario_NORMAL_STAR : public CC_PARAM_BOSS_Mario_NORMAL {
            public: CC_PARAM_BOSS_Mario_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Mario_HARD : public CC_PARAM_BOSS_Mario {
            public: CC_PARAM_BOSS_Mario_HARD();
        };
        class CC_PARAM_BOSS_Mario_HARD_STAR : public CC_PARAM_BOSS_Mario_HARD {
            public: CC_PARAM_BOSS_Mario_HARD_STAR();
        };
        class CC_PARAM_BOSS_Mario_EXPERT : public CC_PARAM_BOSS_Mario {
            public: CC_PARAM_BOSS_Mario_EXPERT();
        };
        class CC_PARAM_BOSS_Mario_EXPERT_STAR : public CC_PARAM_BOSS_Mario_EXPERT {
            public: CC_PARAM_BOSS_Mario_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Luigi {
        class CC_PARAM_BOSS_Luigi : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Luigi();
                void __init2();
        };
        class CC_PARAM_BOSS_Luigi_EASY : public CC_PARAM_BOSS_Luigi {
            public: CC_PARAM_BOSS_Luigi_EASY();
        };
        class CC_PARAM_BOSS_Luigi_EASY_STAR : public CC_PARAM_BOSS_Luigi_EASY {
            public: CC_PARAM_BOSS_Luigi_EASY_STAR();
        };
        class CC_PARAM_BOSS_Luigi_NORMAL : public CC_PARAM_BOSS_Luigi {
            public: CC_PARAM_BOSS_Luigi_NORMAL();
        };
        class CC_PARAM_BOSS_Luigi_NORMAL_STAR : public CC_PARAM_BOSS_Luigi_NORMAL {
            public: CC_PARAM_BOSS_Luigi_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Luigi_HARD : public CC_PARAM_BOSS_Luigi {
            public: CC_PARAM_BOSS_Luigi_HARD();
        };
        class CC_PARAM_BOSS_Luigi_HARD_STAR : public CC_PARAM_BOSS_Luigi_HARD {
            public: CC_PARAM_BOSS_Luigi_HARD_STAR();
        };
        class CC_PARAM_BOSS_Luigi_EXPERT : public CC_PARAM_BOSS_Luigi {
            public: CC_PARAM_BOSS_Luigi_EXPERT();
        };
        class CC_PARAM_BOSS_Luigi_EXPERT_STAR : public CC_PARAM_BOSS_Luigi_EXPERT {
            public: CC_PARAM_BOSS_Luigi_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Peach {
        class CC_PARAM_BOSS_Peach : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Peach();
                void __init2();
        };
        class CC_PARAM_BOSS_Peach_EASY : public CC_PARAM_BOSS_Peach {
            public: CC_PARAM_BOSS_Peach_EASY();
        };
        class CC_PARAM_BOSS_Peach_EASY_STAR : public CC_PARAM_BOSS_Peach_EASY {
            public: CC_PARAM_BOSS_Peach_EASY_STAR();
        };
        class CC_PARAM_BOSS_Peach_NORMAL : public CC_PARAM_BOSS_Peach {
            public: CC_PARAM_BOSS_Peach_NORMAL();
        };
        class CC_PARAM_BOSS_Peach_NORMAL_STAR : public CC_PARAM_BOSS_Peach_NORMAL {
            public: CC_PARAM_BOSS_Peach_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Peach_HARD : public CC_PARAM_BOSS_Peach {
            public: CC_PARAM_BOSS_Peach_HARD();
        };
        class CC_PARAM_BOSS_Peach_HARD_STAR : public CC_PARAM_BOSS_Peach_HARD {
            public: CC_PARAM_BOSS_Peach_HARD_STAR();
        };
        class CC_PARAM_BOSS_Peach_EXPERT : public CC_PARAM_BOSS_Peach {
            public: CC_PARAM_BOSS_Peach_EXPERT();
        };
        class CC_PARAM_BOSS_Peach_EXPERT_STAR : public CC_PARAM_BOSS_Peach_EXPERT {
            public: CC_PARAM_BOSS_Peach_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Daisy {
        class CC_PARAM_BOSS_Daisy : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Daisy();
                void __init2();
        };
        class CC_PARAM_BOSS_Daisy_EASY : public CC_PARAM_BOSS_Daisy {
            public: CC_PARAM_BOSS_Daisy_EASY();
        };
        class CC_PARAM_BOSS_Daisy_EASY_STAR : public CC_PARAM_BOSS_Daisy_EASY {
            public: CC_PARAM_BOSS_Daisy_EASY_STAR();
        };
        class CC_PARAM_BOSS_Daisy_NORMAL : public CC_PARAM_BOSS_Daisy {
            public: CC_PARAM_BOSS_Daisy_NORMAL();
        };
        class CC_PARAM_BOSS_Daisy_NORMAL_STAR : public CC_PARAM_BOSS_Daisy_NORMAL {
            public: CC_PARAM_BOSS_Daisy_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Daisy_HARD : public CC_PARAM_BOSS_Daisy {
            public: CC_PARAM_BOSS_Daisy_HARD();
        };
        class CC_PARAM_BOSS_Daisy_HARD_STAR : public CC_PARAM_BOSS_Daisy_HARD {
            public: CC_PARAM_BOSS_Daisy_HARD_STAR();
        };
        class CC_PARAM_BOSS_Daisy_EXPERT : public CC_PARAM_BOSS_Daisy {
            public: CC_PARAM_BOSS_Daisy_EXPERT();
        };
        class CC_PARAM_BOSS_Daisy_EXPERT_STAR : public CC_PARAM_BOSS_Daisy_EXPERT {
            public: CC_PARAM_BOSS_Daisy_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Yoshi {
        class CC_PARAM_BOSS_Yoshi : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Yoshi();
                void __init2();
        };
        class CC_PARAM_BOSS_Yoshi_EASY : public CC_PARAM_BOSS_Yoshi {
            public: CC_PARAM_BOSS_Yoshi_EASY();
        };
        class CC_PARAM_BOSS_Yoshi_EASY_STAR : public CC_PARAM_BOSS_Yoshi_EASY {
            public: CC_PARAM_BOSS_Yoshi_EASY_STAR();
        };
        class CC_PARAM_BOSS_Yoshi_NORMAL : public CC_PARAM_BOSS_Yoshi {
            public: CC_PARAM_BOSS_Yoshi_NORMAL();
        };
        class CC_PARAM_BOSS_Yoshi_NORMAL_STAR : public CC_PARAM_BOSS_Yoshi_NORMAL {
            public: CC_PARAM_BOSS_Yoshi_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Yoshi_HARD : public CC_PARAM_BOSS_Yoshi {
            public: CC_PARAM_BOSS_Yoshi_HARD();
        };
        class CC_PARAM_BOSS_Yoshi_HARD_STAR : public CC_PARAM_BOSS_Yoshi_HARD {
            public: CC_PARAM_BOSS_Yoshi_HARD_STAR();
        };
        class CC_PARAM_BOSS_Yoshi_EXPERT : public CC_PARAM_BOSS_Yoshi {
            public: CC_PARAM_BOSS_Yoshi_EXPERT();
        };
        class CC_PARAM_BOSS_Yoshi_EXPERT_STAR : public CC_PARAM_BOSS_Yoshi_EXPERT {
            public: CC_PARAM_BOSS_Yoshi_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Wario {
        class CC_PARAM_BOSS_Wario : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Wario();
                void __init2();
        };
        class CC_PARAM_BOSS_Wario_EASY : public CC_PARAM_BOSS_Wario {
            public: CC_PARAM_BOSS_Wario_EASY();
        };
        class CC_PARAM_BOSS_Wario_EASY_STAR : public CC_PARAM_BOSS_Wario_EASY {
            public: CC_PARAM_BOSS_Wario_EASY_STAR();
        };
        class CC_PARAM_BOSS_Wario_NORMAL : public CC_PARAM_BOSS_Wario {
            public: CC_PARAM_BOSS_Wario_NORMAL();
        };
        class CC_PARAM_BOSS_Wario_NORMAL_STAR : public CC_PARAM_BOSS_Wario_NORMAL {
            public: CC_PARAM_BOSS_Wario_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Wario_HARD : public CC_PARAM_BOSS_Wario {
            public: CC_PARAM_BOSS_Wario_HARD();
        };
        class CC_PARAM_BOSS_Wario_HARD_STAR : public CC_PARAM_BOSS_Wario_HARD {
            public: CC_PARAM_BOSS_Wario_HARD_STAR();
        };
        class CC_PARAM_BOSS_Wario_EXPERT : public CC_PARAM_BOSS_Wario {
            public: CC_PARAM_BOSS_Wario_EXPERT();
        };
        class CC_PARAM_BOSS_Wario_EXPERT_STAR : public CC_PARAM_BOSS_Wario_EXPERT {
            public: CC_PARAM_BOSS_Wario_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Waluigi {
        class CC_PARAM_BOSS_Waluigi : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Waluigi();
                void __init2();
        };
        class CC_PARAM_BOSS_Waluigi_EASY : public CC_PARAM_BOSS_Waluigi {
            public: CC_PARAM_BOSS_Waluigi_EASY();
        };
        class CC_PARAM_BOSS_Waluigi_EASY_STAR : public CC_PARAM_BOSS_Waluigi_EASY {
            public: CC_PARAM_BOSS_Waluigi_EASY_STAR();
        };
        class CC_PARAM_BOSS_Waluigi_NORMAL : public CC_PARAM_BOSS_Waluigi {
            public: CC_PARAM_BOSS_Waluigi_NORMAL();
        };
        class CC_PARAM_BOSS_Waluigi_NORMAL_STAR : public CC_PARAM_BOSS_Waluigi_NORMAL {
            public: CC_PARAM_BOSS_Waluigi_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Waluigi_HARD : public CC_PARAM_BOSS_Waluigi {
            public: CC_PARAM_BOSS_Waluigi_HARD();
        };
        class CC_PARAM_BOSS_Waluigi_HARD_STAR : public CC_PARAM_BOSS_Waluigi_HARD {
            public: CC_PARAM_BOSS_Waluigi_HARD_STAR();
        };
        class CC_PARAM_BOSS_Waluigi_EXPERT : public CC_PARAM_BOSS_Waluigi {
            public: CC_PARAM_BOSS_Waluigi_EXPERT();
        };
        class CC_PARAM_BOSS_Waluigi_EXPERT_STAR : public CC_PARAM_BOSS_Waluigi_EXPERT {
            public: CC_PARAM_BOSS_Waluigi_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Donkey {
        class CC_PARAM_BOSS_Donkey : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Donkey();
                void __init2();
        };
        class CC_PARAM_BOSS_Donkey_EASY : public CC_PARAM_BOSS_Donkey {
            public: CC_PARAM_BOSS_Donkey_EASY();
        };
        class CC_PARAM_BOSS_Donkey_EASY_STAR : public CC_PARAM_BOSS_Donkey_EASY {
            public: CC_PARAM_BOSS_Donkey_EASY_STAR();
        };
        class CC_PARAM_BOSS_Donkey_NORMAL : public CC_PARAM_BOSS_Donkey {
            public: CC_PARAM_BOSS_Donkey_NORMAL();
        };
        class CC_PARAM_BOSS_Donkey_NORMAL_STAR : public CC_PARAM_BOSS_Donkey_NORMAL {
            public: CC_PARAM_BOSS_Donkey_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Donkey_HARD : public CC_PARAM_BOSS_Donkey {
            public: CC_PARAM_BOSS_Donkey_HARD();
        };
        class CC_PARAM_BOSS_Donkey_HARD_STAR : public CC_PARAM_BOSS_Donkey_HARD {
            public: CC_PARAM_BOSS_Donkey_HARD_STAR();
        };
        class CC_PARAM_BOSS_Donkey_EXPERT : public CC_PARAM_BOSS_Donkey {
            public: CC_PARAM_BOSS_Donkey_EXPERT();
        };
        class CC_PARAM_BOSS_Donkey_EXPERT_STAR : public CC_PARAM_BOSS_Donkey_EXPERT {
            public: CC_PARAM_BOSS_Donkey_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Diddy {
        class CC_PARAM_BOSS_Diddy : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Diddy();
                void __init2();
        };
        class CC_PARAM_BOSS_Diddy_EASY : public CC_PARAM_BOSS_Diddy {
            public: CC_PARAM_BOSS_Diddy_EASY();
        };
        class CC_PARAM_BOSS_Diddy_EASY_STAR : public CC_PARAM_BOSS_Diddy_EASY {
            public: CC_PARAM_BOSS_Diddy_EASY_STAR();
        };
        class CC_PARAM_BOSS_Diddy_NORMAL : public CC_PARAM_BOSS_Diddy {
            public: CC_PARAM_BOSS_Diddy_NORMAL();
        };
        class CC_PARAM_BOSS_Diddy_NORMAL_STAR : public CC_PARAM_BOSS_Diddy_NORMAL {
            public: CC_PARAM_BOSS_Diddy_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Diddy_HARD : public CC_PARAM_BOSS_Diddy {
            public: CC_PARAM_BOSS_Diddy_HARD();
        };
        class CC_PARAM_BOSS_Diddy_HARD_STAR : public CC_PARAM_BOSS_Diddy_HARD {
            public: CC_PARAM_BOSS_Diddy_HARD_STAR();
        };
        class CC_PARAM_BOSS_Diddy_EXPERT : public CC_PARAM_BOSS_Diddy {
            public: CC_PARAM_BOSS_Diddy_EXPERT();
        };
        class CC_PARAM_BOSS_Diddy_EXPERT_STAR : public CC_PARAM_BOSS_Diddy_EXPERT {
            public: CC_PARAM_BOSS_Diddy_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Kinopio {
        class CC_PARAM_BOSS_Kinopio : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Kinopio();
                void __init2();
        };
        class CC_PARAM_BOSS_Kinopio_EASY : public CC_PARAM_BOSS_Kinopio {
            public: CC_PARAM_BOSS_Kinopio_EASY();
        };
        class CC_PARAM_BOSS_Kinopio_EASY_STAR : public CC_PARAM_BOSS_Kinopio_EASY {
            public: CC_PARAM_BOSS_Kinopio_EASY_STAR();
        };
        class CC_PARAM_BOSS_Kinopio_NORMAL : public CC_PARAM_BOSS_Kinopio {
            public: CC_PARAM_BOSS_Kinopio_NORMAL();
        };
        class CC_PARAM_BOSS_Kinopio_NORMAL_STAR : public CC_PARAM_BOSS_Kinopio_NORMAL {
            public: CC_PARAM_BOSS_Kinopio_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Kinopio_HARD : public CC_PARAM_BOSS_Kinopio {
            public: CC_PARAM_BOSS_Kinopio_HARD();
        };
        class CC_PARAM_BOSS_Kinopio_HARD_STAR : public CC_PARAM_BOSS_Kinopio_HARD {
            public: CC_PARAM_BOSS_Kinopio_HARD_STAR();
        };
        class CC_PARAM_BOSS_Kinopio_EXPERT : public CC_PARAM_BOSS_Kinopio {
            public: CC_PARAM_BOSS_Kinopio_EXPERT();
        };
        class CC_PARAM_BOSS_Kinopio_EXPERT_STAR : public CC_PARAM_BOSS_Kinopio_EXPERT {
            public: CC_PARAM_BOSS_Kinopio_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Koopa {
        class CC_PARAM_BOSS_Koopa : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Koopa();
                void __init2();
        };
        class CC_PARAM_BOSS_Koopa_EASY : public CC_PARAM_BOSS_Koopa {
            public: CC_PARAM_BOSS_Koopa_EASY();
        };
        class CC_PARAM_BOSS_Koopa_EASY_STAR : public CC_PARAM_BOSS_Koopa_EASY {
            public: CC_PARAM_BOSS_Koopa_EASY_STAR();
        };
        class CC_PARAM_BOSS_Koopa_NORMAL : public CC_PARAM_BOSS_Koopa {
            public: CC_PARAM_BOSS_Koopa_NORMAL();
        };
        class CC_PARAM_BOSS_Koopa_NORMAL_STAR : public CC_PARAM_BOSS_Koopa_NORMAL {
            public: CC_PARAM_BOSS_Koopa_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Koopa_HARD : public CC_PARAM_BOSS_Koopa {
            public: CC_PARAM_BOSS_Koopa_HARD();
        };
        class CC_PARAM_BOSS_Koopa_HARD_STAR : public CC_PARAM_BOSS_Koopa_HARD {
            public: CC_PARAM_BOSS_Koopa_HARD_STAR();
        };
        class CC_PARAM_BOSS_Koopa_EXPERT : public CC_PARAM_BOSS_Koopa {
            public: CC_PARAM_BOSS_Koopa_EXPERT();
        };
        class CC_PARAM_BOSS_Koopa_EXPERT_STAR : public CC_PARAM_BOSS_Koopa_EXPERT {
            public: CC_PARAM_BOSS_Koopa_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_KoopaJr {
        class CC_PARAM_BOSS_KoopaJr : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_KoopaJr();
                void __init2();
        };
        class CC_PARAM_BOSS_KoopaJr_EASY : public CC_PARAM_BOSS_KoopaJr {
            public: CC_PARAM_BOSS_KoopaJr_EASY();
        };
        class CC_PARAM_BOSS_KoopaJr_EASY_STAR : public CC_PARAM_BOSS_KoopaJr_EASY {
            public: CC_PARAM_BOSS_KoopaJr_EASY_STAR();
        };
        class CC_PARAM_BOSS_KoopaJr_NORMAL : public CC_PARAM_BOSS_KoopaJr {
            public: CC_PARAM_BOSS_KoopaJr_NORMAL();
        };
        class CC_PARAM_BOSS_KoopaJr_NORMAL_STAR : public CC_PARAM_BOSS_KoopaJr_NORMAL {
            public: CC_PARAM_BOSS_KoopaJr_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_KoopaJr_HARD : public CC_PARAM_BOSS_KoopaJr {
            public: CC_PARAM_BOSS_KoopaJr_HARD();
        };
        class CC_PARAM_BOSS_KoopaJr_HARD_STAR : public CC_PARAM_BOSS_KoopaJr_HARD {
            public: CC_PARAM_BOSS_KoopaJr_HARD_STAR();
        };
        class CC_PARAM_BOSS_KoopaJr_EXPERT : public CC_PARAM_BOSS_KoopaJr {
            public: CC_PARAM_BOSS_KoopaJr_EXPERT();
        };
        class CC_PARAM_BOSS_KoopaJr_EXPERT_STAR : public CC_PARAM_BOSS_KoopaJr_EXPERT {
            public: CC_PARAM_BOSS_KoopaJr_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Moogle {
        class CC_PARAM_BOSS_Moogle : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Moogle();
                void __init2();
        };
        class CC_PARAM_BOSS_Moogle_EASY : public CC_PARAM_BOSS_Moogle {
            public: CC_PARAM_BOSS_Moogle_EASY();
        };
        class CC_PARAM_BOSS_Moogle_EASY_STAR : public CC_PARAM_BOSS_Moogle_EASY {
            public: CC_PARAM_BOSS_Moogle_EASY_STAR();
        };
        class CC_PARAM_BOSS_Moogle_NORMAL : public CC_PARAM_BOSS_Moogle {
            public: CC_PARAM_BOSS_Moogle_NORMAL();
        };
        class CC_PARAM_BOSS_Moogle_NORMAL_STAR : public CC_PARAM_BOSS_Moogle_NORMAL {
            public: CC_PARAM_BOSS_Moogle_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Moogle_HARD : public CC_PARAM_BOSS_Moogle {
            public: CC_PARAM_BOSS_Moogle_HARD();
        };
        class CC_PARAM_BOSS_Moogle_HARD_STAR : public CC_PARAM_BOSS_Moogle_HARD {
            public: CC_PARAM_BOSS_Moogle_HARD_STAR();
        };
        class CC_PARAM_BOSS_Moogle_EXPERT : public CC_PARAM_BOSS_Moogle {
            public: CC_PARAM_BOSS_Moogle_EXPERT();
        };
        class CC_PARAM_BOSS_Moogle_EXPERT_STAR : public CC_PARAM_BOSS_Moogle_EXPERT {
            public: CC_PARAM_BOSS_Moogle_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Saboten {
        class CC_PARAM_BOSS_Saboten : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Saboten();
                void __init2();
        };
        class CC_PARAM_BOSS_Saboten_EASY : public CC_PARAM_BOSS_Saboten {
            public: CC_PARAM_BOSS_Saboten_EASY();
        };
        class CC_PARAM_BOSS_Saboten_EASY_STAR : public CC_PARAM_BOSS_Saboten_EASY {
            public: CC_PARAM_BOSS_Saboten_EASY_STAR();
        };
        class CC_PARAM_BOSS_Saboten_NORMAL : public CC_PARAM_BOSS_Saboten {
            public: CC_PARAM_BOSS_Saboten_NORMAL();
        };
        class CC_PARAM_BOSS_Saboten_NORMAL_STAR : public CC_PARAM_BOSS_Saboten_NORMAL {
            public: CC_PARAM_BOSS_Saboten_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Saboten_HARD : public CC_PARAM_BOSS_Saboten {
            public: CC_PARAM_BOSS_Saboten_HARD();
        };
        class CC_PARAM_BOSS_Saboten_HARD_STAR : public CC_PARAM_BOSS_Saboten_HARD {
            public: CC_PARAM_BOSS_Saboten_HARD_STAR();
        };
        class CC_PARAM_BOSS_Saboten_EXPERT : public CC_PARAM_BOSS_Saboten {
            public: CC_PARAM_BOSS_Saboten_EXPERT();
        };
        class CC_PARAM_BOSS_Saboten_EXPERT_STAR : public CC_PARAM_BOSS_Saboten_EXPERT {
            public: CC_PARAM_BOSS_Saboten_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Ninja {
        class CC_PARAM_BOSS_Ninja : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Ninja();
                void __init2();
        };
        class CC_PARAM_BOSS_Ninja_EASY : public CC_PARAM_BOSS_Ninja {
            public: CC_PARAM_BOSS_Ninja_EASY();
        };
        class CC_PARAM_BOSS_Ninja_EASY_STAR : public CC_PARAM_BOSS_Ninja_EASY {
            public: CC_PARAM_BOSS_Ninja_EASY_STAR();
        };
        class CC_PARAM_BOSS_Ninja_NORMAL : public CC_PARAM_BOSS_Ninja {
            public: CC_PARAM_BOSS_Ninja_NORMAL();
        };
        class CC_PARAM_BOSS_Ninja_NORMAL_STAR : public CC_PARAM_BOSS_Ninja_NORMAL {
            public: CC_PARAM_BOSS_Ninja_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Ninja_HARD : public CC_PARAM_BOSS_Ninja {
            public: CC_PARAM_BOSS_Ninja_HARD();
        };
        class CC_PARAM_BOSS_Ninja_HARD_STAR : public CC_PARAM_BOSS_Ninja_HARD {
            public: CC_PARAM_BOSS_Ninja_HARD_STAR();
        };
        class CC_PARAM_BOSS_Ninja_EXPERT : public CC_PARAM_BOSS_Ninja {
            public: CC_PARAM_BOSS_Ninja_EXPERT();
        };
        class CC_PARAM_BOSS_Ninja_EXPERT_STAR : public CC_PARAM_BOSS_Ninja_EXPERT {
            public: CC_PARAM_BOSS_Ninja_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Siroma {
        class CC_PARAM_BOSS_Siroma : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Siroma();
                void __init2();
        };
        class CC_PARAM_BOSS_Siroma_EASY : public CC_PARAM_BOSS_Siroma {
            public: CC_PARAM_BOSS_Siroma_EASY();
        };
        class CC_PARAM_BOSS_Siroma_EASY_STAR : public CC_PARAM_BOSS_Siroma_EASY {
            public: CC_PARAM_BOSS_Siroma_EASY_STAR();
        };
        class CC_PARAM_BOSS_Siroma_NORMAL : public CC_PARAM_BOSS_Siroma {
            public: CC_PARAM_BOSS_Siroma_NORMAL();
        };
        class CC_PARAM_BOSS_Siroma_NORMAL_STAR : public CC_PARAM_BOSS_Siroma_NORMAL {
            public: CC_PARAM_BOSS_Siroma_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Siroma_HARD : public CC_PARAM_BOSS_Siroma {
            public: CC_PARAM_BOSS_Siroma_HARD();
        };
        class CC_PARAM_BOSS_Siroma_HARD_STAR : public CC_PARAM_BOSS_Siroma_HARD {
            public: CC_PARAM_BOSS_Siroma_HARD_STAR();
        };
        class CC_PARAM_BOSS_Siroma_EXPERT : public CC_PARAM_BOSS_Siroma {
            public: CC_PARAM_BOSS_Siroma_EXPERT();
        };
        class CC_PARAM_BOSS_Siroma_EXPERT_STAR : public CC_PARAM_BOSS_Siroma_EXPERT {
            public: CC_PARAM_BOSS_Siroma_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Slime {
        class CC_PARAM_BOSS_Slime : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Slime();
                void __init2();
        };
        class CC_PARAM_BOSS_Slime_EASY : public CC_PARAM_BOSS_Slime {
            public: CC_PARAM_BOSS_Slime_EASY();
        };
        class CC_PARAM_BOSS_Slime_EASY_STAR : public CC_PARAM_BOSS_Slime_EASY {
            public: CC_PARAM_BOSS_Slime_EASY_STAR();
        };
        class CC_PARAM_BOSS_Slime_NORMAL : public CC_PARAM_BOSS_Slime {
            public: CC_PARAM_BOSS_Slime_NORMAL();
        };
        class CC_PARAM_BOSS_Slime_NORMAL_STAR : public CC_PARAM_BOSS_Slime_NORMAL {
            public: CC_PARAM_BOSS_Slime_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Slime_HARD : public CC_PARAM_BOSS_Slime {
            public: CC_PARAM_BOSS_Slime_HARD();
        };
        class CC_PARAM_BOSS_Slime_HARD_STAR : public CC_PARAM_BOSS_Slime_HARD {
            public: CC_PARAM_BOSS_Slime_HARD_STAR();
        };
        class CC_PARAM_BOSS_Slime_EXPERT : public CC_PARAM_BOSS_Slime {
            public: CC_PARAM_BOSS_Slime_EXPERT();
        };
        class CC_PARAM_BOSS_Slime_EXPERT_STAR : public CC_PARAM_BOSS_Slime_EXPERT {
            public: CC_PARAM_BOSS_Slime_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Kuroma {
        class CC_PARAM_BOSS_Kuroma : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Kuroma();
                void __init2();
        };
        class CC_PARAM_BOSS_Kuroma_EASY : public CC_PARAM_BOSS_Kuroma {
            public: CC_PARAM_BOSS_Kuroma_EASY();
        };
        class CC_PARAM_BOSS_Kuroma_EASY_STAR : public CC_PARAM_BOSS_Kuroma_EASY {
            public: CC_PARAM_BOSS_Kuroma_EASY_STAR();
        };
        class CC_PARAM_BOSS_Kuroma_NORMAL : public CC_PARAM_BOSS_Kuroma {
            public: CC_PARAM_BOSS_Kuroma_NORMAL();
        };
        class CC_PARAM_BOSS_Kuroma_NORMAL_STAR : public CC_PARAM_BOSS_Kuroma_NORMAL {
            public: CC_PARAM_BOSS_Kuroma_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Kuroma_HARD : public CC_PARAM_BOSS_Kuroma {
            public: CC_PARAM_BOSS_Kuroma_HARD();
        };
        class CC_PARAM_BOSS_Kuroma_HARD_STAR : public CC_PARAM_BOSS_Kuroma_HARD {
            public: CC_PARAM_BOSS_Kuroma_HARD_STAR();
        };
        class CC_PARAM_BOSS_Kuroma_EXPERT : public CC_PARAM_BOSS_Kuroma {
            public: CC_PARAM_BOSS_Kuroma_EXPERT();
        };
        class CC_PARAM_BOSS_Kuroma_EXPERT_STAR : public CC_PARAM_BOSS_Kuroma_EXPERT {
            public: CC_PARAM_BOSS_Kuroma_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Mii00 {
        class CC_PARAM_BOSS_Mii00 : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Mii00();
                void __init2();
        };
        class CC_PARAM_BOSS_Mii00_EASY : public CC_PARAM_BOSS_Mii00 {
            public: CC_PARAM_BOSS_Mii00_EASY();
        };
        class CC_PARAM_BOSS_Mii00_EASY_STAR : public CC_PARAM_BOSS_Mii00_EASY {
            public: CC_PARAM_BOSS_Mii00_EASY_STAR();
        };
        class CC_PARAM_BOSS_Mii00_NORMAL : public CC_PARAM_BOSS_Mii00 {
            public: CC_PARAM_BOSS_Mii00_NORMAL();
        };
        class CC_PARAM_BOSS_Mii00_NORMAL_STAR : public CC_PARAM_BOSS_Mii00_NORMAL {
            public: CC_PARAM_BOSS_Mii00_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Mii00_HARD : public CC_PARAM_BOSS_Mii00 {
            public: CC_PARAM_BOSS_Mii00_HARD();
        };
        class CC_PARAM_BOSS_Mii00_HARD_STAR : public CC_PARAM_BOSS_Mii00_HARD {
            public: CC_PARAM_BOSS_Mii00_HARD_STAR();
        };
        class CC_PARAM_BOSS_Mii00_EXPERT : public CC_PARAM_BOSS_Mii00 {
            public: CC_PARAM_BOSS_Mii00_EXPERT();
        };
        class CC_PARAM_BOSS_Mii00_EXPERT_STAR : public CC_PARAM_BOSS_Mii00_EXPERT {
            public: CC_PARAM_BOSS_Mii00_EXPERT_STAR();
        };
    }

    namespace cc_data_boss_Mii01 {
        class CC_PARAM_BOSS_Mii01 : public CHR_CORE_BOSS::CC_PARAM_BOSS {
            public:
                CC_PARAM_BOSS_Mii01();
                void __init2();
        };
        class CC_PARAM_BOSS_Mii01_EASY : public CC_PARAM_BOSS_Mii01 {
            public: CC_PARAM_BOSS_Mii01_EASY();
        };
        class CC_PARAM_BOSS_Mii01_EASY_STAR : public CC_PARAM_BOSS_Mii01_EASY {
            public: CC_PARAM_BOSS_Mii01_EASY_STAR();
        };
        class CC_PARAM_BOSS_Mii01_NORMAL : public CC_PARAM_BOSS_Mii01 {
            public: CC_PARAM_BOSS_Mii01_NORMAL();
        };
        class CC_PARAM_BOSS_Mii01_NORMAL_STAR : public CC_PARAM_BOSS_Mii01_NORMAL {
            public: CC_PARAM_BOSS_Mii01_NORMAL_STAR();
        };
        class CC_PARAM_BOSS_Mii01_HARD : public CC_PARAM_BOSS_Mii01 {
            public: CC_PARAM_BOSS_Mii01_HARD();
        };
        class CC_PARAM_BOSS_Mii01_HARD_STAR : public CC_PARAM_BOSS_Mii01_HARD {
            public: CC_PARAM_BOSS_Mii01_HARD_STAR();
        };
        class CC_PARAM_BOSS_Mii01_EXPERT : public CC_PARAM_BOSS_Mii01 {
            public: CC_PARAM_BOSS_Mii01_EXPERT();
        };
        class CC_PARAM_BOSS_Mii01_EXPERT_STAR : public CC_PARAM_BOSS_Mii01_EXPERT {
            public: CC_PARAM_BOSS_Mii01_EXPERT_STAR();
        };
    }
}

#endif /* !MSW_CC_PARAM_BOSS_HPP_ */
