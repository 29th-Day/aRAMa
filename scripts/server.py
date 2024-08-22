import socket

HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
PORT = 7331  # Port to listen on (non-privileged ports are > 1023)

try:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen()
        conn, addr = s.accept()
        with conn:
            print(f"Connected by {addr}")
            while True:
                data = conn.recv(1024)
                if not data:
                    break
                print(data)
                print(list(data))
                # conn.sendall(data)
except:
    s.close()
    print("Connection refused or canceled")

exit()
