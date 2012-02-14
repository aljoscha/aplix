#include "network.hpp"

bool operator==(const Arc &lhs, const Arc &rhs) {
    if (lhs.v != rhs.v) {
        return false;
    }

    if (lhs.w != rhs.w) {
        return false;
    }

    if (lhs.cost != rhs.cost) {
        return false;
    }

    if (lhs.capacity != rhs.capacity) {
        return false;
    }

    if (lhs.artificial != rhs.artificial) {
        return false;
    }

    return true;
}

