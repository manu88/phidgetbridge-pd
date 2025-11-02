from pythonosc import udp_client
from Phidget22.Phidget import *
from Phidget22.Devices.VoltageRatioInput import *
import time


client = udp_client.SimpleUDPClient("127.0.0.1", 4000)


def onVoltageRatioChange0(self, voltageRatio):
    client.send_message("/voltage", [0, voltageRatio])


def onVoltageRatioChange1(self, voltageRatio):
    client.send_message("/voltage", [1, voltageRatio])


def onVoltageRatioChange2(self, voltageRatio):
    client.send_message("/voltage", [2, voltageRatio])


def onVoltageRatioChange3(self, voltageRatio):
    client.send_message("/voltage", [3, voltageRatio])


def main():
    ch0 = VoltageRatioInput()
    ch0.setChannel(0)
    ch0.setOnVoltageRatioChangeHandler(onVoltageRatioChange0)
    ch0.openWaitForAttachment(100)
    ch0.setDataInterval(1000)

    ch1 = VoltageRatioInput()
    ch1.setChannel(1)
    ch1.setOnVoltageRatioChangeHandler(onVoltageRatioChange1)
    ch1.openWaitForAttachment(100)
    ch1.setDataInterval(1000)

    ch2 = VoltageRatioInput()
    ch2.setChannel(2)
    ch2.setOnVoltageRatioChangeHandler(onVoltageRatioChange2)
    ch2.openWaitForAttachment(100)
    ch2.setDataInterval(1000)

    ch3 = VoltageRatioInput()
    ch3.setChannel(3)
    ch3.setOnVoltageRatioChangeHandler(onVoltageRatioChange3)
    ch3.openWaitForAttachment(100)
    ch3.setDataInterval(1000)

    try:
        while True:
            time.sleep(1)
    except Exception as e:
        print(e)

    print("Closing channels")

    ch0.close()
    ch1.close()
    ch2.close()
    ch3.close()


if __name__ == "__main__":
    main()
