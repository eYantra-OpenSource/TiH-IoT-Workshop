//------------------------------------------------------------------------------
#include "ey_iot.h"
#include "jsmn.h"

//------------------------------------------------------------------------------
static const char *TAG = "eY-IOT";

static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;
static int flag =0;
// Github Server Cert for OTA
// const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");

esp_http_client_handle_t http_client; // HTTP Client Object
char ey_http_url_buffer[500];

esp_mqtt_client_handle_t client;      // MQTT Client Object

// Initialize Function Pointers to Null Function
ey_iot_t ey_iot = {
    .event_id = 0,

    .fn_ptr_WIFI_EVENT_STA_START = ey_callback_null_fn,
    .fn_ptr_WIFI_EVENT_STA_DISCONNECTED = ey_callback_null_fn,

    .fn_ptr_MQTT_EVENT_CONNECTED = ey_callback_null_fn,
    .fn_ptr_MQTT_EVENT_DISCONNECTED = ey_callback_null_fn,
    .fn_ptr_MQTT_EVENT_SUBSCRIBED = ey_callback_null_fn,
    .fn_ptr_MQTT_EVENT_UNSUBSCRIBED = ey_callback_null_fn,
    .fn_ptr_MQTT_EVENT_PUBLISHED = ey_callback_null_fn,
    .fn_ptr_MQTT_EVENT_DATA_RX = ey_callback_null_fn,
    .fn_ptr_MQTT_EVENT_ERROR = ey_callback_null_fn,
    .fn_ptr_MQTT_EVENT_MISC = ey_callback_null_fn,

    .fn_ptr_OTA_EVENT_SUCCESS = ey_callback_null_fn,
    .fn_ptr_OTA_EVENT_FAIL = ey_callback_null_fn,
};

//------------------------------------------------------------------------------

//--------------- eY-IOT Framework Callbacks -------------------

// NULL Function Callback
int ey_callback_null_fn(void){
    // printf("%s\n", "NULL Fn");
    return 0;
}

void ey_callback_fn(void (*ptr)()) { 
    (*ptr) (); // callback to A 
} 

int ey_register_callback(unsigned char event_id, void (*user_fn_ptr)()){
    
    // ey_iot.fn_ptr_MQTT_EVENT_CONNECTED = ey_callback_null_fn;
    switch(event_id){
        
        case EY_WIFI_EVENT_STA_START:
            ey_iot.fn_ptr_WIFI_EVENT_STA_START = user_fn_ptr;
            break;

        case EY_WIFI_EVENT_STA_DISCONNECTED:
            ey_iot.fn_ptr_WIFI_EVENT_STA_DISCONNECTED = user_fn_ptr;
            break;
        
        


        case EY_MQTT_EVENT_CONNECTED:
            ey_iot.fn_ptr_MQTT_EVENT_CONNECTED = user_fn_ptr;
            break;
        case EY_MQTT_EVENT_DISCONNECTED:
            ey_iot.fn_ptr_MQTT_EVENT_DISCONNECTED = user_fn_ptr;
            break;
        
        case EY_MQTT_EVENT_SUBSCRIBED:
            ey_iot.fn_ptr_MQTT_EVENT_SUBSCRIBED = user_fn_ptr;
            break;

        case EY_MQTT_EVENT_UNSUBSCRIBED:
            ey_iot.fn_ptr_MQTT_EVENT_UNSUBSCRIBED = user_fn_ptr;
            break;

        case EY_MQTT_EVENT_PUBLISHED:
            ey_iot.fn_ptr_MQTT_EVENT_PUBLISHED = user_fn_ptr;
            break;

        case EY_MQTT_EVENT_DATA_RX:
            ey_iot.fn_ptr_MQTT_EVENT_DATA_RX = user_fn_ptr;
            break;

        case EY_MQTT_EVENT_ERROR:
            ey_iot.fn_ptr_MQTT_EVENT_ERROR = user_fn_ptr;
            break;

        case EY_MQTT_EVENT_MISC:
            ey_iot.fn_ptr_MQTT_EVENT_MISC = user_fn_ptr;
            break;




        case EY_OTA_EVENT_SUCCESS:
            ey_iot.fn_ptr_OTA_EVENT_SUCCESS = user_fn_ptr;
            break;

        case EY_OTA_EVENT_FAIL:
            ey_iot.fn_ptr_OTA_EVENT_FAIL = user_fn_ptr;
            break;

        default:
        break;
        
    }

    return 0;
    
}




