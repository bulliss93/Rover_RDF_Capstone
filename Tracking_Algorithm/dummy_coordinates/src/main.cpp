#include <Arduino.h>

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600); //start serial at 9600 baud
}

// the loop function runs over and over again forever
void loop() {
  //Picked the starting coordinates of Merryfield Hall (our lab space) Lat: 44.56756, Long -123.27415
  //Randomly generate coordinates between the given bounds Lat: 40-48, Long: -119 to -127
  float lat = (float) (random(40, 48));
  float lon = (float) (random(-120, -127));
  float dec = (float) (random(500) / 100.0);
  lat = (float) (lat + dec);
  lon = (float) (lon + dec);

  //cast these to char strings, since that's what's expected from the actual transciever
  String latstr(lat, 6);
  String lonstr(lon, 6);
  String serialstr = latstr + "," + lonstr + "\n";
  Serial.print(serialstr);

}