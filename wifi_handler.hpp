#ifndef WIFI_HANDLER_HPP
#define WIFI_HANDLER_HPP

#include <Preferences.h>

class WifiHandler {
    private:
    Preferences preferences;

    public:
    WifiHandler();
    bool checkConnection();
    bool restoreConfig();
    void serveSmartConfig();
};

#endif // WIFI_HANDLER_HPP
