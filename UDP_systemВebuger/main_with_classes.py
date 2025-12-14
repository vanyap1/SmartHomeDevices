import time
import datetime
from smartHomeUdpService import SmartHomeGatewayUdpClient
from smart_home_data_classes import SmartHomeDataParser

def main():
    """Головна функція для запуску системи моніторингу"""
    
    # Створюємо парсер даних
    data_parser = SmartHomeDataParser()
    
    # Callback функція для обробки CAN повідомлень
    def on_can_message_received(can_message):
        data_parser.parse_can_message(can_message)
    
    # Створюємо UDP клієнт
    udp_client = SmartHomeGatewayUdpClient(
        cbFn=on_can_message_received,
        gatewayIp="192.168.1.18",
        rxPort=4030,
        txPort=4031,
        bufferSize=1024
    )
    
    print("Запуск Smart Home UDP Service з новими класами даних...")
    print("Натисніть Ctrl+C для виходу")
    
    try:
        # Запускаємо прослуховування
        udp_client.startListener()
        time.sleep(0.5)
        
        # Головний цикл
        while True:
            time.sleep(1)
            
            # Можна додати періодичні дії тут
            # Наприклад, відправка команд або логування
            
    except KeyboardInterrupt:
        print("\nЗупинка сервісу...")
        udp_client.stopListener()
        print("Сервіс зупинено")

if __name__ == "__main__":
    main()