#include "entrypoint.hpp"

// For signal handling
#include <csignal>

Main mainFunction;

/**
 * @brief Handler for Ctrl-C key combination. Tells main loop to stop,
 *        instead of letting Linux terminating the process.
 * 
 * @param signal The signal triggered, ignored
 */
void ctrlc_handler(int signal) {
    // Do not show the message repeatedly
    if(mainFunction.should_run) {
        cwarning << "Ctrl-C received, shutting down" << endlog;
        mainFunction.should_run = false;
    }
}

/**
 * @brief Entrypoint to the program.
 *        Sets Ctrl-C handler, and use a try-catch to catch all internal errors,
 *        so the errors get printed out better.
 * 
 * @param argc Number of arguments passed
 * @param argv Values of arguments passed
 * @return int Return value of the whole program
 */
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