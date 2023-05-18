
INCLUDEPATH += $$PWD

SOURCES += $$PWD/backendLib/backend_debug.c \
    $$PWD/backendLib/backend_fine.c \
    $$PWD/backendLib/backend_main.c \
    $$PWD/backendLib/backend_multi.c \
    $$PWD/backendLib/backend_utils.c \
    $$PWD/backendLib/IRTP_layout.c

HEADERS += $$PWD/backendLib/backend_qtlib.h \
    $$PWD/backendLib/backend_common.h \
    $$PWD/backendLib/backend_debug.h \
    $$PWD/backendLib/backend_fine.h \
    $$PWD/backendLib/backend_main.h \
    $$PWD/backendLib/backend_multi.h \
    $$PWD/backendLib/backend_utils.h \
    $$PWD/backendLib/front_if.h
