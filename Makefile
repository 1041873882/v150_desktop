
CC = arm-linux-gnueabi-gcc
CPP = arm-linux-gnueabi-g++
STRIP = arm-linux-gnueabi-strip
AR = arm-linux-gnueabi-ar

SUBDIR = . misc media ui d600 app \
	app/intercom app/setup app/message \
	app/security app/misc app/smart \
	NT NT/elev NT/intercom NT/msg NT/security NT/setup

INC = $(foreach dir,$(SUBDIR),-I$(dir))
CFLAGS += $(INC) -I./include -I./include/ipwatchd -I./include/freetype2 -I./include/curl
CXXFLAGS += $(INC) -I./include -I./include/ipwatchd -I./include/freetype2 -I./include/curl
CFLAGS += -O2 -mfpu=vfp3 -march=armv7-a -Wall -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare
CXXFLAGS += -O2 -mfpu=vfp3 -march=armv7-a -Wall -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare

LIBS += ./libs/libcharset.a \
	./libs/libiconv.a \
	./libs/libfreetype.a \
	./libs/libjpeg.a \
	./libs/libpng.a \
	./libs/libz.a \
	./libs/libmisc.a \
	./libs/libpcap.a \
	./libs/libnet.a \
	./libs/libswscale.a \
	./libs/libavformat.a \
	./libs/libavcodec.a \
	./libs/libavutil.a \
	./libs/libcurl.a \
	-lpthread -lnsl -lrt -lresolv

CSRCS    = $(foreach dir,$(SUBDIR),$(wildcard $(dir)/*.c))
CPPSRCS  = $(foreach dir,$(SUBDIR),$(wildcard $(dir)/*.cpp))
OBJS     = $(CSRCS:%.c=%.o) $(CPPSRCS:%.cpp=%.o)

TARGET := desktop
.PHONY : clean all

all: upVersion $(TARGET)

$(TARGET): $(OBJS)
	$(CPP) -O2 -o $@ $^ $(LIBS)
	$(STRIP) $@
	# cp $@ /opt/nfs/app
	cp $@ /home/pub/ybh/v150_desktop

upVersion:
	@touch sys.cpp

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CPP) -c $(CXXFLAGS) -o "$@" "$<"

.cc.o:
	$(CPP) -c $(CXXFLAGS) -o "$@" "$<"

.cxx.o:
	$(CPP) -c $(CXXFLAGS) -o "$@" "$<"

.C.o:
	$(CPP) -c $(CXXFLAGS) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) -o "$@" "$<"

clean:
	@rm -f $(TARGET)
	@rm -f $(OBJS)
