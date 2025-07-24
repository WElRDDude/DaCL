
#include "CANListener.hpp"
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <thread>
#include <sys/ioctl.h>

CANListener::CANListener(const std::string &canIface, const std::map<int, std::string>& idToWarning)
    : canIface_(canIface), idToWarning_(idToWarning), newWarning_(false) {}

void CANListener::run() {
    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("socket PF_CAN");
        return;
    }
    struct ifreq ifr;
    std::strncpy(ifr.ifr_name, canIface_.c_str(), IFNAMSIZ);
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl SIOCGIFINDEX");
        close(s);
        return;
    }
    struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(s);
        return;
    }

    while (true) {
        struct can_frame frame;
        int nbytes = read(s, &frame, sizeof(frame));
        if (nbytes > 0) {
            auto it = idToWarning_.find(frame.can_id);
            if (it != idToWarning_.end()) {
                std::lock_guard<std::mutex> lk(mtx_);
                lastWarningType_ = it->second;
                newWarning_ = true;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    close(s);
}

bool CANListener::getLatestWarning(std::string &warningType) {
    std::lock_guard<std::mutex> lk(mtx_);
    if (newWarning_) {
        warningType = lastWarningType_;
        newWarning_ = false;
        return true;
    }
    return false;
}