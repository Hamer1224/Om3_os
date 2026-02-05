@echo off
echo [0/6] Cleaning up...
if exist *.bin del *.bin
if exist *.o del *.o
if exist *.exe del *.exe

echo [1/6] Assembling Kernel Entry...
nasm -f win32 entry.asm -o entry.o
if %errorlevel% neq 0 exit /b %errorlevel%

echo [2/6] Compiling Kernel Modules...
gcc -m32 -ffreestanding -c utils.c -o utils.o
gcc -m32 -ffreestanding -c screen.c -o screen.o
gcc -m32 -ffreestanding -c keyboard.c -o keyboard.o
gcc -m32 -ffreestanding -c disk.c -o disk.o
gcc -m32 -ffreestanding -c fs.c -o fs.o
REM NEW FILE HERE:
gcc -m32 -ffreestanding -c holyhamer.c -o holyhamer.o 
gcc -m32 -ffreestanding -c kernel.c -o kernel.o

if %errorlevel% neq 0 (
    echo Compilation Failed!
    pause
    exit /b %errorlevel%
)

echo [3/6] Linking...
REM ADD holyhamer.o TO THIS LIST:
ld -m i386pe -T link.ld -o kernel.exe entry.o kernel.o utils.o screen.o keyboard.o disk.o fs.o holyhamer.o

if %errorlevel% neq 0 (
    echo Linking Failed!
    pause
    exit /b %errorlevel%
)

echo [4/6] Binary Extraction...
objcopy -O binary kernel.exe kernel.bin

echo [5/6] Bootloader...
nasm -f bin boot.asm -o boot.bin

echo [6/6] Building Hard Drive Image (10MB)...
fsutil file createnew padding.bin 10485760 > nul
copy /b boot.bin+kernel.bin+padding.bin om3os.bin > nul
del padding.bin

echo.
echo Build Successful! Launching...
qemu-system-x86_64 -drive file=om3os.bin,format=raw