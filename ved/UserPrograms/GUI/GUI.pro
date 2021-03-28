QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
win32:RC_ICONS += release\icon\1.ico

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    copy.cpp \
    createnew.cpp \
    main.cpp \
    mainwindow.cpp \
    mount.cpp

HEADERS += \
    copy.h \
    createnew.h \
    mainwindow.h \
    mount.h

FORMS += \
    copy.ui \
    createnew.ui \
    mainwindow.ui \
    mount.ui

# Default rules for deployment.

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../bin/Lib/Win32/release/ -lLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../bin/Lib/Win32/debug/ -lLib

INCLUDEPATH += $$PWD/../../Lib
DEPENDPATH += $$PWD/../../Lib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../bin/Lib/Win32/release/libLib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../bin/Lib/Win32/debug/libLib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../bin/Lib/Win32/release/Lib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../bin/Lib/Win32/debug/Lib.lib


#win32: LIBS += -lntdll
#win32: LIBS += -lAdvapi32

#win32: LIBS += -lntdll


win64: LIBS += -lntdll



win32: LIBS += -lntdll32

win32: LIBS += -lAdvAPI86

DISTFILES += \
    release/icon/1.png
