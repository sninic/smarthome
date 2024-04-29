from flask import Flask, render_template, request, jsonify
from bluepy import btle
import threading

app = Flask(__name__)

# Global variable to hold the BLE connection and ADC value
global_ble_connection = None
global_adc_value = "Waiting for value..."

class MyDelegate(btle.DefaultDelegate):
    def __init__(self):
        super().__init__()

    def handleNotification(self, cHandle, data):
        global global_adc_value
        try:
            # Assuming the data is just the ADC value sent as a string
            global_adc_value = data.decode("utf-8")
        except Exception as e:
            print("Error processing notification:", e)

def create_ble_connection(device_name="ESP32_BLE_Server", attempts=3):
    global global_ble_connection
    if global_ble_connection is None:
        print(f"Connecting to {device_name}...")
        while attempts > 0:
            try:
                scanner = btle.Scanner()
                devices = scanner.scan(10.0)
                for dev in devices:
                    for adtype, desc, value in dev.getScanData():
                        if desc == "Complete Local Name" and value == device_name:
                            global_ble_connection = btle.Peripheral(dev.addr, dev.addrType)
                            global_ble_connection.setDelegate(MyDelegate())
                            print("Connected to ESP32_BLE_Server")
                            return global_ble_connection
                attempts -= 1
                print("Retrying, attempts left:", attempts)
            except Exception as e:
                print("Connection attempt failed:", e)
                attempts -= 1
    return global_ble_connection

def send_ble_data(device, to_send):
    global global_ble_connection
    if global_ble_connection is not None:
        try:
            service_uuid = btle.UUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b")
            char_uuid = btle.UUID("beb5483e-36e1-4688-b7f5-ea07361b26a8")
            if device == "lighting":
                service_uuid = btle.UUID("c5669482-ac2a-40f9-8a50-8ad39f48c44d")
                char_uuid = btle.UUID("ea3e3bd6-2f3f-4600-a2fc-9053895469ca")
            service = global_ble_connection.getServiceByUUID(service_uuid)
            char = service.getCharacteristics(char_uuid)[0]
            char.write(to_send.encode('utf-8'))
            print("Data sent over BLE: ", to_send)
        except Exception as e:
            print("Failed to send data over BLE:", e)
    else:
        print("BLE connection not established.")

def monitor_ble_notifications():
    while True:
        if global_ble_connection:
            global_ble_connection.waitForNotifications(0.1)

@app.route('/adc_value')
def adc_value():
    return jsonify(value=global_adc_value)

@app.route('/')
def main_menu():
    global global_ble_connection
    if global_ble_connection:
        try:
            global_ble_connection.disconnect()
            print("Disconnected from ESP32_BLE_Server")
        except Exception as e:
            print("Failed to disconnect:", e)
        finally:
            global_ble_connection = None
    return render_template('main_menu.html')

@app.route('/lighting', methods=['GET', 'POST'])
def lighting():
    create_ble_connection("ESP32_BLE_Server_lighting")
    if request.method == 'POST':
        if request.headers.get('X-Requested-With') == 'XMLHttpRequest':
            action = request.form.get('action', 'toggle')
            color = request.form.get('color', 'No Color')
            send_ble_data("lighting",f"Lighting:{action}:{color}")
                
            return jsonify(success=True, action=action, color=color)
    return render_template('lighting.html')

@app.route('/blinds', methods=['GET', 'POST'])
def blinds():
    create_ble_connection("ESP32_BLE_Server")
    if request.method == 'POST':
        action = request.form.get('action')
        if action in ['open', 'close', 'up', 'down']:
            send_ble_data("blinds", f"Blinds:{action}")
            if request.headers.get('X-Requested-With') == 'XMLHttpRequest':
                return jsonify(success=True, action=action)
    return render_template('blinds.html')


if __name__ == '__main__':
    threading.Thread(target=monitor_ble_notifications, daemon=True).start()
    app.run(host='0.0.0.0', port=5000)
