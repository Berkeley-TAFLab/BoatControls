import sys
import struct
from PySide6.QtWidgets import QApplication, QTableWidget, QTableWidgetItem, QVBoxLayout, QWidget
from PySide6.QtCore import Qt

class ScrollableTableWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.table_widget = QTableWidget()
        self.table_widget.setRowCount(0)  # Set number of rows
        self.table_widget.setColumnCount(6)  # Set number of columns

        # Set headers
        self.table_widget.setHorizontalHeaderLabels(["ID", "Timestamp", "Status", "Longitude", "Latitude","Heading"])

        # Add data to the table
        for row in range(10):
            for column in range(5):
                item = QTableWidgetItem(f"Item {row+1},{column+1}")
                self.table_widget.setItem(row, column, item)

        # Make the table scrollable
        self.table_widget.setVerticalScrollMode(QTableWidget.ScrollPerPixel)
        self.table_widget.setHorizontalScrollMode(QTableWidget.ScrollPerPixel)

        layout = QVBoxLayout()
        layout.addWidget(self.table_widget)
        self.setLayout(layout)
        self.row_nums = 0 #I feel like row_nums can easily be used to calculate the cur_index

        self.id_mapping = {} #dictionary to store IDs

    def update_table(self,data):
        #Parse data from the data array
        parsed_id = data[0]   
        message_type = data[1]
        parsed_data = data[2]

        #initially set the index to the very last row just in case
        index = self.row_nums

        #If we find the id in the mappings, set the index value to that
        if parsed_id in self.id_mapping:
            index = self.id_mapping[parsed_id]

        #if we can't find hte id mapping, then we need to add it to the mapping
        else: 
            self.id_mapping[parsed_id] = index
            self.row_nums += 1  
            self.table_widget.setRowCount(self.row_nums) #add the new row into the tablem
            self.table_widget.setItem(index, 0, QTableWidgetItem(f"{parsed_id}")) #set the id in the new row

        #Indices 
            # 0 - ID 
            # 1 - Timestamp 
            # 2 - Status 
            # 3 - Longitude 
            # 4 - Latitude 
            # 5 - Heading

        match message_type: 
            #Receive Status
            case 0x08:
                self.table_widget.setItem(index, 2, QTableWidgetItem(f"{parsed_data[0]}"))

            #Receive Longitude
            case 0x09: 
                parsed_data.reverse();
                float_value = struct.unpack('!f', parsed_data)[0]
                print(f"Float value: {float_value}") 
                self.table_widget.setItem(index, 3, QTableWidgetItem(f"{float_value:.5f}"))
            
            #Receive Latitude
            case 0x0A: 
                parsed_data.reverse();
                float_value = struct.unpack('!f', parsed_data)[0]
                print(f"Float value: {float_value}") 
                self.table_widget.setItem(index, 4, QTableWidgetItem(f"{float_value:.5f}"))
            
            #Receive Heading
            case 0x0B: 
                parsed_data.reverse();
                float_value = struct.unpack('!f', parsed_data)[0]
                print(f"Float value: {float_value}") 
                self.table_widget.setItem(index, 5, QTableWidgetItem(f"{float_value:.5f}"))


            
        







