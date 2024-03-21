#-------------------------------------------------
#
# Project created by QtCreator 2015-05-11T16:52:48
#
#-------------------------------------------------

QT       += core gui sql
QT       += widgets
QT       += network
QT       += multimediawidgets
TARGET = login
TEMPLATE = app

RC_FILE = myapp.rc


SOURCES += main.cpp\
    addgroup.cpp \
    adduser.cpp \
    joingroup.cpp \
        login.cpp \
    myqlabel.cpp \
    passwdedit.cpp \
    register.cpp \
    systemtrayicon.cpp\
    server.cpp \
    client.cpp \
    tcp_weiget.cpp \
    udp_weiget.cpp \
    user_list.cpp

HEADERS  += login.h \
    addgroup.h \
    adduser.h \
    joingroup.h \
    myqlabel.h \
    passwdedit.h \
    register.h \
    systemtrayicon.h\
    server.h \
    client.h \
    tcp_weiget.h \
    udp_weiget.h \
    user_list.h

FORMS    += login.ui \
    addgroup.ui \
    adduser.ui \
    joingroup.ui \
    myqlabel.ui \
    passwdedit.ui \
    register.ui\
    server.ui \
    client.ui \
    tcp_weiget.ui \
    udp_weiget.ui \
    user_list.ui



RESOURCES += \
    myqrc.qrc \
    lang.qrc \
    qss.qrc

TRANSLATIONS+=cn.ts\
              en.ts

DISTFILES +=
