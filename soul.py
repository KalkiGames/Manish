import subprocess
import time

while True:
    process = subprocess.Popen(['python3', 'soul1.py'])
    time.sleep(25)
    process.terminate()
    process.wait()