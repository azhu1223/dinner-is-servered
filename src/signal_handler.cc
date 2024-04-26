#include "signal_handler.h"
#include <boost/log/trivial.hpp>
#include <signal.h>

void sig_handler(int sig){
    switch(sig){
        case SIGINT:
            BOOST_LOG_TRIVIAL(fatal) << "Recieved interupt signal. Terminating";
            exit(SIGINT);
    }
}

