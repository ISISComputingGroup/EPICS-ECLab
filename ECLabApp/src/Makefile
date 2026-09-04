TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# Build an IOC support library

LIBRARY_IOC += ECLab

# location of ECLab OEM development kit
# layout changed over time, so specify
# root of SDK
ECLABSDK = $(ICPBINARYDIR)/ECLab
# where to find BLFunctions.h and BLStructs.h
ECLABINCLUDE = $(ECLABSDK)/Examples/C-C++/include
# directory of lib, dll and ecc files
ECLABLIB = $(ECLABSDK)/lib

USR_CXXFLAGS += -I$(ECLABINCLUDE) -DBIOLOGIC_EXPORTS

DBD += ECLab.dbd

# The following are compiled and added to the Support library
ECLab_SRCS += ECLabParams.cpp ECLabDriver.cpp dummyFunctions.cpp ECLabCodeLookup.cpp
ECLab_LIBS += asyn

ifneq ($(findstring windows,$(EPICS_HOST_ARCH)),) 
ECLab_LIBS += EClib64 # blfind64
endif
ifneq ($(findstring win32,$(EPICS_HOST_ARCH)),) 
ECLab_LIBS += EClib # blfind
endif

BIN_INSTALLS_WIN32 += $(wildcard $(ECLABLIB)/*.dll)
LIB_INSTALLS_WIN32 += $(wildcard $(ECLABLIB)/*.lib)
EClib64_DIR = $(ECLABLIB)
EClib_DIR = $(ECLABLIB)

ECLab_LIBS += $(EPICS_BASE_IOC_LIBS)

# A version of MSI is included in epics base 3.15 and 7 and referenced by the MSI3_15 macro
# If this macro is not defined, it will try looking for msi.exe in your current windows PATH

ifeq ($(MSI3_15),)
MSI3_15 := msi
endif

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
../ECLabParams.cpp : BooleanParams.cpp IntegerParams.cpp SingleParams.cpp 
../ECLabParams.cpp : BooleanArrayParams.cpp IntegerArrayParams.cpp SingleArrayParams.cpp

BooleanParams.cpp : $(TOP)/ECLabApp/Db/ECLabBooleanParams.substitutions
	$(MSI3_15) -o $@ -M "TYPE=Boolean" -S $< $(TOP)/ECLabApp/src/ScalarParams.cpp
	
IntegerParams.cpp : $(TOP)/ECLabApp/Db/ECLabIntegerParams.substitutions
	$(MSI3_15) -o $@ -M "TYPE=Integer" -S $< $(TOP)/ECLabApp/src/ScalarParams.cpp

SingleParams.cpp : $(TOP)/ECLabApp/Db/ECLabSingleParams.substitutions
	$(MSI3_15) -o $@ -M "TYPE=Single" -S $< $(TOP)/ECLabApp/src/ScalarParams.cpp

BooleanArrayParams.cpp : $(TOP)/ECLabApp/Db/ECLabBooleanArrayParams.substitutions
	$(MSI3_15) -o $@ -M "TYPE=BooleanArray" -S $< $(TOP)/ECLabApp/src/ArrayParams.cpp
	
IntegerArrayParams.cpp : $(TOP)/ECLabApp/Db/ECLabIntegerArrayParams.substitutions
	$(MSI3_15) -o $@ -M "TYPE=IntegerArray" -S $< $(TOP)/ECLabApp/src/ArrayParams.cpp

SingleArrayParams.cpp : $(TOP)/ECLabApp/Db/ECLabSingleArrayParams.substitutions
	$(MSI3_15) -o $@ -M "TYPE=SingleArray" -S $< $(TOP)/ECLabApp/src/ArrayParams.cpp
	$(MKDIR) $(TOP)/ecc
	$(CP) $(ECLABLIB)/*.ecc $(TOP)/ecc
	$(CP) $(ECLABLIB)/*.bin $(TOP)/ecc
	$(CP) $(ECLABLIB)/*.xlx $(TOP)/ecc
