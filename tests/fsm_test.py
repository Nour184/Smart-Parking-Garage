import serial
import time

COM_PORT = 'COM5' 
BAUD_RATE = 115200

EV_DRIVER_OPEN_GATE        = 0x00
EV_DRIVER_CLOSE_GATE       = 0x01
EV_SECURITY_OPEN_GATE      = 0x02
EV_SECURITY_CLOSE_GATE     = 0x03
EV_LIMIT_OPENING           = 0x04
EV_LIMIT_CLOSING           = 0x05
EV_DRIVER_CONFLICT         = 0x0A

STATES = {
    0x00: "IDLE_CLOSED", 0x01: "IDLE_OPENED",
    0x02: "OPENING",     0x03: "CLOSING",
    0x04: "STOPPED_MIDWAY", 0x05: "REVERSING"
}

OWNERS = {
    0x00: "NONE",
    0x01: "DRIVER",
    0x02: "SECURITY"
}

def send_event_and_get_state(ser, event_byte, event_name):
    print(f"\n  [>>>] Sending: {event_name}")
    ser.reset_input_buffer()
    ser.write(bytes([event_byte]))
    time.sleep(0.3) 
    
    bytes_waiting = ser.in_waiting
    if bytes_waiting >= 3: 
        response = ser.read(bytes_waiting)
        
        owner_before = OWNERS.get(response[0], f"UNKNOWN_{response[0]}")
        state_after  = STATES.get(response[1], f"UNKNOWN_{response[1]}")
        owner_after  = OWNERS.get(response[2], f"UNKNOWN_{response[2]}")
        
        print(f"  [<<<] Tiva-C -> Owner Before: [{owner_before}] | State: [{state_after}] | Owner After: [{owner_after}]")
        return state_after, owner_before, owner_after
    
    print(f"  [<<<] ERROR: Expected 3 bytes, got {bytes_waiting} (Timeout)")
    return "TIMEOUT", "TIMEOUT", "TIMEOUT"

