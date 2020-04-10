#ifndef LOCAL_H
#define LOCAL_H

/**
 * This file is for local settings if not set via defines when calling
 * Really just for local development/usage without autoprogram
 *
 * You probably don't want to commit your changes to this in any PR.
 * I couldn't think of a better way to have a local settings file than just
 * putting it in a file and manually looking at it if it ever shows up in a PR.
 **/

#ifndef NAME
#define NAME "NEW-thermostat"
#endif

#ifndef TOPIC
#define TOPIC "program-me/NEW-thermostat"
#endif

#ifndef WIFI_SSID
#define WIFI_SSID "ssid"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "pass"
#endif

#ifndef MQTT_SERVER
#define MQTT_SERVER "10.13.0.22"
#endif

#ifndef MQTT_PORT
#define MQTT_PORT 1883
#endif

#ifndef HEAT_ONLY
//Set if no cool
//#define HEAT_ONLY
#endif

#endif //LOCAL_H
