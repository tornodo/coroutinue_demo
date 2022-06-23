
module;

import <map>;
import <set>;
import <mutex>;
import <atomic>;
import <thread>;
import <experimental/coroutine>;

#include "spdlog/spdlog.h"

import coro_module;

export module scheduler_module;

export class Scheduler {
    friend class SchedulerManager;
private:
    Scheduler(std::thread::id id): id_(id) { }
    ~Scheduler() {
        std::lock_guard<std::mutex> lock(state_mutex_);
        state_set_.clear();
    }
public:

    Scheduler(const Scheduler&) = delete;
    Scheduler& operator= (const Scheduler&) = delete;

    void AddState(State_base_t* state) {
        std::lock_guard<std::mutex> lock(state_mutex_);
        state_set_.insert(state);
        SPDLOG_INFO("Scheduler::AddState() 0X{0:x}", (uintptr_t)state);
    }

    void RemoveState(State_base_t* state) {
        std::lock_guard<std::mutex> lock(state_mutex_);
        auto iter = state_set_.find(state);
        if (iter != state_set_.end()) {
            state_set_.erase(iter);
        }
        SPDLOG_INFO("Scheduler::RemoveState() 0X{0:x}", (uintptr_t)state);
    }

    Future<void> Quit() {
        quit_ = true;
        SPDLOG_INFO("Scheduler::Quit()");
        co_return;
    }

    void Wait() {
        SPDLOG_INFO("Scheduler::Wait() begin");
        while(!quit_) {

            std::set<State_base_t*> ready_state_set;
            {
                std::lock_guard<std::mutex> lock(state_mutex_);
                if (state_set_.empty()) {
                    std::this_thread::yield();
                    continue;
                } else {
                    std::swap(state_set_, ready_state_set);
                }
            }

            for(auto item: ready_state_set) {
                item->Resume();
            }
            ready_state_set.clear();
        }
        SPDLOG_INFO("Scheduler::Wait() end");
    }
private:
    std::mutex state_mutex_;
    std::set<State_base_t*> state_set_;

    std::atomic_bool quit_{ false };
    std::thread::id id_;
};

export class SchedulerManager {
public:
    static SchedulerManager* GetInstance() {
        static SchedulerManager instance;
        return &instance;
    }

    Scheduler* GetThreadScheduler(std::thread::id id) {
        std::lock_guard<std::mutex> lock(scheduler_mutex_);
        auto iter = thread_scheduler_map_.find(id);
        if (iter != thread_scheduler_map_.end()) {
            return iter->second;
        }

        auto scheduler = new Scheduler(id);
        thread_scheduler_map_[id] = scheduler;
        return scheduler;
    }

    // first Quit(), Wait()
    void RemoveScheduler(std::thread::id id) {
        Scheduler* scheduler = nullptr;
        {
            std::lock_guard<std::mutex> lock(scheduler_mutex_);
            auto iter = thread_scheduler_map_.find(id);
            if (iter != thread_scheduler_map_.end()) {
                scheduler = iter->second;
                thread_scheduler_map_.erase(iter);
            }
        }

        if (scheduler) {
            delete scheduler;
        }
    }

private:
    SchedulerManager() = default;
private:
    std::map<std::thread::id, Scheduler*> thread_scheduler_map_;
    std::mutex scheduler_mutex_;
};