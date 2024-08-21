#ifndef LISTENER_H
#define LISTENER_H

#include <dpp/dpp.h>

class Listener {
public:
    explicit Listener(dpp::cluster& bot);
    void on_message_create(const dpp::message_create_t& event) const;

private:
    dpp::cluster& bot;
};


#endif //LISTENER_H
