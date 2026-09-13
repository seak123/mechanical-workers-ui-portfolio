@echo off
setlocal
where cl >nul 2>nul
if errorlevel 1 (
  echo Run from an x64 Native Tools Command Prompt for Visual Studio.
  exit /b 1
)
pushd "%~dp0"
if not exist build mkdir build
pushd build
cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /I..\Source /c ..\Source\Workers\WorkModel.cpp ..\Source\Workers\HeadPresenter.cpp
if errorlevel 1 goto failed
cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /I..\Source ..\tests\worker_tests.cpp WorkModel.obj HeadPresenter.obj /Fe:worker_tests.exe
if errorlevel 1 goto failed
cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /I..\Source ..\examples\work_trace.cpp WorkModel.obj HeadPresenter.obj /Fe:worker_demo.exe
if errorlevel 1 goto failed
worker_tests.exe
if errorlevel 1 goto failed
worker_demo.exe
if errorlevel 1 goto failed
popd
popd
exit /b 0
:failed
popd
popd
exit /b 1
