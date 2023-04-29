"""
Tracking algorithm code for the 2022-2023 Mars Rover RDF Capstone Project
Author: K. Kopcho
Date Revised: 1/28/2023

"""

import gps
import math
import serial

#print statements are for bug testing- in actuality these coordinates will get sent over PyQt to the groundstation

session = gps.gps(mode=gps.WATCH_ENABLE) #connect to the gps daemon
port = serial.Serial('/dev/ttyACM1') #open up the ACM USB port because it's where the Feather is connected

#the following function is based on the equations found here: https://www.movable-type.co.uk/scripts/latlong.html
def forward_bearing(base_lat, base_long, rover_lat, rover_long): 
    diff_long = (rover_long - base_long)

    #convert to radians before doing calculations
    base_lat = math.radians(base_lat)
    base_long = math.radians(base_long)
    rover_lat = math.radians(rover_lat)
    rover_long = math.radians(rover_long)
    diff_long = math.radians(diff_long)

    #do the actual math now, again this is based on the equations on the moveable-type site
    y = math.sin(diff_long) * math.cos(rover_lat)
    x = math.cos(base_lat) * math.sin(rover_lat) - math.sin(base_lat) * math.cos(rover_lat) * math.cos(diff_long)
    theta = math.atan2(y, x)
    bearing = (theta * 180/math.pi + 360) % 360 #converts rads back to degrees from 0 to 360

    return bearing

#below code adapted from the GPSD Example python client. Checks if lat/long of base are finite and then saves them as floats
def base_read():
        if ((gps.isfinite(session.fix.latitude) and
             gps.isfinite(session.fix.longitude))): #makes sure we have a finite lat/lon
        
            #save lat/long as floats
            blat = session.fix.latitude
            blon = session.fix.longitude
        else:
            #set error values
            blat = -1;
            blon = -1;

        return blat, blon

def rover_read():
    line = port.readline() #reads line ended by '\n'
    line = str(line.decode()) #converts line bytes into a string literal
    coords = line.split(',') #splits line data into a multiple index list using a delimiter
    lat = float(coords[0]) #save first index as latitude
    long = float(coords[1]) #save last index as longitude

    return lat, long
    

#start try/catch for keyboard interrupt (ctrl-c)
try:
    while 1:
        if session.read() == 0:
                if not (gps.MODE_SET & session.valid): #ensure we have a TPV Packet from read
                    continue

        if gps.TIME_SET & session.valid: #print the time if available
            print("Base GPS Time: ", session.fix.time)
        else:
            print("base gps time n/a")
    
        b_lat, b_lon = base_read() #get values from the base gps
        if(b_lat == -1 and b_lon == -1): #if we're not connected or lat/lon isn't finite
            print(" Lat n/a Lon n/a")
        else:
            print("base lat: %.6f, base lon: %.6f \n" % (b_lat, b_lon))
    
        r_lat, r_lon = rover_read()
        print("rover lat: %.6f, rover long %.6f \n" % (r_lat, r_lon))

        bearing = forward_bearing(b_lat, b_lon, r_lat, r_lon)
        print("current bearing angle: %.1f degrees \n" % (bearing))

              
except KeyboardInterrupt:
    print('')

# Got ^C, or fell out of the loop.  Cleanup, and leave.
session.close()
exit(0)