//--------------- NVS ----------------------
int ey_init_nvs(void){
	//Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    return ret;
}


//----------------- Wi-Fi --------------------
void ey_wifi_auto_reconnect(void){
        ESP_LOGW(TAG, "Wi-Fi STA Disconnected");
        ESP_LOGI(TAG, "Wi-Fi STA Trying to Reconnect...");
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
}

void ey_wifi_sta_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    
    if(event_id == WIFI_EVENT_STA_DISCONNECTED){
        ey_callback_fn(ey_iot.fn_ptr_WIFI_EVENT_STA_DISCONNECTED);

    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ey_callback_fn(ey_iot.fn_ptr_WIFI_EVENT_STA_START);
        esp_wifi_connect();
    } 

    // **TODO**: not getting to this event when wifi disconnects going to mqtt disconnect event
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // if (s_retry_num < 100) {
        //     esp_wifi_connect();
        //     s_retry_num++;
        //     ESP_LOGI(TAG, "retry to connect to the AP");
        // } else {
        //     xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        // }
        // ESP_LOGI(TAG,"connect to the AP fail");

        ey_callback_fn(ey_iot.fn_ptr_WIFI_EVENT_STA_DISCONNECTED);

        
    } 

    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }

	// ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;

 //    switch(event_id) {
	//     case SYSTEM_EVENT_STA_START:
	//     	ESP_LOGI(TAG, "Wi-Fi STA Connected");
	//         esp_wifi_connect();
	//         break;
	//     case SYSTEM_EVENT_STA_GOT_IP:
 //        	ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
	//         xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
	//         break;
	//     case SYSTEM_EVENT_STA_DISCONNECTED:
	//         /* This is a workaround as ESP32 WiFi libs don't currently
	//            auto-reassociate. */
	//     	ESP_LOGW(TAG, "Wi-Fi STA Disconnected");
	//     	ESP_LOGI(TAG, "Wi-Fi STA Trying to Reconnect...");
	//         esp_wifi_connect();
	//         xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
	//         break;
	//     default:
	//         break;
 //    }
 //    return ESP_OK;

}

void ey_init_wifi_sta(char str_ssid[], char str_pass[])
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &ey_wifi_sta_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &ey_wifi_sta_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,

            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    
    strcpy((char*)wifi_config.sta.ssid, str_ssid);
    strcpy((char*)wifi_config.sta.password, str_pass);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", str_ssid, str_pass);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", str_ssid, str_pass);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
}



