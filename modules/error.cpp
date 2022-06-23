module;

#include <string>
#include <exception>

export module error_module;

export std::string ErrorNotReady = "not ready";


export class coro_exception: std::exception {
public:
    coro_exception() = default;
    coro_exception(const std::string& msg): message(msg) {}
    virtual const char* what() const throw () {
        return message.c_str();
    }
private:
    std::string message;
};
