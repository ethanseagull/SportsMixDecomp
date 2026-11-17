#ifndef MSW_CC_PARAM_BASKET_HPP_
#define MSW_CC_PARAM_BASKET_HPP_

#include "game/msw/CChrCoreBasket.hpp"

namespace msw {

    // The basketball characters' collision sizes (BasketF
    // game/basket/CChrCoreBasket.cpp; names from the symbols). The M
    // (middle) sizes are constants; the S (small) ones are the M ones times
    // getBodyScaleS(); the L (large) ones are constants again. They fill
    // CChrCore::PRM_BODY_SIZE for the three body size classes.
    class CC_DATA_BASKET {
        public:
            static float getCcBodyBaseXSizeM();
            static float getCcBodyBaseZSizeM();
            static float getCcBodyCatchXSizeM();
            static float getCcBodyCatchZSizeM();
            static float getCcBodyDBXSizeM();
            static float getCcBodyDBZSizeM();
            static float getCcBodyBWXSizeM();
            static float getCcBodyBWZSizeM();
            static float getCcBodyLieXSizeM();
            static float getCcBodyLieZSizeM();
            static float getCcBodyDBPosShiftM();
            static float getCcBodyCatch2XSizeM();
            static float getCcBodyCatch2ZSizeM();
            static float getCcBodyCatch2OffsetM();
            static float getBodyScaleS();
            static float getCcBodyBaseXSizeS();
            static float getCcBodyBaseZSizeS();
            static float getCcBodyCatchXSizeS();
            static float getCcBodyCatchZSizeS();
            static float getCcBodyDBXSizeS();
            static float getCcBodyDBZSizeS();
            static float getCcBodyBWXSizeS();
            static float getCcBodyBWZSizeS();
            static float getCcBodyLieXSizeS();
            static float getCcBodyLieZSizeS();
            static float getCcBodyDBPosShiftS();
            static float getCcBodyCatch2XSizeS();
            static float getCcBodyCatch2ZSizeS();
            static float getCcBodyCatch2OffsetS();
            static float getCcBodyBaseXSizeL();
            static float getCcBodyBaseZSizeL();
            static float getCcBodyCatchXSizeL();
            static float getCcBodyCatchZSizeL();
            static float getCcBodyDBXSizeL();
            static float getCcBodyDBZSizeL();
            static float getCcBodyBWXSizeL();
            static float getCcBodyBWZSizeL();
            static float getCcBodyLieXSizeL();
            static float getCcBodyLieZSizeL();
            static float getCcBodyDBPosShiftL();
            static float getCcBodyCatch2XSizeL();
            static float getCcBodyCatch2ZSizeL();
            static float getCcBodyCatch2OffsetL();
    };

