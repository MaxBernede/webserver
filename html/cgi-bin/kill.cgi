#!/usr/bin/python3

import signal
import os

os.kill(os.getpid(), signal.SIGKILL)
