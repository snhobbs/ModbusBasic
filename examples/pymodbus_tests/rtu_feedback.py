# from pymodbus.client.asynchronous import schedulers
import subprocess
import socat_test as socat
from time import sleep
# from async_twisted_client_serial import ExampleProtocol
# from asynchronous_server import run_async_server

if __name__ == "__main__":
    socat.StartSocatServer()
    sleep(2)
    subprocess.Popen("python3 asynchronous_server.py", shell=True)
    sleep(2)
    subprocess.Popen("python3 async_twisted_client_serial.py", shell=True)
