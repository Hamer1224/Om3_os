@echo off
echo [0/6] Cleaning up...
if exist *.bin del *.bin
if exist *.o del *.o
if exist *.exe del *.exe

echo [1/6] Assembling Kernel Entry...
REM This was missing! We need to create entry.o from entry.asm
nasm -f win32 entry.asm -o entry.o
if %errorlevel% neq 0 (
    echo Entry Assembly Failed!
    pause
    exit /b %errorlevel%
)

echo [2/6] Compiling Kernel Modules...
gcc -m32 -ffreestanding -c utils.c -o utils.o
gcc -m32 -ffreestanding -c screen.c -o screen.o
gcc -m32 -ffreestanding -c keyboard.c -o keyboard.o
gcc -m32 -ffreestanding -c kernel.c -o kernel.o

if %errorlevel% neq 0 (
    echo Compilation Failed!
    pause
    exit /b %errorlevel%
)

echo [3/6] Linking All Modules...
ld -m i386pe -T link.ld -o kernel.exe entry.o kernel.o utils.o screen.o keyboard.o
if %errorlevel% neq 0 (
    echo Linking Failed!
    pause
    exit /b %errorlevel%
)

echo [4/6] Converting to Binary...
objcopy -O binary kernel.exe kernel.bin

echo [5/6] Assembling Bootloader...
nasm -f bin boot.asm -o boot.bin

echo [6/6] Building Final Image...
fsutil file createnew padding.bin 10240 > nul
copy /b boot.bin+kernel.bin+padding.bin om3os.bin > nul
del padding.bin

echo.
echo Build Successful! Launching...
qemu-system-x86_64 om3os.bin