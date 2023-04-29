import serial

#open up the ACM0 USB port because it's where the Feather is connected
port = serial.Serial('/dev/ttyACM1')
print(port.name) # check port name to make sure it's opened correctly

while 1:
    line = port.readline() #reads 1 line of data
    line = str(line.decode()) #converts line bytes into a string literal
    coords = line.split(',') #splits line data into a multiple index list using a delimiter
    lat = float(coords[0])
    long = float(coords[1])
    print("Latitude: %f, Longitude: %f \n" % (lat, long))