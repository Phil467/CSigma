@echo off
setlocal EnableExtensions
cd /d "%~dp0"
set "LOG=%~dp0build-csigma.log"
set "OBJDIR=obj\manual"

if not defined CXX set "CXX=C:\msys64\ucrt64\bin\g++.exe"

if not exist "%CXX%" (
  echo Erreur: le compilateur est introuvable: "%CXX%"
  exit /b 1
)

:: Important hors terminal MSYS2: cc1plus.exe/ld.exe ont des DLL dans ucrt64\bin.
:: On force ce dossier en tete du PATH pour eviter l'erreur 0xC0000135 (DLL manquante).
set "PATH=C:\msys64\ucrt64\bin;%PATH%"

if not defined NLOHMANN set "NLOHMANN=C:\msys64\ucrt64\include"
if exist "include\nlohmann\json.hpp" goto nlo_in_include
if exist "%NLOHMANN%\nlohmann\json.hpp" goto nlo_in_sys
echo Erreur: nlohmann/json.hpp introuvable.
echo  - placez nlohmann/json.hpp sous "include" ou
echo  - MSYS2 UCRT64: pacman -S mingw-w64-ucrt-x86_64-nlohmann-json
exit /b 1
:nlo_in_include
set "INCLUDES=-Iinclude -Iinclude\csArithmetic -Iinclude\csParallelTask"
goto nlo_done
:nlo_in_sys
set "INCLUDES=-Iinclude -Iinclude\csArithmetic -Iinclude\csParallelTask -I%NLOHMANN%"
:nlo_done

if not exist "libs\libcsArithmetic.dll.a"   goto :missinglib
if not exist "libs\libcsParallelTask.dll.a" goto :missinglib
if not exist "libs\libreadwfile.dll.a"      goto :missinglib
goto :libsok
:missinglib
echo Erreur: un fichier d'import manque dans libs\ ^(libcsArithmetic.dll.a, libcsParallelTask.dll.a, libreadwfile.dll.a^)
exit /b 1
:libsok

set "OUTDIR=bin\manual"
set "EXE=CSigma.exe"
:: gnu++20: le projet utilise des initialiseurs designes (ex. MINMAXINFO{.pt...}, CSTEXT{.Text=...}) — invalides en C++17 strict
:: -DUNICODE/-D_UNICODE: active les API Win32 wide-char.
set "CXXFLAGS=-std=gnu++20 -Wall -O2 -g -DUNICODE -D_UNICODE -fno-diagnostics-color %INCLUDES%"
:: Unicode reste assure par -DUNICODE/-D_UNICODE; on garde WinMain (pas wWinMain).
set "LFLAGS=-Llibs -lcsArithmetic -lcsParallelTask -lreadwfile -lgdi32 -luser32 -lmsimg32 -lkernel32 -lcomctl32 -lcomdlg32 -luxtheme -ldwmapi -lwininet -lpthread"

if not exist "%OUTDIR%" mkdir "%OUTDIR%"
if not exist "%OBJDIR%" mkdir "%OBJDIR%"
set "NEED_LINK=0"

echo Compilateur: %CXX%
echo Sortie: "%OUTDIR%\%EXE%"
echo Objets: "%OBJDIR%\"
echo Journal: "%LOG%"
echo.

if exist "%LOG%" del /q "%LOG%" 2>nul
>"%LOG%" echo === CSigma build ===
>>"%LOG%" echo %DATE% %TIME%
>>"%LOG%" "%CXX%" --version 2>&1
>>"%LOG%" echo.

call :compile csigma.cpp
if errorlevel 1 goto :fail
call :compile src\csArgs.cpp
if errorlevel 1 goto :fail
call :compile src\csFilesMan.cpp
if errorlevel 1 goto :fail
call :compile src\csGraphics.cpp
if errorlevel 1 goto :fail
call :compile src\csInput.cpp
if errorlevel 1 goto :fail
call :compile src\csLIST.cpp
if errorlevel 1 goto :fail
call :compile src\csMenu.cpp
if errorlevel 1 goto :fail
call :compile src\csScrollBar.cpp
if errorlevel 1 goto :fail
call :compile src\csSection.cpp
if errorlevel 1 goto :fail
call :compile src\csStrUtils.cpp
if errorlevel 1 goto :fail
call :compile src\csSubClasses.cpp
if errorlevel 1 goto :fail
call :compile src\csTranslator.cpp
if errorlevel 1 goto :fail
call :compile src\csTypes.cpp
if errorlevel 1 goto :fail
call :compile src\csUIFx.cpp
if errorlevel 1 goto :fail
call :compile src\csUIObjects.cpp
if errorlevel 1 goto :fail
call :compile src\csUniformListBox.cpp
if errorlevel 1 goto :fail
call :compile src\csUtils.cpp
if errorlevel 1 goto :fail