//-------------------------- MQTT ------------------------------
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;

    ey_mqtt_event = event->event_id;

    switch (ey_mqtt_event) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            ey_callback_fn(ey_iot.fn_ptr_MQTT_EVENT_CONNECTED);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            ey_callback_fn(ey_iot.fn_ptr_MQTT_EVENT_DISCONNECTED);
            // ey_init_wifi_sta(EY_CONFIG_WIFI_SSID, EY_CONFIG_WIFI_PASS);
            // ey_mqtt_start();
      //       ESP_LOGW(TAG, "Wi-Fi STA Disconnected");
	    	// ESP_LOGI(TAG, "Wi-Fi STA Trying to Reconnect...");
	    	// esp_wifi_connect();
	    	// xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);

            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            // ey_callback_fn(ey_iot.fn_ptr_MQTT_EVENT_SUBSCRIBED);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            // ey_callback_fn(ey_iot.fn_ptr_MQTT_EVENT_UNSUBSCRIBED);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            // ey_callback_fn(ey_iot.fn_ptr_MQTT_EVENT_PUBLISHED);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            
            // printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            // printf("DATA=%.*s\r\n", event->data_len, event->data);

            sprintf(ey_mqtt_sub_topic, "%.*s", event->topic_len, event->topic);
            sprintf(ey_mqtt_sub_data, "%.*s", event->data_len, event->data);
            // ESP_LOGI(TAG, "SUB_TOPIC: %s", ey_mqtt_sub_topic);
            // ESP_LOGI(TAG, "SUB_DATA: %s", ey_mqtt_sub_data);

            ey_callback_fn(ey_iot.fn_ptr_MQTT_EVENT_DATA_RX);
            

            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            // ey_callback_fn(ey_iot.fn_ptr_MQTT_EVENT_ERROR);
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            // ey_callback_fn(ey_iot.fn_ptr_MQTT_EVENT_MISC);
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void ey_mqtt_start(void)
{
    // TODO pass url, port as arguments
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = EY_CONFIG_MQTT_SERVER_URL,
        .port = EY_CONFIG_MQTT_SERVER_PORT,
    };

    // strcpy(mqtt_cfg.uri, str_url);
    // mqtt_cfg.port = str_port;

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}



int ey_mqtt_publish(char str_topic[], char str_data[], unsigned char qos){

	int msg_id = esp_mqtt_client_publish(client, str_topic, str_data, 0, qos, 0);
    ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
    return msg_id;
}

int ey_mqtt_subscribe(char str_topic[], unsigned char qos){

    while(1){
    	if(ey_mqtt_event == MQTT_EVENT_CONNECTED){
	    	int msg_id = esp_mqtt_client_subscribe(client, str_topic, qos);
	    	ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
	    	return msg_id;
    	}
    	else {
    		ESP_LOGW(TAG, "Not Connected to MQTT Server. Can't Subscribe. Retrying...");
    		vTaskDelay(1000/ portTICK_PERIOD_MS);
    	}
    }
    

    
}

int ey_mqtt_unsubscribe(char str_topic[]){

    int msg_id = esp_mqtt_client_unsubscribe(client, str_topic);
	ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
    return msg_id;
}



//------------------------- ENCRPYTION ---------------------
const char* ey_xor_encryptDecrypt(char inpString[]) 
{ 
	// Define XOR key 
	// Any character value will work 
	char xorKey = EY_ENCRYP_XOR_KEY; 

	// calculate length of input string 
	int len = strlen(inpString); 

	// perform XOR operation of key 
	// with every caracter in string 
	for (int i = 0; i < len; i++) 
	{ 
		inpString[i] = inpString[i] ^ xorKey; 
		// printf("%c",inpString[i]); 
	}

	return inpString;
}


//--------------------- NTP -------------------------
void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, EY_NTP_SERVER); //pool.ntp.org
    // sntp_setservername(0, "pool.ntp.org");
    sntp_init();
}

void ey_init_ntp(void)
{
    // ESP_ERROR_CHECK( nvs_flash_init() );
    // initialise_wifi();
    // xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,false, true, portMAX_DELAY);
    initialize_sntp();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;
    while(timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    if(retry == 10){
        esp_restart();
    }
    // ESP_ERROR_CHECK( esp_wifi_stop() );
}

time_t ey_sntp_get_epoch(void){

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    // Is time set? If not, tm_year will be (1970 - 1900).
    
    if (timeinfo.tm_year < (2016 - 1900)) {
        ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
        
    // ESP_ERROR_CHECK( nvs_flash_init() );
    // initialise_wifi();
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT,
                        false, true, portMAX_DELAY);
    initialize_sntp();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;
    while(timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    esp_restart();

    // ESP_ERROR_CHECK( esp_wifi_stop() );

    
        // update 'now' variable with current time
        
        // printf(">>> %lu <<<\n", time(&now));
    }


    return time(&now);
}

char* ey_sntp_date_string(void){
    static char dateString[64];
    char* utcDate;
    time_t now;
    struct tm timeinfo;
    now = ey_sntp_get_epoch();
    
    localtime_r(&now, &timeinfo);
    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2016 - 1900)) {
        ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
        ey_init_ntp();
        // update 'now' variable with current time
        time(&now);
    }
    char strftime_buf[64];

    // Set timezone to Indian Standard Time
    setenv("TZ", EY_NTP_TIMEZONE, 1);
    tzset();
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    sprintf(dateString, "%02d-%02d-%02d %02d:%02d:%02d", (timeinfo.tm_year - 100), 
                                                (timeinfo.tm_mon + 1), 
                                                timeinfo.tm_mday,
                                                timeinfo.tm_hour,
                                                timeinfo.tm_min,
                                                timeinfo.tm_sec);

    return dateString;
}






