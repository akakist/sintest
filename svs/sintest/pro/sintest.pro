#-------------------------------------------------
#
# Project created by QtCreator 2015-07-11T11:51:50
#
#-------------------------------------------------

QT       += core gui printsupport svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

CONFIG(debug, debug|release) {
TARGET = sin-test-d
DEFINES+=DEBUG _DEBUG
} else {
TARGET = sin-test
}
CONFIG+= precompile_header
PRECOMPILED_HEADER  = stable.h


DEFINES+=LARGE_GRAPH_OUTPUT WITHOUT_UPNP WITH_PILLOW_IMAGE WITH_IO __DDRAW REAL_DOUBLE


CONFIG += c++17
DEFINES += __DDRAW USE_QCustomPlot

DEFINES+=_USE_MATH_DEFINES QT5 MEGATRON NO_ICONV WITHOUT_UPNP __WITH_ZLIB
DEFINES += __FULL__ NO_EXTERNAL_CONNECTION  __STDC_LIMIT_MACROS __STDC_CONSTANT_MACROS __STDC_FORMAT_MACROS


INCLUDEPATH+= ../../../megatron ../../../megatron/corelib  ../../../megatron/modules/sqlite ../../../megatron/lib\
              ../../../megatron/lib/jsoncpp-0.5.0/include ../../events ../../common \
              ../../../megatron/main\
            ../../lib/qcustomplot-source   ../../lib/colordiff\
              ../../../megatron/lib/sqlite3 /usr/local/Cellar/openssl@3/3.1.3/include

win32{
    INCLUDEPATH += ../../../megatron/corelib/winpthread/include
    DEFINES += HAVE_STRUCT_TIMESPEC=1
}

#LIBS+= -lzint -lzbar
macx{
INCLUDEPATH+= /usr/local/Cellar/jpeg/8d/include
LIBS+=-L/usr/local/lib
}
LIBS+=-ljpeg -lzbar -lhpdf

macx{
INCLUDEPATH+=  /usr/local/include

}
win32{
INCLUDEPATH+=
}
win32{
INCLUDEPATH+=
}

android {
INCLUDEPATH+=../../../dfs_lib/lib/openssl-1.1.0/include
QT       -= core
    LIBS+= -L../../../megatron/lib/libs/$$ANDROID_TARGET_ARCH

}
macx{
LIBS+=-L/usr/local/lib  -lcrypto -lssl
# -lzint
}
win32{
LIBS+=-lkernel32 -lws2_32  -lcrypto -lssl -lgdi32 -lkernel32  -lz
}
unix{
LIBS+=  -lcrypto -lssl -ldl -lz
}




