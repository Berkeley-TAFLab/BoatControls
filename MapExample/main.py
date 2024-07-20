import sys
from PySide6.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                               QHBoxLayout, QSplitter, QToolBar, QStackedWidget, QInputDialog)
from PySide6.QtCore import Qt, QThread, Signal, Slot, QUrl
from PySide6.QtGui import QAction
from PySide6.QtQuickWidgets import QQuickWidget
from scrollable_table import ScrollableTableWidget
import serial
import serial.tools.list_ports
import time


class UARTHandler(QThread):
    message_received = Signal(str)

    def __init__(self, port, baud_rate):
        super().__init__()
        self.port = port
        self.baud_rate = baud_rate
        self.running = True
        self.serial = None

    def run(self):
        print("Attempting to receive data")
        while self.running:
            if not self.serial or not self.serial.is_open:
                try:
                    self.serial = serial.Serial(self.port, self.baud_rate)
                    print(f"Connected to {self.port}")
                except serial.SerialException as e:
                    print(f"Error opening serial port: {e}")
                    time.sleep(5)
                    continue

            try:
                if self.serial.in_waiting > 0:
                    message = self.serial.readline().decode('utf-8', errors='ignore').strip()
                    self.message_received.emit(message)
            except serial.SerialException as e:
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
        # Parse the message and update the table
        parts = message.split()
        print("Parts: " + str(parts))
        if len(parts) == 5:
            try:
                row = int(parts[1]) - 1
                column = int(parts[3]) - 1
                value = parts[4]
                print("Received message and parsed. Update data")
                if self.stacked_widget.currentIndex() == 0:  # Check if window1 is active
                    self.table_widget.update_table(row, column, value)
            except ValueError:
                print(f"Invalid data received: {message}")

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
        bottom_widget = QWidget()
        bottom_widget.setStyleSheet("background-color: lightyellow;")

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