//------------------------------ OTA -----------------------------------------
esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);

        if (!esp_http_client_is_chunked_response(evt->client)){
        printf("Chunked Data:%.*s\r\n", evt->data_len, (char*)evt->data);
        }
        else{
        if(evt->data_len == 9){
        printf("Not Chunked Data:%.*s\r\n", evt->data_len, (char*) evt->data);
        flag=1;
        }
        }

        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    }
    return ESP_OK;
}
/*
void ey_iot_ota_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting eY-IOT Remote OTA");

    esp_http_client_config_t config = {
        .url = EY_REMOTE_OTA_URL,
        .cert_pem = (char *)server_cert_pem_start,
        .event_handler = _http_event_handler,
    };

    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK) {
        // esp_restart();
        printf("%s\n", "OTA COMPLETE");
        ey_mqtt_event = OTA_SUCCESS_EVENT;
    } else {
        ESP_LOGE(TAG, "Firmware upgrade failed");
        ey_mqtt_event = OTA_FAIL_EVENT;
    }
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


int ey_ota_start(void){
    xTaskCreate(&ey_iot_ota_task, "ey_iot_ota_task", 8192, NULL, 5, NULL);
    return 0;
}
*/

//------------------------- OTA Advanced ----------------------
int versionCompare(char* v1, char* v2) 
{ 
    // vnum stores each numeric 
    // part of version 
    int vnum1 = 0, vnum2 = 0; 

    // loop untill both string are 
    // processed 
    for (int i = 0, j = 0; (i < strlen(v1) 
                            || j < strlen(v2));) { 
        // storing numeric part of 
        // version 1 in vnum1 
        while (i < strlen(v1) && v1[i] != '.') { 
            vnum1 = vnum1 * 10 + (v1[i] - '0'); 
            i++; 
        } 

        // storing numeric part of 
        // version 2 in vnum2 
        while (j < strlen(v2) && v2[j] != '.') { 
            vnum2 = vnum2 * 10 + (v2[j] - '0'); 
            j++; 
        } 

        if (vnum1 > vnum2) 
            return 1; 
        if (vnum2 > vnum1) 
            return -1; 

        // if equal, reset variables and 
        // go for next numeric part 
        vnum1 = vnum2 = 0; 
        i++; 
        j++; 
    } 
    return 0; 
} 

