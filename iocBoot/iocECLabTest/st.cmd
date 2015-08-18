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

##ISIS## Run IOC initialisation 
< $(IOCSTARTUP)/init.cmd

ECLabConfigure("test", 130.246.36.213)

## Load record instances

##ISIS## Load common DB records 
< $(IOCSTARTUP)/dbload.cmd

## Load our record instances
dbLoadRecords("db/ECLab.db","P=$(MYPVPREFIX),PORT=test")
dbLoadRecords("db/ECLabIntegerParams.db","P=$(MYPVPREFIX),PORT=test,CHAN=0")
dbLoadRecords("db/ECLabBooleanParams.db","P=$(MYPVPREFIX),PORT=test,CHAN=0")
dbLoadRecords("db/ECLabSingleParams.db","P=$(MYPVPREFIX),PORT=test,CHAN=0")
dbLoadRecords("db/ECLabIntegerArrayParams.db","P=$(MYPVPREFIX),PORT=test,CHAN=0")
dbLoadRecords("db/ECLabBooleanArrayParams.db","P=$(MYPVPREFIX),PORT=test,CHAN=0")
dbLoadRecords("db/ECLabSingleArrayParams.db","P=$(MYPVPREFIX),PORT=test,CHAN=0")

##ISIS## Stuff that needs to be done after all records are loaded but before iocInit is called 
< $(IOCSTARTUP)/preiocinit.cmd

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncxxx,"user=faa59Host"

##ISIS## Stuff that needs to be done after iocInit is called e.g. sequence programs 
< $(IOCSTARTUP)/postiocinit.cmd
