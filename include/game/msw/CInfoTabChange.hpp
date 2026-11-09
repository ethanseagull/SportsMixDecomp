#ifndef CINFOTABCHANGE_HPP_
#define CINFOTABCHANGE_HPP_

namespace msw {

    // Steps through the tabs of an info window, wrapping at both ends.
    // Only the members used so far; the size is not known yet.
    class CInfoTabChange {
        public:
            // Both store the new tab number and return it.
            long IncTabNo(long nTabNo);
            long DecTabNo(long nTabNo);

            long m_nTabNum; // 0x00
            long m_nTabNo;  // 0x04
    };
}

#endif /* !CINFOTABCHANGE_HPP_ */