int ey_helper_ota_version_compare(char* version1, char* version2) {
    
    if (versionCompare(version1, version2) < 0){
        printf("%s < %s\n", version1,  version2);
        return 2; 
    }
        
    else if (versionCompare(version1, version2) > 0){
        printf("%s > %s\n", version1,  version2);
        return 1; 
    }
        
    else{
        printf("v1 == v2\n");
        return 0;
    }
        

}
/*
static esp_err_t validate_image_header(esp_app_desc_t *new_app_info, int mode)
{
    if (new_app_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t running_app_info;
    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK) {
        ESP_LOGI(TAG, "Running firmware version: %s", running_app_info.version);
    }
    
    printf("New App Version: %s\n", new_app_info->version);
    printf("Current App Version: %s\n", running_app_info.version);

    int ret = ey_helper_ota_version_compare(running_app_info.version, new_app_info->version);
    // SmartOTA/ForceOTA decision tree
    if(ret == 2){
        ESP_LOGW(TAG, "Updating Firmware. %s -> %s", running_app_info.version, new_app_info->version);
        
    } else if (ret == 1){
        ESP_LOGW(TAG, "Downgrading Firmware. %s -> %s", running_app_info.version, new_app_info->version);
    
        if(mode == OTA_MODE_SMART){
            ESP_LOGW(TAG, "Smart OTA: Downgrading not allowed");
            return ESP_FAIL;

        } else if (mode == OTA_MODE_FORCED) {
            ESP_LOGW(TAG, "Forced OTA: Forced Downgrading");
            return ESP_OK;

        } else {
            ESP_LOGW(TAG, "Unknown OTA Mode:Downgrading");
            return ESP_OK;
        }

    } else {
        ESP_LOGW(TAG, "Current running version is the same as the new. %s == %s", running_app_info.version, new_app_info->version);
    
        if(mode == OTA_MODE_SMART){
            ESP_LOGW(TAG, "Smart OTA: Same Version. Not Perfroming OTA.");
            return ESP_FAIL;

        } else if (mode == OTA_MODE_FORCED) {
            ESP_LOGW(TAG, "Forced OTA: Reflashing same Frimware.");
            return ESP_OK;

        } else {
            ESP_LOGW(TAG, "Unknown OTA Mode:Reflashing same Frimware.");
            return ESP_OK;
        }
    }
    
    // if (memcmp(new_app_info->version, running_app_info.version, sizeof(new_app_info->version)) == 0) {
    //     ESP_LOGW(TAG, "Current running version is the same as a new. We will not continue the update.");
    //     return ESP_FAIL;
    // }

    return ESP_OK;
}
*/
/*
void advanced_ota_task(void *pvParameter)
{
    int mode = *((int*)pvParameter);

    if(mode == OTA_MODE_SMART){
        ESP_LOGI(TAG, "Starting Smart OTA");

    } else if (mode == OTA_MODE_FORCED) {
        ESP_LOGI(TAG, "Starting Forced OTA");

    } else {
        ESP_LOGI(TAG, "Unknown Mode. Starting Forced OTA.");
    }

    esp_err_t ota_finish_err = ESP_OK;
    esp_http_client_config_t config = {
        .url = EY_REMOTE_OTA_URL,
        .cert_pem = (char *)server_cert_pem_start,
        .timeout_ms = EY_REMOTE_OTA_TIMEOUT_MS,
    };

    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };

    esp_https_ota_handle_t https_ota_handle = NULL;
    esp_err_t err = esp_https_ota_begin(&ota_config, &https_ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ESP HTTPS OTA Begin failed");
        vTaskDelete(NULL);
    }

    esp_app_desc_t app_desc;
    err = esp_https_ota_get_img_desc(https_ota_handle, &app_desc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_https_ota_read_img_desc failed");
        goto ota_end;
    }
    // err = validate_image_header(&app_desc);
    // if (err != ESP_OK) {
    //     ESP_LOGE(TAG, "image header verification failed");
    //     goto ota_end;
    // }

    if(mode == OTA_MODE_SMART){
        ESP_LOGI(TAG, "Starting Smart OTA");
        err = validate_image_header(&app_desc, OTA_MODE_SMART);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "image header verification failed");
            goto ota_end;
        }

    } else if (mode == OTA_MODE_FORCED) {
        ESP_LOGI(TAG, "Starting Forced OTA");
        err = validate_image_header(&app_desc, OTA_MODE_FORCED);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "image header verification failed");
            goto ota_end;
        }

    } else {
        ESP_LOGI(TAG, "Unknown Mode. Starting Forced OTA.");
        err = validate_image_header(&app_desc, OTA_MODE_FORCED);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "image header verification failed");
            goto ota_end;
        }
    }

    while (1) {
        err = esp_https_ota_perform(https_ota_handle);
        if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
            break;
        }
        // esp_https_ota_perform returns after every read operation which gives user the ability to
        // monitor the status of OTA upgrade by calling esp_https_ota_get_image_len_read, which gives length of image
        // data read so far.
        ESP_LOGD(TAG, "Image bytes read: %d", esp_https_ota_get_image_len_read(https_ota_handle));
    }

    if (esp_https_ota_is_complete_data_received(https_ota_handle) != true) {
        // the OTA image was not completely received and user can customise the response to this situation.
        ESP_LOGE(TAG, "Complete data was not received.");
    }

ota_end:
    ota_finish_err = esp_https_ota_finish(https_ota_handle);
    
    // if ((err == ESP_OK) && (ota_finish_err == ESP_OK)) {
    //     ESP_LOGI(TAG, "ESP_HTTPS_OTA upgrade successful. Rebooting ...");
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    //     esp_restart();
    // } else {
    //     if (ota_finish_err == ESP_ERR_OTA_VALIDATE_FAILED) {
    //         ESP_LOGE(TAG, "Image validation failed, image is corrupted");
    //     }
    //     ESP_LOGE(TAG, "ESP_HTTPS_OTA upgrade failed %d", ota_finish_err);
    //     vTaskDelete(NULL);
    // }

    if ((err == ESP_OK) && (ota_finish_err == ESP_OK)) {
        ESP_LOGI(TAG, "ESP_HTTPS_OTA upgrade successful.");
        printf("%s\n", "OTA COMPLETE");
        // ey_mqtt_event = OTA_SUCCESS_EVENT;
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        ey_callback_fn(ey_iot.fn_ptr_OTA_EVENT_SUCCESS);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
        vTaskDelete(NULL);
        // while(1); // task should not return
    } else {
        ESP_LOGE(TAG, "ESP_HTTPS_OTA upgrade failed %d", ota_finish_err);
        // ey_mqtt_event = OTA_FAIL_EVENT;
        ey_callback_fn(ey_iot.fn_ptr_OTA_EVENT_FAIL);

        vTaskDelete(NULL);
    }
}


int ey_ota_advanced_start(int ota_mode){
    int mode = ota_mode;
    xTaskCreate(&advanced_ota_task, "advanced_ota_task", 1024 * 8, (void*)&mode, 5, NULL);
    return 0;
}
*/