if "%NEED_LINK%"=="1" goto :do_link
if not exist "%OUTDIR%\%EXE%" goto :do_link
echo   Lien ignore ^(aucun .cpp modifie^)
>>"%LOG%" echo === Lien ignore: aucun .cpp modifie ===
goto :after_link

:do_link
>>"%LOG%" echo === Lien ===
"%CXX%" "%OBJDIR%\csigma.o" "%OBJDIR%\csArgs.o" "%OBJDIR%\csFilesMan.o" "%OBJDIR%\csGraphics.o" "%OBJDIR%\csInput.o" "%OBJDIR%\csLIST.o" "%OBJDIR%\csMenu.o" "%OBJDIR%\csScrollBar.o" "%OBJDIR%\csSection.o" "%OBJDIR%\csStrUtils.o" "%OBJDIR%\csSubClasses.o" "%OBJDIR%\csTranslator.o" "%OBJDIR%\csTypes.o" "%OBJDIR%\csUIFx.o" "%OBJDIR%\csUIObjects.o" "%OBJDIR%\csUniformListBox.o" "%OBJDIR%\csUtils.o" -o "%OUTDIR%\%EXE%" %LFLAGS% >>"%LOG%" 2>&1
if errorlevel 1 goto :fail

:after_link

>>"%LOG%" echo === Copie DLL runtime ===
call :copydll "libs\csArithmetic.dll"
call :copydll "libs\csParallelTask.dll"
call :copydll "libs\readwfile.dll"
call :copydll "C:\msys64\ucrt64\bin\libgcc_s_seh-1.dll"
call :copydll "C:\msys64\ucrt64\bin\libstdc++-6.dll"
call :copydll "C:\msys64\ucrt64\bin\libwinpthread-1.dll"

if "%KEEP_LOG%"=="" if exist "%LOG%" del /q "%LOG%" 2>nul
echo.
echo OK: "%OUTDIR%\%EXE%"
echo Lancement: "%OUTDIR%\%EXE%"
start "" "%OUTDIR%\%EXE%"
endlocal
exit /b 0

:compile
set "OBJ=%OBJDIR%\%~n1.o"
if not exist "%OBJ%" goto :compile_now
powershell -NoProfile -Command "$src=(Get-Item '%~1').LastWriteTimeUtc.Ticks; $obj=(Get-Item '%OBJ%').LastWriteTimeUtc.Ticks; if($src -gt $obj){exit 1}else{exit 0}" >nul 2>&1
if errorlevel 1 goto :compile_now
echo   SKIP %~n1
>>"%LOG%" echo --- SKIP "%~1" ^(objet a jour^) ---
exit /b 0

:compile_now
>>"%LOG%" echo --- -c "%~1" -^> "%OBJ%" ---
"%CXX%" %CXXFLAGS% -c "%~1" -o "%OBJ%" >>"%LOG%" 2>&1 || (
  echo Echec: %~1
  exit /b 1
)
set "NEED_LINK=1"
echo   OK %~n1
exit /b 0

:copydll
if exist "%~1" (
  copy /y "%~1" "%OUTDIR%\" >nul
  echo   DLL OK %~nx1
  >>"%LOG%" echo DLL OK: %~1
) else (
  echo   DLL absente ^(ignoree^) %~1
  >>"%LOG%" echo DLL absente: %~1
)
exit /b 0

:fail
echo.
if exist "%LOG%" (
  type "%LOG%"
  echo.
) else (
  echo Journal introuvable.
)
echo Echec - voir "%LOG%" ^(conservez: set KEEP_LOG=1 avant le script^)
endlocal
exit /b 1


