import sys
import socket
import multiprocessing
from http.server import BaseHTTPRequestHandler, HTTPServer


# 自定义 HTTP 请求处理器
class SimpleHTTPRequestHandlerWithText(BaseHTTPRequestHandler):
    def do_GET(self):
        # 固定的 HTTP 响应内容
        response_text = f"HTTP Server on {self.server.port} \n"
        self.send_response(200)
        self.send_header("Content-type", "text/plain")
        self.end_headers()
        self.wfile.write(response_text.encode())


# 启动 HTTP 服务器
def start_http_server(port):
    print(f"Starting HTTP server on port {port}...")
    server = HTTPServer(('127.0.0.1', port), SimpleHTTPRequestHandlerWithText)
    server.port = port
    print(f"HTTP server on port {port} started.")
    server.serve_forever()


# 启动 TCP 服务器
def start_tcp_server(port):
    print(f"Starting TCP server on port {port}...")
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('127.0.0.1', port))
    server_socket.listen(5)
    print(f"TCP server on port {port} started.")

    while True:
        client_socket, _ = server_socket.accept()
        message = f"TCP Server on {port}\n"
        client_socket.send(message.encode())
        client_socket.close()


# 启动 UDP 服务器
def start_udp_server(port):
    print(f"Starting UDP server on port {port}...")
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_socket.bind(('127.0.0.1', port))
    print(f"UDP server on port {port} started.")

    while True:
        message, _ = server_socket.recvfrom(1024)
        response_message = f"UDP Server on {port}\n"
        server_socket.sendto(response_message.encode(), _)


# 启动所有服务
def start_servers():
    # 启动 HTTP 服务器
    http_processes = []
    http_ports = [8081, 8082, 8083]
    for port in http_ports:
        process = multiprocessing.Process(target=start_http_server, args=(port,))
        process.start()
        http_processes.append(process)

    # 启动 TCP 服务器
    tcp_ports = [7081, 7082, 7083]
    tcp_processes = []
    for port in tcp_ports:
        process = multiprocessing.Process(target=start_tcp_server, args=(port,))
        process.start()
        tcp_processes.append(process)

    # 启动 UDP 服务器
    udp_ports = [6081, 6082, 6083]
    udp_processes = []
    for port in udp_ports:
        process = multiprocessing.Process(target=start_udp_server, args=(port,))
        process.start()
        udp_processes.append(process)

    # 等待所有进程结束
    for process in http_processes + tcp_processes + udp_processes:
        process.join()


if __name__ == '__main__':
    start_servers()
 