// ------------------------ GOOGLE SHEET ----------------------
/* @brief: Function to trigger an HTTP request
 * @param: url, the URL to which an HTTP request is to be sent
 * @retval: 
*/
void ey_helper_trigger_http_request(const char *str_url)
{
    esp_http_client_config_t config = {.url = str_url, .event_handler = _http_event_handler};
    http_client = esp_http_client_init(&config);
    // GET
    esp_err_t err = esp_http_client_perform(http_client);
    
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                esp_http_client_get_status_code(http_client),
                esp_http_client_get_content_length(http_client));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    
    esp_http_client_cleanup(http_client);
}

// convert to variadic fucntion
int ey_push_sensor_data(unsigned char mode, float sensor_value){

    switch(mode){

        case GOOGLE_SHEET:

        sprintf(ey_http_url_buffer, "https://script.google.com/macros/s/"SPREADSHEET_ID"/exec?id=Sensor_1&value=%.2f", sensor_value);
        printf("%s\n", ey_http_url_buffer);
        ey_helper_trigger_http_request(ey_http_url_buffer);

        break;

        default:
        break;

    }

    return 0;
}

// convert sensor values to float before passing 10 -> 10.00
int x_ey_push_sensor_data(unsigned char mode, int number_of_values, ... ){

    char sensor_value_buffer[100];

    va_list ap;

    va_start(ap, number_of_values);

    // Logic
    switch(mode){

        case GOOGLE_SHEET:
            sprintf(ey_http_url_buffer, "https://script.google.com/macros/s/"SPREADSHEET_ID"/exec?id=Sensor_1");
            
            for( int i = 1; i <= number_of_values; i++ ){
                
                sprintf(sensor_value_buffer, "&value%d=%0.2f", i, va_arg(ap, double));

                strcat(ey_http_url_buffer, sensor_value_buffer);

                bzero(sensor_value_buffer, strlen(sensor_value_buffer));
            }
            // sprintf(ey_http_url_buffer, "https://script.google.com/macros/s/"SPREADSHEET_ID"/exec?id=Sensor_1&value=%.2f", sensor_value);
            printf("%s\n", ey_http_url_buffer);
            ey_helper_trigger_http_request(ey_http_url_buffer);

        break;

        // **TODO** case Local File:

        default:
        break;

    }

    va_end(ap);


    return 0;

}


