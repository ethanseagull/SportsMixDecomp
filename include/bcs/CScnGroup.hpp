#ifndef BCS_CSCNGROUP_HPP_
#define BCS_CSCNGROUP_HPP_

#include "bcs/CScnObj.hpp"

namespace bcs {

    // A scene object holding other scene objects in a doubly linked list
    // (the links are CScnObj's m_pGroupPrev and m_pGroupNext). A DOL class,
    // size 0x124. Only the members used so far.
    class CScnGroup : public CScnObj {
        public:
            // (parameter meaning not known: the DOL body does not read them)
            CScnGroup(unsigned long, long);
            virtual ~CScnGroup();                                       // 0x08
            virtual long GetObjType() const;                            // 0x0C fn_8004187C, returns 3
            virtual void ObjProc(unsigned long, unsigned long, void *); // 0x1C fn_800434E4
            // Runs pFunc on the group and every child; with bChildFirst
            // the children go first. Returns 1 as soon as a call returns 1.
            virtual int ForEach(ForEachFunc pFunc, void *pArg, int bChildFirst); // 0x20

            // Adds pObj to the group. (return type guessed: the DOL body
            // returns nothing the callers read)
            // (DOL) CScnObj's pure slot 0x10C, so that the group can be
            // created (VolleyF CcSiromaMagicCircle news one); what it does
            // for a group is not known.
            virtual void UpdateFrame(float);

            void AppendChild(CScnObj *pObj);
            // fn_80043174 (name guessed). Takes the last child out of the
            // group and returns it, or null if there is none.
            CScnObj *RemoveLastChild();
            // fn_80043230 (name guessed). Takes the child at nIndex out of
            // the group and returns it; an index past the end takes the
            // last one.
            CScnObj *RemoveChildAt(unsigned long nIndex);
            // fn_8004341C and fn_80043480 (names guessed). The child at
            // nIndex, searched from the nearer end, or null.
            CScnObj *GetChild(unsigned long nIndex) const;
            CScnObj *GetChild(unsigned long nIndex);

        protected:
            // fn_80043520 (name guessed). ObjProc without the IsProcDisabled
            // test; a derived class in the DOL (lbl_804666F8) calls it
            // directly. Tasks 1, 2, 4 and 5 are passed on to the children
            // (1 with the group's world matrix), 0x10001 to 0x10003 keep
            // the child list and the parent.
            void ObjProcGroup(unsigned long nTask, unsigned long nArg, void *pArg);

        private:
            // (name guessed) Unlinks pObj (which must be a child) and tells
            // it that it has no parent any more.
            CScnObj *RemoveChild(CScnObj *pObj)
            {
                if (pObj->m_pGroupPrev != 0) {
                    pObj->m_pGroupPrev->m_pGroupNext = pObj->m_pGroupNext;
                } else {
                    m_pHead = pObj->m_pGroupNext;
                }
                if (pObj->m_pGroupNext != 0) {
                    pObj->m_pGroupNext->m_pGroupPrev = pObj->m_pGroupPrev;
                } else {
                    m_pTail = pObj->m_pGroupPrev;
                }
                pObj->m_pGroupNext = 0;
                pObj->m_pGroupPrev = 0;
                pObj->ObjProc(0x10003, 0, this);
                m_nCount--;
                return pObj;
            }
            // (name guessed) Inline: RemoveChildAt(0).
            CScnObj *RemoveFirstChild()
            {
                CScnObj *pObj = m_pHead;

                if (pObj != 0) {
                    RemoveChild(pObj);
                }
                return pObj;
            }

        private:
            CScnObj *m_pHead;        // 0x118 (name guessed)
            CScnObj *m_pTail;        // 0x11C (name guessed)
            unsigned long m_nCount;  // 0x120 (name guessed)
    };
}

#endif /* !BCS_CSCNGROUP_HPP_ */
