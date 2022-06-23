module;

import <experimental/coroutine>;
import <memory>;

#include "spdlog/spdlog.h"

import coro_module;

export module awaitable_module;

export template <typename T>
class AwaitableBase {
public:
    using value_type = T;
    using state_type = State<value_type>;
    using future_type = Future<value_type>;

    AwaitableBase() {
        SPDLOG_INFO("AwaitableBase() 0X{0:x}", (uintptr_t)this);
    }
    // AwaitableBase() noexcept = default;
    AwaitableBase(const AwaitableBase&) noexcept = default;
    AwaitableBase(AwaitableBase&&) noexcept = default;

    ~AwaitableBase() {
        SPDLOG_INFO("~AwaitableBase() 0X{0:x}", (uintptr_t)this);
    }

    AwaitableBase& operator = (const AwaitableBase&) noexcept = default;
    AwaitableBase& operator = (AwaitableBase&&) = default;

    future_type GetFuture() const noexcept {
        return future_type{ this->state_ };
    }

    state_type* GetState() {
        return this->state_.get();
    }

    // protected:
    std::shared_ptr<state_type> state_ = std::make_shared<state_type>();
};

export template <typename T>
class Awaitable : public AwaitableBase<T> {
public:
    using typename AwaitableBase<T>::value_type;
    using typename AwaitableBase<T>::state_type;
    using AwaitableBase<T>::AwaitableBase;

    template <typename U>
    void SetValue(U&& u) {
        this->state_->SetValue(std::forward<U>(u));
    }
};

export template <>
class Awaitable<void> : public AwaitableBase<void> {
public:
    using typename AwaitableBase<void>::state_type;
    using AwaitableBase<void>::AwaitableBase;

    void SetValue() {
        this->state_->SetValue();
    }
};