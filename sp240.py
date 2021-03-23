from genie_python import genie as g
import inst

def runscript():
    while True:
        g.begin()
        g.waitfor(uamps=85)
        g.end()

def testscript():
    setupCP()
#    setupOCV()
    sp240start()
    while True:
        g.begin()
        g.waitfor(uamps=85)
        g.end()

def setupCP():
    vsinit = [ 0 ]
    istep = [ 3.5e-4 ]
    durstep = [ 200000 ]
    stepn = len(istep) - 1

    g.set_pv("ECLAB_01:C0:T:CP:0:REDT:SP", 120, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:REDE:SP", 10e-3, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:TB:SP", 30e-6, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:VSINIT:SP", vsinit, is_local=True) 
    g.set_pv("ECLAB_01:C0:T:CP:0:ISTEP:SP", istep, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:DURSTEP:SP", durstep, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:IRANGE:SP", 7, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:ERANGE:SP", 1, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:BW:SP", 5, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:STEPN:SP", stepn, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:NCYCLES:SP", 0, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:XCTR:SP", 32+64, is_local=True) # 1 + 32 + 64

    g.set_pv("ECLAB_01:C0:LOADTECH:SP", "cp", is_local=True)

def sp240start():
     g.set_pv("ECLAB_01:C0:START", 1, is_local=True)

def sp240stop():
     g.set_pv("ECLAB_01:C0:STOP", 1, is_local=True)




def setupOCV():
    # set filename
    g.set_pv("ECLAB_01:C0:FILEPREFIX:SP", "c:/data/eclab", is_local=True)

    # set first ocv (0)
    g.set_pv("ECLAB_01:C0:T:OCV:0:RTT:SP", 50000, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:0:REDE:SP", 10e-3, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:0:REDT:SP", 120, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:0:TB:SP", 25e-6, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:0:IRANGE:SP", 12, is_local=True) #7
    g.set_pv("ECLAB_01:C0:T:OCV:0:ERANGE:SP", 3, is_local=True) # 1
    g.set_pv("ECLAB_01:C0:T:OCV:0:BW:SP", 5, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:0:XCTR:SP", 0, is_local=True)

    g.set_pv("ECLAB_01:C0:LOADTECH:SP", "ocv", is_local=True)


def setupCPLimit():
    vsinit = [ 0 ]
    istep = [ 3.5e-4 ]
    durstep = [ 200000 ]
    stepn = len(istep) - 1
# test1conf:
#     default E (potential), +96 for I (current) 
#     default <= , +4 for >=
#     default OR between tests, +2 for AND
#     default not active, +1 for active 
    test1conf = [ 1 ] 
    test1val = [ 1 ]
    exitcond = [ 0 ] # exitcond: 0 = next step, 1=next technique, 2=stop experiment

    g.set_pv("ECLAB_01:C0:T:CPLIMIT:0:REDT:SP", 120, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CPLIMIT:0:REDE:SP", 10e-3, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CPLIMIT:0:TB:SP", 34e-6, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CPLIMIT:0:VSINIT:SP", vsinit, is_local=True) 
    g.set_pv("ECLAB_01:C0:T:CPLIMIT:0:ISTEP:SP", istep, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CPLIMIT:0:DURSTEP:SP", durstep, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CPLIMIT:0:IRANGE:SP", 7, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CPLIMIT:0:ERANGE:SP", 1, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CPLIMIT:0:BW:SP", 5, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CPLIMIT:0:STEPN:SP", stepn, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CPLIMIT:0:NCYCLES:SP", 0, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CPLIMIT:0:XCTR:SP", 32+64, is_local=True) # 1 + 32 + 64
    g.set_pv("ECLAB_01:C0:T:CPLIMIT:0:TEST1CONF:SP", test1conf, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CPLIMIT:0:TEST1VAL:SP", test1val, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CPLIMIT:0:EXITCOND:SP", exitcond, is_local=True)

    g.set_pv("ECLAB_01:C0:LOADTECH:SP", "cplimit", is_local=True)


#### not used below here ###
def sp240setup():
    # set filename
    g.set_pv("ECLAB_01:C0:FILEPREFIX:SP", "c:/data/eclab", is_local=True)

    # set first ocv (0)
    g.set_pv("ECLAB_01:C0:T:OCV:0:RTT:SP", 50000, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:0:REDE:SP", 10e-3, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:0:REDT:SP", 120, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:0:TB:SP", 20e-6, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:0:IRANGE:SP", 7, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:0:ERANGE:SP", 1, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:0:BW:SP", 5, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:0:XCTR:SP", 0, is_local=True)

    # set second ocv (1)
    g.set_pv("ECLAB_01:C0:T:OCV:1:RTT:SP", 10, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:1:REDE:SP", 1, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:1:REDT:SP", 1, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:1:TB:SP", 20e-6, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:1:IRANGE:SP", 8, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:1:ERANGE:SP", 3, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:1:BW:SP", 5, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:1:XCTR:SP", 0, is_local=True)

    vsinit = [ 1 ]
    istep = [ 7e-4 ]
    durstep = [ 100000 ]
    stepn = len(istep) - 1

    g.set_pv("ECLAB_01:C0:T:CP:0:REDT:SP", 120, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:REDE:SP", 10e-3, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:TB:SP", 30e-6, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:VSINIT:SP", vsinit, is_local=True) 
    g.set_pv("ECLAB_01:C0:T:CP:0:ISTEP:SP", istep, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:DURSTEP:SP", durstep, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:IRANGE:SP", 7, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:ERANGE:SP", 3, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:BW:SP", 5, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:STEPN:SP", stepn, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:NCYCLES:SP", 0, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CP:0:XCTR:SP", 32+64, is_local=True) # 1 + 32 + 64

    vsinit = [ 1, 1, 1 ]
    vstep = [ 1.0, 1.0, 1.0 ]
    durstep = [ 10.0, 11.0, 12.0 ]
    stepn = len(vstep) - 1

    g.set_pv("ECLAB_01:C0:T:CA:0:REDT:SP", 0.1, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CA:0:REDI:SP", 0.1, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CA:0:TB:SP", 25e-6, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CA:0:VSINIT:SP", vsinit, is_local=True) 
    g.set_pv("ECLAB_01:C0:T:CA:0:VSTEP:SP", istep, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CA:0:DURSTEP:SP", durstep, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CA:0:IRANGE:SP", 8, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CA:0:ERANGE:SP", 3, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CA:0:BW:SP", 5, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CA:0:STEPN:SP", stepn, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CA:0:NCYCLES:SP", 0, is_local=True)
    g.set_pv("ECLAB_01:C0:T:CA:0:XCTR:SP", 0, is_local=True) # 1 + 32 + 64

    g.set_pv("ECLAB_01:C0:LOADTECH:SP", "ocv,ca,cp,ocv", is_local=True)
     

def sp240stop_ocv_0():
    g.set_pv("ECLAB_01:C0:UPDATE:SP", 1, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:0:RTT:SP", 1, is_local=True)

def sp240stop_ocv_1():
    g.set_pv("ECLAB_01:C0:UPDATE:SP", 1, is_local=True)
    g.set_pv("ECLAB_01:C0:T:OCV:1:RTT:SP", 1, is_local=True)

