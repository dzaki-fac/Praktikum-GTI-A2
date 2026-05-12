@echo off

REM ambil nama file tanpa ekstensi
set filename=%~n1

REM buat folder bin kalau belum ada
if not exist bin mkdir bin

REM compile ke folder bin
g++ %1 -o bin\%filename% -lfreeglut -lopengl32 -lglu32

REM kalau error, stop
if %errorlevel% neq 0 exit /b

REM jalankan exe dari bin
bin\%filename%.exe