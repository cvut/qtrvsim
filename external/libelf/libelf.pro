QT -= gui

TARGET = elf
CONFIG += c++11

TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$PWD

#LIBS += -lelf
QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS_DEBUG += -ggdb

DEFINES += HAVE_CONFIG_H=1

SOURCES += \
    32.fsize.c \
    32.getehdr.c \
    32.getphdr.c \
    32.getshdr.c \
    32.newehdr.c \
    32.newphdr.c \
    32.xlatetof.c \
    64.xlatetof.c \
    assert.c \
    begin.c \
    checksum.c \
    cntl.c \
    cook.c \
    data.c \
    end.c \
    errmsg.c \
    errno.c \
    fill.c \
    flag.c \
    gelfehdr.c \
    gelfphdr.c \
    gelfshdr.c \
    gelftrans.c \
    getarhdr.c \
    getaroff.c \
    getarsym.c \
    getbase.c \
    getdata.c \
    getident.c \
    getscn.c \
    hash.c \
    input.c \
    kind.c \
    ndxscn.c \
    newdata.c \
    newscn.c \
    next.c \
    nextscn.c \
    nlist.c \
    opt.delscn.c \
    rand.c \
    rawdata.c \
    rawfile.c \
    strptr.c \
    swap64.c \
    update.c \
    verdef_32_tof.c \
    verdef_32_tom.c \
    verdef_64_tof.c \
    verdef_64_tom.c \
    version.c \
    x.elfext.c \
    x.movscn.c \
    x.remscn.c \

HEADERS += \
	byteswap.h \
	config.h \
	elf_repl.h \
	errors.h \
	ext_types.h \
	gelf.h \
	libelf.h \
	nlist.h \
	private.h \
	verdef.h \
	verneed.h \
	sys_elf.h \