def run_tests():
    print("Initializing Full HIL Test Suite with Priority Tracking...")
    
    try:
        with serial.Serial(COM_PORT, BAUD_RATE, timeout=2) as ser:
            time.sleep(2) 
            ser.reset_input_buffer()
            ser.write(bytes([0xFF])) 
            time.sleep(0.5)
            if ser.in_waiting > 0:
                ser.read(ser.in_waiting)
                
            print("\n===============================================")
            print(" STARTING TESTS")
            print("===============================================")

            # TC-04 & TC-10
            print("\n--- TC-04 & TC-10: Auto Open & Limit Stop ---")
            state, ob, oa = send_event_and_get_state(ser, EV_DRIVER_OPEN_GATE, "EV_DRIVER_OPEN_GATE")
            assert state == "OPENING" and oa == "DRIVER", "TC-04 FAILED"
            
            state, ob, oa = send_event_and_get_state(ser, EV_LIMIT_OPENING, "EV_LIMIT_OPENING")
            assert state == "IDLE_OPENED" and oa == "NONE", "TC-10 FAILED"
            print(">> TC-04 & TC-10 PASSED \u2705")

            # TC-05 & TC-11
            print("\n--- TC-05 & TC-11: Auto Close & Limit Stop ---")
            state, ob, oa = send_event_and_get_state(ser, EV_DRIVER_CLOSE_GATE, "EV_DRIVER_CLOSE_GATE")
            assert state == "CLOSING" and oa == "DRIVER", "TC-05 FAILED"
            
            state, ob, oa = send_event_and_get_state(ser, EV_LIMIT_CLOSING, "EV_LIMIT_CLOSING")
            assert state == "IDLE_CLOSED" and oa == "NONE", "TC-11 FAILED"
            print(">> TC-05 & TC-11 PASSED \u2705")

            # TC-12
            print("\n--- TC-12: Wrong Limit Ignored ---")
            send_event_and_get_state(ser, EV_DRIVER_OPEN_GATE, "EV_DRIVER_OPEN_GATE")
            state, ob, oa = send_event_and_get_state(ser, EV_LIMIT_CLOSING, "EV_LIMIT_CLOSING (Wrong Limit!)")
            assert state == "OPENING", "TC-12 FAILED: Stopped from wrong limit!"
            send_event_and_get_state(ser, EV_LIMIT_OPENING, "EV_LIMIT_OPENING (Correct Limit)")
            print(">> TC-12 PASSED \u2705")

            # TC-13
            print("\n--- TC-13: Security Open overrides Driver Close ---")
            state, ob, oa = send_event_and_get_state(ser, EV_DRIVER_CLOSE_GATE, "EV_DRIVER_CLOSE_GATE")
            assert state == "CLOSING" and oa == "DRIVER", "Setup Failed"
            
            state, ob, oa = send_event_and_get_state(ser, EV_SECURITY_OPEN_GATE, "EV_SECURITY_OPEN_GATE")
            assert ob == "DRIVER" and oa == "SECURITY", "TC-13 FAILED: Priority didn't upgrade!"
            assert state == "OPENING", "TC-13 FAILED: Direction didn't change!"
            
            send_event_and_get_state(ser, EV_LIMIT_OPENING, "EV_LIMIT_OPENING")
            print(">> TC-13 PASSED \u2705")

            # ---------------------------------------------------------
            # TC-14: Security CLOSE Overrides Driver OPEN
            # ---------------------------------------------------------
            print("\n--- TC-14: Security Close overrides Driver Open ---")
            
            # 1. Setup: The gate is currently IDLE_OPENED from TC-13. 
            # We must close it fully first so the Driver can open it.
            print("  (Setting up: Closing gate fully...)")
            send_event_and_get_state(ser, EV_DRIVER_CLOSE_GATE, "EV_DRIVER_CLOSE_GATE")
            send_event_and_get_state(ser, EV_LIMIT_CLOSING, "EV_LIMIT_CLOSING")
            
            # 2. Driver starts opening the gate
            print("  (Driver begins opening the gate...)")
            state, ob, oa = send_event_and_get_state(ser, EV_DRIVER_OPEN_GATE, "EV_DRIVER_OPEN_GATE")
            assert state == "OPENING" and oa == "DRIVER", "Setup Failed: Driver didn't take control!"
            
            # 3. Security slams the CLOSE button
            print("  (Security overrides with CLOSE...)")
            state, ob, oa = send_event_and_get_state(ser, EV_SECURITY_CLOSE_GATE, "EV_SECURITY_CLOSE_GATE")
            
            # 4. Verify the Priority Upgrade
            assert ob == "DRIVER" and oa == "SECURITY", "TC-14 FAILED: Priority didn't upgrade!"
            assert state == "CLOSING", "TC-14 FAILED: Direction didn't change!"
            
            # 5. Reset to Idle Closed for the next test
            send_event_and_get_state(ser, EV_LIMIT_CLOSING, "EV_LIMIT_CLOSING")
            print(">> TC-14 PASSED \u2705")

            # TC-15 & 16
            print("\n--- TC-15 & 16: Safe Stop on Conflict ---")
            state, ob, oa = send_event_and_get_state(ser, EV_DRIVER_CONFLICT, "EV_DRIVER_CONFLICT")
            assert state == "STOPPED_MIDWAY" and oa == "NONE", "TC-15 FAILED: Did not safely stop!"
            print(">> TC-15 & 16 PASSED \u2705")

            # TC-06
            print("\n--- TC-06: Auto from Midway ---")
            state, ob, oa = send_event_and_get_state(ser, EV_DRIVER_OPEN_GATE, "EV_DRIVER_OPEN_GATE")
            assert state == "OPENING", "TC-06 FAILED: Did not resume from midway"
            send_event_and_get_state(ser, EV_LIMIT_OPENING, "EV_LIMIT_OPENING")
            print(">> TC-06 PASSED \u2705")

            print("\n===============================================")
            print(" ALL TESTS EXECUTED SUCCESSFULLY!")
            print("===============================================")
            
    except serial.SerialException as e:
        print(f"\n[!] Serial Error: {e}")

if __name__ == "__main__":
    run_tests()