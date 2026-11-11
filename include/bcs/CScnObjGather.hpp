#ifndef BCS_CSCNOBJGATHER_HPP_
#define BCS_CSCNOBJGATHER_HPP_

namespace bcs {

    class CScnObj;
    class CFrustum;

    // Collects the scene objects to draw in a frame into two caller-owned
    // arrays (the opaque and the translucent pass), sorts them and chains
    // them through CScnObj's gather links. Size 0x24, vptr at 0x00 (vtable
    // lbl_80466A7C: the destructor alone). A DOL class.
    class CScnObjGather {
        public:
            // fn_80043898 (parameter names guessed).
            CScnObjGather(CScnObj **ppOpa, CScnObj **ppXlu, unsigned long nMax);
            // fn_800438CC.
            virtual ~CScnObjGather();

            // (parameter meaning guessed) Adds pObj to the opaque and/or
            // the translucent list, culled against pFrustum. Returns 2 when
            // the object is culled; bcs::CScnGroup then skips its children,
            // passes the frustum on for 0 and none for 1.
            int Add(CScnObj *pObj, int bOpa, int bXlu, const CFrustum *pFrustum);

            // fn_80044500 (name guessed). Sorts both lists and links them.
            void Sort();
            // fn_800446F8 and fn_80044888 (names guessed). Runs task 6 / 7
            // of ObjProc on every object of the opaque / translucent list
            // and clears the flag Add set on it and its joint list.
            void DrawOpa(void *pArg, unsigned long nArg);
            void DrawXlu(void *pArg, unsigned long nArg);

        private:
            // fn_80044568 and fn_80044630 (names guessed). The sort order:
            // draw priority, then the sort value, then the address.
            static int CompareOpa(const CScnObj *pA, const CScnObj *pB);
            static int CompareXlu(const CScnObj *pA, const CScnObj *pB);

            // fn_80043BD0, fn_80043C74, fn_800447E4 and fn_80044974 (names
            // guessed). Set or clear the flag that marks pObj and its joint
            // list as gathered for the opaque (0x100000) / translucent
            // (0x200000) pass; ExecDelayDeleteObject keeps such objects.
            static void SetGatherOpaJoint(CScnObj *pObj);
            static void SetGatherXluJoint(CScnObj *pObj);
            static void ClearGatherOpaJoint(CScnObj *pObj);
            static void ClearGatherXluJoint(CScnObj *pObj);

            // (name guessed) Inline in the constructor and the destructor:
            // sets the arrays and empties the lists.
            void Init(CScnObj **ppOpa, CScnObj **ppXlu, unsigned long nMax)
            {
                m_ppOpa = ppOpa;
                m_ppXlu = ppXlu;
                m_nMax = nMax;
                m_nOpaNum = 0;
                m_nXluNum = 0;
                m_pOpaHead = 0;
                m_pXluHead = 0;
            }

            // fn_80044A18 (name guessed). Chains each sorted array through
            // the objects' gather links.
            void Link();

            CScnObj **m_ppOpa;          // 0x04
            CScnObj **m_ppXlu;          // 0x08
            unsigned long m_nMax;       // 0x0C
            unsigned long m_nOpaNum;    // 0x10
            unsigned long m_nXluNum;    // 0x14
            CScnObj *m_pOpaHead;        // 0x18
            CScnObj *m_pXluHead;        // 0x1C
            // 0x20 (name guessed): objects whose 0xEE bits meet this mask
            // are culled by Add.
            unsigned long m_nCullMask;
    };
}

#endif /* !BCS_CSCNOBJGATHER_HPP_ */
