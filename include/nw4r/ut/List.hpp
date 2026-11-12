#ifndef NW4R_UT_LIST_HPP_
#define NW4R_UT_LIST_HPP_

#include "nw4r/db/Panic.hpp"

namespace nw4r {
    namespace ut {

        // Intrusive list of the DOL's NW4R library. Minimal declarations
        // written for this project: the function signatures are the DOL's
        // mangled names, the layout is what List_Init and List_GetNext
        // read and write, and the member names are guessed.
        struct Link {
            void *pPrev; // 0x00
            void *pNext; // 0x04, read by List_GetNext
        };

        struct List {
            void *pHead;           // 0x00
            void *pTail;           // 0x04
            unsigned short nNum;   // 0x08
            unsigned short offset; // 0x0A, where the Link is inside an object
        };

        void List_Init(List *pList, unsigned short offset);
        void List_Append(List *pList, void *pObj);
        void List_Remove(List *pList, void *pObj);
        // The first object when pObj is null.
        void *List_GetNext(const List *pList, const void *pObj);
        // The last object when pObj is null.
        void *List_GetPrev(const List *pList, const void *pObj);

        // The number of objects. Inline, with a null check that reports
        // file "list.h", line 210 (VolleyF game/volley/CcDaisyFlowerGarden.cpp
        // emits its two strings as @STRING@List_GetSize objects). Returned
        // as int: the caller there compares it signed (cmpwi/bgt), which an
        // unsigned short return folds to a test for zero.
        inline int List_GetSize(const List *pList)
        {
            if (pList == 0) {
                nw4r::db::Panic("list.h", 210, "NW4R:Pointer must not be NULL (list)");
            }
            return pList->nNum;
        }
    }
}

#endif /* !NW4R_UT_LIST_HPP_ */
