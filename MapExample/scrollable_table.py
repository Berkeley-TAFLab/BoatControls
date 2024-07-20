import sys
from PySide6.QtWidgets import QApplication, QTableWidget, QTableWidgetItem, QVBoxLayout, QWidget
from PySide6.QtCore import Qt

class ScrollableTableWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.table_widget = QTableWidget()
        self.table_widget.setRowCount(10)  # Set number of rows
        self.table_widget.setColumnCount(5)  # Set number of columns

        # Set headers
        self.table_widget.setHorizontalHeaderLabels(["Column 1", "Column 2", "Column 3", "Column 4", "Column 5"])

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

    def update_table(self, row, column, value):
        if 0 <= row < self.table_widget.rowCount() and 0 <= column < self.table_widget.columnCount():
            item = QTableWidgetItem(value)
            self.table_widget.setItem(row, column, item)

