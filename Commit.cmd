@ECHO OFF
PUSHD %~dp0

cls

echo %time% %~nx0 %~1

SET MISSING_AUTOMATRON=
IF NOT EXIST "%~dp0..\SolutionBuild\Automatron02\Release\bin\net5.0\Automatron02.exe" SET MISSING_AUTOMATRON=TRUE
IF NOT EXIST "%~dp0..\SolutionBuild\Automatron02\Release\bin\net5.0\Automatron02.dll" SET MISSING_AUTOMATRON=TRUE

IF defined MISSING_AUTOMATRON (
   echo "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.com" "%~dp0Solution.sln" /build "Release|Gaming.Desktop.x64" /project Automatron02
   call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.com" "%~dp0Solution.sln" /build "Release|Gaming.Desktop.x64" /project Automatron02
   )

echo "%~dp0..\SolutionBuild\Automatron02\Release\bin\net5.0\Automatron02.exe" %~dp0 %~dp0Automatron02\TasksCommitOnly "%~1"
call "%~dp0..\SolutionBuild\Automatron02\Release\bin\net5.0\Automatron02.exe" %~dp0 %~dp0Automatron02\TasksCommitOnly "%~1"
if %errorlevel% NEQ 0 (
   echo Automatron02.exe exited with %errorlevel%
   exit /b %errorlevel%
   )

POPD

exit /b 0


