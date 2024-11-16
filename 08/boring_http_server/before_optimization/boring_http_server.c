#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 80
#define BUF_SIZE 128 
#define FILE_SIZE (1L * 1024 * 1024 * 1024) // 1GB 文件大小

// 模拟大量遍历操作生成文件内容
void generate_file_content(char *file_content, size_t size) {
    for (size_t i = 0; i < size; i++) {
        // 假设内容需要根据字典动态生成
        file_content[i] = (i % 26) + 'A'; // 循环写入 A-Z
    }
}

// 低效写文件：逐字符写入
void write_file_char_by_char(const char *file_content, size_t size, const char *file_path) {
    FILE *file = fopen(file_path, "w");
    if (!file) {
        perror("File opening failed");
        return;
    }

    // 每次写一个字符，模拟低效写入
    for (size_t i = 0; i < size; i++) {
        fputc(file_content[i], file);
    }

    fclose(file);
}

// 处理 HTTP GET 请求
void handle_get_request(int client_socket) {
    char *file_content = malloc(FILE_SIZE);
    if (!file_content) {
        perror("Memory allocation failed");
        close(client_socket);
        return;
    }

    // 模拟生成文件内容（低效遍历）
    generate_file_content(file_content, FILE_SIZE);

    // 先写入文件，逐字符写
    write_file_char_by_char(file_content, FILE_SIZE, "output_file.txt");

    // 构造 HTTP 响应头
    char header[] = "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: 1073741824\r\n\r\n";
    send(client_socket, header, strlen(header), 0);

    // 使用小缓冲区发送数据（低效 I/O）
    char buffer[BUF_SIZE];
    for (size_t i = 0; i < FILE_SIZE; i += BUF_SIZE) {
        size_t bytes_to_send = (i + BUF_SIZE < FILE_SIZE) ? BUF_SIZE : (FILE_SIZE - i);
        memcpy(buffer, file_content + i, bytes_to_send);
        send(client_socket, buffer, bytes_to_send, 0);
    }

    free(file_content);
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
    start_server();
    return 0;
}
