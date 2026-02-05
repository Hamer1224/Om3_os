@echo off
echo [0/5] Cleaning up...
if exist *.bin del *.bin
if exist *.o del *.o
if exist *.exe del *.exe

echo [1/5] Compiling Kernel Modules...
REM We now compile 4 separate files
gcc -m32 -ffreestanding -c utils.c -o utils.o
gcc -m32 -ffreestanding -c screen.c -o screen.o
gcc -m32 -ffreestanding -c keyboard.c -o keyboard.o
gcc -m32 -ffreestanding -c kernel.c -o kernel.o

if %errorlevel% neq 0 (
    echo Compilation Failed!
    pause
    exit /b %errorlevel%
)

echo [2/5] Linking All Modules...
REM We link entry.o FIRST, then the kernel, then the drivers
ld -m i386pe -T link.ld -o kernel.exe entry.o kernel.o utils.o screen.o keyboard.o
if %errorlevel% neq 0 (
    echo Linking Failed!
    pause
    exit /b %errorlevel%
)

echo [3/5] Converting to Binary...
objcopy -O binary kernel.exe kernel.bin

echo [4/5] Assembling Bootloader...
nasm -f bin boot.asm -o boot.bin

echo [5/5] Building Final Image...
fsutil file createnew padding.bin 10240 > nul
copy /b boot.bin+kernel.bin+padding.bin om3os.bin > nul
del padding.bin

echo.
echo Build Successful! Launching...
qemu-system-x86_64 om3os.bins