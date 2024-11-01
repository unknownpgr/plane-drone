import os
from server import SimpleHTTPApiStaticServer
from service import DroneControllerService

PORT = 8000
HTML_DIR = os.path.join(os.path.dirname(__file__), "html")

controller = DroneControllerService()

def callback(data):
    command = data["command"]

    if command == "ports":
        return controller.list_ports()
    
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