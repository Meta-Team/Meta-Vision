#include "entrypoint.hpp"

// For signal handling
#include <csignal>

Main mainFunction;

void ctrlc_handler(int signal) {
    if(mainFunction.should_run) {
        cwarning << "Ctrl-C received, shutting down" << endlog;
        mainFunction.should_run = false;
    }
}

// The real entrypoint, handle some exceptions better
int main(int argc, char** argv) {
    // Set custom handler for Ctrl-C, for clean shutdown
    signal(SIGINT, ctrlc_handler);

    // Run main function with exception catching
    try {
        return mainFunction.main(argc, argv);
    } catch(const std::invalid_argument& e) {
        cerror << e.what() << endlog;
        return -1;
    } catch(const std::runtime_error& e) {
        cerror << e.what() << endlog;
        return -1;
    }
}