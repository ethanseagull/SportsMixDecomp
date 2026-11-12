#ifndef MSW_CC_PARAM_VOLLEY_HPP_
#define MSW_CC_PARAM_VOLLEY_HPP_

namespace msw {

    // Parameter block of a volleyball character core (VolleyF), the
    // counterpart of CHR_CORE_BOSS::CC_PARAM_BOSS. Every character has a
    // CC_PARAM_VOLLEY_<Name> derived from it, with one class per level.
    //
    // Field names are placeholders; types come from the access widths in
    // CC_PARAM_VOLLEY::CC_PARAM_VOLLEY() (the last field it writes is at
    // 0x3E0, so the size is taken to be 0x3E4; not checked against an
    // allocation). m_Unk04 is passed to CBall::GetVolleyReceiveReaction.
    namespace CHR_CORE_VOLLEY {
        class CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY();

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
                float m_Unk88; // 0x88
                float m_Unk8C; // 0x8C
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
                unsigned long m_UnkFC; // 0xFC
                unsigned long m_Unk100; // 0x100
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
                float m_Unk174; // 0x174
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
                float m_Unk1B4; // 0x1B4
                float m_Unk1B8; // 0x1B8
                float m_Unk1BC; // 0x1BC
                float m_Unk1C0; // 0x1C0
                float m_Unk1C4; // 0x1C4
                float m_Unk1C8; // 0x1C8
                float m_Unk1CC; // 0x1CC
                float m_Unk1D0; // 0x1D0
                float m_Unk1D4; // 0x1D4
                float m_Unk1D8; // 0x1D8
                float m_Unk1DC; // 0x1DC
                float m_Unk1E0; // 0x1E0
                float m_Unk1E4; // 0x1E4
                float m_Unk1E8; // 0x1E8
                float m_Unk1EC; // 0x1EC
                float m_Unk1F0; // 0x1F0
                float m_Unk1F4; // 0x1F4
                float m_Unk1F8; // 0x1F8
                float m_Unk1FC; // 0x1FC
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
                float m_Unk234; // 0x234
                float m_Unk238; // 0x238
                float m_Unk23C; // 0x23C
                float m_Unk240; // 0x240
                float m_Unk244; // 0x244
                float m_Unk248; // 0x248
                float m_Unk24C; // 0x24C
                float m_Unk250; // 0x250
                float m_Unk254; // 0x254
                float m_Unk258; // 0x258
                float m_Unk25C; // 0x25C
                float m_Unk260; // 0x260
                float m_Unk264; // 0x264
                float m_Unk268; // 0x268
                float m_Unk26C; // 0x26C
                float m_Unk270; // 0x270
                float m_Unk274; // 0x274
                float m_Unk278; // 0x278
                float m_Unk27C; // 0x27C
                float m_Unk280; // 0x280
                float m_Unk284; // 0x284
                float m_Unk288; // 0x288
                float m_Unk28C; // 0x28C
                float m_Unk290; // 0x290
                float m_Unk294; // 0x294
                float m_Unk298; // 0x298
                float m_Unk29C; // 0x29C
                float m_Unk2A0; // 0x2A0
                float m_Unk2A4; // 0x2A4
                float m_Unk2A8; // 0x2A8
                float m_Unk2AC; // 0x2AC
                float m_Unk2B0; // 0x2B0
                float m_Unk2B4; // 0x2B4
                unsigned long m_Unk2B8; // 0x2B8
                float m_Unk2BC; // 0x2BC
                float m_Unk2C0; // 0x2C0
                float m_Unk2C4; // 0x2C4
                float m_Unk2C8; // 0x2C8
                float m_Unk2CC; // 0x2CC
                float m_Unk2D0; // 0x2D0
                float m_Unk2D4; // 0x2D4
                float m_Unk2D8; // 0x2D8
                unsigned long m_Unk2DC; // 0x2DC
                float m_Unk2E0; // 0x2E0
                float m_Unk2E4; // 0x2E4
                float m_Unk2E8; // 0x2E8
                unsigned long m_Unk2EC; // 0x2EC
                float m_Unk2F0; // 0x2F0
                float m_Unk2F4; // 0x2F4
                float m_Unk2F8; // 0x2F8
                float m_Unk2FC; // 0x2FC
                float m_Unk300; // 0x300
                float m_Unk304; // 0x304
                float m_Unk308; // 0x308
                float m_Unk30C; // 0x30C
                float m_Unk310; // 0x310
                float m_Unk314; // 0x314
                float m_Unk318; // 0x318
                float m_Unk31C; // 0x31C
                float m_Unk320; // 0x320
                float m_Unk324; // 0x324
                float m_Unk328; // 0x328
                float m_Unk32C; // 0x32C
                float m_Unk330; // 0x330
                float m_Unk334; // 0x334
                float m_Unk338; // 0x338
                float m_Unk33C; // 0x33C
                float m_Unk340; // 0x340
                float m_Unk344; // 0x344
                float m_Unk348; // 0x348
                float m_Unk34C; // 0x34C
                float m_Unk350; // 0x350
                float m_Unk354; // 0x354
                float m_Unk358; // 0x358
                float m_Unk35C; // 0x35C
                float m_Unk360; // 0x360
                float m_Unk364; // 0x364
                float m_Unk368; // 0x368
                float m_Unk36C; // 0x36C
                float m_Unk370; // 0x370
                float m_Unk374; // 0x374
                float m_Unk378; // 0x378
                float m_Unk37C; // 0x37C
                float m_Unk380; // 0x380
                float m_Unk384; // 0x384
                float m_Unk388; // 0x388
                float m_Unk38C; // 0x38C
                float m_Unk390; // 0x390
                float m_Unk394; // 0x394
                float m_Unk398; // 0x398
                float m_Unk39C; // 0x39C
                float m_Unk3A0; // 0x3A0
                float m_Unk3A4; // 0x3A4
                unsigned long m_Unk3A8; // 0x3A8
                float m_Unk3AC; // 0x3AC
                float m_Unk3B0; // 0x3B0
                float m_Unk3B4; // 0x3B4
                float m_Unk3B8; // 0x3B8
                float m_Unk3BC; // 0x3BC
                float m_Unk3C0; // 0x3C0
                float m_Unk3C4; // 0x3C4
                float m_Unk3C8; // 0x3C8
                float m_Unk3CC; // 0x3CC
                float m_Unk3D0; // 0x3D0
                float m_Unk3D4; // 0x3D4
                float m_Unk3D8; // 0x3D8
                float m_Unk3DC; // 0x3DC
                unsigned long m_Unk3E0; // 0x3E0
        }; // size 0x3E4 (the NewCcParam_ functions allocate 0x3E4 bytes)
    }

    // Default parameter block, constructed by the static initialiser at the
    // end of the parameter file.
    namespace cc_data_volley_default {
        extern CHR_CORE_VOLLEY::CC_PARAM_VOLLEY m_CcParam;
    }

    // One namespace per character: the character's own values, and one class
    // per game level, each with a _STAR variant for the star player. Unlike
    // the boss mode, the EASY and NORMAL classes (and their _STAR variants)
    // change values here; HARD and EXPERT add nothing. (__init2 is empty;
    // what it is for is not known.)
    namespace cc_data_volley_Mario {
        class CC_PARAM_VOLLEY_Mario : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Mario();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Mario_EASY : public CC_PARAM_VOLLEY_Mario {
            public: CC_PARAM_VOLLEY_Mario_EASY();
        };
        class CC_PARAM_VOLLEY_Mario_EASY_STAR : public CC_PARAM_VOLLEY_Mario_EASY {
            public: CC_PARAM_VOLLEY_Mario_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Mario_NORMAL : public CC_PARAM_VOLLEY_Mario {
            public: CC_PARAM_VOLLEY_Mario_NORMAL();
        };
        class CC_PARAM_VOLLEY_Mario_NORMAL_STAR : public CC_PARAM_VOLLEY_Mario_NORMAL {
            public: CC_PARAM_VOLLEY_Mario_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Mario_HARD : public CC_PARAM_VOLLEY_Mario {
            public: CC_PARAM_VOLLEY_Mario_HARD();
        };
        class CC_PARAM_VOLLEY_Mario_HARD_STAR : public CC_PARAM_VOLLEY_Mario_HARD {
            public: CC_PARAM_VOLLEY_Mario_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Mario_EXPERT : public CC_PARAM_VOLLEY_Mario {
            public: CC_PARAM_VOLLEY_Mario_EXPERT();
        };
        class CC_PARAM_VOLLEY_Mario_EXPERT_STAR : public CC_PARAM_VOLLEY_Mario_EXPERT {
            public: CC_PARAM_VOLLEY_Mario_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Luigi {
        class CC_PARAM_VOLLEY_Luigi : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Luigi();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Luigi_EASY : public CC_PARAM_VOLLEY_Luigi {
            public: CC_PARAM_VOLLEY_Luigi_EASY();
        };
        class CC_PARAM_VOLLEY_Luigi_EASY_STAR : public CC_PARAM_VOLLEY_Luigi_EASY {
            public: CC_PARAM_VOLLEY_Luigi_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Luigi_NORMAL : public CC_PARAM_VOLLEY_Luigi {
            public: CC_PARAM_VOLLEY_Luigi_NORMAL();
        };
        class CC_PARAM_VOLLEY_Luigi_NORMAL_STAR : public CC_PARAM_VOLLEY_Luigi_NORMAL {
            public: CC_PARAM_VOLLEY_Luigi_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Luigi_HARD : public CC_PARAM_VOLLEY_Luigi {
            public: CC_PARAM_VOLLEY_Luigi_HARD();
        };
        class CC_PARAM_VOLLEY_Luigi_HARD_STAR : public CC_PARAM_VOLLEY_Luigi_HARD {
            public: CC_PARAM_VOLLEY_Luigi_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Luigi_EXPERT : public CC_PARAM_VOLLEY_Luigi {
            public: CC_PARAM_VOLLEY_Luigi_EXPERT();
        };
        class CC_PARAM_VOLLEY_Luigi_EXPERT_STAR : public CC_PARAM_VOLLEY_Luigi_EXPERT {
            public: CC_PARAM_VOLLEY_Luigi_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Peach {
        class CC_PARAM_VOLLEY_Peach : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Peach();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Peach_EASY : public CC_PARAM_VOLLEY_Peach {
            public: CC_PARAM_VOLLEY_Peach_EASY();
        };
        class CC_PARAM_VOLLEY_Peach_EASY_STAR : public CC_PARAM_VOLLEY_Peach_EASY {
            public: CC_PARAM_VOLLEY_Peach_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Peach_NORMAL : public CC_PARAM_VOLLEY_Peach {
            public: CC_PARAM_VOLLEY_Peach_NORMAL();
        };
        class CC_PARAM_VOLLEY_Peach_NORMAL_STAR : public CC_PARAM_VOLLEY_Peach_NORMAL {
            public: CC_PARAM_VOLLEY_Peach_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Peach_HARD : public CC_PARAM_VOLLEY_Peach {
            public: CC_PARAM_VOLLEY_Peach_HARD();
        };
        class CC_PARAM_VOLLEY_Peach_HARD_STAR : public CC_PARAM_VOLLEY_Peach_HARD {
            public: CC_PARAM_VOLLEY_Peach_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Peach_EXPERT : public CC_PARAM_VOLLEY_Peach {
            public: CC_PARAM_VOLLEY_Peach_EXPERT();
        };
        class CC_PARAM_VOLLEY_Peach_EXPERT_STAR : public CC_PARAM_VOLLEY_Peach_EXPERT {
            public: CC_PARAM_VOLLEY_Peach_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Daisy {
        class CC_PARAM_VOLLEY_Daisy : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Daisy();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Daisy_EASY : public CC_PARAM_VOLLEY_Daisy {
            public: CC_PARAM_VOLLEY_Daisy_EASY();
        };
        class CC_PARAM_VOLLEY_Daisy_EASY_STAR : public CC_PARAM_VOLLEY_Daisy_EASY {
            public: CC_PARAM_VOLLEY_Daisy_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Daisy_NORMAL : public CC_PARAM_VOLLEY_Daisy {
            public: CC_PARAM_VOLLEY_Daisy_NORMAL();
        };
        class CC_PARAM_VOLLEY_Daisy_NORMAL_STAR : public CC_PARAM_VOLLEY_Daisy_NORMAL {
            public: CC_PARAM_VOLLEY_Daisy_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Daisy_HARD : public CC_PARAM_VOLLEY_Daisy {
            public: CC_PARAM_VOLLEY_Daisy_HARD();
        };
        class CC_PARAM_VOLLEY_Daisy_HARD_STAR : public CC_PARAM_VOLLEY_Daisy_HARD {
            public: CC_PARAM_VOLLEY_Daisy_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Daisy_EXPERT : public CC_PARAM_VOLLEY_Daisy {
            public: CC_PARAM_VOLLEY_Daisy_EXPERT();
        };
        class CC_PARAM_VOLLEY_Daisy_EXPERT_STAR : public CC_PARAM_VOLLEY_Daisy_EXPERT {
            public: CC_PARAM_VOLLEY_Daisy_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Yoshi {
        class CC_PARAM_VOLLEY_Yoshi : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Yoshi();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Yoshi_EASY : public CC_PARAM_VOLLEY_Yoshi {
            public: CC_PARAM_VOLLEY_Yoshi_EASY();
        };
        class CC_PARAM_VOLLEY_Yoshi_EASY_STAR : public CC_PARAM_VOLLEY_Yoshi_EASY {
            public: CC_PARAM_VOLLEY_Yoshi_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Yoshi_NORMAL : public CC_PARAM_VOLLEY_Yoshi {
            public: CC_PARAM_VOLLEY_Yoshi_NORMAL();
        };
        class CC_PARAM_VOLLEY_Yoshi_NORMAL_STAR : public CC_PARAM_VOLLEY_Yoshi_NORMAL {
            public: CC_PARAM_VOLLEY_Yoshi_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Yoshi_HARD : public CC_PARAM_VOLLEY_Yoshi {
            public: CC_PARAM_VOLLEY_Yoshi_HARD();
        };
        class CC_PARAM_VOLLEY_Yoshi_HARD_STAR : public CC_PARAM_VOLLEY_Yoshi_HARD {
            public: CC_PARAM_VOLLEY_Yoshi_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Yoshi_EXPERT : public CC_PARAM_VOLLEY_Yoshi {
            public: CC_PARAM_VOLLEY_Yoshi_EXPERT();
        };
        class CC_PARAM_VOLLEY_Yoshi_EXPERT_STAR : public CC_PARAM_VOLLEY_Yoshi_EXPERT {
            public: CC_PARAM_VOLLEY_Yoshi_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Wario {
        class CC_PARAM_VOLLEY_Wario : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Wario();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Wario_EASY : public CC_PARAM_VOLLEY_Wario {
            public: CC_PARAM_VOLLEY_Wario_EASY();
        };
        class CC_PARAM_VOLLEY_Wario_EASY_STAR : public CC_PARAM_VOLLEY_Wario_EASY {
            public: CC_PARAM_VOLLEY_Wario_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Wario_NORMAL : public CC_PARAM_VOLLEY_Wario {
            public: CC_PARAM_VOLLEY_Wario_NORMAL();
        };
        class CC_PARAM_VOLLEY_Wario_NORMAL_STAR : public CC_PARAM_VOLLEY_Wario_NORMAL {
            public: CC_PARAM_VOLLEY_Wario_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Wario_HARD : public CC_PARAM_VOLLEY_Wario {
            public: CC_PARAM_VOLLEY_Wario_HARD();
        };
        class CC_PARAM_VOLLEY_Wario_HARD_STAR : public CC_PARAM_VOLLEY_Wario_HARD {
            public: CC_PARAM_VOLLEY_Wario_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Wario_EXPERT : public CC_PARAM_VOLLEY_Wario {
            public: CC_PARAM_VOLLEY_Wario_EXPERT();
        };
        class CC_PARAM_VOLLEY_Wario_EXPERT_STAR : public CC_PARAM_VOLLEY_Wario_EXPERT {
            public: CC_PARAM_VOLLEY_Wario_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Waluigi {
        class CC_PARAM_VOLLEY_Waluigi : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Waluigi();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Waluigi_EASY : public CC_PARAM_VOLLEY_Waluigi {
            public: CC_PARAM_VOLLEY_Waluigi_EASY();
        };
        class CC_PARAM_VOLLEY_Waluigi_EASY_STAR : public CC_PARAM_VOLLEY_Waluigi_EASY {
            public: CC_PARAM_VOLLEY_Waluigi_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Waluigi_NORMAL : public CC_PARAM_VOLLEY_Waluigi {
            public: CC_PARAM_VOLLEY_Waluigi_NORMAL();
        };
        class CC_PARAM_VOLLEY_Waluigi_NORMAL_STAR : public CC_PARAM_VOLLEY_Waluigi_NORMAL {
            public: CC_PARAM_VOLLEY_Waluigi_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Waluigi_HARD : public CC_PARAM_VOLLEY_Waluigi {
            public: CC_PARAM_VOLLEY_Waluigi_HARD();
        };
        class CC_PARAM_VOLLEY_Waluigi_HARD_STAR : public CC_PARAM_VOLLEY_Waluigi_HARD {
            public: CC_PARAM_VOLLEY_Waluigi_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Waluigi_EXPERT : public CC_PARAM_VOLLEY_Waluigi {
            public: CC_PARAM_VOLLEY_Waluigi_EXPERT();
        };
        class CC_PARAM_VOLLEY_Waluigi_EXPERT_STAR : public CC_PARAM_VOLLEY_Waluigi_EXPERT {
            public: CC_PARAM_VOLLEY_Waluigi_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Donkey {
        class CC_PARAM_VOLLEY_Donkey : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Donkey();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Donkey_EASY : public CC_PARAM_VOLLEY_Donkey {
            public: CC_PARAM_VOLLEY_Donkey_EASY();
        };
        class CC_PARAM_VOLLEY_Donkey_EASY_STAR : public CC_PARAM_VOLLEY_Donkey_EASY {
            public: CC_PARAM_VOLLEY_Donkey_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Donkey_NORMAL : public CC_PARAM_VOLLEY_Donkey {
            public: CC_PARAM_VOLLEY_Donkey_NORMAL();
        };
        class CC_PARAM_VOLLEY_Donkey_NORMAL_STAR : public CC_PARAM_VOLLEY_Donkey_NORMAL {
            public: CC_PARAM_VOLLEY_Donkey_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Donkey_HARD : public CC_PARAM_VOLLEY_Donkey {
            public: CC_PARAM_VOLLEY_Donkey_HARD();
        };
        class CC_PARAM_VOLLEY_Donkey_HARD_STAR : public CC_PARAM_VOLLEY_Donkey_HARD {
            public: CC_PARAM_VOLLEY_Donkey_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Donkey_EXPERT : public CC_PARAM_VOLLEY_Donkey {
            public: CC_PARAM_VOLLEY_Donkey_EXPERT();
        };
        class CC_PARAM_VOLLEY_Donkey_EXPERT_STAR : public CC_PARAM_VOLLEY_Donkey_EXPERT {
            public: CC_PARAM_VOLLEY_Donkey_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Diddy {
        class CC_PARAM_VOLLEY_Diddy : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Diddy();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Diddy_EASY : public CC_PARAM_VOLLEY_Diddy {
            public: CC_PARAM_VOLLEY_Diddy_EASY();
        };
        class CC_PARAM_VOLLEY_Diddy_EASY_STAR : public CC_PARAM_VOLLEY_Diddy_EASY {
            public: CC_PARAM_VOLLEY_Diddy_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Diddy_NORMAL : public CC_PARAM_VOLLEY_Diddy {
            public: CC_PARAM_VOLLEY_Diddy_NORMAL();
        };
        class CC_PARAM_VOLLEY_Diddy_NORMAL_STAR : public CC_PARAM_VOLLEY_Diddy_NORMAL {
            public: CC_PARAM_VOLLEY_Diddy_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Diddy_HARD : public CC_PARAM_VOLLEY_Diddy {
            public: CC_PARAM_VOLLEY_Diddy_HARD();
        };
        class CC_PARAM_VOLLEY_Diddy_HARD_STAR : public CC_PARAM_VOLLEY_Diddy_HARD {
            public: CC_PARAM_VOLLEY_Diddy_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Diddy_EXPERT : public CC_PARAM_VOLLEY_Diddy {
            public: CC_PARAM_VOLLEY_Diddy_EXPERT();
        };
        class CC_PARAM_VOLLEY_Diddy_EXPERT_STAR : public CC_PARAM_VOLLEY_Diddy_EXPERT {
            public: CC_PARAM_VOLLEY_Diddy_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Kinopio {
        class CC_PARAM_VOLLEY_Kinopio : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Kinopio();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Kinopio_EASY : public CC_PARAM_VOLLEY_Kinopio {
            public: CC_PARAM_VOLLEY_Kinopio_EASY();
        };
        class CC_PARAM_VOLLEY_Kinopio_EASY_STAR : public CC_PARAM_VOLLEY_Kinopio_EASY {
            public: CC_PARAM_VOLLEY_Kinopio_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Kinopio_NORMAL : public CC_PARAM_VOLLEY_Kinopio {
            public: CC_PARAM_VOLLEY_Kinopio_NORMAL();
        };
        class CC_PARAM_VOLLEY_Kinopio_NORMAL_STAR : public CC_PARAM_VOLLEY_Kinopio_NORMAL {
            public: CC_PARAM_VOLLEY_Kinopio_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Kinopio_HARD : public CC_PARAM_VOLLEY_Kinopio {
            public: CC_PARAM_VOLLEY_Kinopio_HARD();
        };
        class CC_PARAM_VOLLEY_Kinopio_HARD_STAR : public CC_PARAM_VOLLEY_Kinopio_HARD {
            public: CC_PARAM_VOLLEY_Kinopio_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Kinopio_EXPERT : public CC_PARAM_VOLLEY_Kinopio {
            public: CC_PARAM_VOLLEY_Kinopio_EXPERT();
        };
        class CC_PARAM_VOLLEY_Kinopio_EXPERT_STAR : public CC_PARAM_VOLLEY_Kinopio_EXPERT {
            public: CC_PARAM_VOLLEY_Kinopio_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Koopa {
        class CC_PARAM_VOLLEY_Koopa : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Koopa();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Koopa_EASY : public CC_PARAM_VOLLEY_Koopa {
            public: CC_PARAM_VOLLEY_Koopa_EASY();
        };
        class CC_PARAM_VOLLEY_Koopa_EASY_STAR : public CC_PARAM_VOLLEY_Koopa_EASY {
            public: CC_PARAM_VOLLEY_Koopa_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Koopa_NORMAL : public CC_PARAM_VOLLEY_Koopa {
            public: CC_PARAM_VOLLEY_Koopa_NORMAL();
        };
        class CC_PARAM_VOLLEY_Koopa_NORMAL_STAR : public CC_PARAM_VOLLEY_Koopa_NORMAL {
            public: CC_PARAM_VOLLEY_Koopa_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Koopa_HARD : public CC_PARAM_VOLLEY_Koopa {
            public: CC_PARAM_VOLLEY_Koopa_HARD();
        };
        class CC_PARAM_VOLLEY_Koopa_HARD_STAR : public CC_PARAM_VOLLEY_Koopa_HARD {
            public: CC_PARAM_VOLLEY_Koopa_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Koopa_EXPERT : public CC_PARAM_VOLLEY_Koopa {
            public: CC_PARAM_VOLLEY_Koopa_EXPERT();
        };
        class CC_PARAM_VOLLEY_Koopa_EXPERT_STAR : public CC_PARAM_VOLLEY_Koopa_EXPERT {
            public: CC_PARAM_VOLLEY_Koopa_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_KoopaJr {
        class CC_PARAM_VOLLEY_KoopaJr : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_KoopaJr();
                void __init2();
        };
        class CC_PARAM_VOLLEY_KoopaJr_EASY : public CC_PARAM_VOLLEY_KoopaJr {
            public: CC_PARAM_VOLLEY_KoopaJr_EASY();
        };
        class CC_PARAM_VOLLEY_KoopaJr_EASY_STAR : public CC_PARAM_VOLLEY_KoopaJr_EASY {
            public: CC_PARAM_VOLLEY_KoopaJr_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_KoopaJr_NORMAL : public CC_PARAM_VOLLEY_KoopaJr {
            public: CC_PARAM_VOLLEY_KoopaJr_NORMAL();
        };
        class CC_PARAM_VOLLEY_KoopaJr_NORMAL_STAR : public CC_PARAM_VOLLEY_KoopaJr_NORMAL {
            public: CC_PARAM_VOLLEY_KoopaJr_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_KoopaJr_HARD : public CC_PARAM_VOLLEY_KoopaJr {
            public: CC_PARAM_VOLLEY_KoopaJr_HARD();
        };
        class CC_PARAM_VOLLEY_KoopaJr_HARD_STAR : public CC_PARAM_VOLLEY_KoopaJr_HARD {
            public: CC_PARAM_VOLLEY_KoopaJr_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_KoopaJr_EXPERT : public CC_PARAM_VOLLEY_KoopaJr {
            public: CC_PARAM_VOLLEY_KoopaJr_EXPERT();
        };
        class CC_PARAM_VOLLEY_KoopaJr_EXPERT_STAR : public CC_PARAM_VOLLEY_KoopaJr_EXPERT {
            public: CC_PARAM_VOLLEY_KoopaJr_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Moogle {
        class CC_PARAM_VOLLEY_Moogle : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Moogle();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Moogle_EASY : public CC_PARAM_VOLLEY_Moogle {
            public: CC_PARAM_VOLLEY_Moogle_EASY();
        };
        class CC_PARAM_VOLLEY_Moogle_EASY_STAR : public CC_PARAM_VOLLEY_Moogle_EASY {
            public: CC_PARAM_VOLLEY_Moogle_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Moogle_NORMAL : public CC_PARAM_VOLLEY_Moogle {
            public: CC_PARAM_VOLLEY_Moogle_NORMAL();
        };
        class CC_PARAM_VOLLEY_Moogle_NORMAL_STAR : public CC_PARAM_VOLLEY_Moogle_NORMAL {
            public: CC_PARAM_VOLLEY_Moogle_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Moogle_HARD : public CC_PARAM_VOLLEY_Moogle {
            public: CC_PARAM_VOLLEY_Moogle_HARD();
        };
        class CC_PARAM_VOLLEY_Moogle_HARD_STAR : public CC_PARAM_VOLLEY_Moogle_HARD {
            public: CC_PARAM_VOLLEY_Moogle_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Moogle_EXPERT : public CC_PARAM_VOLLEY_Moogle {
            public: CC_PARAM_VOLLEY_Moogle_EXPERT();
        };
        class CC_PARAM_VOLLEY_Moogle_EXPERT_STAR : public CC_PARAM_VOLLEY_Moogle_EXPERT {
            public: CC_PARAM_VOLLEY_Moogle_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Saboten {
        class CC_PARAM_VOLLEY_Saboten : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Saboten();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Saboten_EASY : public CC_PARAM_VOLLEY_Saboten {
            public: CC_PARAM_VOLLEY_Saboten_EASY();
        };
        class CC_PARAM_VOLLEY_Saboten_EASY_STAR : public CC_PARAM_VOLLEY_Saboten_EASY {
            public: CC_PARAM_VOLLEY_Saboten_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Saboten_NORMAL : public CC_PARAM_VOLLEY_Saboten {
            public: CC_PARAM_VOLLEY_Saboten_NORMAL();
        };
        class CC_PARAM_VOLLEY_Saboten_NORMAL_STAR : public CC_PARAM_VOLLEY_Saboten_NORMAL {
            public: CC_PARAM_VOLLEY_Saboten_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Saboten_HARD : public CC_PARAM_VOLLEY_Saboten {
            public: CC_PARAM_VOLLEY_Saboten_HARD();
        };
        class CC_PARAM_VOLLEY_Saboten_HARD_STAR : public CC_PARAM_VOLLEY_Saboten_HARD {
            public: CC_PARAM_VOLLEY_Saboten_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Saboten_EXPERT : public CC_PARAM_VOLLEY_Saboten {
            public: CC_PARAM_VOLLEY_Saboten_EXPERT();
        };
        class CC_PARAM_VOLLEY_Saboten_EXPERT_STAR : public CC_PARAM_VOLLEY_Saboten_EXPERT {
            public: CC_PARAM_VOLLEY_Saboten_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Ninja {
        class CC_PARAM_VOLLEY_Ninja : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Ninja();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Ninja_EASY : public CC_PARAM_VOLLEY_Ninja {
            public: CC_PARAM_VOLLEY_Ninja_EASY();
        };
        class CC_PARAM_VOLLEY_Ninja_EASY_STAR : public CC_PARAM_VOLLEY_Ninja_EASY {
            public: CC_PARAM_VOLLEY_Ninja_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Ninja_NORMAL : public CC_PARAM_VOLLEY_Ninja {
            public: CC_PARAM_VOLLEY_Ninja_NORMAL();
        };
        class CC_PARAM_VOLLEY_Ninja_NORMAL_STAR : public CC_PARAM_VOLLEY_Ninja_NORMAL {
            public: CC_PARAM_VOLLEY_Ninja_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Ninja_HARD : public CC_PARAM_VOLLEY_Ninja {
            public: CC_PARAM_VOLLEY_Ninja_HARD();
        };
        class CC_PARAM_VOLLEY_Ninja_HARD_STAR : public CC_PARAM_VOLLEY_Ninja_HARD {
            public: CC_PARAM_VOLLEY_Ninja_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Ninja_EXPERT : public CC_PARAM_VOLLEY_Ninja {
            public: CC_PARAM_VOLLEY_Ninja_EXPERT();
        };
        class CC_PARAM_VOLLEY_Ninja_EXPERT_STAR : public CC_PARAM_VOLLEY_Ninja_EXPERT {
            public: CC_PARAM_VOLLEY_Ninja_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Siroma {
        class CC_PARAM_VOLLEY_Siroma : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Siroma();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Siroma_EASY : public CC_PARAM_VOLLEY_Siroma {
            public: CC_PARAM_VOLLEY_Siroma_EASY();
        };
        class CC_PARAM_VOLLEY_Siroma_EASY_STAR : public CC_PARAM_VOLLEY_Siroma_EASY {
            public: CC_PARAM_VOLLEY_Siroma_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Siroma_NORMAL : public CC_PARAM_VOLLEY_Siroma {
            public: CC_PARAM_VOLLEY_Siroma_NORMAL();
        };
        class CC_PARAM_VOLLEY_Siroma_NORMAL_STAR : public CC_PARAM_VOLLEY_Siroma_NORMAL {
            public: CC_PARAM_VOLLEY_Siroma_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Siroma_HARD : public CC_PARAM_VOLLEY_Siroma {
            public: CC_PARAM_VOLLEY_Siroma_HARD();
        };
        class CC_PARAM_VOLLEY_Siroma_HARD_STAR : public CC_PARAM_VOLLEY_Siroma_HARD {
            public: CC_PARAM_VOLLEY_Siroma_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Siroma_EXPERT : public CC_PARAM_VOLLEY_Siroma {
            public: CC_PARAM_VOLLEY_Siroma_EXPERT();
        };
        class CC_PARAM_VOLLEY_Siroma_EXPERT_STAR : public CC_PARAM_VOLLEY_Siroma_EXPERT {
            public: CC_PARAM_VOLLEY_Siroma_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Slime {
        class CC_PARAM_VOLLEY_Slime : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Slime();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Slime_EASY : public CC_PARAM_VOLLEY_Slime {
            public: CC_PARAM_VOLLEY_Slime_EASY();
        };
        class CC_PARAM_VOLLEY_Slime_EASY_STAR : public CC_PARAM_VOLLEY_Slime_EASY {
            public: CC_PARAM_VOLLEY_Slime_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Slime_NORMAL : public CC_PARAM_VOLLEY_Slime {
            public: CC_PARAM_VOLLEY_Slime_NORMAL();
        };
        class CC_PARAM_VOLLEY_Slime_NORMAL_STAR : public CC_PARAM_VOLLEY_Slime_NORMAL {
            public: CC_PARAM_VOLLEY_Slime_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Slime_HARD : public CC_PARAM_VOLLEY_Slime {
            public: CC_PARAM_VOLLEY_Slime_HARD();
        };
        class CC_PARAM_VOLLEY_Slime_HARD_STAR : public CC_PARAM_VOLLEY_Slime_HARD {
            public: CC_PARAM_VOLLEY_Slime_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Slime_EXPERT : public CC_PARAM_VOLLEY_Slime {
            public: CC_PARAM_VOLLEY_Slime_EXPERT();
        };
        class CC_PARAM_VOLLEY_Slime_EXPERT_STAR : public CC_PARAM_VOLLEY_Slime_EXPERT {
            public: CC_PARAM_VOLLEY_Slime_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Kuroma {
        class CC_PARAM_VOLLEY_Kuroma : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Kuroma();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Kuroma_EASY : public CC_PARAM_VOLLEY_Kuroma {
            public: CC_PARAM_VOLLEY_Kuroma_EASY();
        };
        class CC_PARAM_VOLLEY_Kuroma_EASY_STAR : public CC_PARAM_VOLLEY_Kuroma_EASY {
            public: CC_PARAM_VOLLEY_Kuroma_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Kuroma_NORMAL : public CC_PARAM_VOLLEY_Kuroma {
            public: CC_PARAM_VOLLEY_Kuroma_NORMAL();
        };
        class CC_PARAM_VOLLEY_Kuroma_NORMAL_STAR : public CC_PARAM_VOLLEY_Kuroma_NORMAL {
            public: CC_PARAM_VOLLEY_Kuroma_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Kuroma_HARD : public CC_PARAM_VOLLEY_Kuroma {
            public: CC_PARAM_VOLLEY_Kuroma_HARD();
        };
        class CC_PARAM_VOLLEY_Kuroma_HARD_STAR : public CC_PARAM_VOLLEY_Kuroma_HARD {
            public: CC_PARAM_VOLLEY_Kuroma_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Kuroma_EXPERT : public CC_PARAM_VOLLEY_Kuroma {
            public: CC_PARAM_VOLLEY_Kuroma_EXPERT();
        };
        class CC_PARAM_VOLLEY_Kuroma_EXPERT_STAR : public CC_PARAM_VOLLEY_Kuroma_EXPERT {
            public: CC_PARAM_VOLLEY_Kuroma_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Mii00 {
        class CC_PARAM_VOLLEY_Mii00 : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Mii00();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Mii00_EASY : public CC_PARAM_VOLLEY_Mii00 {
            public: CC_PARAM_VOLLEY_Mii00_EASY();
        };
        class CC_PARAM_VOLLEY_Mii00_EASY_STAR : public CC_PARAM_VOLLEY_Mii00_EASY {
            public: CC_PARAM_VOLLEY_Mii00_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Mii00_NORMAL : public CC_PARAM_VOLLEY_Mii00 {
            public: CC_PARAM_VOLLEY_Mii00_NORMAL();
        };
        class CC_PARAM_VOLLEY_Mii00_NORMAL_STAR : public CC_PARAM_VOLLEY_Mii00_NORMAL {
            public: CC_PARAM_VOLLEY_Mii00_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Mii00_HARD : public CC_PARAM_VOLLEY_Mii00 {
            public: CC_PARAM_VOLLEY_Mii00_HARD();
        };
        class CC_PARAM_VOLLEY_Mii00_HARD_STAR : public CC_PARAM_VOLLEY_Mii00_HARD {
            public: CC_PARAM_VOLLEY_Mii00_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Mii00_EXPERT : public CC_PARAM_VOLLEY_Mii00 {
            public: CC_PARAM_VOLLEY_Mii00_EXPERT();
        };
        class CC_PARAM_VOLLEY_Mii00_EXPERT_STAR : public CC_PARAM_VOLLEY_Mii00_EXPERT {
            public: CC_PARAM_VOLLEY_Mii00_EXPERT_STAR();
        };
    }

    namespace cc_data_volley_Mii01 {
        class CC_PARAM_VOLLEY_Mii01 : public CHR_CORE_VOLLEY::CC_PARAM_VOLLEY {
            public:
                CC_PARAM_VOLLEY_Mii01();
                void __init2();
        };
        class CC_PARAM_VOLLEY_Mii01_EASY : public CC_PARAM_VOLLEY_Mii01 {
            public: CC_PARAM_VOLLEY_Mii01_EASY();
        };
        class CC_PARAM_VOLLEY_Mii01_EASY_STAR : public CC_PARAM_VOLLEY_Mii01_EASY {
            public: CC_PARAM_VOLLEY_Mii01_EASY_STAR();
        };
        class CC_PARAM_VOLLEY_Mii01_NORMAL : public CC_PARAM_VOLLEY_Mii01 {
            public: CC_PARAM_VOLLEY_Mii01_NORMAL();
        };
        class CC_PARAM_VOLLEY_Mii01_NORMAL_STAR : public CC_PARAM_VOLLEY_Mii01_NORMAL {
            public: CC_PARAM_VOLLEY_Mii01_NORMAL_STAR();
        };
        class CC_PARAM_VOLLEY_Mii01_HARD : public CC_PARAM_VOLLEY_Mii01 {
            public: CC_PARAM_VOLLEY_Mii01_HARD();
        };
        class CC_PARAM_VOLLEY_Mii01_HARD_STAR : public CC_PARAM_VOLLEY_Mii01_HARD {
            public: CC_PARAM_VOLLEY_Mii01_HARD_STAR();
        };
        class CC_PARAM_VOLLEY_Mii01_EXPERT : public CC_PARAM_VOLLEY_Mii01 {
            public: CC_PARAM_VOLLEY_Mii01_EXPERT();
        };
        class CC_PARAM_VOLLEY_Mii01_EXPERT_STAR : public CC_PARAM_VOLLEY_Mii01_EXPERT {
            public: CC_PARAM_VOLLEY_Mii01_EXPERT_STAR();
        };
    }

}

#endif /* !MSW_CC_PARAM_VOLLEY_HPP_ */
