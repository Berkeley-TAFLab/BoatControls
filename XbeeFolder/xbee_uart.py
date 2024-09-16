import serial
import serial.tools.list_ports
import time

# XBee API frame start delimiter
START_DELIMITER = 0x7E

def read_xbee_message(ser):
    """
    Read and parse an XBee API frame from the serial port.
    """
    # Wait for the start delimiter
    while ser.read() != START_DELIMITER.to_bytes(1, 'big'):
        pass

    # Read frame length (2 bytes)
    length = int.from_bytes(ser.read(2), 'big')

    # Read frame data and checksum
    frame_data = ser.read(length + 1)  # +1 for checksum

    # Verify checksum
    calculated_checksum = (0xFF - (sum(frame_data[:-1]) & 0xFF)) & 0xFF
    received_checksum = frame_data[-1]

    if calculated_checksum != received_checksum:
        print("Checksum mismatch. Discarding frame.")
        return None

    # Parse frame type
    frame_type = frame_data[0]

    if frame_type == 0x90:  # ZigBee Receive Packet
        # Extract source address and data
        source_address = frame_data[1:9]
        source_network_address = frame_data[9:11]
        received_data = frame_data[12:-1]  # Exclude options byte and checksum

        return {
            'source_address': ':'.join(f'{b:02X}' for b in source_address),
            'source_network_address': f'{source_network_address[0]:02X}{source_network_address[1]:02X}',
            'data': received_data.decode('ascii', errors='replace')
        }
    else:
        print(f"Unsupported frame type: {frame_type:02X}")
        return None

def get_available_ports():
    """
    Get a list of available serial ports.
    """
    return list(serial.tools.list_ports.comports())

def select_port(ports):
    """
    Allow the user to select a port from the list of available ports.
    """
    print("Available ports:")
    for i, port in enumerate(ports):
        print(f"{i+1}. {port.device} - {port.description}")
    
    while True:
        try:
            choice = int(input("Select a port number: ")) - 1
            if 0 <= choice < len(ports):
                return ports[choice].device
            else:
                print("Invalid selection. Please try again.")
        except ValueError:
            print("Invalid input. Please enter a number.")

def main():
    available_ports = get_available_ports()
    
    if not available_ports:
        print("No serial ports found. Please check your connections.")
        return

    selected_port = select_port(available_ports)
    baud_rate = 9600
    message_count = 0

    try:
        with serial.Serial(selected_port, baud_rate, timeout=1) as ser:
            print(f"Listening for XBee messages on {selected_port}...")
            print("Press Ctrl+C to exit.")
            while True:
                message = read_xbee_message(ser)
                if message:
                    message_count += 1
                    print(f"\nMessage #{message_count}")
                    print(f"Received message from {message['source_address']} "
                          f"(Network: {message['source_network_address']}):")
                    print(f"Data: {message['data']}")
                    print(f"Total messages received: {message_count}")
                time.sleep(0.1)  # Small delay to prevent CPU overuse
    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")
    except KeyboardInterrupt:
        print(f"\nScript terminated by user.")
        print(f"Total messages received: {message_count}")

if __name__ == "__main__":
    main()
