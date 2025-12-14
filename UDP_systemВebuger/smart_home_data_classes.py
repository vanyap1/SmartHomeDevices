from dataclasses import dataclass
from typing import List
import struct

@dataclass
class SensorUnitData:
    """Дані з датчика температур (CAN ID: 0x69)"""
    dallas_temp: int = 0
    boiler_temperature: int = 0
    coolant_temperature: int = 0
    solar_collector_temp: int = 0
    
    @classmethod
    def from_can_data(cls, data: List[int]) -> 'SensorUnitData':
        """Створює об'єкт з CAN даних"""
        return cls(
            dallas_temp=data[0] if len(data) > 0 else 0,
            boiler_temperature=data[1] if len(data) > 1 else 0,
            coolant_temperature=data[2] if len(data) > 2 else 0,
            solar_collector_temp=data[3] if len(data) > 3 else 0
        )
    
    def __str__(self) -> str:
        return (f"SensorUnit - Dallas: {self.dallas_temp}°C, "
                f"Boiler: {self.boiler_temperature}°C, "
                f"Coolant: {self.coolant_temperature}°C, "
                f"Solar: {self.solar_collector_temp}°C")

@dataclass 
class UPSFlags:
    """Бітові прапорці UPS системи"""
    power_source: bool = False      # sys_flags.0
    pump_ok: bool = False          # sys_flags.1
    kotel_error: bool = False      # sys_flags.2
    gateway_error: bool = False    # sys_flags.3
    low_battery: bool = False      # sys_flags.4
    overload: bool = False         # sys_flags.5
    
    @classmethod
    def from_byte(cls, flags_byte: int) -> 'UPSFlags':
        """Створює об'єкт з байта прапорців"""
        return cls(
            power_source=(flags_byte & 0x01) != 0,
            pump_ok=(flags_byte & 0x02) != 0,
            kotel_error=(flags_byte & 0x04) != 0,
            gateway_error=(flags_byte & 0x08) != 0,
            low_battery=(flags_byte & 0x10) != 0,
            overload=(flags_byte & 0x20) != 0
        )

@dataclass
class UPSBatteryData:
    """Дані батареї UPS (CAN ID: 0x33)"""
    voltage: float = 0.0           # В вольтах (data[0]/10)
    current: int = 0               # В мілліамперах (data[1] << 8 | data[2])
    capacity: int = 0              # В мілліампер-годинах (data[3:7])
    flags: UPSFlags = None
    
    def __post_init__(self):
        if self.flags is None:
            self.flags = UPSFlags()
    
    @classmethod
    def from_can_data(cls, data: List[int]) -> 'UPSBatteryData':
        """Створює об'єкт з CAN даних"""
        voltage = data[0] / 10.0 if len(data) > 0 else 0.0
        current = (data[1] << 8 | data[2]) if len(data) > 2 else 0
        capacity = (data[3] << 24 | data[4] << 16 | data[5] << 8 | data[6]) if len(data) > 6 else 0
        flags = UPSFlags.from_byte(data[7]) if len(data) > 7 else UPSFlags()
        
        return cls(
            voltage=voltage,
            current=current,
            capacity=capacity,
            flags=flags
        )
    
    def __str__(self) -> str:
        return (f"UPS Battery - U: {self.voltage:.1f}V, "
                f"I: {self.current}mA, SOC: {self.capacity}mAh")

@dataclass
class UPSPumpData:
    """Дані помпи UPS (CAN ID: 0x43)"""
    pump_state: int = 0
    mode_flags: UPSFlags = None
    
    def __post_init__(self):
        if self.mode_flags is None:
            self.mode_flags = UPSFlags()
    
    @classmethod
    def from_can_data(cls, data: List[int]) -> 'UPSPumpData':
        """Створює об'єкт з CAN даних"""
        pump_state = data[0] if len(data) > 0 else 0
        mode_flags = UPSFlags.from_byte(data[7]) if len(data) > 7 else UPSFlags()
        
        return cls(
            pump_state=pump_state,
            mode_flags=mode_flags
        )
    
    def __str__(self) -> str:
        return f"UPS Pump - State: {self.pump_state}, Running: {'Yes' if self.pump_state > 100 else 'No'}"

@dataclass
class KotelFlags:
    """Прапорці стану котла"""
    rel1: bool = False             # sys_status.5
    relay_stat: bool = False       # sys_status.7  
    door_sensor: bool = False      # sys_status.6
    
    @classmethod
    def from_byte(cls, status_byte: int) -> 'KotelFlags':
        """Створює об'єкт з байта стану"""
        return cls(
            rel1=(status_byte & 0x20) != 0,          # біт 5
            door_sensor=(status_byte & 0x40) != 0,    # біт 6
            relay_stat=(status_byte & 0x80) != 0      # біт 7
        )

