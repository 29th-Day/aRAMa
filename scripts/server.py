import socketserver

class Handler(socketserver.BaseRequestHandler):
    def handle(self):
        while data := self.request.recv(1024):
            print("[" + ", ".join([f"{x:02x}" for x in list(data)]) + "]")

if __name__ == "__main__":
    try:
        server = socketserver.TCPServer(("127.0.0.1", 7331), Handler)
        server.serve_forever()
    except KeyboardInterrupt:
        ...
    except Exception as e:
        print(f"Exception: {e}")
    finally:
        server.server_close()
