import sys
from PySide6.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                               QHBoxLayout, QSplitter, QToolBar, QStackedWidget)
from PySide6.QtCore import Qt, QUrl
from PySide6.QtGui import QAction
from PySide6.QtQuickWidgets import QQuickWidget
from scrollable_table import ScrollableTableWidget 

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

    def create_window1(self):
        window = QWidget()
        layout = QVBoxLayout(window)

        # Create main vertical splitter
        main_splitter = QSplitter(Qt.Vertical)

        # Create top splitter for top left and top right widgets
        top_splitter = QSplitter(Qt.Horizontal)

        # Top left widget
        top_left_widget = ScrollableTableWidget()
        top_splitter.addWidget(top_left_widget)

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

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())


