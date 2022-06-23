import <iostream>;
import <atomic>;
import <experimental/coroutine>;

#include "spdlog/spdlog.h"

import error_module;
import coro_module;
import awaitable_module;
import scheduler_module;

void async_call(std::function<void()> call) {
    std::thread([call] {
        spdlog::info("async_call begin， sleep 1 second");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        call();
        spdlog::info("async_call end");
    }).detach();
}

Future<int> call_async() {
    Awaitable<int> await;
    State<int>* state = await.state_.get();

    spdlog::info("call_async begin");
    async_call([state]() {
        spdlog::info("callback finished");
        state->SetValue(666);
        Scheduler* scheduler = state->GetScheduler();
        scheduler->AddState(state);
    });
    spdlog::info("call_async end");

    return await.GetFuture();
}

Future<void> call() {
    Scheduler* scheduler = SchedulerManager::GetInstance()->GetThreadScheduler(std::this_thread::get_id());
    spdlog::info("call begin");
    auto res = co_await call_async();
    spdlog::info("call() = {0:d}", res);
    co_await scheduler->Quit();
    co_return;
}

int main(int argc, char** argv) {
    spdlog::set_pattern("*** [%Y:%m:%d %H:%M:%S %f] [thread %t] %v ***");
    SPDLOG_INFO("begin");
    try {
        Scheduler* scheduler = SchedulerManager::GetInstance()->GetThreadScheduler(std::this_thread::get_id());
        call();
        scheduler->Wait();
        SchedulerManager::GetInstance()->RemoveScheduler(std::this_thread::get_id());
    } catch (std::exception& e) {
        std::cout << "exception " << e.what() << std::endl;
    } catch (...) {
        std::cout << "exception ..." << std::endl;
    }
    SPDLOG_INFO("end");
    std::cout << "Press q to quit" << std::endl;
    std::string str;
    std::cin >> str;
    return 0;
}