SOURCES +=\
    ../../../megatron/apps/server/mainApp.cpp \
    ../../../megatron/lib/jsoncpp-0.5.0/src/lib_json/json_reader.cpp \
    ../../../megatron/lib/jsoncpp-0.5.0/src/lib_json/json_value.cpp \
    ../../../megatron/lib/jsoncpp-0.5.0/src/lib_json/json_writer.cpp \
    ../../../megatron/corelib/__crc32.cpp \
    ../../../megatron/corelib/_bitStream.cpp \
    ../../../megatron/corelib/commonError.cpp \
    ../../../megatron/corelib/configDB.cpp \
    ../../../megatron/corelib/eventDeque.cpp \
    ../../../megatron/corelib/Integer.cpp \
    ../../../megatron/corelib/ioBuffer.cpp \
    ../../../megatron/corelib/jpge.cpp \
    ../../../megatron/corelib/msockaddr_in.cpp \
    ../../../megatron/corelib/mtimespec.cpp \
    ../../../megatron/corelib/mutexable.cpp \
    ../../../megatron/corelib/mutexInspector.cpp \
    ../../../megatron/corelib/Rational.cpp \
    ../../../megatron/corelib/unknown.cpp \
    ../../../megatron/corelib/url.cpp \
    ../../../megatron/main/CInstance.cpp \
    ../../../megatron/main/configObj.cpp \
    ../../../megatron/main/CUtils.cpp \
    ../../../megatron/main/megatron.cpp \
    ../../../megatron/main/threadNameCtl.cpp \
    ../../../megatron/corelib/_QUERY.cpp \
    ../../../megatron/corelib/broadcaster.cpp \
    ../../../megatron/corelib/DBH.cpp \
    ../../../megatron/corelib/epoll_socket_info.cpp \
    ../../../megatron/corelib/httpConnection.cpp \
    ../../../megatron/corelib/listenerBase.cpp \
    ../../../megatron/corelib/listenerBuffered.cpp \
    ../../../megatron/corelib/listenerBuffered1Thread.cpp \
    ../../../megatron/corelib/listenerPolled.cpp \
    ../../../megatron/corelib/listenerSimple.cpp \
    ../../../megatron/corelib/logging.cpp \
    ../../../megatron/corelib/objectHandler.cpp \
    ../../../megatron/corelib/route_t.cpp \
    ../../../megatron/corelib/socketsContainer.cpp \
    ../../../megatron/corelib/st_rsa.cpp \
    ../../../megatron/corelib/webDumpable.cpp \
    ../../../megatron/corelib/jsonHandler.cpp \
    ../../../megatron/corelib/js_utils.cpp \
    ../../../megatron/lib/SQLiteCpp/Backup.cpp \
    ../../../megatron/lib/SQLiteCpp/Column.cpp \
    ../../../megatron/lib/SQLiteCpp/Database.cpp \
    ../../../megatron/lib/SQLiteCpp/Exception.cpp \
    ../../../megatron/lib/SQLiteCpp/Statement.cpp \
    ../../../megatron/lib/SQLiteCpp/Transaction.cpp \
    ../../../megatron/main/iutils_decl.cpp \
    ../../../megatron/corelib/networkMultiplexor.cpp \
    ../../../megatron/corelib/neighbours.cpp \
    ../../../megatron/lib/sqlite3/sqlite3.c \
    ../../../megatron/modules/dfsCaps/CapsAlgorithm.cpp \
    ../../../megatron/modules/dfsCaps/dfsCapsService.cpp \
    ../../../megatron/modules/dfsReferrer/dfsReferrerService.cpp \
    ../../../megatron/modules/dfsReferrer/uplinkconnectionstate.cpp \
    ../../../megatron/modules/dfsReferrer/uriReferals.cpp \
    ../../../megatron/modules/errorDispatcher/errorDispatcherService.cpp \
    ../../../megatron/modules/http/httpService.cpp \
    ../../../megatron/modules/http/mime_types.cpp \
    ../../../megatron/modules/objectProxy/objectProxyService.cpp \
    ../../../megatron/modules/oscar/oscarService.cpp \
    ../../../megatron/modules/oscarSecure/oscarSecureService.cpp \
    ../../../megatron/modules/oscarSecure/oscarSecureUser.cpp \
    ../../../megatron/modules/referrerClient/referrerClientService.cpp \
    ../../../megatron/modules/rpc/rpcService.cpp \
    ../../../megatron/modules/socket/socketService.cpp \
    ../../../megatron/modules/ssl/ssl_reg.cpp \
    ../../../megatron/modules/ssl/st_rsa_impl.cpp \
    ../../../megatron/modules/telnet/telnetService.cpp \
    ../../../megatron/modules/timer/timerService.cpp \
    ../../../megatron/modules/webHandler/webHandlerService.cpp \
    ../client/sinTestClientService.cpp \
    ../server/sinTestServerService.cpp

win32{

}


win32{

}

