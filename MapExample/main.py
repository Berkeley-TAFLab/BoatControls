import sys
from PySide6.QtWidgets import QApplication, QWidget, QVBoxLayout, QHBoxLayout, QSplitter
from PySide6.QtCore import Qt, QUrl
from PySide6.QtQuickWidgets import QQuickWidget

class MainWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Three Widget Layout with Splitters")
        self.setGeometry(100, 100, 600, 400)

        # Create main layout
        main_layout = QVBoxLayout()

        # Create main vertical splitter
        main_splitter = QSplitter(Qt.Vertical)

        # Create top splitter for top left and top right widgets
        top_splitter = QSplitter(Qt.Horizontal)

        # Top left widget
        top_left_widget = QWidget()
        top_left_widget.setStyleSheet("background-color: lightblue;")
        top_splitter.addWidget(top_left_widget)

        # Top right widget
        map_widget = QQuickWidget()  

        map_widget.setSource(QUrl.fromLocalFile("TAF_Map_View.qml"))
        map_widget.setResizeMode(QQuickWidget.SizeRootObjectToView)
        top_splitter.addWidget(map_widget)



	#Below is testing code
        #top_right_widget = QWidget()
        #top_right_widget.setStyleSheet("background-color: lightgreen;")
        #top_splitter.addWidget(top_right_widget)

        # Add top splitter to main splitter
        main_splitter.addWidget(top_splitter)

        # Bottom widget
        bottom_widget = QWidget()
        bottom_widget.setStyleSheet("background-color: lightyellow;")

        # Add bottom widget to main splitter
        main_splitter.addWidget(bottom_widget)

        # Set initial sizes for main splitter (top section : bottom widget)
        main_splitter.setSizes([300, 100])  # Adjust these values as needed

        # Set initial sizes for top splitter (left : right)
        top_splitter.setSizes([300, 300])  # Adjust these values as needed

        # Add main splitter to the layout
        main_layout.addWidget(main_splitter)

        self.setLayout(main_layout)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())


