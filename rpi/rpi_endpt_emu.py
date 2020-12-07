from hsif_endpoint import HSIFEndpoint, HSIFMsg
import random
import sys
import time

'''
    -------------- Rpi Emulation Endpoint Example --------------
    This example initializes an HSIF python endpoint and sends
    simulated temperature data to the designated endpoint.
'''
def main():
    # If not enough arguments provided, return 
    if len(sys.argv) < 6:
        print("Not enough arguments provided (ex: rpi_endpt_emu <ip> <port> <endpt_id> <dest_id> <latency>)")
        return
    # Set necessary variables
    ip = sys.argv[1]
    port = int(sys.argv[2])
    from_id = sys.argv[3]
    to_id = sys.argv[4]
    latency = int(sys.argv[5])
    # Start endpoint
    print("Starting " + from_id + " endpoint on port 8888")
    endpoint = HSIFEndpoint()
    endpoint.connect(ip, port, from_id)
    # Acknowledgement variable used for message synchronization
    ack = True
    while endpoint.valid:
        # Send message once acknowledgement is received
        if ack:
            ack = False
            # Temperature sensor not available in emulation. Simulate on emulator. 
            temp_data = random.random() * 100
            # Send data message to outbox
            test_data = { "temp-data" : str(temp_data) }
            hsif_msg = HSIFMsg(from_id, to_id, test_data)
            endpoint.msg_outbox.put(hsif_msg.get_json())
            print("Sending temperature data: " + str(temp_data))
        # If incoming messages, print and set acknowledgement variable
        while endpoint.msg_inbox.qsize() != 0:
            ack = True
            print("Message received from " + to_id + ": " + str(endpoint.msg_inbox.get()))
        time.sleep(latency)
    print("Closing endpoint")

if __name__ == "__main__":
    main()