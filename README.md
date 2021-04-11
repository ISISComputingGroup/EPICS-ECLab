# EPICS driver for BioLogic EC-LAB potentiostats (SP-300, SP-240 etc.)

You will need a copy of the EC-Lab(R) Development package from https://www.biologic.net/support-software/ec-lab-oem-development-package/ to build and use this software.
This vendor package is only available for Windows, so this IOC can only be built and runs on Microsoft Windows.

The `ECLABSDK` macro in `ECLabApp/src/Makefile` will then need to be changed to point to the installation location of the above developer package

Besides EPICS BASE, the ASYN module is also needed and its location must be added to `configure/RELEASE`
Other EPICS modules are optional as decribed in the comments.

Note that the EC-Lab(R) Development package will download firmware to the potentiostat that is compatible with the EC-Lab Exrpess software rather than the
full EC-Lab software. It is thus not possible to have the full EC-Lab software and the IOC communicating with the potentiostat at the same time. It is possible
to have both the EC-Lab Express and IOC communicating with the potentiostat simultaneously, but coordination is required into which piece of software uploads
techniques and downloads data.

The system is configured by specifying parameters for techniques via process variables, see the `*.substitutions` files in https://github.com/ISISComputingGroup/EPICS-ECLab/tree/master/ECLabApp/Db
for how these are set up for different techniques. The `PARAM` column defines a part of a process variale name, which corresponds to a EcLab parameter name (`LABEL`) as specified in the
ECLab developer manual for the particular technique. The biologic GUI software and developer library sometimes store parameters differently e.g separate values on the GUI may
be specified via an array - see the developer library documentation for the technique for the convention used.

Each technique has its own set of parameters and internal data format, so the software may not currently handle the technique you require. It is not too hard to add another, feel
free to contact us.    

An example of simple usage is in `test_ocv.bat`, see also `sp240.py`

The basic principle is:

- Set a filename prefix for data (written in csv format)
- Set parameters for each technique
- Write list of techniques names to use to `LOADTECH` pv - this defines the order techniques are run in (if multiple techniques are listed)
- Write to the `START` pv

To set the `record every dE` parameter for the `CV` technique you would set the `<prefix>:C0:T:CV:0:REDE:SP`
process variable before loading the technique onto the potentiostat (via LOADTECH pv). The `:C0:` in the name is for channel 0 of the potentiostat,
the second `:0:` means this value is to be used for the first instance of that named technique.

Contact freddie.akeroyd@stfc.ac.uk for further details
