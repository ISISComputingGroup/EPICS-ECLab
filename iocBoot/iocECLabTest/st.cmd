#!../../bin/windows-x64-debug/ECLabTest

## You may have to change ECLabTest to something else
## everywhere it appears in this file

## Increase this if you get <<TRUNCATED>> or discarded messages warnings in your errlog output
errlogInit2(65536, 256)

< envPaths

cd ${TOP}

## prefix for IOC PVs in addition to $(MYPVPREFIX)
epicsEnvSet("IOCPRFX", "ECLAB_01")

## Register all support components
dbLoadDatabase "dbd/ECLabTest.dbd"
ECLabTest_registerRecordDeviceDriver pdbbase

## ECLabConfigure(port_name, ip_address, force_firmware_reload)
##     port_name: (string) name of asyn port to create 
##     ip_address: (string) pass "SIM" as ip address for simulation mode, else something in form "1.2.3.4"
##     force_firmware_reload: (integer) try 1 here if it doesn't seem to reload automatically when it is 0
ECLabConfigure("chan0", "SIM", 0)

## Load our record instances
dbLoadRecords("db/ECLab.db","P=$(MYPVPREFIX),Q=$(IOCPRFX),PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabIntegerParams.db","P=$(MYPVPREFIX),Q=$(IOCPRFX),PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabBooleanParams.db","P=$(MYPVPREFIX),Q=$(IOCPRFX),PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabSingleParams.db","P=$(MYPVPREFIX),Q=$(IOCPRFX),PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabIntegerArrayParams.db","P=$(MYPVPREFIX),Q=$(IOCPRFX),PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabBooleanArrayParams.db","P=$(MYPVPREFIX),Q=$(IOCPRFX),PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabSingleArrayParams.db","P=$(MYPVPREFIX),Q=$(IOCPRFX),PORT=chan0,CHAN=0")

## the HEARTBEAT PV from deviocstats is used for the CS Studio OPI, otherwise the module is optional
dbLoadRecords("$(DEVIOCSTATS)/db/iocAdminSoft.db","IOC=$(MYPVPREFIX)CS:IOC:$(IOCPRFX):DEVIOS")

cd ${TOP}/iocBoot/${IOC}
iocInit
