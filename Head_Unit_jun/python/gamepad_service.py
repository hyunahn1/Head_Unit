#!/usr/bin/env python3
"""
PiRacer Gamepad Service - Stub
Reads gamepad, controls throttle, publishes gear via VSOMEIP.
TODO: Implement VSOMEIP C++ bridge or Python bindings.
Architecture: docs/ARCHITECTURE_DESIGN.md
"""
import time
import sys

def main():
    print("Gamepad service (stub) - VSOMEIP integration TBD", file=sys.stderr)
    while True:
        time.sleep(1)

if __name__ == "__main__":
    main()
