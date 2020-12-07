import random
import json
import threading
import select
import socket
import sys
import time
import queue

class HSIFMsg():
    '''
    -------------- HSIF Message Base Class --------------
        Base class used to wrap data that can be sent to
        HSIF server. Requires identifier to send to, 
        identifer of sending endpoint, and data. Data must
        be in JSON format (str or dict).
    '''
    def __init__(self, from_id, to_id, data):
        '''
            Initialization given endpoint identifiers and
            outgoing data. Data provided must be a dict obj.
        '''
        self.valid = False
        self.dict = { "type" : "message", "from" : from_id, "to" : to_id, "data" : data }
        super().__init__()

    @classmethod
    def from_json(cls, args):
        '''
            Initialization given a json string.
        '''
        msg_type = "message"
        to_id = from_id = data = ""
        try:
            data_obj = json.loads(args)
            to_id = data_obj["to"]
            from_id = data_obj["from"]
            data = data_obj["data"]
            cls.valid = True
        except TypeError:
            cls.valid = False
            print("JSON input not valid: " + args)
        return cls(to_id, from_id, data)

    def get_json(self):
        '''
            Creates and returns JSON string from current HSIFMsg instance.
        '''
        json_msg = json.dumps(self.dict)
        json_msg = str(len(json_msg.encode('utf-8'))) + '\r' + json_msg
        return json_msg

class HSIFEndpoint():
    '''
    -------------- HSIF Endpoint Base Class --------------
        Base class used to encapsulate JSON messaging
        facilities for Endpoint. 
    '''
    def __init__(self, sock=None):
        # Initialize class variables
        self.packet_size=1024
        self.valid = False
        self.msg_inbox = queue.Queue()
        self.msg_outbox = queue.Queue()
        self.message = ""
        self.msg_thread = None
        # If socket provided, set
        if sock is None:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        else:
            self.sock = sock

    def connect(self, host, port, id):
        '''
            Connects socket class variable to HSIF server instance. 
            Creates and initializes listening thread for managing
            data IO.
        '''
        try:
            # Begin by generating registration message for endpoint.
            self.register(id)
            self.sock.connect((host, port))
            self.valid = True
            # Create and start IO management thread.
            self.msg_thread = threading.Thread(target=self.poll, args=())
            self.msg_thread.daemon = True
            self.msg_thread.start()
        except socket.error:
            print("Unable to connect to HSIF server")
    
    def register(self, id):
        '''
            Creates a registration message for the current endpoint
            and sends to outgoing message structure. Endpoint must be registered to 
            receive messages.
        '''
        json_data = "{ \"type\" : \"registration\", \"value\" : \"" + id + "\" }"
        json_data = str(len(json_data.encode('utf-8'))) + '\r' + json_data
        self.msg_outbox.put(json_data)

    def disconnect(self):
        '''
            Closes socket connection and invalidates endpoint.
        '''
        self.sock.close()
        self.valid = False
    
    def poll(self):
        '''
            Poll function manages IO on listening socket. Select function is
            used to poll readable and writable sockets. Select blocks when 
            nothing has changed on socket handle in order to remove unnecessary
            computation.
        '''
        # Initialize local variables
        #msg_bytes_recv = 0
        msg_len = 0
        msg_chunks = []
        header_delim = bytearray(b'\r')
        inputs = [self.sock]
        outputs = []
        # Only poll while socket is active
        while self.valid:
            # If outgoing messages are available, flag socket for writing
            if self.msg_outbox.qsize() != 0:
                outputs = [self.sock]
            else:
                outputs = []
            # Retrieve lists of readable, writable, and exceptional sockets. Block if nothing has changed.
            readable, writable, exceptional = select.select(inputs, outputs, [], 3)
            # If writable, send message
            if self.sock in writable:
                self.send_msg(self.msg_outbox.get())
            # If readable, process current packet
            if self.sock in readable:
                chunk = self.sock.recv(self.packet_size)
                # If empty, disconnect
                if chunk == b'':
                    raise RuntimeError("socket connection broken")
                # First message or new message. Look for size header first.
                while chunk != b'':
                    if msg_len == 0:
                        if header_delim not in chunk:
                            # Header should never be missing from new message.
                            raise RuntimeError("error trying to process message")
                        else:
                            # Split message by delimeter
                            partitions = chunk.split(header_delim, 1)
                            msg_len = int((b''.join(msg_chunks) + partitions.pop(0)).decode('utf-8'))
                            # If message body in packet, process it
                            if partitions:
                                chunk = partitions.pop()
                    # If size known, attempt to extract packet
                    else:
                        # If payload in chunk, extract
                        if len(chunk) >= msg_len:
                            # Start by extracting previous message
                            remaining_data = chunk[:msg_len]
                            msg = (b''.join(msg_chunks) + remaining_data).decode('utf-8')
                            hsif_msg = HSIFMsg.from_json(msg)
                            self.msg_inbox.put(hsif_msg.dict["data"])
                            # Reset buffers and size variable
                            chunk = chunk[msg_len:]
                            msg_chunks.clear()
                            msg_len = 0
                        else:
                            # Append to buffer and continue
                            msg_len -= len(chunk)
                            msg_chunks.append(chunk)
                            chunk = b''
            # If problem occurs on socket, close and exit
            if self.sock in exceptional:
                print("Socket disconnected from server.")
                self.disconnect()
        # IO loop exited successfully
        print("Endpoint closed")

    def send_msg(self, msg):
        '''
            Sends HSIFMsg message provided. Loops over 
            TCP send function until all bytes have been
            transferred.
        '''
        totalsent = 0
        msg_bytes = msg.encode('utf-8')
        msg_len = len(msg_bytes)
        while totalsent < msg_len:
            sent = self.sock.send(msg_bytes[totalsent:])
            if sent == 0:
                raise RuntimeError("Socket connection broken")
                break
            totalsent = totalsent + sent