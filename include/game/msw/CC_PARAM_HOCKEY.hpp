#ifndef MSW_CC_PARAM_HOCKEY_HPP_
#define MSW_CC_PARAM_HOCKEY_HPP_

namespace msw {

    // Parameter block of a volleyball character core (VolleyF), the
    // counterpart of CHR_CORE_BOSS::CC_PARAM_BOSS. Every character has a
    // CC_PARAM_HOCKEY_<Name> derived from it, with one class per level.
    //
    // Field names are placeholders; types come from the access widths in
    // CC_PARAM_HOCKEY::CC_PARAM_HOCKEY() (the last field it writes is at
    // 0x620; the NewCcParam_ of the characters allocate 0x624). m_Unk04 is passed to CBall::GetHockeyReceiveReaction.
    namespace CHR_CORE_HOCKEY {
        class CC_PARAM_HOCKEY {
            public:
                CC_PARAM_HOCKEY();

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
                // 0x3E4 to 0x620: the DodgeF constructor writes these too (every
                // NewCcParam_ allocates 0x624; CChrCoreHockeyKinopio reads 0x560).
                float m_Unk3E4; // 0x3E4
                float m_Unk3E8; // 0x3E8
                float m_Unk3EC; // 0x3EC
                float m_Unk3F0; // 0x3F0
                float m_Unk3F4; // 0x3F4
                float m_Unk3F8; // 0x3F8
                float m_Unk3FC; // 0x3FC
                float m_Unk400; // 0x400
                float m_Unk404; // 0x404
                float m_Unk408; // 0x408
                float m_Unk40C; // 0x40C
                float m_Unk410; // 0x410
                float m_Unk414; // 0x414
                float m_Unk418; // 0x418
                float m_Unk41C; // 0x41C
                float m_Unk420; // 0x420
                float m_Unk424; // 0x424
                float m_Unk428; // 0x428
                float m_Unk42C; // 0x42C
                float m_Unk430; // 0x430
                unsigned long m_Unk434; // 0x434
                unsigned long m_Unk438; // 0x438
                float m_Unk43C; // 0x43C
                float m_Unk440; // 0x440
                float m_Unk444; // 0x444
                float m_Unk448; // 0x448
                float m_Unk44C; // 0x44C
                float m_Unk450; // 0x450
                float m_Unk454; // 0x454
                float m_Unk458; // 0x458
                float m_Unk45C; // 0x45C
                float m_Unk460; // 0x460
                float m_Unk464; // 0x464
                float m_Unk468; // 0x468
                float m_Unk46C; // 0x46C
                float m_Unk470; // 0x470
                float m_Unk474; // 0x474
                float m_Unk478; // 0x478
                float m_Unk47C; // 0x47C
                float m_Unk480; // 0x480
                float m_Unk484; // 0x484
                float m_Unk488; // 0x488
                float m_Unk48C; // 0x48C
                float m_Unk490; // 0x490
                float m_Unk494; // 0x494
                float m_Unk498; // 0x498
                float m_Unk49C; // 0x49C
                float m_Unk4A0; // 0x4A0
                float m_Unk4A4; // 0x4A4
                float m_Unk4A8; // 0x4A8
                float m_Unk4AC; // 0x4AC
                unsigned long m_Unk4B0; // 0x4B0
                unsigned long m_Unk4B4; // 0x4B4
                unsigned long m_Unk4B8; // 0x4B8
                unsigned long m_Unk4BC; // 0x4BC
                unsigned long m_Unk4C0; // 0x4C0
                unsigned long m_Unk4C4; // 0x4C4
                unsigned long m_Unk4C8; // 0x4C8
                float m_Unk4CC; // 0x4CC
                float m_Unk4D0; // 0x4D0
                float m_Unk4D4; // 0x4D4
                float m_Unk4D8; // 0x4D8
                float m_Unk4DC; // 0x4DC
                float m_Unk4E0; // 0x4E0
                float m_Unk4E4; // 0x4E4
                float m_Unk4E8; // 0x4E8
                float m_Unk4EC; // 0x4EC
                float m_Unk4F0; // 0x4F0
                float m_Unk4F4; // 0x4F4
                float m_Unk4F8; // 0x4F8
                float m_Unk4FC; // 0x4FC
                float m_Unk500; // 0x500
                float m_Unk504; // 0x504
                float m_Unk508; // 0x508
                float m_Unk50C; // 0x50C
                float m_Unk510; // 0x510
                float m_Unk514; // 0x514
                float m_Unk518; // 0x518
                float m_Unk51C; // 0x51C
                float m_Unk520; // 0x520
                float m_Unk524; // 0x524
                float m_Unk528; // 0x528
                float m_Unk52C; // 0x52C
                float m_Unk530; // 0x530
                float m_Unk534; // 0x534
                float m_Unk538; // 0x538
                float m_Unk53C; // 0x53C
                float m_Unk540; // 0x540
                float m_Unk544; // 0x544
                float m_Unk548; // 0x548
                float m_Unk54C; // 0x54C
                float m_Unk550; // 0x550
                float m_Unk554; // 0x554
                float m_Unk558; // 0x558
                float m_Unk55C; // 0x55C
                float m_Unk560; // 0x560
                float m_Unk564; // 0x564
                float m_Unk568; // 0x568
                float m_Unk56C; // 0x56C
                float m_Unk570; // 0x570
                float m_Unk574; // 0x574
                float m_Unk578; // 0x578
                float m_Unk57C; // 0x57C
                float m_Unk580; // 0x580
                float m_Unk584; // 0x584
                float m_Unk588; // 0x588
                float m_Unk58C; // 0x58C
                float m_Unk590; // 0x590
                float m_Unk594; // 0x594
                float m_Unk598; // 0x598
                float m_Unk59C; // 0x59C
                float m_Unk5A0; // 0x5A0
                float m_Unk5A4; // 0x5A4
                float m_Unk5A8; // 0x5A8
                float m_Unk5AC; // 0x5AC
                float m_Unk5B0; // 0x5B0
                float m_Unk5B4; // 0x5B4
                float m_Unk5B8; // 0x5B8
                float m_Unk5BC; // 0x5BC
                float m_Unk5C0; // 0x5C0
                float m_Unk5C4; // 0x5C4
                float m_Unk5C8; // 0x5C8
                float m_Unk5CC; // 0x5CC
                float m_Unk5D0; // 0x5D0
                float m_Unk5D4; // 0x5D4
                float m_Unk5D8; // 0x5D8
                float m_Unk5DC; // 0x5DC
                float m_Unk5E0; // 0x5E0
                float m_Unk5E4; // 0x5E4
                float m_Unk5E8; // 0x5E8
                float m_Unk5EC; // 0x5EC
                float m_Unk5F0; // 0x5F0
                float m_Unk5F4; // 0x5F4
                float m_Unk5F8; // 0x5F8
                float m_Unk5FC; // 0x5FC
                float m_Unk600; // 0x600
                float m_Unk604; // 0x604
                float m_Unk608; // 0x608
                float m_Unk60C; // 0x60C
                float m_Unk610; // 0x610
                float m_Unk614; // 0x614
                float m_Unk618; // 0x618
                float m_Unk61C; // 0x61C
                unsigned long m_Unk620; // 0x620
        };
    }
}

#endif /* !MSW_CC_PARAM_HOCKEY_HPP_ */
