@echo off

WHERE git > nul
IF %errorlevel%==0 (
	IF NOT EXIST _vendor/_util (
		git clone https://github.com/peTheProgrammer/c_workspace_windows.git c_workspace_windows
		robocopy c_workspace_windows/ _vendor/ * /E /MOV
		rmdir c_workspace_windows /S /Q
	)
	cd _vendor
	CALL _util\_setup.bat
	IF NOT EXIST project_list.lua (
		echo include "c_core"> project_list.lua
		echo include "c_file">> project_list.lua
		echo include "c_db">> project_list.lua
		echo include "c_log">> project_list.lua
	)
	set config=Release
	CALL build.bat c_db
	cd ..
	python setup.py build
) ELSE (
	echo Git not found.
	echo Please download and install git.
	echo Make sure it is in the PATH environment variable and run this script again.
)
