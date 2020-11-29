from abc import ABC, abstractmethod
import json
import threading
import select
import socket
import sys
import time
import queue

class HSIFMsg(ABC):

    def __init__(self, data):
        self.valid = False
        self.json_data = ""
        self.serialize(data)
        super().__init__()

    @abstractmethod
    def serialize(self, jsonStr):
        try:
            self.json_data = json.dumps(jsonStr)
            self.valid = True
        except TypeError:
            self.valid = False
            print("JSON input not valid: " + jsonStr)

    @abstractmethod
    def get_data(self):
        msg = str(len(msg.encode('utf-8'))) + ':' + msg
        msg_bytes = bytearray(self.json_data, 'utf-8')
        msg_len = len(self.json_data.encode('utf-8'))
        return msg

class HSIFEndpoint():
    '''
    -------------- HSIF Endpoint Base Class --------------
        Base class used to encapsulate JSON messaging
        facilities for Endpoint. 
    '''
    def __init__(self, packet_size=1024, sock=None):
        self.packet_size=packet_size
        self.valid = False
        self.msg_inbox = queue.Queue()
        self.msg_outbox = queue.Queue()
        self.message = ""
        self.msg_thread = None
        if sock is None:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        else:
            self.sock = sock

    def connect(self, host, port, name):
        try:
            self.register(name)
            self.sock.connect((host, port))
            self.valid = True
            self.msg_thread = threading.Thread(target=self.io, args=())
            self.msg_thread.daemon = True
            self.msg_thread.start()
        except socket.error:
            print("Unable to connect to socket")
    
    def register(self, name):
        # Before sending any data, you must register the endpoint
        json_data = "{ \"type\" : \"registration\", \"value\" : \"" + name + "\" }"
        self.msg_outbox.put(json_data)

    def disconnect(self):
        self.sock.close()
        self.valid = False
    
    def io(self):
        msg_bytes_recv = 0
        msg_len = None
        msg_chunks = []
        header_delim = bytearray(b'\r')
        inputs = [self.sock]
        outputs = []
        while inputs:
            if not self.msg_outbox.empty():
                outputs = [self.sock]
            readable, writable, exceptional = select.select(inputs, outputs, inputs)
            if self.sock in writable: #self.sock in writable:
                self.send_msg(self.msg_outbox.get(), header_delim)
            if self.sock in readable:
                chunk = self.sock.recv(self.packet_size)
                if chunk == b'':
                    raise RuntimeError("socket connection broken")
                if msg_len == None: # First message or new message. Looking for size
                    if header_delim not in chunk:
                        # Header should never be cut off. Break here.
                        raise RuntimeError("error trying to process message")
                    else:
                        # Loop through partitions until empty...in case there are multiple messages in recv
                        while header_delim in chunk:
                            partitions = chunk.split(header_delim, 1)
                            msg_len = int((b''.join(msg_chunks) + partitions.pop(0)).decode('utf-8'))
                            if partitions:
                                data = partitions.pop(0)
                                if len(data) < msg_len:
                                    msg_chunks.append(data)
                                    msg_bytes_recv += len(data)
                                    chunk = b''
                                else:
                                    msg = data[:msg_len]
                                    chunk = data[msg_len:]
                                    self.msg_inbox.put(msg.decode('utf-8'))
                                    msg_len = None
                                    msg_bytes_recv = 0
                else: # Large message continued to be processed
                    if len(chunk) > msg_len - msg_bytes_recv:
                        remaining_data = chunk[:msg_len - msg_bytes_recv]
                        msg = (b''.join(msg_chunks) + remaining_data).decode('utf-8')
                        self.msg_inbox.put(msg)
                        chunk = chunk[msg_len - msg_bytes_recv:]
                        msg_len = None
                        msg_bytes_recv = 0
                        # Loop through partitions until empty...in case there are multiple messages in recv
                        while header_delim in chunk:
                            partitions = chunk.split(header_delim, 1)
                            msg_len = int((b''.join(msg_chunks) + partitions.pop(0)).decode('utf-8'))
                            if partitions:
                                data = partitions.pop(0)
                                if len(data) < msg_len:
                                    msg_chunks.append(data)
                                    msg_bytes_recv += len(data)
                                    chunk = b''
                                else:
                                    msg = data[:msg_len]
                                    chunk = data[msg_len:]
                                    self.msg_inbox.put(msg.decode('utf-8'))
                                    msg_chunks = []
                                    msg_len = None
                                    msg_bytes_recv = 0
                    elif len(chunk) + msg_bytes_recv == msg_len:
                        msg = (b''.join(msg_chunks) + chunk).decode('utf-8')
                        self.msg_inbox.put(msg)
                        msg_len = None
                        msg_chunks = []
                        msg_bytes_recv = 0
                    else:
                        msg_chunks.append(chunk)
                        msg_bytes_recv += chunk.size
            if self.sock in exceptional:
                self.valid = False
                print("Something bad happened!")
                self.disconnect()
        self.valid = False

    def send_msg(self, msg, delim):
        totalsent = 0
        data_len = str(len(msg.encode('utf-8')))
        data_bytes = bytearray(msg, 'utf-8')
        final_msg = bytearray(data_len, 'utf-8') + delim + data_bytes
        msg_len = len(final_msg)
        while totalsent < msg_len:
            sent = self.sock.send(final_msg[totalsent:])
            if sent == 0:
                raise RuntimeError("socket connection broken")
                break
            totalsent = totalsent + sent

def main():
    print("Starting the endpoint!")
    index = 0
    endpoint = HSIFEndpoint()
    endpoint.connect("192.168.0.26", 8888, "john")
    time.sleep(3)
    while endpoint.valid:
        #while True:
        test_data = "{ \"type\": \"message\", \"to\" : \"bob\", \"data\": \"test-data " + str(index) + "\" }"
        endpoint.msg_outbox.put(test_data)
        if not endpoint.msg_inbox.empty():
            print("John just got a letter!" + endpoint.msg_inbox.get())
        #print(endpoint.message)
        time.sleep(3)
        #print(endpoint.msg_inbox.get())
        index += 1
    print("Closin")

if __name__ == "__main__":
    main()