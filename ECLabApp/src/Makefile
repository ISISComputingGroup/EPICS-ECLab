TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LIBRARY_IOC += ECLab

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install ECLab.dbd into <top>/dbd
DBD += ECLab.dbd

# specify all source files to be compiled and added to the library
#ECLab_SRCS += xxx

ECLab_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

