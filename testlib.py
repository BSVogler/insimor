#!/usr/local/bin/python3
import sys
import time
import ctypes
from numpy.ctypeslib import ndpointer

try:
    insimou = _lib = ctypes.cdll.LoadLibrary("libinsimou.dylib")
except OSError:
    print("Unable to load the system C library")
    sys.exit()

insimou.start_sync()
insimou.setinput()
time.sleep(0.1)
print("Action:")
insimou.getAction.restype = ndpointer(dtype=ctypes.c_float, shape=(1,))
print(insimou.getAction())
print(insimou.getAction())
print(insimou.getAction())
insimou.stop()