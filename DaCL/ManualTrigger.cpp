#include "ManualTrigger.h"
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <atomic>
#include <thread>

extern bool running;

ManualTrigger::ManualTrigger(int pin) : gpioPin(pin), chip(nullptr), line(nullptr) {
    // Open GPIO chip
    chip = gpiod_chip_open("/dev/gpiochip0");
    if (!chip) {
        throw std::runtime_error("Failed to open GPIO chip");
    }

    // Get GPIO line
    line = gpiod_chip_get_line(chip, gpioPin);
    if (!line) {
        gpiod_chip_close(chip);
        throw std::runtime_error("Failed to get GPIO line");
    }

    // Request input direction
    if (gpiod_line_request_input(line, "manual_trigger") < 0) {
        gpiod_line_release(line);
        gpiod_chip_close(chip);
        throw std::runtime_error("Failed to set GPIO line as input");
    }
}

ManualTrigger::~ManualTrigger() {
    if (line) {
        gpiod_line_release(line);
    }
    if (chip) {
        gpiod_chip_close(chip);
    }
}

bool ManualTrigger::checkTrigger() {
    while (running) {
        // Check GPIO pin
        int value = gpiod_line_get_value(line);
        if (value < 0) {
            std::cerr << "Error reading GPIO value" << std::endl;
            return false;
        }
        bool gpioState = (value == 1);

        // Check terminal input
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        if (ch == 't') {
            return true;
        }
        if (ch == 'e') {
            running = false;
            return false; // End program gracefully
        }
        if (gpioState) {
            return true;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return false;
}
