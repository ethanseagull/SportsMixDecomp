#ifndef CDATAWORK_HPP_
#define CDATAWORK_HPP_

namespace msw {

    // Only the members used so far. Both functions are empty in the DOL, so
    // only the call sites show that they take an object.
    class CDataWork {
        public:
            void Initialize();
            void Finalize();

            static CDataWork s_Instance;

            // (added for MenuF game/menu/CSelectMenu.cpp; inline, emitted
            // there; names from the symbols) The instance, and a flag kept
            // for the attract demo (meaning guessed).
            static CDataWork *GetInstance() { return &s_Instance; }
            int GetCntInfoDemo() const { return m_bCntInfoDemo; }
            void SetCntInfoDemo(int bDemo) { m_bCntInfoDemo = bDemo; }

        private:
            int m_bCntInfoDemo; // 0x00
    };
}

#endif /* !CDATAWORK_HPP_ */
