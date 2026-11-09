#ifndef NW4R_UT_LIST_HPP_
#define NW4R_UT_LIST_HPP_

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
    }
}

#endif /* !NW4R_UT_LIST_HPP_ */
