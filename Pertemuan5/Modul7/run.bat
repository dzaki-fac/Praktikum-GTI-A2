@echo off
set filename=ProgramTekstur

if not exist bin mkdir bin

REM Compile SEMUA file .cpp menjadi satu executable
g++ *.cpp -o bin\%filename%.exe -lfreeglut -lopengl32 -lglu32

if %errorlevel% neq 0 (
    echo.
    echo Terjadi kesalahan saat compile!
    pause
    exit /b
)

echo Compile berhasil. Menjalankan program...
bin\%filename%.exe