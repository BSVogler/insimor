#!/usr/local/bin/python3
import sys
import time
import ctypes

try:
    insimou = _lib = ctypes.cdll.LoadLibrary("libinsimou.dylib")
except OSError:
    print("Unable to load the system C library")
    sys.exit()
insimou.start_sync()
insimou.setinput()
time.sleep(0.1)
insimou.stop()