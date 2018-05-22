## -*- Makefile -*-
##
## User: Alexandre
## Time: 16 janv. 2018 14:45:46
## Makefile created by Oracle Developer Studio.
##
## This file is generated automatically.
##


#### Compiler and tool definitions shared by all build targets #####
CCC = g++
CXX = g++
INCLUDE = -I./include -I. -I/usr/local/include -L/usr/local/lib64
BASICOPTS = -g -w -std=c++11 $(INCLUDE) -Wno-deprecated-declarations -lboost_system -lboost_filesystem -pthread -fPIC -lssl -lcrypto -lboost_thread -lavdevice -lavutil -lavformat -lavcodec -lcurl -lsodium -static-libstdc++ -fno-use-cxa-atexit -fexceptions -DWITH_DOM  -DWITH_OPENSSL
CCFLAGS = $(BASICOPTS)
CXXFLAGS = $(BASICOPTS) --param ggc-min-expand=20 --param ggc-min-heapsize=4096
CCADMIN =


# Define the target directories.
TARGETDIR_VideoRecorder=GNU-amd64-Linux


all: $(TARGETDIR_VideoRecorder)/VideoRecorder

## Target: VideoRecorder
OBJS_VideoRecorder =  \
	$(TARGETDIR_VideoRecorder)/main.o \
	$(TARGETDIR_VideoRecorder)/wsaapi.o \
	$(TARGETDIR_VideoRecorder)/wsseapi.o \
	$(TARGETDIR_VideoRecorder)/threads.o \
	$(TARGETDIR_VideoRecorder)/duration.o \
        $(TARGETDIR_VideoRecorder)/smdevp.o \
	$(TARGETDIR_VideoRecorder)/mecevp.o \
	$(TARGETDIR_VideoRecorder)/stdsoap2.o \
	$(TARGETDIR_VideoRecorder)/soapC.o \
	$(TARGETDIR_VideoRecorder)/soapClient.o \
	$(TARGETDIR_VideoRecorder)/dom.o \
	$(TARGETDIR_VideoRecorder)/soapDeviceBindingProxy.o \
	$(TARGETDIR_VideoRecorder)/soapMediaBindingProxy.o \
	$(TARGETDIR_VideoRecorder)/soapRemoteDiscoveryBindingProxy.o \
	$(TARGETDIR_VideoRecorder)/Manager.o \
	$(TARGETDIR_VideoRecorder)/Watchdog.o \
	$(TARGETDIR_VideoRecorder)/CustomException.o \
	$(TARGETDIR_VideoRecorder)/Utility.o \
	$(TARGETDIR_VideoRecorder)/Camera.o
USERLIBS_VideoRecorder = $(SYSLIBS_VideoRecorder)
DEPLIBS_VideoRecorder =
LDLIBS_VideoRecorder = $(USERLIBS_VideoRecorder)


# Link or archive
$(TARGETDIR_VideoRecorder)/VideoRecorder: $(TARGETDIR_VideoRecorder) $(OBJS_VideoRecorder) $(DEPLIBS_VideoRecorder)
	$(LINK.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ $(OBJS_VideoRecorder) $(LDLIBS_VideoRecorder)

# Compile source files into .o files
$(TARGETDIR_VideoRecorder)/main.o: $(TARGETDIR_VideoRecorder) main.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ main.cpp

$(TARGETDIR_VideoRecorder)/Manager.o: $(TARGETDIR_VideoRecorder) Manager.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ Manager.cpp

$(TARGETDIR_VideoRecorder)/Watchdog.o: $(TARGETDIR_VideoRecorder) Watchdog.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ Watchdog.cpp

$(TARGETDIR_VideoRecorder)/Camera.o: $(TARGETDIR_VideoRecorder) Camera.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ Camera.cpp

$(TARGETDIR_VideoRecorder)/CustomException.o: $(TARGETDIR_VideoRecorder) CustomException.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ CustomException.cpp

$(TARGETDIR_VideoRecorder)/Utility.o: $(TARGETDIR_VideoRecorder) Utility.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ Utility.cpp

$(TARGETDIR_VideoRecorder)/wsaapi.o: $(TARGETDIR_VideoRecorder) plugin/wsaapi.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ plugin/wsaapi.cpp

$(TARGETDIR_VideoRecorder)/wsseapi.o: $(TARGETDIR_VideoRecorder) plugin/wsseapi.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ plugin/wsseapi.cpp

$(TARGETDIR_VideoRecorder)/threads.o: $(TARGETDIR_VideoRecorder) plugin/threads.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ plugin/threads.cpp

$(TARGETDIR_VideoRecorder)/duration.o: $(TARGETDIR_VideoRecorder) plugin/duration.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ plugin/duration.cpp

$(TARGETDIR_VideoRecorder)/smdevp.o: $(TARGETDIR_VideoRecorder) plugin/smdevp.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ plugin/smdevp.cpp

$(TARGETDIR_VideoRecorder)/mecevp.o: $(TARGETDIR_VideoRecorder) plugin/mecevp.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ plugin/mecevp.cpp

$(TARGETDIR_VideoRecorder)/dom.o: $(TARGETDIR_VideoRecorder) plugin/dom.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ plugin/dom.cpp

$(TARGETDIR_VideoRecorder)/soapDeviceBindingProxy.o: $(TARGETDIR_VideoRecorder) proxycpp/soapDeviceBindingProxy.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ proxycpp/soapDeviceBindingProxy.cpp

$(TARGETDIR_VideoRecorder)/soapMediaBindingProxy.o: $(TARGETDIR_VideoRecorder) proxycpp/soapMediaBindingProxy.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ proxycpp/soapMediaBindingProxy.cpp

$(TARGETDIR_VideoRecorder)/soapRemoteDiscoveryBindingProxy.o: $(TARGETDIR_VideoRecorder) proxycpp/soapRemoteDiscoveryBindingProxy.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ proxycpp/soapRemoteDiscoveryBindingProxy.cpp

$(TARGETDIR_VideoRecorder)/stdsoap2.o: $(TARGETDIR_VideoRecorder) stdsoap2.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ stdsoap2.cpp

$(TARGETDIR_VideoRecorder)/soapC.o: $(TARGETDIR_VideoRecorder) soapC.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ soapC.cpp

$(TARGETDIR_VideoRecorder)/soapClient.o: $(TARGETDIR_VideoRecorder) soapClient.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ soapClient.cpp

#### Clean target deletes all generated files ####
clean:
	rm -f -r $(TARGETDIR_VideoRecorder)


# Create the target directory (if needed)
$(TARGETDIR_VideoRecorder):
	mkdir -p $(TARGETDIR_VideoRecorder)


# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-amd64-Linux

