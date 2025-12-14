import sys
import time
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                            QHBoxLayout, QGridLayout, QLabel, QGroupBox, 
                            QFrame, QPushButton, QTextEdit, QScrollArea)
from PyQt5.QtCore import QTimer, pyqtSignal, QObject, QThread
from PyQt5.QtGui import QFont, QPalette, QColor
from PyQt5.QtCore import Qt
from datetime import datetime

from smartHomeUdpService import SmartHomeGatewayUdpClient
from smart_home_data_classes import SmartHomeDataParser

class SmartHomeMonitorGUI(QMainWindow):
    def __init__(self):
        super().__init__()
        self.data_parser = SmartHomeDataParser()
        self.udp_client = None
        self.init_ui()
        self.setup_udp_client()
        
        # Таймер для оновлення часу
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_time)
        self.timer.start(1000)  # Оновлення кожну секунду
        
    def init_ui(self):
        self.setWindowTitle('Smart Home Monitor')
        self.setGeometry(100, 100, 800, 600)
        self.setStyleSheet("""
            QMainWindow {
                background-color: #2b2b2b;
                color: #ffffff;
            }
            QGroupBox {
                font-weight: bold;
                border: 2px solid #555555;
                border-radius: 5px;
                margin-top: 1ex;
                padding-top: 10px;
                background-color: #3c3c3c;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 10px;
                padding: 0 5px 0 5px;
                color: #00ff00;
            }
            QLabel {
                color: #ffffff;
                font-size: 12px;
            }
            .value-label {
                color: #00ffff;
                font-weight: bold;
            }
            .status-ok {
                color: #00ff00;
            }
            .status-warning {
                color: #ffff00;
            }
            .status-error {
                color: #ff0000;
            }
        """)
        
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        main_layout = QVBoxLayout(central_widget)
        
        # Заголовок з часом
        self.header_label = QLabel("Smart Home Monitor")
        self.header_label.setAlignment(Qt.AlignCenter)
        self.header_label.setFont(QFont("Arial", 16, QFont.Bold))
        self.header_label.setStyleSheet("color: #00ff00; margin: 10px;")
        main_layout.addWidget(self.header_label)
        
        self.time_label = QLabel()
        self.time_label.setAlignment(Qt.AlignCenter)
        self.time_label.setFont(QFont("Arial", 12))
        self.time_label.setStyleSheet("color: #ffff00; margin-bottom: 10px;")
        main_layout.addWidget(self.time_label)
        
        # Основна сітка для груп
        grid_layout = QGridLayout()
        main_layout.addLayout(grid_layout)
        
        # Створення груп
        self.sensor_group = self.create_sensor_group()
        self.ups_group = self.create_ups_group()
        self.kotel_group = self.create_kotel_group()
        self.system_group = self.create_system_group()
        
        # Розміщення груп у сітці 2x2
        grid_layout.addWidget(self.sensor_group, 0, 0)
        grid_layout.addWidget(self.ups_group, 0, 1)
        grid_layout.addWidget(self.kotel_group, 1, 0)
        grid_layout.addWidget(self.system_group, 1, 1)
        
        # Журнал подій внизу
        self.log_text = QTextEdit()
        self.log_text.setMaximumHeight(150)
        self.log_text.setStyleSheet("""
            QTextEdit {
                background-color: #1e1e1e;
                color: #00ff00;
                border: 1px solid #555555;
                font-family: monospace;
                font-size: 10px;
            }
        """)
        main_layout.addWidget(QLabel("Журнал подій:"))
        main_layout.addWidget(self.log_text)
        
        # Кнопки управління
        button_layout = QHBoxLayout()
        self.connect_btn = QPushButton("Підключити")
        self.disconnect_btn = QPushButton("Відключити")
        self.clear_log_btn = QPushButton("Очистити журнал")
        
        self.connect_btn.clicked.connect(self.connect_udp)
        self.disconnect_btn.clicked.connect(self.disconnect_udp)
        self.clear_log_btn.clicked.connect(self.clear_log)
        
        button_layout.addWidget(self.connect_btn)
        button_layout.addWidget(self.disconnect_btn)
        button_layout.addWidget(self.clear_log_btn)
        button_layout.addStretch()
        
        main_layout.addLayout(button_layout)
        
        # Ініціалізація даних парсера
        self.data_parser.add_data_callback(self.on_data_updated)
        self.update_all_displays()
        
    def create_sensor_group(self):
        group = QGroupBox("Датчики температури")
        layout = QVBoxLayout(group)
        
        self.dallas_temp_label = QLabel("Dallas температура: --°C")
        self.boiler_temp_label = QLabel("Температура Бойлера: --°C")
        self.coolant_temp_label = QLabel("Температура теплоносія: --°C")
        self.solar_temp_label = QLabel("Температура сонячного колектора: --°C")
        
        for label in [self.dallas_temp_label, self.boiler_temp_label, 
                     self.coolant_temp_label, self.solar_temp_label]:
            label.setStyleSheet("margin: 3px;")
            layout.addWidget(label)
            
        return group
        
    def create_ups_group(self):
        group = QGroupBox("UPS система")
        layout = QVBoxLayout(group)
        
        # Батарея
        battery_frame = QFrame()
        battery_layout = QVBoxLayout(battery_frame)
        battery_frame.setFrameStyle(QFrame.StyledPanel)
        
        battery_title = QLabel("Батарея:")
        battery_title.setStyleSheet("font-weight: bold; color: #ffff00;")
        battery_layout.addWidget(battery_title)
        
        self.battery_voltage_label = QLabel("Напруга: --V")
        self.battery_current_label = QLabel("Струм: --mA")
        self.battery_capacity_label = QLabel("Ємність: --mAh")
        
        battery_layout.addWidget(self.battery_voltage_label)
        battery_layout.addWidget(self.battery_current_label)
        battery_layout.addWidget(self.battery_capacity_label)
        
        # Помпа
        pump_frame = QFrame()
        pump_layout = QVBoxLayout(pump_frame)
        pump_frame.setFrameStyle(QFrame.StyledPanel)
        
        pump_title = QLabel("Помпа:")
        pump_title.setStyleSheet("font-weight: bold; color: #ffff00;")
        pump_layout.addWidget(pump_title)
        
        self.pump_state_label = QLabel("Стан: --")
        self.pump_running_label = QLabel("Робота: --")
        
        pump_layout.addWidget(self.pump_state_label)
        pump_layout.addWidget(self.pump_running_label)
        
        # Прапорці UPS
        flags_frame = QFrame()
        flags_layout = QVBoxLayout(flags_frame)
        flags_frame.setFrameStyle(QFrame.StyledPanel)
        
        flags_title = QLabel("Статус:")
        flags_title.setStyleSheet("font-weight: bold; color: #ffff00;")
        flags_layout.addWidget(flags_title)
        
        self.power_source_label = QLabel("Джерело живлення: --")
        self.pump_ok_label = QLabel("Помпа OK: --")
        self.low_battery_label = QLabel("Низький заряд: --")
        self.overload_label = QLabel("Перевантаження: --")
        
        flags_layout.addWidget(self.power_source_label)
        flags_layout.addWidget(self.pump_ok_label)
        flags_layout.addWidget(self.low_battery_label)
        flags_layout.addWidget(self.overload_label)
        
        layout.addWidget(battery_frame)
        layout.addWidget(pump_frame)
        layout.addWidget(flags_frame)
        
        return group
        
    def create_kotel_group(self):
        group = QGroupBox("Котел")
        layout = QVBoxLayout(group)
        
        self.kotel_mode_label = QLabel("Режим: --")
        self.kotel_temp_label = QLabel("Поточна температура: --°C")
        self.kotel_limits_label = QLabel("Межі: --/--°C")
        self.kotel_run_stop_label = QLabel("Пуск/Стоп: --/--°C")
        self.kotel_delta_label = QLabel("Дельта: --°C")
        
        # Прапорці котла
        flags_frame = QFrame()
        flags_layout = QVBoxLayout(flags_frame)
        flags_frame.setFrameStyle(QFrame.StyledPanel)
        
        flags_title = QLabel("Стан:")
        flags_title.setStyleSheet("font-weight: bold; color: #ffff00;")
        flags_layout.addWidget(flags_title)
        
        self.kotel_rel1_label = QLabel("Реле 1: --")
        self.kotel_relay_stat_label = QLabel("Реле статус: --")
        self.kotel_door_label = QLabel("Датчик дверей: --")
        
        flags_layout.addWidget(self.kotel_rel1_label)
        flags_layout.addWidget(self.kotel_relay_stat_label)
        flags_layout.addWidget(self.kotel_door_label)
        
        layout.addWidget(self.kotel_mode_label)
        layout.addWidget(self.kotel_temp_label)
        layout.addWidget(self.kotel_limits_label)
        layout.addWidget(self.kotel_run_stop_label)
        layout.addWidget(self.kotel_delta_label)
        layout.addWidget(flags_frame)
        
        return group
        
    def create_system_group(self):
        group = QGroupBox("Система")
        layout = QVBoxLayout(group)
        
        self.connection_status_label = QLabel("З'єднання: Відключено")
        self.last_update_label = QLabel("Останнє оновлення: --")
        self.packets_received_label = QLabel("Пакетів отримано: 0")
        
        layout.addWidget(self.connection_status_label)
        layout.addWidget(self.last_update_label)
        layout.addWidget(self.packets_received_label)
        
        # Додаткові системні показники
        layout.addStretch()
        
        return group
        
    def setup_udp_client(self):
        """Налаштування UDP клієнта"""
        self.udp_client = SmartHomeGatewayUdpClient(
            cbFn=self.on_can_message_received,
            gatewayIp="192.168.1.18",
            rxPort=4030,
            txPort=4031,
            bufferSize=1024
        )
        
    def connect_udp(self):
        """Підключення до UDP"""
        try:
            if self.udp_client:
                self.udp_client.startListener()
                self.connection_status_label.setText("З'єднання: Підключено")
                self.connection_status_label.setStyleSheet("color: #00ff00;")
                self.add_log("UDP з'єднання встановлено")
                self.connect_btn.setEnabled(False)
                self.disconnect_btn.setEnabled(True)
        except Exception as e:
            self.add_log(f"Помилка підключення: {e}")
            
    def disconnect_udp(self):
        """Відключення від UDP"""
        try:
            if self.udp_client:
                self.udp_client.stopListener()
                self.connection_status_label.setText("З'єднання: Відключено")
                self.connection_status_label.setStyleSheet("color: #ff0000;")
                self.add_log("UDP з'єднання закрито")
                self.connect_btn.setEnabled(True)
                self.disconnect_btn.setEnabled(False)
        except Exception as e:
            self.add_log(f"Помилка відключення: {e}")
            
    def on_can_message_received(self, can_message):
        """Обробка отриманого CAN повідомлення"""
        try:
            self.data_parser.parse_can_message(can_message)
            self.last_update_label.setText(f"Останнє оновлення: {datetime.now().strftime('%H:%M:%S')}")
            
            # Оновлення лічильника пакетів
            current_count = int(self.packets_received_label.text().split(': ')[1])
            self.packets_received_label.setText(f"Пакетів отримано: {current_count + 1}")
            
        except Exception as e:
            self.add_log(f"Помилка обробки CAN повідомлення: {e}")
            
    def on_data_updated(self, parser):
        """Callback для оновлення GUI при зміні даних"""
        self.update_all_displays()
        
    def update_all_displays(self):
        """Оновлення всіх відображень даних"""
        # Датчики
        sensor = self.data_parser.sensor_data
        self.dallas_temp_label.setText(f"Dallas температура: {sensor.dallas_temp}°C")
        self.boiler_temp_label.setText(f"Температура бойлера: {sensor.boiler_temperature}°C")
        self.coolant_temp_label.setText(f"Температура теплоносія: {sensor.coolant_temperature}°C")
        self.solar_temp_label.setText(f"Температура сонячного колектора: {sensor.solar_collector_temp}°C")
        
        # UPS батарея
        battery = self.data_parser.ups_battery_data
        self.battery_voltage_label.setText(f"Напруга: {battery.voltage:.1f}V")
        self.battery_current_label.setText(f"Струм: {battery.current}mA")
        self.battery_capacity_label.setText(f"Ємність: {battery.capacity}mAh")
        
        # UPS помпа
        pump = self.data_parser.ups_pump_data
        self.pump_state_label.setText(f"Стан: {pump.pump_state}")
        pump_running = "Так" if pump.pump_state > 100 else "Ні"
        pump_color = "color: #00ff00;" if pump.pump_state > 100 else "color: #ff0000;"
        self.pump_running_label.setText(f"Робота: {pump_running}")
        self.pump_running_label.setStyleSheet(pump_color)
        
        # UPS прапорці
        self.update_flag_label(self.power_source_label, "Джерело живлення", battery.flags.power_source)
        self.update_flag_label(self.pump_ok_label, "Помпа OK", battery.flags.pump_ok)
        self.update_flag_label(self.low_battery_label, "Низький заряд", battery.flags.low_battery)
        self.update_flag_label(self.overload_label, "Перевантаження", battery.flags.overload)
        
        # Котел
        kotel = self.data_parser.kotel_data
        self.kotel_mode_label.setText(f"Режим: {kotel.get_mode_text()}")
        self.kotel_temp_label.setText(f"Поточна температура: {kotel.actual_temp:.2f}°C")
        self.kotel_limits_label.setText(f"Межі: {kotel.low_limit}/{kotel.high_limit}°C")
        self.kotel_run_stop_label.setText(f"Пуск/Стоп: {kotel.run_temp}/{kotel.stop_temp}°C")
        self.kotel_delta_label.setText(f"Дельта: {kotel.delta_temp}°C")
        
        # Прапорці котла
        self.update_flag_label(self.kotel_rel1_label, "Реле 1", kotel.flags.rel1)
        self.update_flag_label(self.kotel_relay_stat_label, "Реле статус", kotel.flags.relay_stat)
        self.update_flag_label(self.kotel_door_label, "Датчик дверей", kotel.flags.door_sensor)
        
    def update_flag_label(self, label, text, flag_value):
        """Оновлення мітки прапорця з відповідним кольором"""
        status = "Так" if flag_value else "Ні"
        color = "color: #00ff00;" if flag_value else "color: #ff0000;"
        label.setText(f"{text}: {status}")
        label.setStyleSheet(color)
        
    def update_time(self):
        """Оновлення часу в заголовку"""
        current_time = datetime.now().strftime("%d.%m.%Y %H:%M:%S")
        self.time_label.setText(f"Поточний час: {current_time}")
        
    def add_log(self, message):
        """Додавання повідомлення до журналу"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        log_entry = f"[{timestamp}] {message}"
        self.log_text.append(log_entry)
        
        # Автоматичне прокручування до кінця
        cursor = self.log_text.textCursor()
        cursor.movePosition(cursor.End)
        self.log_text.setTextCursor(cursor)
        
    def clear_log(self):
        """Очищення журналу"""
        self.log_text.clear()
        self.add_log("Журнал очищено")
        
    def closeEvent(self, event):
        """Обробка закриття вікна"""
        if self.udp_client:
            self.udp_client.stopListener()
        event.accept()

def main():
    app = QApplication(sys.argv)
    window = SmartHomeMonitorGUI()
    window.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()