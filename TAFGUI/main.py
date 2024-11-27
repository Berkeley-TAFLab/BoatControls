import sys
from PySide6.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                               QHBoxLayout, QSplitter, QToolBar, QStackedWidget, QInputDialog)
from PySide6.QtCore import Qt, QThread, Signal, Slot, QUrl, QTimer, QDateTime
from PySide6.QtGui import QAction
from PySide6.QtQuickWidgets import QQuickWidget
from scrollable_table import ScrollableTableWidget
from controlbar import ControlBar
from graph_window import GraphsWindow  
import serial
import serial.tools.list_ports
import time
import struct

# XBee API frame start delimiter
START_DELIMITER = 0x7E

class UARTHandler(QThread):
    message_received = Signal(dict)

    def __init__(self, port, baud_rate):
        super().__init__()
        self.port = port
        self.baud_rate = baud_rate
        self.running = True
        self.serial = None
        self.buffer = bytearray() 
        self.messages = {}

    def run(self):
        print("Attempting to receive data")
        while self.running:
            if not self.serial or not self.serial.is_open:
                try:
                    self.serial = serial.Serial(self.port, self.baud_rate, timeout=0.1)
                    print(f"Connected to {self.port}")
                except serial.SerialException as e:
                    print(f"Error opening serial port: {e}")
                    time.sleep(5)
                    continue

            try:
                if self.serial.in_waiting > 0:
                    chunk = self.serial.read(self.serial.in_waiting)
                    self.buffer.extend(chunk)
                    
                    # Try to parse XBee message
                    message = self.read_xbee_message()
                    if message:
                        self.message_received.emit(message)
                        
            except (serial.SerialException, OSError) as e:
                print(f"Serial error: {e}")
                self.serial.close()
                time.sleep(5)

    def read_xbee_message(self):
        """
        Read and parse an XBee API frame from the buffer.
        """
        if len(self.buffer) < 3:
            return None

        # Check for start delimiter
        start_index = self.buffer.find(START_DELIMITER.to_bytes(1, 'big'))
        if start_index == -1:
            # Discard unrecognized data
            self.buffer.clear()
            return None

        # Remove data before start delimiter
        self.buffer = self.buffer[start_index:]

        if len(self.buffer) < 4:
            return None

        # Read frame length
        length = int.from_bytes(self.buffer[1:3], 'big')

        # Check for oversized or incomplete frames
        if length > 1024:
            print("Frame length exceeds maximum allowed size. Discarding buffer.")
            self.buffer.clear()
            return None

        if len(self.buffer) < length + 4:
            return None

        # Extract frame data and checksum
        frame_data = self.buffer[3:length+3]
        received_checksum = self.buffer[length+3]

        # Verify checksum
        calculated_checksum = (0xFF - (sum(frame_data) & 0xFF)) & 0xFF
        if calculated_checksum != received_checksum:
            print("Checksum mismatch. Discarding frame.")
            self.buffer = self.buffer[length+4:]  # Trim processed data
            return None

        # Parse frame type
        frame_type = frame_data[0]

        if frame_type == 0x90:  # ZigBee Receive Packet
            # Extract source address and data
            source_address = frame_data[1:9]
            source_network_address = frame_data[9:11]
            received_data = frame_data[12:]  # Exclude options byte

            # Remove processed frame from buffer
            self.buffer = self.buffer[length+4:]

            # Create message dictionary
            message = {
                'timestamp': QDateTime.currentDateTime(),
                'source_address': ':'.join(f'{b:02X}' for b in source_address),
                'source_network_address': f'{source_network_address[0]:02X}{source_network_address[1]:02X}',
                'data': received_data.hex()
            }

            # Limit message storage per device
            if message['source_address'] not in self.messages:
                self.messages[message['source_address']] = []
            if len(self.messages[message['source_address']]) >= 1000:
                self.messages[message['source_address']].pop(0)

            self.messages[message['source_address']].append(message)
            return message

        else:
            # Handle unsupported frame types
            self.buffer = self.buffer[length+4:]
            return None


    def stop(self):
        self.running = False
        if self.serial and self.serial.is_open:
            self.serial.close()

    def get_messages_for_device(self, device):
        return self.messages.get(device, [])

    def get_device_list(self):
        """Returns a list of devices currently in the messages dictionary."""
        return list(self.messages.keys()) 
    
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("TAFLab Boat Control UI")
        self.setGeometry(100, 100, 600, 400)

        # Create a central widget and layout
        central_widget = QWidget()
        main_layout = QVBoxLayout(central_widget)

        # Create toolbar
        self.toolbar = QToolBar()
        self.addToolBar(self.toolbar)

        # Create stacked widget to hold different "windows"
        self.stacked_widget = QStackedWidget()

        # Create and add "windows"
        self.create_window1()
        self.create_window2()

        # Add stacked widget to main layout
        main_layout.addWidget(self.stacked_widget)

        # Set central widget
        self.setCentralWidget(central_widget)

        # Create toolbar actions
        self.create_toolbar_actions()

        # Set up UART handler
        self.uart_handler = None
        self.setup_uart()
        
        # Create a timer for polling XBee modules
        self.poll_timer = QTimer(self)
        self.poll_timer.timeout.connect(self.poll_xbee_modules)
        self.poll_timer.start(300)  # Poll every 500ms


    def setup_uart(self):
        # Select serial port
        try:
            selected_port = self.select_serial_port()
            self.uart_handler = UARTHandler(selected_port, 9600)
            self.uart_handler.message_received.connect(self.process_serial_message)
            print("Initialized UART handler")
            self.uart_handler.start()
        except Exception as e:
            print(e)
            sys.exit(1)

    def select_serial_port(self):
        ports = serial.tools.list_ports.comports()
        port_list = [port.device for port in ports]
        if not port_list:
            raise Exception("No serial ports found")
        selected_port, ok = QInputDialog.getItem(None, "Select Serial Port", "Available serial ports:", port_list, 0, False)
        if ok and selected_port:
            return selected_port
        else:
            raise Exception("No serial port selected")

    @Slot(dict)
    def process_serial_message(self, message):
        print(f"\nReceived XBee message from {message['source_address']} "
            f"(Network: {message['source_network_address']}):")
        print(f"Data (hex): {message['data']}")
        
        # Update table
        self.table_widget.update_table(message)
        
        # Update graphs
        if hasattr(self, 'graphs_window'):
            self.graphs_window.update_device_list()
            self.graphs_window.update_graphs()
        
        # Record data if recording is active
        if hasattr(self, 'graphs_window') and self.graphs_window.recording:
            self.graphs_window.record_data()

    def send_uart_message(self, message):
        print("Transmitting uart message")
        if self.uart_handler and self.uart_handler.serial and self.uart_handler.serial.is_open:
            self.uart_handler.serial.write(message)

    def create_window1(self):
        window = QWidget()
        layout = QVBoxLayout(window)

        # Create main vertical splitter
        main_splitter = QSplitter(Qt.Vertical)

        # Create top splitter for top left and top right widgets
        top_splitter = QSplitter(Qt.Horizontal)

        # Top left widget
        self.table_widget = ScrollableTableWidget()  # Keep reference for UART updates
        top_splitter.addWidget(self.table_widget)

        # Top right widget
        map_widget = QQuickWidget()  
        map_widget.setSource(QUrl.fromLocalFile("TAF_Map_View.qml"))
        map_widget.setResizeMode(QQuickWidget.SizeRootObjectToView)
        top_splitter.addWidget(map_widget)

        # Add top splitter to main splitter
        main_splitter.addWidget(top_splitter)

        # Bottom widget
        bottom_widget = ControlBar(self, self.table_widget)

        # Add bottom widget to main splitter
        main_splitter.addWidget(bottom_widget)

        # Set initial sizes for main splitter (top section : bottom widget)
        main_splitter.setSizes([300, 100])

        # Set initial sizes for top splitter (left : right)
        top_splitter.setSizes([300, 300])

        layout.addWidget(main_splitter)
        self.stacked_widget.addWidget(window)
        
    def create_window2(self):
        self.graphs_window = GraphsWindow(self)  # Pass self (MainWindow) instead of self.table_widget
        self.stacked_widget.addWidget(self.graphs_window)

    def create_toolbar_actions(self):
        action1 = QAction("Trace", self)
        action1.triggered.connect(lambda: self.stacked_widget.setCurrentIndex(0))
        self.toolbar.addAction(action1)

        action2 = QAction("Graphs", self)
        action2.triggered.connect(lambda: self.stacked_widget.setCurrentIndex(1))
        self.toolbar.addAction(action2)

    def closeEvent(self, event):
        if self.uart_handler:
            self.uart_handler.stop()
        event.accept()

    def poll_xbee_modules(self):
        print("Polling detected XBee modules...")
        for source_address in self.table_widget.address_mapping.keys():
            # Create a message to poll the XBee module
            self.create_poll_message(source_address,'FFFE')


    def create_poll_message(self, dest_address, network_address):
        """
        Creates and sends an XBee API frame to poll a specific XBee device.
        
        :param dest_address: 64-bit destination address of the XBee module (as a hex string, e.g., '0013A20012345678')
        :param network_address: 16-bit network address of the XBee module (as a hex string, e.g., 'FFFE')
        """
        frame_type = 0x10  # ZigBee Transmit Request
        frame_id = 0x01  # Frame ID
        broadcast_radius = 0x00  # Use maximum number of hops
        options = 0x00  # Disable ACK and route discovery
        
        # Convert destination address (64-bit) and network address (16-bit) to bytes
        dest_address_bytes = bytes.fromhex(dest_address.replace(":", ""))
        network_address_bytes = bytes.fromhex(network_address)

        poll_array = [b'\x05', b'\x06', b'\x07',b'\x0C']
        
        for poll_id in poll_array: 


            # Frame data: Frame type + Frame ID + Destination Address + Network Address + Broadcast Radius + Options + Payload
            frame_data = struct.pack('!B B 8s 2s B B', frame_type, frame_id, dest_address_bytes, network_address_bytes, broadcast_radius, options) + poll_id +b'\x01'
            
            # Calculate length (excluding start delimiter and length bytes)
            length = len(frame_data)
            
            # Calculate checksum: 0xFF - (sum of frame data & 0xFF)
            checksum = 0xFF - (sum(frame_data) & 0xFF)

            # Construct full frame: Start delimiter (0x7E) + length + frame data + checksum
            frame = struct.pack('!B H', START_DELIMITER, length) + frame_data + struct.pack('!B', checksum)

            # Send frame over UART
            print(f"Sending poll message to {dest_address} ({network_address})")
            print(f"Frame (hex): {frame.hex()}")
            self.send_uart_message(frame)
    def update_graphs_device_list(self):
        if hasattr(self, 'graphs_window'):
            self.graphs_window.update_device_list() 

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())