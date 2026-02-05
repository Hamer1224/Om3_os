@echo off
echo [0/6] Cleaning up...
if exist *.bin del *.bin
if exist *.o del *.o
if exist *.exe del *.exe

echo [1/6] Assembling Kernel Entry...
REM This file ensures we jump to kmain correctly
nasm -f win32 entry.asm -o entry.o
if %errorlevel% neq 0 exit /b %errorlevel%

echo [2/6] Compiling C Kernel...
gcc -m32 -ffreestanding -c kernel.c -o kernel.o
if %errorlevel% neq 0 exit /b %errorlevel%

echo [3/6] Linking Kernel...
REM Notice entry.o comes BEFORE kernel.o
ld -m i386pe -T link.ld -o kernel.exe entry.o kernel.o
if %errorlevel% neq 0 exit /b %errorlevel%

echo [4/6] Converting to Binary...
objcopy -O binary kernel.exe kernel.bin
if %errorlevel% neq 0 exit /b %errorlevel%

echo [5/6] Assembling Bootloader...
nasm -f bin boot.asm -o boot.bin
if %errorlevel% neq 0 exit /b %errorlevel%

echo [5.5/6] Creating Padding...
fsutil file createnew padding.bin 10240 > nul

echo [6/6] Building Final Image...
copy /b boot.bin+kernel.bin+padding.bin om3os.bin > nul

echo.
echo Build Successful! Launching...
qemu-system-x86_64 om3os.bin