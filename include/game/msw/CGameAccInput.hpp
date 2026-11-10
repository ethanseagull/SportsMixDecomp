#ifndef MSW_CGAMEACCINPUT_HPP_
#define MSW_CGAMEACCINPUT_HPP_

namespace msw {

    // (purpose guessed from the name) The game's use of the accelerometer
    // input. Lives in the DOL. Whether it is a class with static members or
    // a namespace cannot be told from the symbols; a class is a guess.
    class CGameAccInput {
        public:
            static void Initialize();
            static void Finalize();
    };
}

#endif /* !MSW_CGAMEACCINPUT_HPP_ */
