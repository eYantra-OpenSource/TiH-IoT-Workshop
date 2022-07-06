#ifndef __EY_IOT_CONFIG_H__
#define __EY_IOT_CONFIG_H__

//--------- Wi-Fi --------- 
#define EY_CONFIG_WIFI_SSID      "Galaxy A520293"
#define EY_CONFIG_WIFI_PASS      "Batch@20"

//--------- MQTT --------- 
#define EY_CONFIG_MQTT_SERVER_URL 	"mqtt://broker.emqx.io"
#define EY_CONFIG_MQTT_SERVER_PORT 	1883
#define EY_SUB_TOPIC_LEN			100
#define EY_SUB_DATA_LEN				100


//--------- Encryption --------- 
#define EY_ENCRYP_XOR_KEY	'e'

//--------- NTP --------- 
#define EY_NTP_SERVER	    "pool.ntp.org"
#define EY_NTP_TIMEZONE     "UTC-5:30"

//--------- OTA --------- 
#define EY_REMOTE_OTA_URL           "https://raw.githubusercontent.com/"
#define EY_REMOTE_OTA_TIMEOUT_MS	60000

//--------- Database --------- 
#define SPREADSHEET_ID              "AKfycbxkw5ymQqup_7UTY6qNMm9KH4voc-XIoZEQOBpN3WyiQweIIUVUiTRwHfvowoqDdCD5"

#endif /* __EY_IOT_CONFIG_H__ */