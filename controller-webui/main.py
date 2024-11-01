import os
from threading import Event
from server import SimpleHTTPApiStaticServer
from service import DroneControllerService

PORT = 8000
HTML_DIR = os.path.join(os.path.dirname(__file__), "html")

controller = DroneControllerService()

ports = controller.list_ports()
port = controller.get_port()
logs = controller.get_hw_logs()
update_event = Event()

def update_state():
    global ports, port, logs, is_updated
    ports = controller.list_ports()
    port = controller.get_port()
    logs = controller.get_hw_logs()
    update_event.set()

controller.add_listener(update_state)

def callback(data):
    command = data["command"]

    if command == "status":
        update_event.wait(timeout=5)
        update_event.clear()
        return {
            "status": "ok",
            "ports": ports,
            "port": port,
            "logs": logs
        }
    
    if command == "connect":
        serial_port = data["port"]
        controller.connect(serial_port)
        return {"status": "ok"}
    
    if command == "port":
        return controller.get_port()
    
    if command == "disconnect":
        controller.disconnect()
        return {"status": "ok"}
    
    if command == "log":
        return controller.get_hw_logs()
    
    if command == "throttle":
        value = data["value"]
        controller.set_throttle(value)
        return {"status": "ok"}
    
    if command == "pitch":
        value = data["value"]
        controller.set_pitch(value)
        return {"status": "ok"}
    
    if command == "roll":
        value = data["value"]
        controller.set_roll(value)
        return {"status": "ok"}

    else:
        return {"status": "error", "message": f"Unknown command: {command}"}

def main():
    server = SimpleHTTPApiStaticServer(port=PORT, html_dir=HTML_DIR, callback=callback)
    server.start()

if __name__ == "__main__":
    main()