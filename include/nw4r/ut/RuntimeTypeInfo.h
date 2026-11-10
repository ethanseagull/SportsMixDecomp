#ifndef NW4R_UT_RUNTIMETYPEINFO_H_
#define NW4R_UT_RUNTIMETYPEINFO_H_

// The run-time type check of nw4r (a class's static typeInfo names its
// parent's). Written for this project from what the game's code shows
// (msw::CCockpitHockeyCoin::SetCoinValue in HockeyF, and
// CMenuPaneAccessor::SetTextBoxString in the DOL, inline both times); the
// names follow the public ogws decompilation's description of nw4r::ut (from
// memory, no code copied).

namespace nw4r {
    namespace ut {
        namespace detail {

            struct RuntimeTypeInfo {
                const RuntimeTypeInfo *mParentTypeInfo; // 0x00, null at the root

                // True when this type is pInfo or derives from it.
                bool IsDerivedFrom(const RuntimeTypeInfo *pInfo) const
                {
                    for (const RuntimeTypeInfo *p = this; p != 0; p = p->mParentTypeInfo) {
                        if (p == pInfo) {
                            return true;
                        }
                    }
                    return false;
                }
            };

            // The typeInfo of the class T points to.
            template <typename T>
            inline const RuntimeTypeInfo *GetTypeInfoFromPtr_(T *)
            {
                return &T::typeInfo;
            }
        }

        // pObj as a TDerived (a pointer type), or null when pObj is null or of
        // another type. TBase needs a GetRuntimeTypeInfo() virtual.
        template <typename TDerived, typename TBase>
        inline TDerived DynamicCast(TBase *pObj)
        {
            const detail::RuntimeTypeInfo *pInfo = detail::GetTypeInfoFromPtr_(static_cast<TDerived>(0));

            if (pObj != 0 && pObj->GetRuntimeTypeInfo()->IsDerivedFrom(pInfo)) {
                return static_cast<TDerived>(pObj);
            }
            return 0;
        }
    }
}

#endif /* !NW4R_UT_RUNTIMETYPEINFO_H_ */
