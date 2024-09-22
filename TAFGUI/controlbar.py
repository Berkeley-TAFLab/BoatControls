import sys
from PySide6.QtCore import Qt, Slot, QTimer
from PySide6.QtGui import QIntValidator
from PySide6.QtWidgets import (
    QApplication, QWidget, QGridLayout, QDial, QLabel, QLineEdit, QVBoxLayout,
    QCheckBox, QComboBox, QHBoxLayout
)
import struct

# XBee API frame start delimiter
START_DELIMITER = 0x7E

class ControlBar(QWidget):
    def __init__(self, main_window, scrollable_table):
        super().__init__()

        # We need to reference the main window in order to use the uart handler 
        self.main_window = main_window
        self.scrollable_table = scrollable_table

        # Create the dials
        self.sail_dial = QDial()
        self.sail_dial.setRange(0, 180)
        self.sail_dial.setValue(90)
        self.sail_dial.valueChanged.connect(self.update_sail_disp_val)

        self.rudder_dial = QDial()
        self.rudder_dial.setRange(0, 180)
        self.rudder_dial.setValue(90)
        self.rudder_dial.valueChanged.connect(self.update_rudder_disp_val)

        # Create the value line edits
        self.sail_disp_val = QLineEdit("90")
        self.sail_disp_val.setAlignment(Qt.AlignCenter)
        self.sail_disp_val.setValidator(QIntValidator(0, 180))
        self.sail_disp_val.editingFinished.connect(self.update_sail_val)

        self.rudder_disp_val = QLineEdit("90")
        self.rudder_disp_val.setAlignment(Qt.AlignCenter)
        self.rudder_disp_val.setValidator(QIntValidator(0, 180))
        self.rudder_disp_val.editingFinished.connect(self.update_rudder_val)

        # Create the dial labels
        self.sail_dial_label = QLabel("Sail Control")
        self.sail_dial_label.setAlignment(Qt.AlignCenter)

        self.rudder_dial_label = QLabel("Rudder Control")
        self.rudder_dial_label.setAlignment(Qt.AlignCenter)

        # Create the toggle switch, dropdown menu, and line edits
        self.transmit_switch = QCheckBox("Transmit")
        self.transmit_switch.stateChanged.connect(self.handle_transmit_switch)

        self.mode_select = QComboBox()
        self.mode_select.addItems(["IDLE", "MANUAL", "AUTO"])
        
        self.id_select = QLineEdit("0013A20012345678")  # Default to a valid 64-bit address
        self.id_select.setAlignment(Qt.AlignCenter)

        self.longitude_select = QLineEdit("0.000")
        self.longitude_select.setAlignment(Qt.AlignCenter)

        self.latitude_select = QLineEdit("0.000")
        self.latitude_select.setAlignment(Qt.AlignCenter)

        # Set up the layout
        layout = QGridLayout()

        # First dial section
        sail_dial_layout = QVBoxLayout()
        sail_dial_layout.addWidget(self.sail_dial_label)
        sail_dial_layout.addWidget(self.sail_dial)
        sail_dial_layout.addWidget(self.sail_disp_val)
        
        # Second dial section
        rudder_dial_layout = QVBoxLayout()
        rudder_dial_layout.addWidget(self.rudder_dial_label)
        rudder_dial_layout.addWidget(self.rudder_dial)
        rudder_dial_layout.addWidget(self.rudder_disp_val)

        # Create a timer in order to transmit values via UART every so often
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.transmit_values)

        # Control layout
        control_layout = QVBoxLayout()
        control_layout.addWidget(self.transmit_switch)
        control_layout.addWidget(QLabel("Steer Mode"))
        control_layout.addWidget(self.mode_select)
        control_layout.addWidget(QLabel("Boat ID (64-bit address)"))
        control_layout.addWidget(self.id_select)
        control_layout.addWidget(QLabel("Requested Longitude"))
        control_layout.addWidget(self.longitude_select)
        control_layout.addWidget(QLabel("Requested Latitude"))
        control_layout.addWidget(self.latitude_select)

        # Adding to main layout
        layout.addLayout(sail_dial_layout, 0, 0)
        layout.addLayout(rudder_dial_layout, 0, 1)
        layout.addLayout(control_layout, 0, 2)

        self.setLayout(layout)
        self.setWindowTitle("Boat Control")

    @Slot()
    def update_sail_disp_val(self, value):
        self.sail_disp_val.setText(str(value))

    @Slot()
    def update_rudder_disp_val(self, value):
        self.rudder_disp_val.setText(str(value))

    @Slot()
    def update_sail_val(self):
        value = int(self.sail_disp_val.text())
        self.sail_dial.setValue(value)

    @Slot()
    def update_rudder_val(self):
        value = int(self.rudder_disp_val.text())
        self.rudder_dial.setValue(value)

    @Slot()
    def handle_transmit_switch(self, state):
        if state == 2:  # Originally supposed to be Qt.Checked but for some reason that doesn't work
            print("Transmitting")
            self.timer.start(250)  # Transmit every 50 ms 
        else:
            print("Not Transmitting")
            self.timer.stop()

    @Slot()
    def transmit_values(self):
        sail_dial_value = self.sail_dial.value()
        rudder_dial_value = self.rudder_dial.value()
        transmit_value = self.transmit_switch.isChecked()
        mode_value = self.mode_select.currentText()
        id_value = self.id_select.text()
        longitude_value = float(self.longitude_select.text())
        latitude_value = float(self.latitude_select.text())

        # Create XBee API frames for each value
        self.create_xbee_frame(0x01, struct.pack('!B', ['IDLE', 'MANUAL', 'AUTO'].index(mode_value)))  # State Transition
        self.create_xbee_frame(0x02, struct.pack('!BB', sail_dial_value, rudder_dial_value))  # Steer Controls
        self.create_xbee_frame(0x03, struct.pack('!f', longitude_value))  # Set Longitude
        self.create_xbee_frame(0x04, struct.pack('!f', latitude_value))  # Set Latitude

    def create_xbee_frame(self, frame_type, payload):
        frame_id = 0x01
        dest_address = bytes.fromhex(self.id_select.text().replace(":", ""))  # Use the ID from id_select
        network_address = bytes.fromhex('FFFE')  # Use 'FFFE' as we don't know the 16-bit address
        broadcast_radius = 0x00
        options = 0x00

        frame_data = struct.pack('!B B 8s 2s B B', 0x10, frame_id, dest_address, network_address, broadcast_radius, options)
        frame_data += struct.pack('!B', frame_type) + payload

        length = len(frame_data)
        checksum = 0xFF - (sum(frame_data) & 0xFF)

        frame = struct.pack('!B H', START_DELIMITER, length) + frame_data + struct.pack('!B', checksum)

        print(f"Sending XBee frame to {self.id_select.text()}: {frame.hex()}")
        self.main_window.send_uart_message(frame)