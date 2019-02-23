#ifndef WIFI_HANDLER_HPP
#define WIFI_HANDLER_HPP

#include <Preferences.h>

class WifiHandler {
  public:
    WifiHandler();
    bool checkConnection();
    bool restoreConfig();
    void serveSmartConfig();

  private:
    Preferences preferences;
};

#endif // WIFI_HANDLER_HPP
