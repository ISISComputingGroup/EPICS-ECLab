TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# Build an IOC support library

LIBRARY_IOC += ECLab

ECLABSDK = $(TOP)/Vendor

USR_CXXFLAGS += -I$(ECLABSDK)/include

DBD += ECLab.dbd

# The following are compiled and added to the Support library
ECLab_SRCS += ECLabDriver.cpp 

ifneq ($(findstring windows,$(EPICS_HOST_ARCH)),) 
ECLab_LIBS += eclib64
BIN_INSTALLS_WIN32 += $(wildcard $(ECLABSDK)/dll/win64/*.dll)
LIB_INSTALLS_WIN32 += $(wildcard $(ECLABSDK)/lib/win64/*.lib)
endif
ifneq ($(findstring win32,$(EPICS_HOST_ARCH)),) 
ECLab_LIBS += eclib
BIN_INSTALLS_WIN32 += $(wildcard $(ECLABSDK)/dll/win32/*.dll)
LIB_INSTALLS_WIN32 += $(wildcard $(ECLABSDK)/lib/win32/*.lib)
endif

eclib64_DIR = $(ECLABSDK)/lib/win64
eclib_DIR = $(ECLABSDK)/lib/win32

ECLab_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
