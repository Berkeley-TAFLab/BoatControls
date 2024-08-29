import sys
from PySide6.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                               QHBoxLayout, QSplitter, QToolBar, QStackedWidget, QInputDialog)
from PySide6.QtCore import Qt, QThread, Signal, Slot, QUrl
from PySide6.QtGui import QAction
from PySide6.QtQuickWidgets import QQuickWidget
from scrollable_table import ScrollableTableWidget
from controlbar import ControlBar
import serial
import serial.tools.list_ports
import time


class UARTHandler(QThread):
    message_received = Signal(bytes)

    def __init__(self, port, baud_rate):
        super().__init__()
        self.port = port
        self.baud_rate = baud_rate
        self.running = True
        self.serial = None
        self.buffer = bytearray()

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
                    
                    while b'\n' in self.buffer:
                        message, self.buffer = self.buffer.split(b'\n', 1)
                        if len(message) >= 2:  # Ensure we have at least ID and Data bytes
                            self.message_received.emit(message)
                        else:
                            print(f"Received incomplete message: {message.hex()}")
                        
            except (serial.SerialException, OSError) as e:
                print(f"Serial error: {e}")
                self.serial.close()
                time.sleep(5)

    def stop(self):
        self.running = False
        if self.serial and self.serial.is_open:
            self.serial.close()


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

    @Slot(str)
    def process_serial_message(self, message):
        print(f"Received raw message: {message.hex()}")
        
        if len(message) >= 2:
            id_byte = message[0] 
            message_type = message[1] 
            data_bytes = message[2:]

            data_bytes_hex = ' '.join(f"0x{byte:02X}" for byte in data_bytes)

            print(f"Received ID byte: 0x{id_byte:02X}, Message Type: 0x{message_type:02X} Data bytes: {data_bytes_hex}")


            # Update table if window1 is active
            if self.stacked_widget.currentIndex() == 0:
                self.table_widget.update_table([id_byte, message_type, data_bytes])

        else:
            print(f"Incomplete message received: {message.hex()}")

    def send_uart_message(self, message):
        print("Transmitting uart message")
        if self.uart_handler and self.uart_handler.serial and self.uart_handler.serial.is_open:
            # Ensure the message ends with a newline
            if not message.endswith('\n'):
                message += '\n'
            self.uart_handler.serial.write(message.encode('utf-8'))

    def send_uart_message(self, message):
        print("Transmitting uart message")
        if self.uart_handler and self.uart_handler.serial and self.uart_handler.serial.is_open:
            self.uart_handler.serial.write(message.encode('utf-8'))        

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
        bottom_widget = ControlBar(self)

        # Add bottom widget to main splitter
        main_splitter.addWidget(bottom_widget)

        # Set initial sizes for main splitter (top section : bottom widget)
        main_splitter.setSizes([300, 100])

        # Set initial sizes for top splitter (left : right)
        top_splitter.setSizes([300, 300])

        layout.addWidget(main_splitter)
        self.stacked_widget.addWidget(window)
        
    def create_window2(self):
        window = QWidget()
        layout = QVBoxLayout(window)
        layout.addWidget(QWidget())  # Placeholder, replace with your content
        layout.addWidget(QWidget())  # Placeholder, replace with your content
        self.stacked_widget.addWidget(window)

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

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
