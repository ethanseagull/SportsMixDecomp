#ifndef REVOLUTION_GX_GXENUM_H_
#define REVOLUTION_GX_GXENUM_H_

// Only what is used so far: the enumeration named in
// msw::CEventModel::SetMatTevColor's mangled name.
typedef enum _GXTevRegID {
    GX_TEVPREV,
    GX_TEVREG0,
    GX_TEVREG1,
    GX_TEVREG2,
    GX_MAX_TEVREG
} GXTevRegID;

// Named in msw::CBall::Eff_SetMdlKColorReg's mangled name.
typedef enum _GXTevKColorID {
    GX_KCOLOR0,
    GX_KCOLOR1,
    GX_KCOLOR2,
    GX_KCOLOR3,
    GX_MAX_KCOLOR
} GXTevKColorID;

#endif /* !REVOLUTION_GX_GXENUM_H_ */
