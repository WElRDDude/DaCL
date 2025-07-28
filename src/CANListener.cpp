#include "CANListener.hpp"
#include "utils.hpp"
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <sys/ioctl.h>
#include <stdexcept>

CANListener::CANListener(const std::string &canIface, const std::map<int, std::string>& idToWarning)
    : canIface_(canIface), idToWarning_(idToWarning), newWarning_(false) {
    // Input validation
    if (canIface.empty()) {
        throw std::invalid_argument("CAN interface name cannot be empty");
    }
    
    // Initialize atomic variables with default values
    vehicleSpeed_ = 0;
    tripMileage_ = 0;
    totalMileage_ = 0;
    hour_ = 0;
    minute_ = 0;
    second_ = 0;
    day_ = 1;
    month_ = 1;
    year_ = 2024;
}

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

            // Parse specific signals
            switch (frame.can_id) {
                case 0x1A1: // ESC_V_VEH
                    vehicleSpeed_ = extractSignal(frame.data, 16, 16, true, 0.015625, 0); // Speed in km/h
                    break;

                case 0x3F3: // IC_BORD_COMP_TRIP_A
                    tripMileage_ = extractSignal(frame.data, 32, 17, true, 0.1, 0); // Trip Mileage in km
                    break;

                case 0x19D: // IC_Kilometerstand_2
                    totalMileage_ = extractSignal(frame.data, 0, 32, true, 0.001, 0); // Total Mileage in km
                    break;

                case 0x2F8: // IC_UHRZEIT_DATUM
                    hour_ = extractSignal(frame.data, 0, 8, true, 1.0, 0);   // Hour
                    minute_ = extractSignal(frame.data, 8, 8, true, 1.0, 0); // Minute
                    second_ = extractSignal(frame.data, 16, 8, true, 1.0, 0); // Second
                    day_ = extractSignal(frame.data, 24, 8, true, 1.0, 0);    // Day
                    month_ = extractSignal(frame.data, 36, 4, true, 1.0, 0);  // Month
                    year_ = extractSignal(frame.data, 40, 16, true, 1.0, 0);  // Year
                    break;
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

std::string CANListener::getCANBasedTimestamp() const {
    char buf[32];
    snprintf(buf, sizeof(buf), "%04d%02d%02d_%02d%02d%02d", 
             year_.load(), month_.load(), day_.load(), 
             hour_.load(), minute_.load(), second_.load());
    return std::string(buf);
}