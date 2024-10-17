import sys
import struct
from PySide6.QtWidgets import QApplication, QTableWidget, QTableWidgetItem, QVBoxLayout, QWidget
from PySide6.QtCore import Qt

MESSAGE_ID_LIST = [0x08, 0x09, 0x0A, 0x0B, 0x0D]

class ScrollableTableWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.table_widget = QTableWidget()
        self.table_widget.setRowCount(0)
        self.table_widget.setColumnCount(7)

        self.table_widget.setHorizontalHeaderLabels(["Source Address", "Network Address", "Status", "Longitude", "Latitude", "Heading", "Wind Dir"])

        self.table_widget.setVerticalScrollMode(QTableWidget.ScrollPerPixel)
        self.table_widget.setHorizontalScrollMode(QTableWidget.ScrollPerPixel)

        layout = QVBoxLayout()
        layout.addWidget(self.table_widget)
        self.setLayout(layout)

        self.address_mapping = {}

    def update_table(self, message):
        source_address = message['source_address']
        network_address = message['source_network_address']
        data = bytes.fromhex(message['data'])

        if len(data) < 2:
            print("Invalid data length")
            return

        message_type = data[0]
        parsed_data = data[1:]

        if message_type not in MESSAGE_ID_LIST:
            print(f"Unknown message type: {message_type}")
            return

        if source_address in self.address_mapping:
            row = self.address_mapping[source_address]
        else:
            row = self.table_widget.rowCount()
            self.table_widget.insertRow(row)
            self.address_mapping[source_address] = row

        self.table_widget.setItem(row, 0, QTableWidgetItem(source_address))
        self.table_widget.setItem(row, 1, QTableWidgetItem(network_address))

        try:
            if message_type == 0x08:  # Receive Status
                self.table_widget.setItem(row, 2, QTableWidgetItem(f"{parsed_data[0]}"))
            elif message_type == 0x09:  # Receive Longitude
                int_value = struct.unpack('!i', parsed_data[:4])[0]
                float_value = int_value / 100000.0
                self.table_widget.setItem(row, 3, QTableWidgetItem(f"{float_value:.5f}"))
            elif message_type == 0x0A:  # Receive Latitude
                int_value = struct.unpack('!i', parsed_data[:4])[0]
                float_value = int_value / 100000.0
                self.table_widget.setItem(row, 4, QTableWidgetItem(f"{float_value:.5f}"))
            elif message_type == 0x0B:  # Receive Heading
                float_value = struct.unpack('!f', parsed_data[:4])[0]
                self.table_widget.setItem(row, 5, QTableWidgetItem(f"{float_value:.5f}"))  
            elif message_type == 0x0D:  # Receive WindDir
                int_value = struct.unpack('!H', parsed_data[:2])[0]
                self.table_widget.setItem(row, 6, QTableWidgetItem(str(int_value))) 
            

        except struct.error:
            print(f"Error parsing data for message type {message_type}")

        self.table_widget.resizeColumnsToContents()