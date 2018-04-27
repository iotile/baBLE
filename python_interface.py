import subprocess
import sys
import time

process = subprocess.Popen(["./build/debug/baBLE_linux"], stdout=subprocess.PIPE, stdin=subprocess.PIPE)

process.stdin.write("2,0")

time.sleep(2)

process.stdin.write("5,0")

try:
    while True:
        output = process.stdout.readline()
        if output == '' and process.poll() is not None:
            break
        if output:
            print("STDOUT: {}".format(output.strip()))
except KeyboardInterrupt:
    print("Stopping python interface...")

print("Python interface terminated.")
