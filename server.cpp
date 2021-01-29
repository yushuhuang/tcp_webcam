#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"

#define SERV_PORT 8000

void recv_all(int sock, int size, std::vector<uchar> &buf)
{
    int pos{0}, tmp_size{0};
    std::vector<uchar> tmp_buf(size);
    while (pos < size)
    {
        tmp_size = recv(sock, tmp_buf.data(), size - pos, 0);
        if (tmp_size < 0)
            exit(1);
        std::copy_n(tmp_buf.begin(), tmp_size, &buf[pos]);
        pos += tmp_size;
    }
}

void recv_all(int sock, int size, char *buf)
{
    int pos{0}, tmp_size{0};
    std::vector<uchar> tmp_buf(size);
    while (pos < size)
    {
        tmp_size = recv(sock, tmp_buf.data(), size - pos, 0);
        if (tmp_size < 0)
            exit(1);
        std::copy_n(tmp_buf.begin(), tmp_size, &buf[pos]);
        pos += tmp_size;
    }
}

int main()
{
    int sock_fd, new_sock;
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "socket creation failed" << std::endl;
        exit(1);
    }

    sockaddr_in addr_serv{};
    int len;
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_port = htons(SERV_PORT);
    addr_serv.sin_addr.s_addr = INADDR_ANY;
    len = sizeof(addr_serv);

    int opt = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    if (bind(sock_fd, (struct sockaddr *)&addr_serv, len) < 0)
    {
        std::cerr << "binding error" << std::endl;
        exit(1);
    }

    if (listen(sock_fd, 3) < 0)
    {
        std::cerr << "Listening error" << std::endl;
        exit(1);
    }

    new_sock = accept(sock_fd, (sockaddr *)&addr_serv, (socklen_t *)&len);

    int32_t img_size;

    std::vector<uchar> img_buf;
    cv::Mat frame;

    for (;;)
    {
        // recv head
        recv_all(new_sock, sizeof(int32_t), (char *)&img_size);
        img_size = ntohl(img_size);

        // read origin image
        img_buf.resize(img_size);
        recv_all(new_sock, img_size, img_buf);

        frame = cv::imdecode(img_buf, cv::IMREAD_COLOR);

        cv::imshow("recv", frame);
        cv::waitKey(1);
    }

    return 0;
}