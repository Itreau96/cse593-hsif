# Echo server program
import socket
import time

HOST = '127.0.0.1'                 # Symbolic name meaning all available interfaces
PORT = 8888              # Arbitrary non-privileged port
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('', PORT))
s.listen(1)

conn, addr = s.accept()
print('Connected by', addr)
sent = False
while 1:
  #data = conn.recv(1024)
  #print(data.decode('utf-8'))
  #if not data: break
  delim = bytearray(b'\r')
  test_data = "{ value: 'test-data' }"
  data_len = str(len(test_data.encode('utf-8')))
  data_bytes = bytearray(test_data, 'utf-8')
  final_msg = bytearray(data_len, 'utf-8') + delim + data_bytes
  print(final_msg)
  #two_msgs = final_msg + final_msg
  conn.sendall(final_msg)
  #print("sending " + final_msg)
  sent = True
  time.sleep(5)
conn.close()