@dataclass
class KotelData:
    """Дані котла (CAN ID: 0x247)"""
    run_mode: int = 0              # 0-стоп, 1-передпуск, 2-робота, 5-помилка
    actual_temp: float = 0.0       # Поточна температура в °C
    high_limit: int = 0            # Верхня межа температури
    low_limit: int = 0             # Нижня межа температури  
    run_temp: int = 0              # Температура запуску
    stop_temp: int = 0             # Температура зупинки
    delta_temp: int = 0            # Дельта температури
    flags: KotelFlags = None
    
    def __post_init__(self):
        if self.flags is None:
            self.flags = KotelFlags()
    
    @classmethod
    def from_can_data(cls, data: List[int]) -> 'KotelData':
        """Створює об'єкт з CAN даних"""
        run_mode = data[0] if len(data) > 0 else 0
        actual_temp = ((data[1] << 8 | data[2]) / 100.0) if len(data) > 2 else 0.0
        high_limit = data[3] if len(data) > 3 else 0
        low_limit = data[4] if len(data) > 4 else 0
        run_temp = data[5] if len(data) > 5 else 0
        stop_temp = data[6] if len(data) > 6 else 0
        delta_temp = data[7] if len(data) > 7 else 0
        
        # Прапорці беруться з run_mode (перші 4 біти - режим, решта - прапорці)
        flags = KotelFlags.from_byte(run_mode)
        
        return cls(
            run_mode=run_mode & 0x0F,  # Тільки перші 4 біти
            actual_temp=actual_temp,
            high_limit=high_limit,
            low_limit=low_limit,
            run_temp=run_temp,
            stop_temp=stop_temp,
            delta_temp=delta_temp,
            flags=flags
        )
    
    def get_mode_text(self) -> str:
        """Повертає текстовий опис режиму"""
        modes = {
            0: "Стоп",
            1: "Передпуск", 
            2: "Робота",
            3: "Зупинка",
            5: "Помилка"
        }
        return modes.get(self.run_mode, f"Невідомий ({self.run_mode})")
    
    def __str__(self) -> str:
        return (f"Kotel - Mode: {self.get_mode_text()}, "
                f"Temp: {self.actual_temp:.2f}°C, "
                f"Limits: {self.low_limit}-{self.high_limit}°C, "
                f"Run/Stop: {self.run_temp}/{self.stop_temp}°C")

class SmartHomeDataParser:
    """Клас для парсингу всіх типів CAN повідомлень"""
    
    def __init__(self):
        self.sensor_data = SensorUnitData()
        self.ups_battery_data = UPSBatteryData()
        self.ups_pump_data = UPSPumpData()
        self.kotel_data = KotelData()
        self.data_callbacks = []
    
    def add_data_callback(self, callback):
        """Додає callback для сповіщення про нові дані"""
        self.data_callbacks.append(callback)
    
    def notify_callbacks(self):
        """Викликає всі зареєстровані callbacks"""
        for callback in self.data_callbacks:
            try:
                callback(self)
            except Exception as e:
                print(f"Error in callback: {e}")
    
    def parse_can_message(self, can_message):
        """Парсить CAN повідомлення та оновлює відповідні дані"""
        can_id = can_message.can_id
        data_bytes = can_message.data
        
        updated = False
        
        if can_id == 0x69:  # Sensor Unit
            self.sensor_data = SensorUnitData.from_can_data(data_bytes)
            print(str(self.sensor_data))
            updated = True
            
        elif can_id == 0x33:  # UPS Battery
            self.ups_battery_data = UPSBatteryData.from_can_data(data_bytes)
            print(str(self.ups_battery_data))
            updated = True
            
        elif can_id == 0x43:  # UPS Pump
            self.ups_pump_data = UPSPumpData.from_can_data(data_bytes)
            print(str(self.ups_pump_data))
            updated = True
            
        elif can_id == 0x247:  # Kotel
            self.kotel_data = KotelData.from_can_data(data_bytes)
            print(str(self.kotel_data))
            updated = True
        
        # Повідомляємо callbacks про оновлення даних
        if updated:
            self.notify_callbacks()
    
    def get_all_data(self):
        """Повертає всі поточні дані"""
        return {
            'sensor': self.sensor_data,
            'ups_battery': self.ups_battery_data,
            'ups_pump': self.ups_pump_data,
            'kotel': self.kotel_data
        }