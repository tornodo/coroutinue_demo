module;

import <experimental/coroutine>;
import <string>;
import <atomic>;
import <memory>;

#include "spdlog/spdlog.h"

import error_module;

export module coro_module;

template <typename T>
class Promise;

class Scheduler;

export class State_base_t {
public:
    State_base_t() {}

    virtual ~State_base_t() {}

    virtual void Resume() = 0;

    void SetScheduler(Scheduler* scheduler) {
        scheduler_ = scheduler;
    }

    Scheduler* GetScheduler() {
        return scheduler_;
    }
    
protected:
    Scheduler* scheduler_ = nullptr;
};

export template <typename T>
class State: public State_base_t {
public:
    State() {
         ready_ = false;
        SPDLOG_INFO("State() 0X{0:x}", (uintptr_t)this);
    }
    ~State() {
        SPDLOG_INFO("~State() 0X{0:x}", (uintptr_t)this);
    }

    void SetSuspendHandle(std::experimental::coroutine_handle<> handle) {
        handle_ = handle;
    }

    void Resume() {
        ready_ = true;
        auto handle = handle_;
        handle_ = nullptr;

        if (handle) {
            handle.resume();
        }
    }

    template <typename U>
    void SetValue(U&& u) {
        value_.store(std::move(u));
    }

    auto GetValue() {
        if (ready_) {
            return value_.load();
        }
        throw coro_exception{ ErrorNotReady };
    }

    bool Ready() const {
        return ready_;
    }

    State(const State&) = delete;
    State& operator= (const State&) = delete;
private:
    std::atomic<T> value_;
    std::atomic_bool ready_;
    std::experimental::coroutine_handle<> handle_;
};

export template <>
class State<void>: public State_base_t  {
public:
    State() = default;
    ~State() {
    }

    void SetSuspendHandle(std::experimental::coroutine_handle<> handle) {
        handle_ = handle;
    }

    void Resume() {
        ready_ = true;
        auto handle = handle_;
        handle_ = nullptr;

        if (handle) {
            handle.resume();
        }
    }

    void SetValue() {
    }

    auto GetValue() {
        if (!ready_) {
            throw coro_exception{ ErrorNotReady };
        }
    }

    bool Ready() const {
        return ready_;
    }

    State(const State&) = delete;
    State& operator= (const State&) = delete;
private:
    bool ready_ = false;
    std::experimental::coroutine_handle<> handle_;
};

export template <typename T>
class PromiseBase {
public:
    using value_type = T;
    // using future_type = Future<T>;
    using state_type = State<T>;
    using promise_type = Promise<T>;

    PromiseBase() noexcept = default;
    PromiseBase(PromiseBase&&) = default;
    PromiseBase& operator= (PromiseBase&&) = default;

    PromiseBase(const PromiseBase&) = delete;
    PromiseBase& operator= (const PromiseBase&) = delete;

    // future_type GetFuture() {
    //     return future_type(state_);
    // }

    auto GetState() {
        return state_.get();
    }

    auto get_return_object() {
        SPDLOG_INFO("PromiseBase get_return_object");
        return state_;
    }

    // future_type get_return_object() {
    //     std::cout << "get_return_object" << std::endl;
    //     return future_type(state_);
    // }

    auto initial_suspend() const {
        SPDLOG_INFO("PromiseBase initial_suspend");
        return std::experimental::suspend_always{};
    }

    auto final_suspend() noexcept {
        SPDLOG_INFO("PromiseBase final_suspend");
        return std::experimental::suspend_never{};
    }

    void unhandled_exception() {
        std::terminate();
    }

protected:
    std::shared_ptr<state_type> state_ = std::make_shared<state_type>();
};

export template <typename T>
class Promise : public PromiseBase<T> {
public:
    using value_type = T;
    using PromiseBase<T>::get_return_object;

    template<class U>
    void return_value(U&& u) {
        SPDLOG_INFO("Promise return_value(U&& u)");
        this->state_->SetValue(std::forward<U>(u));
    }
};

export template <>
class Promise<void> {
public:
    // using future_type = Future<void>;
    using state_type = State<void>;

    Promise() noexcept = default;
    Promise(Promise&&) = default;
    Promise& operator= (Promise&&) = default;

    Promise(const Promise&) = delete;
    Promise& operator= (const Promise&) = delete;

    // future_type GetFuture() {
    //     return future_type(state_);
    // }

    // future_type get_return_object() {
    //     std::cout << "Promise<void> get_return_object" << std::endl;
    //     return future_type(state_);
    // }

    auto get_return_object() {
        SPDLOG_INFO("Promise<void> get_return_object");
        return state_;
    }

    auto initial_suspend() const {
        SPDLOG_INFO("Promise<void> initial_suspend");
        return std::experimental::suspend_never{};
    }

    auto final_suspend() const noexcept {
        SPDLOG_INFO("Promise<void> final_suspend");
        return std::experimental::suspend_never{};
    }

    void return_void() {
        SPDLOG_INFO("Promise<void> return_void");
        state_->SetValue();
    }

    void unhandled_exception() {
        std::terminate();
    }

protected:
    std::shared_ptr<state_type> state_ = std::make_shared<state_type>();
};

export template <typename T>
class [[nodiscard]] Future {
public:
    using value_type = T;
    using promise_type = Promise<T>;
    using state_type = State<T>;

    Future(std::shared_ptr<state_type> state) : state_(state) { }

    Future(const Future&) noexcept = default;
    Future(Future&&) noexcept = default;
    Future& operator= (const Future&) noexcept = default;
    Future& operator= (Future&&) noexcept = default;


    T await_resume() const {
        SPDLOG_INFO("Future await_resume");
        return state_->GetValue();
    }

    bool await_ready() const {
        SPDLOG_INFO("Future await_ready");
        return state_->Ready();
    }

    void await_suspend(std::experimental::coroutine_handle<> handle) {
        SPDLOG_INFO("Future await_suspend");
        state_->SetSuspendHandle(handle);
    }

    auto GetValue() const {
        return state_->GetValue();
    }
private:
    std::shared_ptr<state_type> state_;
};
