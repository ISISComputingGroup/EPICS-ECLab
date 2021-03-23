#!../../bin/windows-x64-debug/ECLabTest

## You may have to change ECLabTest to something else
## everywhere it appears in this file

## Increase this if you get <<TRUNCATED>> or discarded messages warnings in your errlog output
errlogInit2(65536, 256)

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/ECLabTest.dbd"
ECLabTest_registerRecordDeviceDriver pdbbase

## ECLabConfigure() arguments:
##     port_name: name of asyn port to create 
##     ip_address: pass "SIM" as ip address for simulation mode, else something in form "1.2.3.4"
##     force_firmware_reload: pass 1 here if it doesn't seem to reload automatically when it is set to 0
ECLabConfigure("chan0", "SIM", 0)

## Load our record instances
dbLoadRecords("db/ECLab.db","P=$(MYPVPREFIX),Q=ECLAB_01,PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabIntegerParams.db","P=$(MYPVPREFIX),Q=ECLAB_01,PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabBooleanParams.db","P=$(MYPVPREFIX),Q=ECLAB_01,PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabSingleParams.db","P=$(MYPVPREFIX),Q=ECLAB_01,PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabIntegerArrayParams.db","P=$(MYPVPREFIX),Q=ECLAB_01,PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabBooleanArrayParams.db","P=$(MYPVPREFIX),Q=ECLAB_01,PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabSingleArrayParams.db","P=$(MYPVPREFIX),Q=ECLAB_01,PORT=chan0,CHAN=0")

## the HEARTBEAT PV from deviocstats is used for the OPI, otherwise it is optional
dbLoadRecords("$(DEVIOCSTATS)/db/iocAdminSoft.db","IOC=$(MYPVPREFIX)CS:IOC:ECLAB_01:DEVIOS")

cd ${TOP}/iocBoot/${IOC}
iocInit
