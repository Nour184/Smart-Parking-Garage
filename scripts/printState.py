import serial

COMM_PORT = 'COM7'
BAUD_RATE = 115200

try:
    ser = serial.Serial(COMM_PORT, BAUD_RATE)
    ser.timeout = 1

    print("Connected to " + COMM_PORT + " at " + str(BAUD_RATE) + " baud")
    while True:
        line = ser.readline().decode('utf-8').strip()
        if(line):
            print(line)

except Exception as e:
    print(e)
