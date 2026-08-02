/**
 * 题目: 基于 RAII 实现文件资源管理类 RAIIFile
 * 难度: ⭐⭐
 * 标签: RAII, 文件句柄, 资源管理, 异常安全
 *
 * 要点:
 * 1. RAII 核心: 构造函数获取资源（fopen），析构函数释放资源（fclose）
 * 2. 拷贝控制: 禁止拷贝（避免 double free），实现移动语义转移所有权
 * 3. 异常安全: 析构函数 noexcept，栈展开时文件必然被正确关闭
 * 4. 面试常问: 与裸 FILE* 对比、移动后源对象状态、Rule of Three/Five
 */

#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

class RAIIFile {
public:
  explicit RAIIFile(const char *path, const char *mode = "r") {
    fp_ = std::fopen(path, mode);
    if (!fp_) {
      throw std::runtime_error(std::string("无法打开文件: ") + path);
    }
  }

  ~RAIIFile() { close(); }

  // 禁止拷贝: 两个对象共享同一个 FILE* 会导致 double free
  RAIIFile(const RAIIFile &) = delete;
  RAIIFile &operator=(const RAIIFile &) = delete;

  // 移动构造/移动赋值: 转移文件所有权，源对象置空
  RAIIFile(RAIIFile &&other) noexcept : fp_(other.fp_) { other.fp_ = nullptr; }

  RAIIFile &operator=(RAIIFile &&other) noexcept {
    if (this != &other) {
      close();
      fp_ = other.fp_;
      other.fp_ = nullptr;
    }
    return *this;
  }

  void write(const char *data) {
    if (!fp_) {
      throw std::runtime_error("写入失败: 文件已关闭");
    }
    if (std::fputs(data, fp_) == EOF) {
      throw std::runtime_error("写入失败: fputs 返回 EOF");
    }
  }

  // 读一行，返回是否成功（已到 EOF 或出错时返回 false）
  bool readLine(char *buf, std::size_t size) {
    if (!fp_) {
      throw std::runtime_error("读取失败: 文件已关闭");
    }
    return std::fgets(buf, static_cast<int>(size), fp_) != nullptr;
  }

  explicit operator bool() const { return fp_ != nullptr; }

private:
  void close() {
    if (fp_) {
      std::fclose(fp_);
      fp_ = nullptr;
    }
  }

  FILE *fp_ = nullptr;
};

int main() {
  const char *path = "demo_raii.txt";

  try {
    {
      RAIIFile file(path, "w"); // 构造函数获取资源
      file.write("hello RAII\n");
      file.write("resource released on scope exit\n");
    } // 离开作用域 → 析构函数自动 fclose

    // 移动语义: 转移文件所有权，f1 变为空
    RAIIFile f1(path, "a");
    RAIIFile f2 = std::move(f1);
    if (f1) {
      std::cout << "错误: f1 移动后不应持有资源" << std::endl;
    } else {
      std::cout << "移动后 f1 已置空" << std::endl;
    }
    f2.write("moved ownership\n");

    // 异常安全: 抛出异常时栈展开，f3 的析构函数依然会执行
    RAIIFile f3(path, "a");
    throw std::runtime_error("模拟异常，验证栈展开时文件被自动关闭");
  } catch (const std::exception &e) {
    std::cout << "捕获异常: " << e.what() << std::endl;
  }

  // 回读验证文件内容
  RAIIFile f(path, "r");
  char buf[256];
  std::cout << "--- 文件内容 ---" << std::endl;
  while (f.readLine(buf, sizeof buf)) {
    std::cout << buf;
  }
  std::cout << "--- 结束 ---" << std::endl;

  std::remove(path);
  return 0;
}