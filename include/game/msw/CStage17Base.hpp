#ifndef MSW_CSTAGE17BASE_HPP_
#define MSW_CSTAGE17BASE_HPP_

namespace bcs {
    class CScnObj;
}

namespace msw {

    // The gimmicks of stage 17 (turntables, trolley, signals, trains),
    // shared by the game modules' stage managers. A DOL class with 55
    // member functions (Stage17_Create, Stage17_Main, GetTurnTable, ...).
    // Only the nested CTurnTable is mapped so far: the outer class has no
    // data members here, so it must not be instantiated, derived from or
    // measured with sizeof until its layout is written.
    class CStage17Base {
        public:
            class CTurnTable;
    };

    // One turntable of stage 17. A DOL class, size 0x54: owner at 0x00,
    // own members, vptr at 0x50 (the vtable has only the destructor, which
    // is empty and inline). Members read from the constructor and Assign;
    // names are guesses.
    class CStage17Base::CTurnTable {
        public:
            CTurnTable(CStage17Base *pOwner);
            void Assign(long nIdx, bcs::CScnObj *pObj);

        private:
            CStage17Base *m_pOwner; // 0x00
            long m_nIdx;            // 0x04, set by Assign
            float m_Unk08;          // 0x08
            float m_Unk0C;          // 0x0C
            float m_Unk10;          // 0x10
            float m_Unk14;          // 0x14
            float m_Unk18;          // 0x18
            float m_Unk1C;          // 0x1C
            float m_Unk20;          // 0x20
            float m_Unk24;          // 0x24
            float m_Unk28;          // 0x28
            float m_Unk2C;          // 0x2C
            float m_Unk30;          // 0x30
            unsigned long m_Unk34;  // 0x34
            long m_Unk38;           // 0x38
            unsigned long m_Unk3C;  // 0x3C
            unsigned long m_Unk40;  // 0x40
            float m_Unk44;          // 0x44
            bcs::CScnObj *m_pObj;   // 0x48, set by Assign
            long m_Unk4C;           // 0x4C

        public:
            virtual ~CTurnTable() {}
    };
}

#endif /* !MSW_CSTAGE17BASE_HPP_ */
