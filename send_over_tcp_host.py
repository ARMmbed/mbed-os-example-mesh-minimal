import socket
import argparse
import threading

DEFAULT_PORT = 1234
#https://docs.python.org/2/howto/sockets.html

parser = argparse.ArgumentParser(description='TCP tool')
parser.add_argument('--port', type=int, default=DEFAULT_PORT,
                    help="Port to send to")
args = parser.parse_args()

port = args.port

done = False

def read_socket(socket):
    while (True):
        data = socket.recv(4096)
        if len(data) > 0:
            print("Data recieved: %s" % data)
        if done:
            print("Read thread exiting")
            break

sock_listen = socket.socket(socket.AF_INET6, # Internet
                     socket.SOCK_STREAM)
sock_listen.bind(('', port))
sock_listen.listen(5)
sock, addr = sock_listen.accept()
print 'tcp %s:%d connect' % (addr[0], addr[1])

read_thread = threading.Thread(target=read_socket, args=(sock,))
read_thread.daemon = True
read_thread.start()
try:
    while (True):
        data = raw_input("Enter command:")
        sock.send(data)
        print("Message sent: %s" % data)
finally:
    sock.shutdown(socket.SHUT_RDWR)
    sock.close()
