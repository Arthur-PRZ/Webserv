#ifndef _SIGNALSHANDLING_HPP_
#define _SIGNALSHANDLING_HPP_

#include <csignal>

class SignalsHandling
{
    private :

    static volatile bool _stop;
    static void signalsHandler(int signal);

    public :

    SignalsHandling();
    SignalsHandling(const SignalsHandling& other);
    ~SignalsHandling();
    SignalsHandling &operator=(const SignalsHandling& other);

    void setSignals();

    bool getStopStatus();
};

#endif