#ifndef BCS_CPOOL_HPP_
#define BCS_CPOOL_HPP_

namespace bcs {

    namespace pool {
        // Locking policy of a pool that is used from one thread only.
        class NoLock;
    }

    // A pool of TNode objects that hold a T each. Only the names are
    // declared: so far the pool is known from the mangled names of functions
    // that take a Handle by reference, and nothing here has read its layout.
    template <typename T, typename TNode, typename TLock>
    class CPool {
        public:
            // Refers to one node of the pool.
            class Handle;
    };
}

#endif /* !BCS_CPOOL_HPP_ */