    // The basketball characters' second parameter blocks (BasketF
    // game/basket/CChrCoreBasket.cpp). CHR_CORE_BASKET::CC_PARAM_BASKET2
    // (CChrCoreBasket.hpp) holds the defaults. Each character has a
    // cc_data_basket_<Name>::CC_PARAM_BASKET_<Name> whose constructor writes
    // every field again with the character's values, and one class per game
    // level (EASY ... EXPERT, each with a _STAR variant for the star player
    // derived from it) that changes a few of them.
    // CChrCoreBasket<Name>::NewCcParam2_ picks the class from the game level
    // and the star flag; all are 0x278 bytes.
    namespace cc_data_basket_Mario {
        class CC_PARAM_BASKET_Mario : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Mario();
        };
        class CC_PARAM_BASKET_Mario_EASY : public CC_PARAM_BASKET_Mario {
            public:
                CC_PARAM_BASKET_Mario_EASY();
        };
        class CC_PARAM_BASKET_Mario_EASY_STAR : public CC_PARAM_BASKET_Mario_EASY {
            public:
                CC_PARAM_BASKET_Mario_EASY_STAR();
        };
        class CC_PARAM_BASKET_Mario_NORMAL : public CC_PARAM_BASKET_Mario {
            public:
                CC_PARAM_BASKET_Mario_NORMAL();
        };
        class CC_PARAM_BASKET_Mario_NORMAL_STAR : public CC_PARAM_BASKET_Mario_NORMAL {
            public:
                CC_PARAM_BASKET_Mario_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Mario_HARD : public CC_PARAM_BASKET_Mario {
            public:
                CC_PARAM_BASKET_Mario_HARD();
        };
        class CC_PARAM_BASKET_Mario_HARD_STAR : public CC_PARAM_BASKET_Mario_HARD {
            public:
                CC_PARAM_BASKET_Mario_HARD_STAR();
        };
        class CC_PARAM_BASKET_Mario_EXPERT : public CC_PARAM_BASKET_Mario {
            public:
                CC_PARAM_BASKET_Mario_EXPERT();
        };
        class CC_PARAM_BASKET_Mario_EXPERT_STAR : public CC_PARAM_BASKET_Mario_EXPERT {
            public:
                CC_PARAM_BASKET_Mario_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Luigi {
        class CC_PARAM_BASKET_Luigi : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Luigi();
        };
        class CC_PARAM_BASKET_Luigi_EASY : public CC_PARAM_BASKET_Luigi {
            public:
                CC_PARAM_BASKET_Luigi_EASY();
        };
        class CC_PARAM_BASKET_Luigi_EASY_STAR : public CC_PARAM_BASKET_Luigi_EASY {
            public:
                CC_PARAM_BASKET_Luigi_EASY_STAR();
        };
        class CC_PARAM_BASKET_Luigi_NORMAL : public CC_PARAM_BASKET_Luigi {
            public:
                CC_PARAM_BASKET_Luigi_NORMAL();
        };
        class CC_PARAM_BASKET_Luigi_NORMAL_STAR : public CC_PARAM_BASKET_Luigi_NORMAL {
            public:
                CC_PARAM_BASKET_Luigi_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Luigi_HARD : public CC_PARAM_BASKET_Luigi {
            public:
                CC_PARAM_BASKET_Luigi_HARD();
        };
        class CC_PARAM_BASKET_Luigi_HARD_STAR : public CC_PARAM_BASKET_Luigi_HARD {
            public:
                CC_PARAM_BASKET_Luigi_HARD_STAR();
        };
        class CC_PARAM_BASKET_Luigi_EXPERT : public CC_PARAM_BASKET_Luigi {
            public:
                CC_PARAM_BASKET_Luigi_EXPERT();
        };
        class CC_PARAM_BASKET_Luigi_EXPERT_STAR : public CC_PARAM_BASKET_Luigi_EXPERT {
            public:
                CC_PARAM_BASKET_Luigi_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Peach {
        class CC_PARAM_BASKET_Peach : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Peach();
        };
        class CC_PARAM_BASKET_Peach_EASY : public CC_PARAM_BASKET_Peach {
            public:
                CC_PARAM_BASKET_Peach_EASY();
        };
        class CC_PARAM_BASKET_Peach_EASY_STAR : public CC_PARAM_BASKET_Peach_EASY {
            public:
                CC_PARAM_BASKET_Peach_EASY_STAR();
        };
        class CC_PARAM_BASKET_Peach_NORMAL : public CC_PARAM_BASKET_Peach {
            public:
                CC_PARAM_BASKET_Peach_NORMAL();
        };
        class CC_PARAM_BASKET_Peach_NORMAL_STAR : public CC_PARAM_BASKET_Peach_NORMAL {
            public:
                CC_PARAM_BASKET_Peach_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Peach_HARD : public CC_PARAM_BASKET_Peach {
            public:
                CC_PARAM_BASKET_Peach_HARD();
        };
        class CC_PARAM_BASKET_Peach_HARD_STAR : public CC_PARAM_BASKET_Peach_HARD {
            public:
                CC_PARAM_BASKET_Peach_HARD_STAR();
        };
        class CC_PARAM_BASKET_Peach_EXPERT : public CC_PARAM_BASKET_Peach {
            public:
                CC_PARAM_BASKET_Peach_EXPERT();
        };
        class CC_PARAM_BASKET_Peach_EXPERT_STAR : public CC_PARAM_BASKET_Peach_EXPERT {
            public:
                CC_PARAM_BASKET_Peach_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Daisy {
        class CC_PARAM_BASKET_Daisy : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Daisy();
        };
        class CC_PARAM_BASKET_Daisy_EASY : public CC_PARAM_BASKET_Daisy {
            public:
                CC_PARAM_BASKET_Daisy_EASY();
        };
        class CC_PARAM_BASKET_Daisy_EASY_STAR : public CC_PARAM_BASKET_Daisy_EASY {
            public:
                CC_PARAM_BASKET_Daisy_EASY_STAR();
        };
        class CC_PARAM_BASKET_Daisy_NORMAL : public CC_PARAM_BASKET_Daisy {
            public:
                CC_PARAM_BASKET_Daisy_NORMAL();
        };
        class CC_PARAM_BASKET_Daisy_NORMAL_STAR : public CC_PARAM_BASKET_Daisy_NORMAL {
            public:
                CC_PARAM_BASKET_Daisy_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Daisy_HARD : public CC_PARAM_BASKET_Daisy {
            public:
                CC_PARAM_BASKET_Daisy_HARD();
        };
        class CC_PARAM_BASKET_Daisy_HARD_STAR : public CC_PARAM_BASKET_Daisy_HARD {
            public:
                CC_PARAM_BASKET_Daisy_HARD_STAR();
        };
        class CC_PARAM_BASKET_Daisy_EXPERT : public CC_PARAM_BASKET_Daisy {
            public:
                CC_PARAM_BASKET_Daisy_EXPERT();
        };
        class CC_PARAM_BASKET_Daisy_EXPERT_STAR : public CC_PARAM_BASKET_Daisy_EXPERT {
            public:
                CC_PARAM_BASKET_Daisy_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Yoshi {
        class CC_PARAM_BASKET_Yoshi : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Yoshi();
        };
        class CC_PARAM_BASKET_Yoshi_EASY : public CC_PARAM_BASKET_Yoshi {
            public:
                CC_PARAM_BASKET_Yoshi_EASY();
        };
        class CC_PARAM_BASKET_Yoshi_EASY_STAR : public CC_PARAM_BASKET_Yoshi_EASY {
            public:
                CC_PARAM_BASKET_Yoshi_EASY_STAR();
        };
        class CC_PARAM_BASKET_Yoshi_NORMAL : public CC_PARAM_BASKET_Yoshi {
            public:
                CC_PARAM_BASKET_Yoshi_NORMAL();
        };
        class CC_PARAM_BASKET_Yoshi_NORMAL_STAR : public CC_PARAM_BASKET_Yoshi_NORMAL {
            public:
                CC_PARAM_BASKET_Yoshi_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Yoshi_HARD : public CC_PARAM_BASKET_Yoshi {
            public:
                CC_PARAM_BASKET_Yoshi_HARD();
        };
        class CC_PARAM_BASKET_Yoshi_HARD_STAR : public CC_PARAM_BASKET_Yoshi_HARD {
            public:
                CC_PARAM_BASKET_Yoshi_HARD_STAR();
        };
        class CC_PARAM_BASKET_Yoshi_EXPERT : public CC_PARAM_BASKET_Yoshi {
            public:
                CC_PARAM_BASKET_Yoshi_EXPERT();
        };
        class CC_PARAM_BASKET_Yoshi_EXPERT_STAR : public CC_PARAM_BASKET_Yoshi_EXPERT {
            public:
                CC_PARAM_BASKET_Yoshi_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Wario {
        class CC_PARAM_BASKET_Wario : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Wario();
        };
        class CC_PARAM_BASKET_Wario_EASY : public CC_PARAM_BASKET_Wario {
            public:
                CC_PARAM_BASKET_Wario_EASY();
        };
        class CC_PARAM_BASKET_Wario_EASY_STAR : public CC_PARAM_BASKET_Wario_EASY {
            public:
                CC_PARAM_BASKET_Wario_EASY_STAR();
        };
        class CC_PARAM_BASKET_Wario_NORMAL : public CC_PARAM_BASKET_Wario {
            public:
                CC_PARAM_BASKET_Wario_NORMAL();
        };
        class CC_PARAM_BASKET_Wario_NORMAL_STAR : public CC_PARAM_BASKET_Wario_NORMAL {
            public:
                CC_PARAM_BASKET_Wario_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Wario_HARD : public CC_PARAM_BASKET_Wario {
            public:
                CC_PARAM_BASKET_Wario_HARD();
        };
        class CC_PARAM_BASKET_Wario_HARD_STAR : public CC_PARAM_BASKET_Wario_HARD {
            public:
                CC_PARAM_BASKET_Wario_HARD_STAR();
        };
        class CC_PARAM_BASKET_Wario_EXPERT : public CC_PARAM_BASKET_Wario {
            public:
                CC_PARAM_BASKET_Wario_EXPERT();
        };
        class CC_PARAM_BASKET_Wario_EXPERT_STAR : public CC_PARAM_BASKET_Wario_EXPERT {
            public:
                CC_PARAM_BASKET_Wario_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Waluigi {
        class CC_PARAM_BASKET_Waluigi : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Waluigi();
        };
        class CC_PARAM_BASKET_Waluigi_EASY : public CC_PARAM_BASKET_Waluigi {
            public:
                CC_PARAM_BASKET_Waluigi_EASY();
        };
        class CC_PARAM_BASKET_Waluigi_EASY_STAR : public CC_PARAM_BASKET_Waluigi_EASY {
            public:
                CC_PARAM_BASKET_Waluigi_EASY_STAR();
        };
        class CC_PARAM_BASKET_Waluigi_NORMAL : public CC_PARAM_BASKET_Waluigi {
            public:
                CC_PARAM_BASKET_Waluigi_NORMAL();
        };
        class CC_PARAM_BASKET_Waluigi_NORMAL_STAR : public CC_PARAM_BASKET_Waluigi_NORMAL {
            public:
                CC_PARAM_BASKET_Waluigi_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Waluigi_HARD : public CC_PARAM_BASKET_Waluigi {
            public:
                CC_PARAM_BASKET_Waluigi_HARD();
        };
        class CC_PARAM_BASKET_Waluigi_HARD_STAR : public CC_PARAM_BASKET_Waluigi_HARD {
            public:
                CC_PARAM_BASKET_Waluigi_HARD_STAR();
        };
        class CC_PARAM_BASKET_Waluigi_EXPERT : public CC_PARAM_BASKET_Waluigi {
            public:
                CC_PARAM_BASKET_Waluigi_EXPERT();
        };
        class CC_PARAM_BASKET_Waluigi_EXPERT_STAR : public CC_PARAM_BASKET_Waluigi_EXPERT {
            public:
                CC_PARAM_BASKET_Waluigi_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Donkey {
        class CC_PARAM_BASKET_Donkey : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Donkey();
        };
        class CC_PARAM_BASKET_Donkey_EASY : public CC_PARAM_BASKET_Donkey {
            public:
                CC_PARAM_BASKET_Donkey_EASY();
        };
        class CC_PARAM_BASKET_Donkey_EASY_STAR : public CC_PARAM_BASKET_Donkey_EASY {
            public:
                CC_PARAM_BASKET_Donkey_EASY_STAR();
        };
        class CC_PARAM_BASKET_Donkey_NORMAL : public CC_PARAM_BASKET_Donkey {
            public:
                CC_PARAM_BASKET_Donkey_NORMAL();
        };
        class CC_PARAM_BASKET_Donkey_NORMAL_STAR : public CC_PARAM_BASKET_Donkey_NORMAL {
            public:
                CC_PARAM_BASKET_Donkey_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Donkey_HARD : public CC_PARAM_BASKET_Donkey {
            public:
                CC_PARAM_BASKET_Donkey_HARD();
        };
        class CC_PARAM_BASKET_Donkey_HARD_STAR : public CC_PARAM_BASKET_Donkey_HARD {
            public:
                CC_PARAM_BASKET_Donkey_HARD_STAR();
        };
        class CC_PARAM_BASKET_Donkey_EXPERT : public CC_PARAM_BASKET_Donkey {
            public:
                CC_PARAM_BASKET_Donkey_EXPERT();
        };
        class CC_PARAM_BASKET_Donkey_EXPERT_STAR : public CC_PARAM_BASKET_Donkey_EXPERT {
            public:
                CC_PARAM_BASKET_Donkey_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Diddy {
        class CC_PARAM_BASKET_Diddy : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Diddy();
        };
        class CC_PARAM_BASKET_Diddy_EASY : public CC_PARAM_BASKET_Diddy {
            public:
                CC_PARAM_BASKET_Diddy_EASY();
        };
        class CC_PARAM_BASKET_Diddy_EASY_STAR : public CC_PARAM_BASKET_Diddy_EASY {
            public:
                CC_PARAM_BASKET_Diddy_EASY_STAR();
        };
        class CC_PARAM_BASKET_Diddy_NORMAL : public CC_PARAM_BASKET_Diddy {
            public:
                CC_PARAM_BASKET_Diddy_NORMAL();
        };
        class CC_PARAM_BASKET_Diddy_NORMAL_STAR : public CC_PARAM_BASKET_Diddy_NORMAL {
            public:
                CC_PARAM_BASKET_Diddy_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Diddy_HARD : public CC_PARAM_BASKET_Diddy {
            public:
                CC_PARAM_BASKET_Diddy_HARD();
        };
        class CC_PARAM_BASKET_Diddy_HARD_STAR : public CC_PARAM_BASKET_Diddy_HARD {
            public:
                CC_PARAM_BASKET_Diddy_HARD_STAR();
        };
        class CC_PARAM_BASKET_Diddy_EXPERT : public CC_PARAM_BASKET_Diddy {
            public:
                CC_PARAM_BASKET_Diddy_EXPERT();
        };
        class CC_PARAM_BASKET_Diddy_EXPERT_STAR : public CC_PARAM_BASKET_Diddy_EXPERT {
            public:
                CC_PARAM_BASKET_Diddy_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Koopa {
        class CC_PARAM_BASKET_Koopa : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Koopa();
        };
        class CC_PARAM_BASKET_Koopa_EASY : public CC_PARAM_BASKET_Koopa {
            public:
                CC_PARAM_BASKET_Koopa_EASY();
        };
        class CC_PARAM_BASKET_Koopa_EASY_STAR : public CC_PARAM_BASKET_Koopa_EASY {
            public:
                CC_PARAM_BASKET_Koopa_EASY_STAR();
        };
        class CC_PARAM_BASKET_Koopa_NORMAL : public CC_PARAM_BASKET_Koopa {
            public:
                CC_PARAM_BASKET_Koopa_NORMAL();
        };
        class CC_PARAM_BASKET_Koopa_NORMAL_STAR : public CC_PARAM_BASKET_Koopa_NORMAL {
            public:
                CC_PARAM_BASKET_Koopa_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Koopa_HARD : public CC_PARAM_BASKET_Koopa {
            public:
                CC_PARAM_BASKET_Koopa_HARD();
        };
        class CC_PARAM_BASKET_Koopa_HARD_STAR : public CC_PARAM_BASKET_Koopa_HARD {
            public:
                CC_PARAM_BASKET_Koopa_HARD_STAR();
        };
        class CC_PARAM_BASKET_Koopa_EXPERT : public CC_PARAM_BASKET_Koopa {
            public:
                CC_PARAM_BASKET_Koopa_EXPERT();
        };
        class CC_PARAM_BASKET_Koopa_EXPERT_STAR : public CC_PARAM_BASKET_Koopa_EXPERT {
            public:
                CC_PARAM_BASKET_Koopa_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_KoopaJr {
        class CC_PARAM_BASKET_KoopaJr : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_KoopaJr();
        };
        class CC_PARAM_BASKET_KoopaJr_EASY : public CC_PARAM_BASKET_KoopaJr {
            public:
                CC_PARAM_BASKET_KoopaJr_EASY();
        };
        class CC_PARAM_BASKET_KoopaJr_EASY_STAR : public CC_PARAM_BASKET_KoopaJr_EASY {
            public:
                CC_PARAM_BASKET_KoopaJr_EASY_STAR();
        };
        class CC_PARAM_BASKET_KoopaJr_NORMAL : public CC_PARAM_BASKET_KoopaJr {
            public:
                CC_PARAM_BASKET_KoopaJr_NORMAL();
        };
        class CC_PARAM_BASKET_KoopaJr_NORMAL_STAR : public CC_PARAM_BASKET_KoopaJr_NORMAL {
            public:
                CC_PARAM_BASKET_KoopaJr_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_KoopaJr_HARD : public CC_PARAM_BASKET_KoopaJr {
            public:
                CC_PARAM_BASKET_KoopaJr_HARD();
        };
        class CC_PARAM_BASKET_KoopaJr_HARD_STAR : public CC_PARAM_BASKET_KoopaJr_HARD {
            public:
                CC_PARAM_BASKET_KoopaJr_HARD_STAR();
        };
        class CC_PARAM_BASKET_KoopaJr_EXPERT : public CC_PARAM_BASKET_KoopaJr {
            public:
                CC_PARAM_BASKET_KoopaJr_EXPERT();
        };
        class CC_PARAM_BASKET_KoopaJr_EXPERT_STAR : public CC_PARAM_BASKET_KoopaJr_EXPERT {
            public:
                CC_PARAM_BASKET_KoopaJr_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Kinopio {
        class CC_PARAM_BASKET_Kinopio : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Kinopio();
        };
        class CC_PARAM_BASKET_Kinopio_EASY : public CC_PARAM_BASKET_Kinopio {
            public:
                CC_PARAM_BASKET_Kinopio_EASY();
        };
        class CC_PARAM_BASKET_Kinopio_EASY_STAR : public CC_PARAM_BASKET_Kinopio_EASY {
            public:
                CC_PARAM_BASKET_Kinopio_EASY_STAR();
        };
        class CC_PARAM_BASKET_Kinopio_NORMAL : public CC_PARAM_BASKET_Kinopio {
            public:
                CC_PARAM_BASKET_Kinopio_NORMAL();
        };
        class CC_PARAM_BASKET_Kinopio_NORMAL_STAR : public CC_PARAM_BASKET_Kinopio_NORMAL {
            public:
                CC_PARAM_BASKET_Kinopio_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Kinopio_HARD : public CC_PARAM_BASKET_Kinopio {
            public:
                CC_PARAM_BASKET_Kinopio_HARD();
        };
        class CC_PARAM_BASKET_Kinopio_HARD_STAR : public CC_PARAM_BASKET_Kinopio_HARD {
            public:
                CC_PARAM_BASKET_Kinopio_HARD_STAR();
        };
        class CC_PARAM_BASKET_Kinopio_EXPERT : public CC_PARAM_BASKET_Kinopio {
            public:
                CC_PARAM_BASKET_Kinopio_EXPERT();
        };
        class CC_PARAM_BASKET_Kinopio_EXPERT_STAR : public CC_PARAM_BASKET_Kinopio_EXPERT {
            public:
                CC_PARAM_BASKET_Kinopio_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Moogle {
        class CC_PARAM_BASKET_Moogle : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Moogle();
        };
        class CC_PARAM_BASKET_Moogle_EASY : public CC_PARAM_BASKET_Moogle {
            public:
                CC_PARAM_BASKET_Moogle_EASY();
        };
        class CC_PARAM_BASKET_Moogle_EASY_STAR : public CC_PARAM_BASKET_Moogle_EASY {
            public:
                CC_PARAM_BASKET_Moogle_EASY_STAR();
        };
        class CC_PARAM_BASKET_Moogle_NORMAL : public CC_PARAM_BASKET_Moogle {
            public:
                CC_PARAM_BASKET_Moogle_NORMAL();
        };
        class CC_PARAM_BASKET_Moogle_NORMAL_STAR : public CC_PARAM_BASKET_Moogle_NORMAL {
            public:
                CC_PARAM_BASKET_Moogle_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Moogle_HARD : public CC_PARAM_BASKET_Moogle {
            public:
                CC_PARAM_BASKET_Moogle_HARD();
        };
        class CC_PARAM_BASKET_Moogle_HARD_STAR : public CC_PARAM_BASKET_Moogle_HARD {
            public:
                CC_PARAM_BASKET_Moogle_HARD_STAR();
        };
        class CC_PARAM_BASKET_Moogle_EXPERT : public CC_PARAM_BASKET_Moogle {
            public:
                CC_PARAM_BASKET_Moogle_EXPERT();
        };
        class CC_PARAM_BASKET_Moogle_EXPERT_STAR : public CC_PARAM_BASKET_Moogle_EXPERT {
            public:
                CC_PARAM_BASKET_Moogle_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Saboten {
        class CC_PARAM_BASKET_Saboten : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Saboten();
        };
        class CC_PARAM_BASKET_Saboten_EASY : public CC_PARAM_BASKET_Saboten {
            public:
                CC_PARAM_BASKET_Saboten_EASY();
        };
        class CC_PARAM_BASKET_Saboten_EASY_STAR : public CC_PARAM_BASKET_Saboten_EASY {
            public:
                CC_PARAM_BASKET_Saboten_EASY_STAR();
        };
        class CC_PARAM_BASKET_Saboten_NORMAL : public CC_PARAM_BASKET_Saboten {
            public:
                CC_PARAM_BASKET_Saboten_NORMAL();
        };
        class CC_PARAM_BASKET_Saboten_NORMAL_STAR : public CC_PARAM_BASKET_Saboten_NORMAL {
            public:
                CC_PARAM_BASKET_Saboten_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Saboten_HARD : public CC_PARAM_BASKET_Saboten {
            public:
                CC_PARAM_BASKET_Saboten_HARD();
        };
        class CC_PARAM_BASKET_Saboten_HARD_STAR : public CC_PARAM_BASKET_Saboten_HARD {
            public:
                CC_PARAM_BASKET_Saboten_HARD_STAR();
        };
        class CC_PARAM_BASKET_Saboten_EXPERT : public CC_PARAM_BASKET_Saboten {
            public:
                CC_PARAM_BASKET_Saboten_EXPERT();
        };
        class CC_PARAM_BASKET_Saboten_EXPERT_STAR : public CC_PARAM_BASKET_Saboten_EXPERT {
            public:
                CC_PARAM_BASKET_Saboten_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Ninja {
        class CC_PARAM_BASKET_Ninja : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Ninja();
        };
        class CC_PARAM_BASKET_Ninja_EASY : public CC_PARAM_BASKET_Ninja {
            public:
                CC_PARAM_BASKET_Ninja_EASY();
        };
        class CC_PARAM_BASKET_Ninja_EASY_STAR : public CC_PARAM_BASKET_Ninja_EASY {
            public:
                CC_PARAM_BASKET_Ninja_EASY_STAR();
        };
        class CC_PARAM_BASKET_Ninja_NORMAL : public CC_PARAM_BASKET_Ninja {
            public:
                CC_PARAM_BASKET_Ninja_NORMAL();
        };
        class CC_PARAM_BASKET_Ninja_NORMAL_STAR : public CC_PARAM_BASKET_Ninja_NORMAL {
            public:
                CC_PARAM_BASKET_Ninja_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Ninja_HARD : public CC_PARAM_BASKET_Ninja {
            public:
                CC_PARAM_BASKET_Ninja_HARD();
        };
        class CC_PARAM_BASKET_Ninja_HARD_STAR : public CC_PARAM_BASKET_Ninja_HARD {
            public:
                CC_PARAM_BASKET_Ninja_HARD_STAR();
        };
        class CC_PARAM_BASKET_Ninja_EXPERT : public CC_PARAM_BASKET_Ninja {
            public:
                CC_PARAM_BASKET_Ninja_EXPERT();
        };
        class CC_PARAM_BASKET_Ninja_EXPERT_STAR : public CC_PARAM_BASKET_Ninja_EXPERT {
            public:
                CC_PARAM_BASKET_Ninja_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Siroma {
        class CC_PARAM_BASKET_Siroma : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Siroma();
        };
        class CC_PARAM_BASKET_Siroma_EASY : public CC_PARAM_BASKET_Siroma {
            public:
                CC_PARAM_BASKET_Siroma_EASY();
        };
        class CC_PARAM_BASKET_Siroma_EASY_STAR : public CC_PARAM_BASKET_Siroma_EASY {
            public:
                CC_PARAM_BASKET_Siroma_EASY_STAR();
        };
        class CC_PARAM_BASKET_Siroma_NORMAL : public CC_PARAM_BASKET_Siroma {
            public:
                CC_PARAM_BASKET_Siroma_NORMAL();
        };
        class CC_PARAM_BASKET_Siroma_NORMAL_STAR : public CC_PARAM_BASKET_Siroma_NORMAL {
            public:
                CC_PARAM_BASKET_Siroma_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Siroma_HARD : public CC_PARAM_BASKET_Siroma {
            public:
                CC_PARAM_BASKET_Siroma_HARD();
        };
        class CC_PARAM_BASKET_Siroma_HARD_STAR : public CC_PARAM_BASKET_Siroma_HARD {
            public:
                CC_PARAM_BASKET_Siroma_HARD_STAR();
        };
        class CC_PARAM_BASKET_Siroma_EXPERT : public CC_PARAM_BASKET_Siroma {
            public:
                CC_PARAM_BASKET_Siroma_EXPERT();
        };
        class CC_PARAM_BASKET_Siroma_EXPERT_STAR : public CC_PARAM_BASKET_Siroma_EXPERT {
            public:
                CC_PARAM_BASKET_Siroma_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Slime {
        class CC_PARAM_BASKET_Slime : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Slime();
        };
        class CC_PARAM_BASKET_Slime_EASY : public CC_PARAM_BASKET_Slime {
            public:
                CC_PARAM_BASKET_Slime_EASY();
        };
        class CC_PARAM_BASKET_Slime_EASY_STAR : public CC_PARAM_BASKET_Slime_EASY {
            public:
                CC_PARAM_BASKET_Slime_EASY_STAR();
        };
        class CC_PARAM_BASKET_Slime_NORMAL : public CC_PARAM_BASKET_Slime {
            public:
                CC_PARAM_BASKET_Slime_NORMAL();
        };
        class CC_PARAM_BASKET_Slime_NORMAL_STAR : public CC_PARAM_BASKET_Slime_NORMAL {
            public:
                CC_PARAM_BASKET_Slime_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Slime_HARD : public CC_PARAM_BASKET_Slime {
            public:
                CC_PARAM_BASKET_Slime_HARD();
        };
        class CC_PARAM_BASKET_Slime_HARD_STAR : public CC_PARAM_BASKET_Slime_HARD {
            public:
                CC_PARAM_BASKET_Slime_HARD_STAR();
        };
        class CC_PARAM_BASKET_Slime_EXPERT : public CC_PARAM_BASKET_Slime {
            public:
                CC_PARAM_BASKET_Slime_EXPERT();
        };
        class CC_PARAM_BASKET_Slime_EXPERT_STAR : public CC_PARAM_BASKET_Slime_EXPERT {
            public:
                CC_PARAM_BASKET_Slime_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Kuroma {
        class CC_PARAM_BASKET_Kuroma : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Kuroma();
        };
        class CC_PARAM_BASKET_Kuroma_EASY : public CC_PARAM_BASKET_Kuroma {
            public:
                CC_PARAM_BASKET_Kuroma_EASY();
        };
        class CC_PARAM_BASKET_Kuroma_EASY_STAR : public CC_PARAM_BASKET_Kuroma_EASY {
            public:
                CC_PARAM_BASKET_Kuroma_EASY_STAR();
        };
        class CC_PARAM_BASKET_Kuroma_NORMAL : public CC_PARAM_BASKET_Kuroma {
            public:
                CC_PARAM_BASKET_Kuroma_NORMAL();
        };
        class CC_PARAM_BASKET_Kuroma_NORMAL_STAR : public CC_PARAM_BASKET_Kuroma_NORMAL {
            public:
                CC_PARAM_BASKET_Kuroma_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Kuroma_HARD : public CC_PARAM_BASKET_Kuroma {
            public:
                CC_PARAM_BASKET_Kuroma_HARD();
        };
        class CC_PARAM_BASKET_Kuroma_HARD_STAR : public CC_PARAM_BASKET_Kuroma_HARD {
            public:
                CC_PARAM_BASKET_Kuroma_HARD_STAR();
        };
        class CC_PARAM_BASKET_Kuroma_EXPERT : public CC_PARAM_BASKET_Kuroma {
            public:
                CC_PARAM_BASKET_Kuroma_EXPERT();
        };
        class CC_PARAM_BASKET_Kuroma_EXPERT_STAR : public CC_PARAM_BASKET_Kuroma_EXPERT {
            public:
                CC_PARAM_BASKET_Kuroma_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Mii00 {
        class CC_PARAM_BASKET_Mii00 : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Mii00();
        };
        class CC_PARAM_BASKET_Mii00_EASY : public CC_PARAM_BASKET_Mii00 {
            public:
                CC_PARAM_BASKET_Mii00_EASY();
        };
        class CC_PARAM_BASKET_Mii00_EASY_STAR : public CC_PARAM_BASKET_Mii00_EASY {
            public:
                CC_PARAM_BASKET_Mii00_EASY_STAR();
        };
        class CC_PARAM_BASKET_Mii00_NORMAL : public CC_PARAM_BASKET_Mii00 {
            public:
                CC_PARAM_BASKET_Mii00_NORMAL();
        };
        class CC_PARAM_BASKET_Mii00_NORMAL_STAR : public CC_PARAM_BASKET_Mii00_NORMAL {
            public:
                CC_PARAM_BASKET_Mii00_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Mii00_HARD : public CC_PARAM_BASKET_Mii00 {
            public:
                CC_PARAM_BASKET_Mii00_HARD();
        };
        class CC_PARAM_BASKET_Mii00_HARD_STAR : public CC_PARAM_BASKET_Mii00_HARD {
            public:
                CC_PARAM_BASKET_Mii00_HARD_STAR();
        };
        class CC_PARAM_BASKET_Mii00_EXPERT : public CC_PARAM_BASKET_Mii00 {
            public:
                CC_PARAM_BASKET_Mii00_EXPERT();
        };
        class CC_PARAM_BASKET_Mii00_EXPERT_STAR : public CC_PARAM_BASKET_Mii00_EXPERT {
            public:
                CC_PARAM_BASKET_Mii00_EXPERT_STAR();
        };
    }