int addingNumbers( int nHowMany, ... ) {
  int nSum =0;
  
  va_list  intArgumentPointer;
  
  va_start( intArgumentPointer, nHowMany );
  for( int i = 0; i < nHowMany; i++ )
    nSum += va_arg( intArgumentPointer, int );
  va_end( intArgumentPointer );
  
  return nSum;
} 


//----------------------------------- LOCATION -----------------------

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}


const char *get_json_value(char* JSON_data, char value[])
{

  jsmn_parser p;
  jsmntok_t t[256];
  jsmn_init(&p);
  char *name;
  int r1;
  int i;

  static char str_buffer[1000];

  r1 = jsmn_parse(&p, JSON_data, strlen(JSON_data), t, sizeof(t) / sizeof(t[0]));

  for (i = 1; i < r1; i++) 
  {
    if (jsoneq(JSON_data, &t[i], value) == 0) 
    {
    //   printf("Key: %.*s\n", t[i + 1].end - t[i + 1].start,
    //          JSON_data + t[i + 1].start);

      name = JSON_data + t[i + 1].start;
      for(int k =0;k<t[i + 1].end - t[i + 1].start;k++){
        //printf("%c",*name++);
        str_buffer[k] = *name++;
        // printf("%c", str_buffer[k]);
      }
      str_buffer[t[i + 1].end - t[i + 1].start] = '\0';  
    //   printf("\n");
      i++;
    } 
  }
  char *rtun = "0";
  return str_buffer;
}



