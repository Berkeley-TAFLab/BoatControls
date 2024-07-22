import sys
from PySide6.QtWidgets import QApplication, QTableWidget, QTableWidgetItem, QVBoxLayout, QWidget
from PySide6.QtCore import Qt

class ScrollableTableWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.table_widget = QTableWidget()
        self.table_widget.setRowCount(0)  # Set number of rows
        self.table_widget.setColumnCount(5)  # Set number of columns

        # Set headers
        self.table_widget.setHorizontalHeaderLabels(["ID", "Timestamp", "Column 3", "Column 4", "Column 5"])

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
        parsed_timestamp = data[1]

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

        self.table_widget.setItem(index, 1, QTableWidgetItem(f"{parsed_timestamp}"))

