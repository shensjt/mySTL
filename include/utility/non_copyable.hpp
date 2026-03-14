#pragma once

namespace mystl {

class NonCopyableMovable {
protected:
    NonCopyableMovable() = default;
    ~NonCopyableMovable() = default;

    NonCopyableMovable(const NonCopyableMovable&) = delete;
    NonCopyableMovable& operator=(const NonCopyableMovable&) = delete;

    NonCopyableMovable(NonCopyableMovable&&) = default;
    NonCopyableMovable& operator=(NonCopyableMovable&&) = default;
};

} // namespace mystl
