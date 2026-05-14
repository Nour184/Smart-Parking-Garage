import time
import serial

COMM_PORT = 'COM7'
BAUD_RATE = 115200

def _read_string(ser: serial.Serial) -> str:
    result = ""
    
    # 1. Wait for the first byte to arrive
    while not ser.in_waiting:
        time.sleep(0.05) # Much faster response time

    # 2. Read continuously until the null terminator arrives
    while True:
        # read(1) will block for up to ser.timeout if the buffer is empty mid-string
        new_byte = ser.read(1)
        
        if not new_byte: # Timeout occurred
            break
            
        new_char = new_byte.decode('ASCII', errors='ignore')
        
        if new_char == '\0': # End of string reached
            break
            
        result += new_char
        
    return result

def test_safety_task():
    try:
        ser=serial.Serial(COMM_PORT, BAUD_RATE)
        ser.timeout = 1
        print(f"Connected to {COMM_PORT} at {BAUD_RATE} baud rate")
        msg = _read_string(ser) # Wait for initial "Ready" message
        print(msg)
        ser.write(bytes([0x0]))
        print("Sent 0x0 to Tiva-C")
        msg = _read_string(ser)
        if("started" in msg):
            print(f"Tester should see the RED LED on\n")
        else:
            print(f"FAIL: recieved {msg}\n")
            return

        msg = _read_string(ser)
        if("end" in msg):
            print(f"Tester should see the RED LED off\n")
        else:
            print(f"FAIL: recieved {msg}\n")
            return

        print(f"PASS\n")
        return 1
    except Exception as e:
        print(f"Error: {e}")

test_safety_task()