#ifndef MENU_CSTATESAVE_HPP_
#define MENU_CSTATESAVE_HPP_

namespace msw {
    namespace menu {

        // Menu state kept across mode changes. Lives in the DOL: one static
        // instance of 0xBB0 bytes (GetInstance returns its address; the
        // static initialiser clears bytes 0 and 1, stores the vptr at 0xBAC
        // and registers the destructor). Only bytes 0 and 1 are used by any
        // code found so far; names are guesses.
        class CStateSave {
            public:
                CStateSave() : m_bActive(0), m_nStartMenuMode(0) {}

                static CStateSave *GetInstance();
                void Initialize();
                void Finalize();

                // MenuF emits weak copies of these two.
                unsigned char GetStartMenuMode() const { return m_nStartMenuMode; }
                void SetStartMenuMode(unsigned char mode) { m_nStartMenuMode = mode; }

            private:
                // The instance GetInstance returns (name guessed).
                static CStateSave s_Instance;

                unsigned char m_bActive;        // 0x000, 1 after Initialize, 0 after Finalize
                unsigned char m_nStartMenuMode; // 0x001
                unsigned char m_Unk002[0xBAA];

            public:
                // vptr at 0xBAC; the vtable has only the destructor.
                virtual ~CStateSave() {}
        };
    }
}

#endif /* !MENU_CSTATESAVE_HPP_ */
