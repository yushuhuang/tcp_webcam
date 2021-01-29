#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"

#define DEST_PORT 8000
#define DSET_IP_ADDRESS "127.0.0.1"

int main()
{
    int sock_fd;
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "socket creation failed" << std::endl;
        exit(1);
    }

    sockaddr_in addr_serv{};
    int len;
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = inet_addr(DSET_IP_ADDRESS);
    addr_serv.sin_port = htons(DEST_PORT);
    len = sizeof(addr_serv);

    if (connect(sock_fd, (sockaddr *)&addr_serv, len) < 0)
    {
        std::cerr << "Connection Failed" << std::endl;
    }

    cv::VideoCapture cap;
    cap.open(0);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640.0);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480.0);

    if (!cap.isOpened())
    {
        std::cerr << "ERROR! Unable to open camera" << std::endl;
        return -1;
    }

    cv::Mat frame;
    std::vector<uchar> img_buf;
    int32_t head;

    while (cap.read(frame))
    {
        if (!frame.empty())
        {
            cv::imencode(".jpg", frame, img_buf);

            // send head
            head = htonl(img_buf.size());
            send(sock_fd, &head, sizeof(head), 0);

            // send origin image
            if (send(sock_fd, img_buf.data(), img_buf.size(), 0) < 0)
                break;

            cv::imshow("sent", frame);
            cv::waitKey(1);
        }
    }
    return 0;
}