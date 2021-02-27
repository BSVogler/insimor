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
except OSError:
    print("Unable to load the system C library")
    sys.exit()

insimou.start_sync()
observations = [1.0, 2.0, 3.0, 4.0]  # Or whatever values you want to pass, but be sure to give only 4
FloatArrayDin = ctypes.c_float * len(observations)  # Define a 4-length array of floats
parameter_array = FloatArrayDin(*observations)  # Define the actual array to pass to your C function

print(parameter_array)
insimou.setinput(parameter_array, len(parameter_array))
time.sleep(0.1)
print("Action:")
print(insimou.getAction())
print(insimou.getAction())
print(insimou.getAction())
insimou.stop()
