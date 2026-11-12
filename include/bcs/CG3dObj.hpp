#ifndef BCS_CG3DOBJ_HPP_
#define BCS_CG3DOBJ_HPP_

namespace bcs {

    // Base of the scene objects and of the animation objects. Size 0x08,
    // vptr at 0x00. A DOL class. The slots are read from the vtables of
    // bcs::CMdlObj and bcs::CAnmMatClr.
    class CG3dObj {
        public:
            // Inline: the animation constructors store the null parent
            // directly.
            CG3dObj() : m_pParent(0) {}
            virtual ~CG3dObj();           // 0x08
            // 0x0C to 0x18: names from the vtable of
            // msw::SelectMenu::CCourtBallMdlObj in MenuF (return types guessed).
            virtual long GetObjType() const;        // 0x0C returns 4 for a CMdlObj, 2 for an animation
            virtual int IsScnObj() const;           // 0x10 returns 1 for a CMdlObj, 0 for an animation
            // 0x14 and 0x18 are inline (weak copies in VolleyF
            // game/volley/CStageNetVolley.cpp, MenuF game/menu/CBgMdl.cpp).
            virtual int IsAnmObj() const { return 0; }  // 0x14 1 for an animation
            virtual void NotifyAppendDelayDelete() {}   // 0x18 empty in both
            // 0x1C. The return type is guessed and the base version is not
            // known. DetachFromParent calls the parent's with (0x10001, 0, this).
            virtual void ObjProc(unsigned long, unsigned long, void *) = 0;

            // Tells the parent to drop this object. Returns the old parent.
            CG3dObj *DetachFromParent();
            // (name guessed; added for game/msw/CStgMdl.cpp, whose
            // SceneEntry registers only objects without a parent) The
            // object this one is attached to.
            CG3dObj *GetParent() const { return m_pParent; }

        protected:
            // (name guessed) Inline: bcs::CGDLObj::ObjProc stores the parent
            // directly when it is attached (task 0x10002) or detached
            // (0x10003).
            void SetParent(CG3dObj *pParent) { m_pParent = pParent; }

        private:
            CG3dObj *m_pParent; // 0x04 (name guessed)
    };
}

#endif /* !BCS_CG3DOBJ_HPP_ */
