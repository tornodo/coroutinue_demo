# clang++ -Wall -std=c++20 -stdlib=libc++ -fmodules -c modules/error.cpp -Xclang -emit-module-interface -o ./modules_out/error_module.pcm 
clang++ -Wall -std=c++20 -stdlib=libc++ -I $PWD/third_party/spdlog/include/ -fmodules -c modules/coro.cpp -Xclang -emit-module-interface -fprebuilt-module-path=./modules_out -o ./modules_out/coro_module.pcm
clang++ -Wall -std=c++20 -stdlib=libc++ -I $PWD/third_party/spdlog/include/ -fmodules -c modules/scheduler.cpp -Xclang -emit-module-interface -fprebuilt-module-path=./modules_out -o ./modules_out/scheduler_module.pcm
clang++ -Wall -std=c++20 -stdlib=libc++ -I $PWD/third_party/spdlog/include/ -fmodules -c modules/awaitable.cpp -Xclang -emit-module-interface -fprebuilt-module-path=./modules_out -o ./modules_out/awaitable_module.pcm
# clang++ -Wall -std=c++20 -stdlib=libc++ -I $PWD/third_party/spdlog/include/ -fmodules -c modules/mqtt.cpp -Xclang -emit-module-interface -fprebuilt-module-path=./modules_out -o ./modules_out/mqtt_module.pcm
clang++ -Wall -std=c++20 -stdlib=libc++ -I $PWD/third_party/spdlog/include/ -fmodules -fprebuilt-module-path=./modules_out -lpaho-mqtt3as -lpthread modules/error.cpp modules/coro.cpp modules/scheduler.cpp modules/awaitable.cpp main.cpp -o demo

# no scheduler
# clang++ -Wall -std=c++20 -stdlib=libc++ -fmodules -c modules/error.cpp -Xclang -emit-module-interface -o ./modules_out/error_module.pcm 
# clang++ -Wall -std=c++20 -stdlib=libc++ -I $PWD/third_party/spdlog/include/ -fmodules -c modules/coro_noscheduler.cpp -Xclang -emit-module-interface -fprebuilt-module-path=./modules_out -o ./modules_out/coro_module.pcm
# clang++ -Wall -std=c++20 -stdlib=libc++ -I $PWD/third_party/spdlog/include/ -fmodules -c modules/awaitable_noscheduler.cpp -Xclang -emit-module-interface -fprebuilt-module-path=./modules_out -o ./modules_out/awaitable_module.pcm
# clang++ -Wall -std=c++20 -stdlib=libc++ -I $PWD/third_party/spdlog/include/ -fmodules -c modules/mqtt_noscheduler.cpp -Xclang -emit-module-interface -fprebuilt-module-path=./modules_out -o ./modules_out/mqtt_module.pcm
# clang++ -Wall -std=c++20 -stdlib=libc++ -I $PWD/third_party/spdlog/include/ -fmodules -fprebuilt-module-path=./modules_out -lpaho-mqtt3as -lpthread modules/error.cpp modules/coro_noscheduler.cpp modules/awaitable_noscheduler.cpp modules/mqtt_noscheduler.cpp main.cpp -o demo