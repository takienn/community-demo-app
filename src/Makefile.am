XERCES_LIBS = -l$(LIB_XERCES)

bin_PROGRAMS = itscoopdemoapp

COMMON_LIBS = libitscoopdemoapp.a \
./utils/options/liboptions.a \
./utils/common/libcommon.a \
./utils/xml/libxml.a \
./utils/iodevices/libiodevices.a \
./foreign/tcpip/libtcpip.a \
./utils/log/liblog.a \
$(XERCES_LIBS)

noinst_LIBRARIES = libitscoopdemoapp.a

libitscoopdemoapp_a_SOURCES = server.h server.cpp \
app-commands-subscriptions-constants.h \
application-logic.h application-logic.cpp

itscoopdemoapp_SOURCES = its_demo_app_main.cpp 

itscoopdemoapp_LDFLAGS = $(XERCES_LDFLAGS)

itscoopdemoapp_LDADD = $(COMMON_LIBS)

SUBDIRS = utils foreign

EXTRA_DIST = config.h
