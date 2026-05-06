@echo off
setlocal EnableExtensions
cd /d "%~dp0"
set "LOG=%~dp0build-compta.log"
set "OBJDIR=obj\compta"
set "STAMPDIR=%OBJDIR%\.stamps"

if not defined CXX set "CXX=C:\msys64\ucrt64\bin\g++.exe"

if not exist "%CXX%" (
  echo Erreur: le compilateur est introuvable: "%CXX%"
  exit /b 1
)

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

set "OUTDIR=bin\compta"
set "EXE=Compta.exe"
set "CXXFLAGS=-std=gnu++20 -Wall -O2 -g -DUNICODE -D_UNICODE -fno-diagnostics-color %INCLUDES%"
set "LFLAGS=-Llibs -lcsArithmetic -lcsParallelTask -lreadwfile -lgdi32 -luser32 -lmsimg32 -lkernel32 -lcomctl32 -lcomdlg32 -luxtheme -ldwmapi -lwininet -lpthread"

if not exist "%OUTDIR%" mkdir "%OUTDIR%"
if not exist "%OBJDIR%" mkdir "%OBJDIR%"
if not exist "%STAMPDIR%" mkdir "%STAMPDIR%"
set "NEED_LINK=0"

echo Compilateur: %CXX%
echo Sortie: "%OUTDIR%\%EXE%"
echo Objets: "%OBJDIR%\"
echo Journal: "%LOG%"
echo.

if exist "%LOG%" del /q "%LOG%" 2>nul
>"%LOG%" echo === Compta build ===
>>"%LOG%" echo %DATE% %TIME%
>>"%LOG%" "%CXX%" --version 2>&1
>>"%LOG%" echo.
for /f %%T in ('powershell -NoProfile -Command "$m=(Get-ChildItem 'include' -Recurse -File -Include *.h,*.hpp | Sort-Object LastWriteTimeUtc -Descending | Select-Object -First 1).LastWriteTimeUtc; if($m){$m.ToString('yyyyMMddHHmmssffff')} else {'0'}"') do set "HEADER_STAMP=%%T"
if "%HEADER_STAMP%"=="" set "HEADER_STAMP=0"

call :compile compta.cpp
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
call :compile src\csRuler.cpp
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
"%CXX%" "%OBJDIR%\compta.o" "%OBJDIR%\csArgs.o" "%OBJDIR%\csFilesMan.o" "%OBJDIR%\csGraphics.o" "%OBJDIR%\csInput.o" "%OBJDIR%\csLIST.o" "%OBJDIR%\csMenu.o" "%OBJDIR%\csRuler.o" "%OBJDIR%\csScrollBar.o" "%OBJDIR%\csSection.o" "%OBJDIR%\csStrUtils.o" "%OBJDIR%\csSubClasses.o" "%OBJDIR%\csTranslator.o" "%OBJDIR%\csTypes.o" "%OBJDIR%\csUIFx.o" "%OBJDIR%\csUIObjects.o" "%OBJDIR%\csUniformListBox.o" "%OBJDIR%\csUtils.o" -o "%OUTDIR%\%EXE%" %LFLAGS% >>"%LOG%" 2>&1
if errorlevel 1 (
  echo Echec de l'edition de liens.
  goto :fail
)

:after_link

>>"%LOG%" echo === Copie DLL runtime ===
call :copydll "libs\csArithmetic.dll"
call :copydll "libs\csParallelTask.dll"
call :copydll "libs\readwfile.dll"
call :copydll "C:\msys64\ucrt64\bin\libgcc_s_seh-1.dll"
call :copydll "C:\msys64\ucrt64\bin\libstdc++-6.dll"
call :copydll "C:\msys64\ucrt64\bin\libwinpthread-1.dll"
call :copyasset "comptaResources\compta-logo.ico" "comptaResources\compta-logo.ico"

