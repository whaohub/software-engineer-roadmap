#include <iostream>
#include <stdexcept>

class Test {
public:
    explicit Test(const char* name) : name_(name) {
        std::cout << "构造: " << name_ << std::endl;
    }
    ~Test() {
        std::cout << "析构: " << name_ << std::endl;
    }
private:
    const char* name_;
};

void func() {
    Test t("func中的对象");
    throw std::runtime_error("未捕获的异常!");
}

/* bt no catch

(lldb) bt
* thread #1, queue = 'com.apple.main-thread', stop reason = signal SIGABRT
  * frame #0: 0x000000018ab405e8 libsystem_kernel.dylib`__pthread_kill + 8
    frame #1: 0x000000018ab7b8d8 libsystem_pthread.dylib`pthread_kill + 296
    frame #2: 0x000000018aa81978 libsystem_c.dylib`abort + 148
    frame #3: 0x000000018ab3272c libc++abi.dylib`__abort_message + 132
    frame #4: 0x000000018ab1f570 libc++abi.dylib`demangling_terminate_handler() + 272
    frame #5: 0x000000018a72c8a4 libobjc.A.dylib`_objc_terminate() + 172
    frame #6: 0x000000018ab2f75c libc++abi.dylib`std::__terminate(void (*)()) + 16
    frame #7: 0x000000018ab31be4 libc++abi.dylib`__cxxabiv1::failed_throw(__cxxabiv1::__cxa_exception*) + 88
    frame #8: 0x000000018ab1e09c libc++abi.dylib`__cxa_throw + 92
    frame #9: 0x0000000100000d18 test_exception`func() at test_exception.cpp:18:5
    frame #10: 0x0000000100000de4 test_exception`main at test_exception.cpp:23:5
    frame #11: 0x000000018a7b04e4 dyld`start + 6992

*/
int main() {
    Test t("main中的对象");
    try {
        func();
    } catch (const std::exception& e) {
        std::cout << "捕获异常: " << e.what() << std::endl;
    }
    return 0;
}