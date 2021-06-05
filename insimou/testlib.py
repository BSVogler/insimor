#!/usr/local/bin/python3
import sys
import os
import time
import ctypes
from numpy.ctypeslib import ndpointer

inputdim = 4
try:
    path = "./libinsimou.dylib"  # go back because we are in the result path
    if os.path.isfile(path):
        try:
            insimor: ctypes.CDLL = ctypes.cdll.LoadLibrary(path)
        except OSError:
            print(f"Unable to load the system C library {path}")
            sys.exit()
    else:
        print(f"cannot find library {path}")
        sys.exit()
    insimor.setinput.argtypes = ctypes.POINTER(
        ctypes.c_float), ctypes.c_size_t  # pointer to float to mark an array and the size
    insimor.setinput.restype = None
    insimor.give_reward.argtypes = [ctypes.c_float]
    insimor.give_reward.restype = None
    insimor.getWeights.restype = ndpointer(dtype=ctypes.c_float,
                                           shape=(inputdim,))  # use numpy array to access C array
    insimor.getAction.restype = ndpointer(dtype=ctypes.c_float,
                                          shape=(1,))  # use numpy array to access C array
    insimor.printstats.restype = None
    insimor.stop.restype = None
except OSError:
    print("Unable to load the system C library")
    sys.exit()

observations = [3.0, 1000.0, 4.4, 3]  # Or whatever values you want to pass, but be sure to give only 4
FloatArrayDin = ctypes.c_float * len(observations)  # Define a 4-length array of floats
parameter_array = FloatArrayDin(*observations)  # Define the actual array to pass to your C function
#the following currently crashes
print("setinput…", end="")
insimor.setinput(parameter_array, len(parameter_array))
print("OK")
print("startsync…", end="")
insimor.start_sync() #blocks until spawned
print("OK")
time.sleep(0.1)
print("printstats…", end="")
insimor.printstats()
print("OK")
#will crash after a while
print("action (3x)…", end="")
insimor.setinput(parameter_array, len(parameter_array))
print(insimor.getAction())
print(insimor.getAction())
print(insimor.getAction())
print("OK")
print("stop…", end="")
insimor.stop()
print("OK")
insimor.setinput(parameter_array, len(parameter_array))
print("OK")
#might not be needed
import _ctypes
_ctypes.dlclose(insimor._handle)
del insimor