#ifndef BCS_G2D_CLYTRESOURCE_HPP_
#define BCS_G2D_CLYTRESOURCE_HPP_

namespace bcs {
    namespace G2D {

        // Lives in the DOL. Only the members used so far.
        class CLytResource {
            public:
                static CLytResource *GetInstance();

                // Releases the archive loaded under pName (return type guessed).
                void Cleanup(const char *pName);
        };
    }
}

#endif /* !BCS_G2D_CLYTRESOURCE_HPP_ */
