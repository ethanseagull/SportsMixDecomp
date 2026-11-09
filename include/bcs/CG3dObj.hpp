#ifndef BCS_CG3DOBJ_HPP_
#define BCS_CG3DOBJ_HPP_

namespace bcs {

    // Base of the scene objects and of the animation objects. Size 0x08,
    // vptr at 0x00. A DOL class; only DetachFromParent has a name there, so
    // the virtual functions other than the destructor and ObjProc are
    // placeholders that keep the vtable slots in place. The slots are read
    // from the vtables of bcs::CMdlObj and bcs::CAnmMatClr.
    class CG3dObj {
        public:
            virtual ~CG3dObj();           // 0x08
            virtual long VFunc0C() const; // 0x0C (guessed) returns 4 for a CMdlObj, 2 for an animation
            virtual int VFunc10() const;  // 0x10 (guessed) returns 1 for a CMdlObj, 0 for an animation
            virtual int VFunc14() const;  // 0x14 (guessed) returns 0 for a CMdlObj, 1 for an animation
            virtual void VFunc18();       // 0x18 (guessed) empty in both
            // 0x1C. The return type is guessed and the base version is not
            // known. DetachFromParent calls the parent's with (0x10001, 0, this).
            virtual void ObjProc(unsigned long, unsigned long, void *) = 0;

            // Tells the parent to drop this object. Returns the old parent.
            CG3dObj *DetachFromParent();

        private:
            CG3dObj *m_pParent; // 0x04 (name guessed)
    };
}

#endif /* !BCS_CG3DOBJ_HPP_ */
