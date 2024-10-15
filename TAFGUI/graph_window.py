from PySide6.QtWidgets import (QWidget, QVBoxLayout, QHBoxLayout, QPushButton, 
                               QComboBox, QCheckBox, QLabel, QGridLayout, QScrollArea)
from PySide6.QtCharts import QChart, QChartView, QLineSeries, QValueAxis, QDateTimeAxis
from PySide6.QtCore import Qt, Slot, QTimer, QDateTime
from PySide6.QtGui import QPainter, QColor
import csv
from datetime import datetime, timedelta
import math
import struct

class GraphsWindow(QWidget):
    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window
        self.layout = QVBoxLayout(self)
        self.device_selector = None
        self.charts = {}
        self.chart_views = {}
        self.series = {}
        self.recording = False
        self.csv_file = None
        self.csv_writer = None
        self.start_time = QDateTime.currentDateTime()

        self.setup_ui()

        # Timer for updating the graph
        self.update_timer = QTimer(self)
        self.update_timer.timeout.connect(self.update_graphs)
        self.update_timer.start(1000)  # Update every second

    def setup_ui(self):
        # Device selector
        self.device_selector = QComboBox()
        self.device_selector.currentIndexChanged.connect(self.update_graphs)

        # Graph selectors
        self.graph_selectors = {}
        for attribute in ["Longitude", "Latitude", "Heading"]:
            checkbox = QCheckBox(attribute)
            checkbox.stateChanged.connect(self.update_graph_visibility)
            self.graph_selectors[attribute] = checkbox

        # Recording button
        self.record_button = QPushButton("Start Recording")
        self.record_button.clicked.connect(self.toggle_recording)

        # Top layout for controls
        top_layout = QHBoxLayout()
        top_layout.addWidget(QLabel("Select Device:"))
        top_layout.addWidget(self.device_selector)
        for checkbox in self.graph_selectors.values():
            top_layout.addWidget(checkbox)
        top_layout.addWidget(self.record_button)
        top_layout.addStretch()

        self.layout.addLayout(top_layout)

        # Create a scroll area for charts
        scroll_area = QScrollArea()
        scroll_area.setWidgetResizable(True)
        charts_widget = QWidget()
        self.charts_layout = QGridLayout(charts_widget)
        scroll_area.setWidget(charts_widget)
        self.layout.addWidget(scroll_area)

    @Slot()
    def update_graphs(self):
        selected_device = self.device_selector.currentText()
        if not selected_device:
            return

        for i, (attribute, checkbox) in enumerate(self.graph_selectors.items()):
            if checkbox.isChecked():
                if attribute not in self.charts:
                    self.create_chart(attribute, i)
                self.update_single_graph(self.charts[attribute], selected_device, attribute)
            elif attribute in self.charts:
                self.remove_chart(attribute)

    def create_chart(self, attribute, index):
        chart = QChart()
        chart.setAnimationOptions(QChart.NoAnimation)
        chart.legend().hide()

        axis_x = QValueAxis()
        axis_x.setTitleText("Time (seconds)")
        chart.addAxis(axis_x, Qt.AlignBottom)

        axis_y = QValueAxis()
        axis_y.setTitleText(attribute)
        if attribute == "Heading":
            axis_y.setRange(0, 360)
        elif attribute == "Latitude":
            axis_y.setRange(-90, 90)
        elif attribute == "Longitude":
            axis_y.setRange(-180, 180)
        chart.addAxis(axis_y, Qt.AlignLeft)

        series = QLineSeries()
        chart.addSeries(series)
        series.attachAxis(axis_x)
        series.attachAxis(axis_y)

        chart_view = QChartView(chart)
        chart_view.setRenderHint(QPainter.Antialiasing)
        chart_view.setMinimumHeight(300)
        chart_view.setMinimumWidth(400)

        self.charts[attribute] = chart
        self.chart_views[attribute] = chart_view
        self.series[attribute] = series

        row = index // 2
        col = index % 2
        self.charts_layout.addWidget(chart_view, row, col)

    def update_single_graph(self, chart, device, attribute):
        series = self.series[attribute]
        axis_x = chart.axes(Qt.Horizontal)[0]
        axis_y = chart.axes(Qt.Vertical)[0]
        
        # Clear previous data
        series.clear()
        
        current_time = QDateTime.currentDateTime()
        elapsed_time = self.start_time.msecsTo(current_time) / 1000.0  # Total elapsed time in seconds
        
        # Fetch data from UART messages
        data = self.get_data_from_uart(device, attribute)
        
        for timestamp, value in data:
            time_diff = self.start_time.msecsTo(timestamp) / 1000.0  # Time difference in seconds
            series.append(time_diff, value)

        if series.count() > 0:
            min_x = 0
            max_x = elapsed_time
            
            # Set fixed y-axis range based on attribute
            if attribute == "Heading":
                min_y, max_y = 0, 360
            elif attribute == "Latitude":
                min_y, max_y = -90, 90
            elif attribute == "Longitude":
                min_y, max_y = -180, 180
            else:
                min_y = min(point.y() for point in series.points())
                max_y = max(point.y() for point in series.points())
                range_y = max_y - min_y
                min_y -= 0.1 * range_y
                max_y += 0.1 * range_y

            axis_x.setRange(min_x, max_x)
            axis_y.setRange(min_y, max_y)

            # Adjust x-axis scale
            if max_x <= 60:
                axis_x.setTickCount(7)  # Show tick every 10 seconds
            elif max_x <= 300:
                axis_x.setTickCount(6)  # Show tick every minute
            else:
                axis_x.setTickCount(7)  # Show tick every 5 minutes

        chart.setTitle(f"{attribute} vs Time for {device}")

    def get_data_from_uart(self, device, attribute):
        data = []
        messages = self.main_window.uart_handler.get_messages_for_device(device)
        
        for message in messages:
            timestamp = message['timestamp']
            parsed_data = bytes.fromhex(message['data'])
            
            if len(parsed_data) < 2:
                continue
            
            message_type = parsed_data[0]
            payload = parsed_data[1:]
            
            if (attribute == "Longitude" and message_type == 0x09) or \
               (attribute == "Latitude" and message_type == 0x0A) or \
               (attribute == "Heading" and message_type == 0x0B):
                try:
                    if message_type in [0x09, 0x0A]:  # Longitude and Latitude
                        int_value = struct.unpack('!i', payload[:4])[0]
                        float_value = int_value / 100000.0
                    elif message_type == 0x0B:  # Heading
                        float_value = struct.unpack('!f', payload[:4])[0]
                    
                    data.append((timestamp, float_value))
                except struct.error:
                    print(f"Error parsing data for message type {message_type}")
        
        return data

    def remove_chart(self, attribute):
        if attribute in self.charts:
            self.charts_layout.removeWidget(self.chart_views[attribute])
            self.chart_views[attribute].deleteLater()
            del self.charts[attribute]
            del self.chart_views[attribute]
            del self.series[attribute]

    @Slot()
    def update_graph_visibility(self):
        self.update_graphs()

    def update_device_list(self):
        current_devices = set(self.main_window.uart_handler.get_device_list())
        
        for device in current_devices:
            if self.device_selector.findText(device) == -1:
                self.device_selector.addItem(device)

    @Slot()
    def toggle_recording(self):
        if not self.recording:
            self.start_recording()
        else:
            self.stop_recording()

    def start_recording(self):
        filename = f"data_recording_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
        self.csv_file = open(filename, 'w', newline='')
        self.csv_writer = csv.writer(self.csv_file)
        
        # Write header
        header = ["Timestamp", "Device", "Longitude", "Latitude", "Heading"]
        self.csv_writer.writerow(header)
        
        self.recording = True
        self.record_button.setText("Stop Recording")

    def stop_recording(self):
        if self.csv_file:
            self.csv_file.close()
        self.recording = False
        self.record_button.setText("Start Recording")

    def record_data(self):
        if not self.recording:
            return

        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        for device in self.main_window.uart_handler.get_device_list():
            longitude = self.get_latest_value(device, "Longitude")
            latitude = self.get_latest_value(device, "Latitude")
            heading = self.get_latest_value(device, "Heading")
            
            self.csv_writer.writerow([timestamp, device, longitude, latitude, heading])

    def get_latest_value(self, device, attribute):
        data = self.get_data_from_uart(device, attribute)
        if data:
            return data[-1][1]  # Return the latest value
        return ''

    def closeEvent(self, event):
        self.stop_recording()
        super().closeEvent(event)