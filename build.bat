@echo off
echo [0/4] Cleaning up old files...
REM "if exist" checks if the file is there so the script doesn't crash if it's already clean
if exist *.bin del *.bin
if exist *.o del *.o
if exist *.exe del *.exe
if exist *.tmp del *.tmp

echo [1/4] Compiling Kernel...
gcc -m32 -ffreestanding -c kernel.c -o kernel.o
if %errorlevel% neq 0 exit /b %errorlevel%

echo [2/4] Linking (Creating Windows Executable)...
ld -m i386pe -T link.ld -o kernel.exe kernel.o
if %errorlevel% neq 0 exit /b %errorlevel%

echo [2.5/4] Converting to Raw Binary...
objcopy -O binary kernel.exe kernel.bin
if %errorlevel% neq 0 exit /b %errorlevel%

echo [3/4] Assembling Bootloader...
nasm -f bin boot.asm -o boot.bin
if %errorlevel% neq 0 exit /b %errorlevel%

echo [4/4] Building OM3 OS Image...
copy /b boot.bin+kernel.bin om3os.bin > nul

echo.
echo Build Successful! Launching QEMU...
qemu-system-x86_64 om3os.bin