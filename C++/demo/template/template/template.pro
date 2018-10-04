QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    threadpool.cpp \
    test.cpp

INCLUDEPATH += /usr/include/boost
INCLUDEPATH += /usr/local/include/boost

LIBS += /usr/local/lib/libboost_timer.a
LIBS += /usr/local/lib/libboost_system.a
LIBS += /usr/local/lib/libboost_thread.a
LIBS += /usr/local/lib/libboost_math_c99.a
LIBS += /usr/local/lib/libboost_atomic.a
LIBS += /usr/local/lib/libboost_chrono.a
LIBS += /usr/local/lib/libboost_container.a
LIBS += /usr/local/lib/libboost_context.a
LIBS += /usr/local/lib/libboost_contract.a
LIBS += /usr/local/lib/libboost_coroutine.a
LIBS += /usr/local/lib/libboost_date_time.a
LIBS += /usr/local/lib/libboost_exception.a
LIBS += /usr/local/lib/libboost_filesystem.a
LIBS += /usr/local/lib/libboost_graph.a
LIBS += /usr/local/lib/libboost_iostreams.a
LIBS += /usr/local/lib/libboost_locale.a
LIBS += /usr/local/lib/libboost_log.a
LIBS += /usr/local/lib/libboost_math_c99l.a
LIBS += /usr/local/lib/libboost_math_tr1.a
LIBS += /usr/local/lib/libboost_math_tr1f.a
LIBS += /usr/local/lib/libboost_math_tr1l.a
LIBS += /usr/local/lib/libboost_python27.a
LIBS += /usr/local/lib/libboost_random.a
LIBS += /usr/local/lib/libboost_regex.a
LIBS += /usr/local/lib/libboost_serialization.a
LIBS += /usr/local/lib/libboost_signals.a

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    logcall.h \
    scopeguard.h \
    threadpool.h \
    test.h