    namespace cc_data_basket_Mii01 {
        class CC_PARAM_BASKET_Mii01 : public CHR_CORE_BASKET::CC_PARAM_BASKET2 {
            public:
                CC_PARAM_BASKET_Mii01();
        };
        class CC_PARAM_BASKET_Mii01_EASY : public CC_PARAM_BASKET_Mii01 {
            public:
                CC_PARAM_BASKET_Mii01_EASY();
        };
        class CC_PARAM_BASKET_Mii01_EASY_STAR : public CC_PARAM_BASKET_Mii01_EASY {
            public:
                CC_PARAM_BASKET_Mii01_EASY_STAR();
        };
        class CC_PARAM_BASKET_Mii01_NORMAL : public CC_PARAM_BASKET_Mii01 {
            public:
                CC_PARAM_BASKET_Mii01_NORMAL();
        };
        class CC_PARAM_BASKET_Mii01_NORMAL_STAR : public CC_PARAM_BASKET_Mii01_NORMAL {
            public:
                CC_PARAM_BASKET_Mii01_NORMAL_STAR();
        };
        class CC_PARAM_BASKET_Mii01_HARD : public CC_PARAM_BASKET_Mii01 {
            public:
                CC_PARAM_BASKET_Mii01_HARD();
        };
        class CC_PARAM_BASKET_Mii01_HARD_STAR : public CC_PARAM_BASKET_Mii01_HARD {
            public:
                CC_PARAM_BASKET_Mii01_HARD_STAR();
        };
        class CC_PARAM_BASKET_Mii01_EXPERT : public CC_PARAM_BASKET_Mii01 {
            public:
                CC_PARAM_BASKET_Mii01_EXPERT();
        };
        class CC_PARAM_BASKET_Mii01_EXPERT_STAR : public CC_PARAM_BASKET_Mii01_EXPERT {
            public:
                CC_PARAM_BASKET_Mii01_EXPERT_STAR();
        };
    }

}

#endif /* !MSW_CC_PARAM_BASKET_HPP_ */
