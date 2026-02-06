# Om3 OS - HolyHamer Edition 🚀

Om3 OS is a custom 32-bit x86 operating system built from scratch. It features a custom shell environment and a built-in compiler/interpreter called **HolyHamer**.

## ✨ Features
* **Custom Kernel**: Built in C and Assembly.
* **HolyHamer Compiler**: A variable-based logic environment with support for math, variables (a-z), and conditional logic.
* **VGA Graphics**: Support for background (`bg`) and text color customization.
* **Filesystem Simulation**: Basic directory navigation with `cd` and path tracking.
* **DevOps Ready**: Integrated GitHub Actions CI/CD pipeline and automated deployment scripts.

## 🛠️ How to Build & Run

### Prerequisites
You need a Linux environment (like GitHub Codespaces) with the following:
* `gcc` (i386-elf-gcc or gcc-multilib)
* `nasm`
* `qemu-system-i386`
* `novnc` & `websockify` (for web-based display)

### Instant Launch
Simply run the automated DevOps script:
```bash
chmod +x run.sh
./run.sh