char* ey_get_public_ip(void){
    static char str_return[100];

    char *buffer = malloc(MAX_HTTP_RECV_BUFFER + 1);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Cannot malloc http receive buffer");
        return;
    }
    // http://api.ipify.org/?format=json
    // http://ip-api.com/json/157.32.130.153
    esp_http_client_config_t config = {
        .url = "http://api.ipify.org/?format=json",
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err;
    if ((err = esp_http_client_open(client, 0)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        free(buffer);
        return;
    }
    int content_length =  esp_http_client_fetch_headers(client);
    int total_read_len = 0, read_len;
    if (total_read_len < content_length && content_length <= MAX_HTTP_RECV_BUFFER) {
        read_len = esp_http_client_read(client, buffer, content_length);
        if (read_len <= 0) {
            ESP_LOGE(TAG, "Error read data");
        }
        buffer[read_len] = 0;
        ESP_LOGD(TAG, "read_len = %d", read_len);
        // printf("%s\n", buffer);
        // printf("%s\n", get_json_value(buffer, "ip"));
        strcpy(str_return, get_json_value(buffer, "ip"));
    }
    ESP_LOGI(TAG, "HTTP Stream reader Status = %d, content_length = %d",
                    esp_http_client_get_status_code(client),
                    esp_http_client_get_content_length(client));
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    free(buffer);


    return str_return;
}


char* ey_get_location_isp(void){
    /*
    {
        "status": "success", 
        "country": "India", 
        "countryCode": "IN", 
        "region": "GJ", 
        "regionName": "Gujarat", 
        "city": "Ahmedabad", 
        "zip": "380052", 
        "lat": 23.0276, 
        "lon": 72.5871, 
        "timezone": "Asia/Kolkata", 
        "isp": "Reliance Jio Infocomm Limited", 
        "org": "RJIL Gujarat LTE SUBSCRIBER PUBLIC", 
        "as": "AS55836 Reliance Jio Infocomm Limited", 
        "query": "157.32.130.153"
    }
    */

    static char str_return[100];
    char str_url[100];
    sprintf(str_url, "http://ip-api.com/json/%s", ey_get_public_ip());

    char *buffer = malloc(MAX_HTTP_RECV_BUFFER + 1);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Cannot malloc http receive buffer");
        return;
    }
    // http://api.ipify.org/?format=json
    // http://ip-api.com/json/157.32.130.153
    esp_http_client_config_t config = {
        .url = str_url,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err;
    if ((err = esp_http_client_open(client, 0)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        free(buffer);
        return;
    }
    int content_length =  esp_http_client_fetch_headers(client);
    int total_read_len = 0, read_len;
    if (total_read_len < content_length && content_length <= MAX_HTTP_RECV_BUFFER) {
        read_len = esp_http_client_read(client, buffer, content_length);
        if (read_len <= 0) {
            ESP_LOGE(TAG, "Error read data");
        }
        buffer[read_len] = 0;
        ESP_LOGD(TAG, "read_len = %d", read_len);
        // printf("%s\n", buffer);
        // printf("%s\n", get_json_value(buffer, "ip"));
        strcpy(ey_iot.isp_country, get_json_value(buffer, "country"));
        strcpy(ey_iot.isp_countryCode, get_json_value(buffer, "countryCode"));
        strcpy(ey_iot.isp_region, get_json_value(buffer, "region"));
        strcpy(ey_iot.isp_regionName, get_json_value(buffer, "regionName"));
        strcpy(ey_iot.isp_city, get_json_value(buffer, "city"));
        strcpy(ey_iot.isp_zip, get_json_value(buffer, "zip"));
        strcpy(ey_iot.isp_lat, get_json_value(buffer, "lat"));
        strcpy(ey_iot.isp_lon, get_json_value(buffer, "lon"));
        strcpy(ey_iot.isp_timezone, get_json_value(buffer, "timezone"));
        strcpy(ey_iot.isp_isp, get_json_value(buffer, "isp"));
        strcpy(ey_iot.isp_org, get_json_value(buffer, "org"));
        strcpy(ey_iot.isp_as, get_json_value(buffer, "as"));
        strcpy(ey_iot.isp_public_ip, get_json_value(buffer, "query"));
        

    }
    ESP_LOGI(TAG, "HTTP Stream reader Status = %d, content_length = %d",
                    esp_http_client_get_status_code(client),
                    esp_http_client_get_content_length(client));
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    free(buffer);


    return str_return;
}

void ey_task_populate_isp_location(void *pvParameters)
{
	ESP_LOGI(TAG, "Getting ISP Information\n");
    ey_get_location_isp();
    printf("-----------------------------------------------\n");
    printf("Country: %s\n", ey_iot.isp_country);
    printf("Country Code: %s\n", ey_iot.isp_countryCode);
    printf("Region: %s\n", ey_iot.isp_region);
    printf("Region Name: %s\n", ey_iot.isp_regionName);
    printf("City: %s\n", ey_iot.isp_city);
    printf("Zip: %s\n", ey_iot.isp_zip);
    printf("Latitude: %s\n", ey_iot.isp_lat);
    printf("Longitude: %s\n", ey_iot.isp_lon);
    printf("Timezone: %s\n", ey_iot.isp_timezone);
    printf("ISP: %s\n", ey_iot.isp_isp);
    printf("ORG: %s\n", ey_iot.isp_org);
    printf("AS: %s\n", ey_iot.isp_as);
    printf("Public IP: %s\n", ey_iot.isp_public_ip);
    printf("-----------------------------------------------\n");

    ey_iot.isp_loc_status = 1;
    ESP_LOGI(TAG, "ISP Information Gathered\n");

    // **TODO** Error Handling; ey_iot.isp_loc_status = -1;

    vTaskDelete(NULL);
}

void ey_populate_isp_location(void){
    xTaskCreate(&ey_task_populate_isp_location, "ey_task_populate_isp_location", 8192, NULL, 5, NULL);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------