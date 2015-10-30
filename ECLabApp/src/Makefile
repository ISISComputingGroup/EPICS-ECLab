TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# Build an IOC support library

LIBRARY_IOC += ECLab

ECLABSDK = $(TOP)/Vendor

USR_CXXFLAGS += -I$(ECLABSDK)/include -DBIOLOGIC_EXPORTS

DBD += ECLab.dbd

# The following are compiled and added to the Support library
ECLab_SRCS += ECLabParams.cpp ECLabDriver.cpp dummyFunctions.cpp

ECLab_LIBS += asyn

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
../ECLabParams.cpp : BooleanParams.cpp IntegerParams.cpp SingleParams.cpp 
../ECLabParams.cpp : BooleanArrayParams.cpp IntegerArrayParams.cpp SingleArrayParams.cpp

BooleanParams.cpp : $(TOP)/ECLabApp/Db/ECLabBooleanParams.substitutions
	$(MSI) -o $@ -M "TYPE=Boolean" -S $< $(TOP)/ECLabApp/src/ScalarParams.cpp
	
IntegerParams.cpp : $(TOP)/ECLabApp/Db/ECLabIntegerParams.substitutions
	$(MSI) -o $@ -M "TYPE=Integer" -S $< $(TOP)/ECLabApp/src/ScalarParams.cpp

SingleParams.cpp : $(TOP)/ECLabApp/Db/ECLabSingleParams.substitutions
	$(MSI) -o $@ -M "TYPE=Single" -S $< $(TOP)/ECLabApp/src/ScalarParams.cpp

BooleanArrayParams.cpp : $(TOP)/ECLabApp/Db/ECLabBooleanArrayParams.substitutions
	$(MSI) -o $@ -M "TYPE=BooleanArray" -S $< $(TOP)/ECLabApp/src/ArrayParams.cpp
	
IntegerArrayParams.cpp : $(TOP)/ECLabApp/Db/ECLabIntegerArrayParams.substitutions
	$(MSI) -o $@ -M "TYPE=IntegerArray" -S $< $(TOP)/ECLabApp/src/ArrayParams.cpp

SingleArrayParams.cpp : $(TOP)/ECLabApp/Db/ECLabSingleArrayParams.substitutions
	$(MSI) -o $@ -M "TYPE=SingleArray" -S $< $(TOP)/ECLabApp/src/ArrayParams.cpp

