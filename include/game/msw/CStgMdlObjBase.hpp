#ifndef MSW_CSTGMDLOBJBASE_HPP_
#define MSW_CSTGMDLOBJBASE_HPP_

namespace msw {

    // The second base of msw::CStgMdlObjBase<T>: an interface with only a
    // virtual destructor (name guessed). The DOL destructor of
    // CStgMdlObjBase<bcs::CMdlObj> (fn_8014BE04) calls nothing for it, so its
    // destructor is inline and empty; its vtable part holds just the
    // destructor thunk (fn_8014BE5C, "@456@").
    class IStgMdlObj {
        public:
            virtual ~IStgMdlObj() {}
    };

    // A scene object of a stage model: the bcs object T plus IStgMdlObj.
    // CStgMdlObjBase<bcs::CMdlObj> is 0x1CC bytes (vptrs at 0x00 and 0x1C8),
    // its vtable is lbl_80471CB0 in the DOL
    // ("__vt__Q23msw30CStgMdlObjBase<Q23bcs7CMdlObj>" in VolleyF). The model
    // classes' NewMdl make one with an inline constructor that stores only
    // the two vtable pointers.
    template <class T>
    class CStgMdlObjBase : public T, public IStgMdlObj {
        public:
            CStgMdlObjBase() {}
            virtual ~CStgMdlObjBase() {}
    };
}

#endif /* !MSW_CSTGMDLOBJBASE_HPP_ */
