import subprocess

ports = ["/tmp/ptyp0", "/tmp/ttyp0"]
kBaudRate = 9600


def StartSocatServer(devs=ports, speed=kBaudRate):
    socat_call = "socat -d -d PTY,link={ports[1]},raw,echo=0,ispeed={speed} PTY,link={ports[0]},raw,echo=0,ospeed={speed}".format(
        ports=devs, speed=speed
    )
    subprocess.call(socat_call, shell=True)


if __name__ == "__main__":
    StartSocatServer(ports, kBaudRate)
    # connections = [
    #    Serial(ports[0], kBaudRate, timeout = 1),
    #    Serial(ports[1], kBaudRate, timeout = 1)
    #    ]

    # i = 0
    # while True:
    #    time.sleep(.1)
    #    i += 1
    #    connections[0].write(bytes("hello %d\n"%i, encoding = "utf-8"))
    #    print(connections[1].readline())
