#include "SignalsHandling.hpp"

volatile bool SignalsHandling::_stop = false;

SignalsHandling::SignalsHandling()
{
};
SignalsHandling::SignalsHandling(const SignalsHandling& other)
{
    (void)other;
};
SignalsHandling::~SignalsHandling()
{
};
SignalsHandling &SignalsHandling::operator=(const SignalsHandling& other)
{
    (void)other;
    return *this;
};

void SignalsHandling::signalsHandler(int signal)
{
    if (signal == SIGINT || signal == SIGQUIT)
        _stop = true;
}

void SignalsHandling::setSignals()
{
    std::signal(SIGINT, signalsHandler);
    std::signal(SIGQUIT, signalsHandler);
}

bool SignalsHandling::getStopStatus()
{
    return _stop;
}