from pythonosc import udp_client
from Phidget22.Phidget import *
from Phidget22.Devices.VoltageRatioInput import *
import time


client = udp_client.SimpleUDPClient("127.0.0.1", 4000)


def onVoltageRatioChange(self, voltageRatio):
    print("VoltageRatio: " + str(voltageRatio))


def main():
    ch = VoltageRatioInput()
    ch.setChannel(0)
    ch.setOnVoltageRatioChangeHandler(onVoltageRatioChange)
    ch.openWaitForAttachment(1000)
    while True:
        print("send")
        client.send_message("/filter", 10)
        time.sleep(1)

    ch.close()


if __name__ == "__main__":
    main()