HEADERS  += \
    ../../../megatron/corelib/__crc32.h \
    ../../../megatron/corelib/AUTHCOOKIE_id.h \
    ../../../megatron/corelib/_bitStream.h \
    ../../../megatron/corelib/bufferVerify.h \
    ../../../megatron/corelib/CHUNK_id.h \
    ../../../megatron/corelib/CLASS_id.h \
    ../../../megatron/corelib/commonError.h \
    ../../../megatron/corelib/compat_win32.h \
    ../../../megatron/corelib/configDB.h \
    ../../../megatron/corelib/CONTAINER.h \
    ../../../megatron/corelib/DB_id.h \
    ../../../megatron/corelib/errorDispatcherCodes.h \
    ../../../megatron/corelib/EVENT_id.h \
    ../../../megatron/corelib/eventDeque.h \
    ../../../megatron/corelib/IConfigDB.h \
    ../../../megatron/corelib/IConfigObj.h \
    ../../../megatron/corelib/ifaces.h \
    ../../../megatron/corelib/IInstance.h \
    ../../../megatron/corelib/ILogger.h \
    ../../../megatron/corelib/Integer.h \
    ../../../megatron/corelib/IObjectProxy.h \
    ../../../megatron/corelib/ioBuffer.h \
    ../../../megatron/corelib/IProgress.h \
    ../../../megatron/corelib/IRPC.h \
    ../../../megatron/corelib/ISSL.h \
    ../../../megatron/corelib/IThreadNameController.h \
    ../../../megatron/corelib/IUtils.h \
    ../../../megatron/corelib/jpge.h \
    ../../../megatron/corelib/logging.h \
    ../../../megatron/corelib/msockaddr_in.h \
    ../../../megatron/corelib/mtimespec.h \
    ../../../megatron/corelib/mutex_inspector_entry.h \
    ../../../megatron/corelib/mutexable.h \
    ../../../megatron/corelib/mutexInspector.h \
    ../../../megatron/corelib/Rational.h \
    ../../../megatron/corelib/REF.h \
    ../../../megatron/corelib/refstring.h \
    ../../../megatron/corelib/route_t.h \
    ../../../megatron/corelib/SERVICE_id.h \
    ../../../megatron/corelib/SOCKET_fd.h \
    ../../../megatron/corelib/SOCKET_id.h \
    ../../../megatron/corelib/st_FILE.h \
    ../../../megatron/corelib/st_malloc.h \
    ../../../megatron/corelib/st_rsa.h \
    ../../../megatron/corelib/tools_mt.h \
    ../../../megatron/corelib/unknown.h \
    ../../../megatron/corelib/unknownCastDef.h \
    ../../../megatron/corelib/url.h \
    ../../../megatron/corelib/USER_id.h \
    ../../../megatron/corelib/VERSION_id.h \
    ../../../megatron/corelib/version_mega.h \
    ../../../megatron/main/CInstance.h \
    ../../../megatron/main/configObj.h \
    ../../../megatron/main/CUtils.h \
    ../../../megatron/main/megatron.h \
    ../../../megatron/main/progressor.h \
    ../../../megatron/main/threadNameCtl.h \
    ../../../megatron/corelib/_QUERY.h \
    ../../../megatron/corelib/broadcaster.h \
    ../../../megatron/corelib/DBH.h \
    ../../../megatron/corelib/e_poll.h \
    ../../../megatron/corelib/epoll_socket_info.h \
    ../../../megatron/corelib/event.h \
    ../../../megatron/corelib/eventEnum.h \
    ../../../megatron/corelib/GlobalCookie_id.h \
    ../../../megatron/corelib/httpConnection.h \
    ../../../megatron/corelib/listenerBase.h \
    ../../../megatron/corelib/listenerBuffered.h \
    ../../../megatron/corelib/listenerBuffered1Thread.h \
    ../../../megatron/corelib/listenerPolled.h \
    ../../../megatron/corelib/listenerSimple.h \
    ../../../megatron/corelib/objectHandler.h \
    ../../../megatron/corelib/queryResult.h \
    ../../../megatron/corelib/serviceEnum.h \
    ../../../megatron/corelib/socketsContainer.h \
    ../../../megatron/corelib/webDumpable.h \
    ../../../megatron/corelib/jsonHandler.h \
    ../../../megatron/corelib/js_utils.h \
    ../../../megatron/lib/SQLiteCpp/Assertion.h \
    ../../../megatron/lib/SQLiteCpp/Backup.h \
    ../../../megatron/lib/SQLiteCpp/Column.h \
    ../../../megatron/lib/SQLiteCpp/Database.h \
    ../../../megatron/lib/SQLiteCpp/Exception.h \
    ../../../megatron/lib/SQLiteCpp/SQLiteCpp.h \
    ../../../megatron/lib/SQLiteCpp/Statement.h \
    ../../../megatron/lib/SQLiteCpp/Transaction.h \
    ../../../megatron/lib/SQLiteCpp/Utils.h \
    ../../../megatron/lib/SQLiteCpp/VariadicBind.h \
    ../../../megatron/main/utils_local.h \
    ../../../megatron/corelib/networkMultiplexor.h \
    ../../../megatron/corelib/neighbours.h \
    ../../../megatron/lib/sqlite3/sqlite3.h \
    ../../../megatron/modules/dfsReferrer/dfsReferrerService.h \
    ../../../megatron/modules/errorDispatcher/errorDispatcherService.h \
    ../../../megatron/modules/http/httpService.h \
    ../../../megatron/modules/objectProxy/objectProxyService.h \
    ../../../megatron/modules/oscar/oscarService.h \
    ../../../megatron/modules/oscarSecure/oscarSecureService.h \
    ../../../megatron/modules/oscarSecure/oscarSecureUser.h \
    ../../../megatron/modules/referrerClient/referrerClientService.h \
    ../../../megatron/modules/rpc/rpcService.h \
    ../../../megatron/modules/socket/socketService.h \
    ../../../megatron/modules/ssl/st_rsa_impl.h \
    ../../../megatron/modules/telnet/telnetService.h \
    ../../../megatron/modules/timer/timerService.h \
    ../../../megatron/modules/webHandler/webHandlerService.h \
    ../client/sinTestClientService.h \
    ../server/sinTestServerService.h




FORMS    += \

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \

RESOURCES += \

SUBDIRS += \

DISTFILES += \
