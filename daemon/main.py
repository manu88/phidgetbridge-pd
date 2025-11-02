from pythonosc import udp_client
import time


client = udp_client.SimpleUDPClient("127.0.0.1", 4000)


def main():
    while True:
        print("send")
        client.send_message("/filter", 10)
        time.sleep(1)


if __name__ == "__main__":
    main()
