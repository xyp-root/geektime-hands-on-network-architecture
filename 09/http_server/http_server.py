from http.server import BaseHTTPRequestHandler, HTTPServer

class LargeFileHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        # Prepare a 1GB response
        self.send_response(200)
        self.send_header("Content-Type", "application/octet-stream")
        self.send_header("Content-Length", str(1 * 1024 * 1024 * 1024))  # 1GB
        self.end_headers()

        # Send 1GB of data in chunks to avoid high memory usage
        chunk_size = 1024 * 1024  # 1MB
        for _ in range(1024):  # 1024 chunks of 1MB
            self.wfile.write(b"x" * chunk_size)  # Send a chunk of "x"
        print("1GB data sent to the client.")

def run(server_class=HTTPServer, handler_class=LargeFileHandler, port=8080):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print(f"Starting HTTP server on port {port}...")
    httpd.serve_forever()

if __name__ == "__main__":
    run()
