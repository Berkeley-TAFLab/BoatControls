import sys
from PySide6.QtCore import Qt, Slot
from PySide6.QtGui import QIntValidator
from PySide6.QtWidgets import (
    QApplication, QWidget, QGridLayout, QDial, QLabel, QLineEdit, QVBoxLayout,
    QCheckBox, QComboBox, QHBoxLayout
)

class ControlBar(QWidget):
    def __init__(self):
        super().__init__()

        # Create the dials
        self.dial1 = QDial()
        self.dial1.setRange(0, 100)
        self.dial1.setValue(50)
        self.dial1.valueChanged.connect(self.update_line_edit1)

        self.dial2 = QDial()
        self.dial2.setRange(0, 100)
        self.dial2.setValue(50)
        self.dial2.valueChanged.connect(self.update_line_edit2)

        # Create the value line edits
        self.line_edit1 = QLineEdit("50")
        self.line_edit1.setAlignment(Qt.AlignCenter)
        self.line_edit1.setValidator(QIntValidator(0, 100))
        self.line_edit1.editingFinished.connect(self.update_dial1)

        self.line_edit2 = QLineEdit("50")
        self.line_edit2.setAlignment(Qt.AlignCenter)
        self.line_edit2.setValidator(QIntValidator(0, 100))
        self.line_edit2.editingFinished.connect(self.update_dial2)

        # Create the dial labels
        self.dial_label1 = QLabel("Sail Control")
        self.dial_label1.setAlignment(Qt.AlignCenter)

        self.dial_label2 = QLabel("Rudder Control")
        self.dial_label2.setAlignment(Qt.AlignCenter)

        # Create the toggle switch, dropdown menu, and line edits
        self.toggle_switch = QCheckBox("Transmit")

        self.combo1 = QComboBox()
        self.combo1.addItems(["IDLE", "MANUAL", "AUTO"])
        
        self.line_edit3 = QLineEdit("ID 0x00")
        self.line_edit3.setAlignment(Qt.AlignCenter)

        self.line_edit4 = QLineEdit("0.000")
        self.line_edit4.setAlignment(Qt.AlignCenter)

        self.line_edit5 = QLineEdit("0.000")
        self.line_edit5.setAlignment(Qt.AlignCenter)

        # Set up the layout
        layout = QGridLayout()

        # First dial section
        dial1_layout = QVBoxLayout()
        dial1_layout.addWidget(self.dial_label1)
        dial1_layout.addWidget(self.dial1)
        dial1_layout.addWidget(self.line_edit1)
        
        # Second dial section
        dial2_layout = QVBoxLayout()
        dial2_layout.addWidget(self.dial_label2)
        dial2_layout.addWidget(self.dial2)
        dial2_layout.addWidget(self.line_edit2)

        # Control layout
        control_layout = QVBoxLayout()
        control_layout.addWidget(self.toggle_switch)
        control_layout.addWidget(QLabel("Steer Mode"))
        control_layout.addWidget(self.combo1)
        control_layout.addWidget(QLabel("Boat ID"))
        control_layout.addWidget(self.line_edit3)
        control_layout.addWidget(QLabel("Requested Longitude"))
        control_layout.addWidget(self.line_edit4)
        control_layout.addWidget(QLabel("Requested Latitude"))
        control_layout.addWidget(self.line_edit5)

        # Adding to main layout
        layout.addLayout(dial1_layout, 0, 0)
        layout.addLayout(dial2_layout, 0, 1)
        layout.addLayout(control_layout, 0, 2)

        self.setLayout(layout)
        self.setWindowTitle("Dial Widget Example")

    @Slot()
    def update_line_edit1(self, value):
        self.line_edit1.setText(str(value))

    @Slot()
    def update_line_edit2(self, value):
        self.line_edit2.setText(str(value))

    @Slot()
    def update_dial1(self):
        value = int(self.line_edit1.text())
        self.dial1.setValue(value)

    @Slot()
    def update_dial2(self):
        value = int(self.line_edit2.text())
        self.dial2.setValue(value)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = DialWidget()
    widget.show()
    sys.exit(app.exec())
