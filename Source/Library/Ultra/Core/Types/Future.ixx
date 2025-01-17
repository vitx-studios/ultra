﻿export module Ultra.Core.Future;

import Ultra.Core;

export namespace Ultra {

///
/// @brief Container for future objects, with some syntactic sugar.
/// @note ToDo: Implement future utility classes, functions here.
///
template<typename T>
class Future {
public:
    Future() noexcept = default;
    Future(std::future<T> &&future) noexcept: mFuture(std::move(future)) {}

    T &Get() noexcept {
        if (mFuture.valid()) mCurrent = mFuture.get();
        return *mCurrent;
    }

    constexpr operator bool() const noexcept {
        return mFuture.valid() || mCurrent;
    }
    constexpr operator T &() const noexcept {
        return *Get();
    }

    bool operator==(const Future &rhs) const noexcept { return (mFuture == rhs.mFuture && mCurrent == rhs.mCurrent); }
    bool operator!=(const Future &rhs) const noexcept { return !operator==(rhs); }
    T &operator*() noexcept { return Get(); }
    T &operator->() noexcept { return Get(); }

private:
    std::optional<T> mCurrent;
    std::future<T> mFuture;
};

}
