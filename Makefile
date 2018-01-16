## -*- Makefile -*-
##
## User: Alexandre
## Time: 16 janv. 2018 10:54:21
## Makefile created by Oracle Developer Studio.
##
## This file is generated automatically.
##


#### Compiler and tool definitions shared by all build targets #####
CCC = g++
CXX = g++
BASICOPTS = -g -std=c++11 -I /usr/local/include -L /usr/local/lib -lopencv_core -lopencv_highgui
CCFLAGS = $(BASICOPTS)
CXXFLAGS = $(BASICOPTS)
CCADMIN = 


# Define the target directories.
TARGETDIR_VideoRecorder=GNU-amd64-Linux


all: $(TARGETDIR_VideoRecorder)/VideoRecorder

## Target: VideoRecorder
OBJS_VideoRecorder =  \
	$(TARGETDIR_VideoRecorder)/Camera.o \
	$(TARGETDIR_VideoRecorder)/Utility.o \
	$(TARGETDIR_VideoRecorder)/main.o
USERLIBS_VideoRecorder = $(SYSLIBS_VideoRecorder) 
DEPLIBS_VideoRecorder =  
LDLIBS_VideoRecorder = $(USERLIBS_VideoRecorder)


# Link or archive
$(TARGETDIR_VideoRecorder)/VideoRecorder: $(TARGETDIR_VideoRecorder) $(OBJS_VideoRecorder) $(DEPLIBS_VideoRecorder)
	$(LINK.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ $(OBJS_VideoRecorder) $(LDLIBS_VideoRecorder)


# Compile source files into .o files
$(TARGETDIR_VideoRecorder)/Camera.o: $(TARGETDIR_VideoRecorder) Camera.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ Camera.cpp

$(TARGETDIR_VideoRecorder)/Utility.o: $(TARGETDIR_VideoRecorder) Utility.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ Utility.cpp

$(TARGETDIR_VideoRecorder)/main.o: $(TARGETDIR_VideoRecorder) main.cpp
	$(COMPILE.cc) $(CCFLAGS_VideoRecorder) $(CPPFLAGS_VideoRecorder) -o $@ main.cpp



#### Clean target deletes all generated files ####
clean:
	rm -f \
		$(TARGETDIR_VideoRecorder)/VideoRecorder \
		$(TARGETDIR_VideoRecorder)/Camera.o \
		$(TARGETDIR_VideoRecorder)/Utility.o \
		$(TARGETDIR_VideoRecorder)/main.o
	$(CCADMIN)
	rm -f -r $(TARGETDIR_VideoRecorder)


# Create the target directory (if needed)
$(TARGETDIR_VideoRecorder):
	mkdir -p $(TARGETDIR_VideoRecorder)


# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-amd64-Linux

