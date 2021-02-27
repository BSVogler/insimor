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

observations = [1.0, 2.0, 3.0]  # Or whatever values you want to pass, but be sure to give only 4
FloatArrayDin = ctypes.c_float * len(observations)  # Define a 4-length array of floats
parameter_array = FloatArrayDin(*observations)  # Define the actual array to pass to your C function
insimou.setinput(parameter_array, len(parameter_array))
insimou.printstats()
insimou.start_sync() #blocks until spawned
insimou.printstats()
time.sleep(0.1)
print("Action:")
print(insimou.getAction())
print(insimou.getAction())
print(insimou.getAction())
insimou.stop()
