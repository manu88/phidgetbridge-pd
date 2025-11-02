from pythonosc import udp_client
from Phidget22.Phidget import *
from Phidget22.Devices.VoltageRatioInput import *
import time


client = udp_client.SimpleUDPClient("127.0.0.1", 4000)


def onVoltageRatioChange(self, voltageRatio):
    print("VoltageRatio: " + str(voltageRatio))


def main():
    ch = VoltageRatioInput()
    ch.setOnVoltageRatioChangeHandler(onVoltageRatioChange)
    ch.open()
    while True:
        print("send")
        client.send_message("/filter", 10)
        time.sleep(1)


if __name__ == "__main__":
    main()
