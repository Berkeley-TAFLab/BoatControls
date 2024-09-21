import serial
import serial.tools.list_ports
import time
import binascii

# Get list of available serial ports
def list_serial_ports():
    ports = serial.tools.list_ports.comports()
    return [port.device for port in ports]

def select_port():
    available_ports = list_serial_ports()
    if not available_ports:
        print("No available serial ports.")
        return None
    print("Available ports:")
    for idx, port in enumerate(available_ports):
        print(f"{idx}: {port}")
    while True:
        try:
            choice = int(input("Select the port number: "))
            if 0 <= choice < len(available_ports):
                return available_ports[choice]
            else:
                print("Invalid selection. Try again.")
        except ValueError:
            print("Please enter a valid number.")

def calculate_checksum(frame):
    total = sum(frame[3:])  # Start summing from the 4th byte (ignoring start delimiter and length bytes)
    return 0xFF - (total & 0xFF)

def build_api_frame(destination, data):
    frame = [0x7E, 0x00, 0x00, 0x10, 0x01]  # Start delimiter, length placeholder, frame type (0x10: Transmit Request), frame ID
    frame += destination  # Destination address (64-bit)
    frame += [0xFF, 0xFE]  # 16-bit broadcast address
    frame += [0x00, 0x00]  # Broadcast radius and options
    frame += [ord(c) for c in data]  # Data payload
    frame[2] = len(frame) - 3  # Set length (number of bytes after length field)
    frame.append(calculate_checksum(frame))  # Append checksum
    return frame

def transmit_xbee_message(ser, destination, data):
    frame = build_api_frame(destination, data)
    try:
        ser.write(bytearray(frame))  # Transmit the API frame
        print(f"Sent: {binascii.hexlify(bytearray(frame)).decode()}")
    except Exception as e:
        print(f"Error sending message: {e}")

def main():
    port = select_port()
    if not port:
        return
    baud_rate = 9600
    message_interval = 2
    destination_address = [0x00, 0x13, 0xA2, 0x00, 0x41, 0xFC, 0xCB, 0x86]  # Replace with your XBee's 64-bit address

    try:
        with serial.Serial(port, baud_rate, timeout=1) as ser:
            while True:
                message = "Hello XBee"  # Modify the message as needed
                transmit_xbee_message(ser, destination_address, message)
                time.sleep(message_interval)  # Wait before sending the next message
    except serial.SerialException as e:
        print(f"Serial error: {e}")

if __name__ == "__main__":
    main()

