/************************ Adafruit IO Config *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME    "thatbrock"
#define IO_KEY         "f2631bd3801e4ab9b17ae4ea46be1cb7"
#define WIFI_SSID      "CenturyLink0502"
#define WIFI_PASS      "121212121224G"
//#define WIFI_SSID       "Badger's new Phone"
//#define WIFI_PASS       "12121212"

//#define WIFI_SSID    "University of Washington"
//#define WIFI_PASS    ""

#include "AdafruitIO_WiFi.h"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
