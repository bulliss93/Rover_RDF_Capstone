#include <stdio.h>
#include "pico/stdlib.h"
#include "LoRa-RP2040.h"
#include "hardware/uart.h"
#include "pico/time.h"
#include "pico/printf.h"
#include "pico/stdio/driver.h"
#include "pico/stdio_usb.h"
#include "pico/stdio_uart.h"
//#include "pico/multicore.h"
#include <string.h>
#include <string>

#define BAUD_RATE 9600
#define UART_ID uart0

#define LED_PIN 25

#define UART_TX_PIN 12
#define UART_RX_PIN 13

#define NMEA_BUF_SIZE 100
#define NMEA_SENTENCE_GPGGA "$GPGGA"

using std::string;

typedef struct {
	double latitude;
	double longitude;
	int hour;
	int minute;
	int second;
} NMEA_data;


// NMEA_data parsing function which takes in an NMEA sentence and fills in the
// longitude, latitude, and time members of the NMEA_data struct.
void parse_nmea_sentence(const char *sentence, NMEA_data* data) {

  if (strstr(sentence, NMEA_SENTENCE_GPGGA) == sentence) {
    char *p = (char *)sentence;

    // move pointer to time
    p = strchr(p, ',') + 1;

    // parse time
    int hour, minute, second;
    sscanf(p, "%2d%2d%2d", &hour, &minute, &second);
    data->hour = hour;
    data->minute = minute;
    data->second = second;

    // move pointer to latitude
    p = strchr(p, ',') + 1;

    // parse latitude
    double latitude, latitude_minutes;
    sscanf(p, "%2lf%lf", &latitude, &latitude_minutes);
    data->latitude = latitude + latitude_minutes / 60.0;

    // move pointer to N/S indicator
    p = strchr(p, ',') + 1;

    // check N/S indicator and adjust latitude
    if (*p == 'S') {
      data->latitude = -data->latitude;
    }

    // move pointer to longitude
    p = strchr(p, ',') + 1;

    // parse longitude
    double longitude, longitude_minutes;
    sscanf(p, "%3lf%lf", &longitude, &longitude_minutes);
    data->longitude = longitude + longitude_minutes / 60.0;

    // move pointer to E/W indicator
    p = strchr(p, ',') + 1;

    // check E/W indicator and adjust longitude
    if (*p == 'W') {
      data->longitude = -data->longitude;
    }
  }
}

void init_lora(){
	sleep_ms(5000); // wait 5 seconds
	printf("Starting LoRa\n");
	if (!LoRa.begin(915E6)){ // establish connection to module
		printf("Starting LoRa failed!\n");
		while(1){
			printf("Failed!\n");
			sleep_ms(1000);
		}
	}
}


void init_nmea(){


	// Set up LED
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	// Set up UART with required speed
	uart_init(UART_ID, BAUD_RATE);

	//Set the TX and RX pins by using the function select on the GPIO
	gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
	gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}


int main(){
	stdio_init_all();
	//stdio_set_driver_enabled(&stdio_usb, true);
	//stdio_set_driver_enabled(&stdio_uart, false);

	init_nmea();
	init_lora();

	char nmea_sentence[NMEA_BUF_SIZE]; // create variables
	NMEA_data data = {0};
	NMEA_data* data_ptr = &data;

 	int counter = 0;
	
	string message = "Insert GPS Value Here.\0";
	string str = "";

	while(1){
		for(int i=0; i<7; i++){
			scanf("%s", nmea_sentence);
			parse_nmea_sentence(nmea_sentence, data_ptr);
			// printf("%s\n", nmea_sentence);
			// printf("%lf,%lf,%02d:%02d:%02dUTC\n", data.latitude, data.longitude, data.hour, data.minute, data.second);
			// message = "Packet #";
			// message += std::__cxx11::to_string(counter);
			// message += ": ";
			// message += std::__cxx11::to_string(data.latitude);
			message = std::__cxx11::to_string(data.latitude);
			message += ",";
			message += std::__cxx11::to_string(data.longitude);
			message += ",";

			str = std::to_string(data.hour);
			if (str.length() < 2) {
				str = "0" + str;
			}

			message += str;
			message += ":";

			str = std::to_string(data.minute);
			if (str.length() < 2) {
				str = "0" + str;
			}

			message += str;
			message += ":";

			str = std::to_string(data.second);
			if (str.length() < 2) {
				str = "0" + str;
			}

			message += str;
			message += "UTC";

			message += " #";
			message += std::__cxx11::to_string(counter);
		}

		if (data.latitude != 0 && data.longitude != 0) {
			printf("%s\n", message.c_str());
			// set tx mode
	  	LoRa.beginPacket();                   // start packet
	  	LoRa.print(message.c_str());                  // add payload
	  	LoRa.endPacket();                 // finish packet and send it
	   	// gpio_put(LED_PIN, 1);
	    // sleep_ms(100);
	    // gpio_put(LED_PIN, 0);
			counter++;
			// sleep_ms(750);
		}else{
			printf("Searching...\n");
		}

 	}
 return 0;
}
