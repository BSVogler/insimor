#!/usr/local/bin/python3
import sys
import time
import ctypes
from numpy.ctypeslib import ndpointer
from ctypes import *

try:
    insimou = _lib = ctypes.cdll.LoadLibrary("libinsimou.dylib")
    insimou.setinput.argtypes = ctypes.POINTER(c_float), c_size_t
    insimou.setinput.restype = None
    insimou.getAction.restype = ndpointer(dtype=ctypes.c_float, shape=(1,))
    insimou.printstats.restype = None
except OSError:
    print("Unable to load the system C library")
    sys.exit()

observations = [3.0, 1000.0, 4.4, 3]  # Or whatever values you want to pass, but be sure to give only 4
FloatArrayDin = ctypes.c_float * len(observations)  # Define a 4-length array of floats
parameter_array = FloatArrayDin(*observations)  # Define the actual array to pass to your C function
#the following currently crashes
print("setinput…", end="")
insimou.setinput(parameter_array, len(parameter_array))
print("OK")
print("startsync…", end="")
insimou.start_sync() #blocks until spawned
print("OK")
time.sleep(0.1)
print("printstats…", end="")
insimou.printstats()
print("OK")
#will crash after a while
print("action (3x)…", end="")
print(insimou.getAction())
print(insimou.getAction())
print(insimou.getAction())
print("OK")
print("stop…", end="")
insimou.stop()
print("OK")

#might not be needed
import _ctypes
_ctypes.dlclose(insimou._handle)
del insimou