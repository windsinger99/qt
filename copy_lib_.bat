@ECHO OFF
SET BUILD=Release

SET LIB_DIR=backend_750_qtlib_malloc
@REM SET LIB_DIR=backend_750_qtlib
SET LIB_NAME=libbackend_750_qtlib.a
SET PRJ_DIR=.
SET SRC_DIR=..\%LIB_DIR%\%BUILD%
@REM dir %SRC_DIR%

SET SRC_FILE=%SRC_DIR%\%LIB_NAME%
SET DST_FILE=%PRJ_DIR%\%LIB_NAME%

ECHO %LIB_NAME%
ECHO %PRJ_DIR%
ECHO %SRC_DIR%
ECHO ...

dir /q %SRC_FILE%
copy /y/v/b %SRC_FILE% %DST_FILE% 
dir /q %DST_FILE% 

pause