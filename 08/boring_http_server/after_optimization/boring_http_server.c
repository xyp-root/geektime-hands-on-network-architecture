#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 80
#define BUF_SIZE (64 * 1024) 
#define FILE_SIZE (1L * 1024 * 1024 * 1024) // 1GB 文件大小
#define FILE_PATH "output_file.txt"

// 预先生成文件内容，并将其写入文件
void generate_and_write_file_content(const char *file_path, size_t size) {
    FILE *file = fopen(file_path, "w");
    if (!file) {
        perror("File opening failed");
        return;
    }

    // 生成并写入文件内容，一次写完
    for (size_t i = 0; i < size; i++) {
        fputc((i % 26) + 'A', file); // 循环写入 A-Z
    }

    fclose(file);
}

// 处理 HTTP GET 请求
void handle_get_request(int client_socket) {
    FILE *file = fopen(FILE_PATH, "rb");
    if (!file) {
        perror("File opening failed");
        close(client_socket);
        return;
    }

    // 构造 HTTP 响应头
    char header[] = "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: 1073741824\r\n\r\n";
    send(client_socket, header, strlen(header), 0);

    // 使用大缓冲区发送文件（流式处理）
    char buffer[BUF_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUF_SIZE, file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    fclose(file);
    close(client_socket);
}

// 启动 HTTP 服务器
void start_server() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // 创建套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 设置套接字选项
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Set socket options failed");
        exit(EXIT_FAILURE);
    }

    // 绑定地址和端口
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 开始监听
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("HTTP server is running on port %d...\n", PORT);

    // 处理连接
    while ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0) {
        handle_get_request(client_socket);
    }

    if (client_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
}

int main() {
    // 预先生成并写入文件内容
    generate_and_write_file_content(FILE_PATH, FILE_SIZE);

    // 启动服务器
    start_server();
    return 0;
}