if "%KEEP_LOG%"=="" if exist "%LOG%" del /q "%LOG%" 2>nul
echo.
echo OK: "%OUTDIR%\%EXE%"
echo Lancement: "%OUTDIR%\%EXE%"
start "" "%OUTDIR%\%EXE%"
endlocal
exit /b 0

:compile
set "OBJ=%OBJDIR%\%~n1.o"
set "STAMP=%STAMPDIR%\%~n1.stamp"
for %%F in ("%~1") do set "CUR_SRC_STAMP=%%~tF"
if not exist "%OBJ%" goto :compile_now
if not exist "%STAMP%" goto :compile_now
set "OLD_SRC_STAMP="
set "OLD_HEADER_STAMP="
for /f "usebackq tokens=1,2 delims=|" %%a in ("%STAMP%") do (
  set "OLD_SRC_STAMP=%%a"
  set "OLD_HEADER_STAMP=%%b"
)
if not "%CUR_SRC_STAMP%"=="%OLD_SRC_STAMP%" goto :compile_now
if not "%HEADER_STAMP%"=="%OLD_HEADER_STAMP%" goto :compile_now
echo   SKIP %~n1
>>"%LOG%" echo --- SKIP "%~1" ^(objet + stamp a jour^) ---
exit /b 0

:compile_now
>>"%LOG%" echo --- -c "%~1" -^> "%OBJ%" ---
"%CXX%" %CXXFLAGS% -c "%~1" -o "%OBJ%" >>"%LOG%" 2>&1 || (
  echo Echec compilation: %~1
  exit /b 1
)
>"%STAMP%" echo %CUR_SRC_STAMP%^|%HEADER_STAMP%
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

:copyasset
if not exist "%OUTDIR%\comptaResources" mkdir "%OUTDIR%\comptaResources" >nul 2>&1
if exist "%~1" (
  copy /y "%~1" "%OUTDIR%\%~2" >nul
  echo   ASSET OK %~1
  >>"%LOG%" echo ASSET OK: %~1
) else (
  echo   ASSET absent ^(ignore^) %~1
  >>"%LOG%" echo ASSET absent: %~1
)
exit /b 0

:show_error_phrase_between_semicolons
if not exist "%~1" exit /b 0
set "BUILD_COMPTA_ERRLOG=%~1"
powershell -NoProfile -NoLogo -ExecutionPolicy Bypass -Command "$p=$env:BUILD_COMPTA_ERRLOG; $r=Get-Content -LiteralPath $p -Raw -Encoding UTF8 -ErrorAction SilentlyContinue; if ($null -eq $r) { $r = Get-Content -LiteralPath $p -Raw -ErrorAction SilentlyContinue }; if ([string]::IsNullOrEmpty($r)) { Write-Host ''; Write-Host 'Journal vide ou illisible.'; exit 0 }; $i=$r.IndexOf('error',[StringComparison]::OrdinalIgnoreCase); if ($i -lt 0) { Write-Host ''; Write-Host 'Aucun mot error dans le journal.'; exit 0 }; if ($i -gt 0) { $prev=$r.LastIndexOf(';',$i-1) } else { $prev=-1 }; $start=$prev+1; $next=$r.IndexOf(';',$i+5); if ($next -lt 0) { $next=$r.Length }; $len=[Math]::Max(0,$next-$start); $s=$r.Substring($start,$len).Trim(); Write-Host ''; Write-Host '--- Phrase entre ; avant error et ; apres error ---'; Write-Host $s"
set "BUILD_COMPTA_ERRLOG="
exit /b 0

:fail
echo.
if exist "%LOG%" (
  echo === Journal complet ===
  type "%LOG%"
  echo.
  call :show_error_phrase_between_semicolons "%LOG%"
  echo.
) else (
  echo Journal introuvable.
)
echo Echec - voir "%LOG%" ^(conservez: set KEEP_LOG=1 avant le script^)
endlocal
exit /b 1
