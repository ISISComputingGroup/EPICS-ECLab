#!../../bin/windows-x64-debug/ECLabTest

## You may have to change ECLabTest to something else
## everywhere it appears in this file

# Increase this if you get <<TRUNCATED>> or discarded messages warnings in your errlog output
errlogInit2(65536, 256)

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/ECLabTest.dbd"
ECLabTest_registerRecordDeviceDriver pdbbase


#ECLabConfigure("chan0", 130.246.36.213)
# pass "SIM" as ip address for simulation mode
ECLabConfigure("chan0", "SIM")

## Load our record instances
dbLoadRecords("db/ECLab.db","P=$(MYPVPREFIX),Q=ECLAB,PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabIntegerParams.db","P=$(MYPVPREFIX),Q=ECLAB,PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabBooleanParams.db","P=$(MYPVPREFIX),Q=ECLAB,PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabSingleParams.db","P=$(MYPVPREFIX),Q=ECLAB,PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabIntegerArrayParams.db","P=$(MYPVPREFIX),Q=ECLAB,PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabBooleanArrayParams.db","P=$(MYPVPREFIX),Q=ECLAB,PORT=chan0,CHAN=0")
dbLoadRecords("db/ECLabSingleArrayParams.db","P=$(MYPVPREFIX),Q=ECLAB,PORT=chan0,CHAN=0")

cd ${TOP}/iocBoot/${IOC}
iocInit

