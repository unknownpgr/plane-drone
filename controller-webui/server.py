import http.server
import socketserver
import urllib.parse
import json
import os

class SimpleHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, callback=None, html_dir="./html", **kwargs):
        self.callback = callback
        self.html_dir = html_dir
        super().__init__(*args, **kwargs)

    def __get_content_type(self, file_path):
        if file_path.endswith(".html"):
            content_type = "text/html"
        elif file_path.endswith(".css"):
            content_type = "text/css"
        elif file_path.endswith(".js"):
            content_type = "application/javascript"
        elif file_path.endswith(".png"):
            content_type = "image/png"
        elif file_path.endswith(".jpg") or file_path.endswith(".jpeg"):
            content_type = "image/jpeg"
        elif file_path.endswith(".json"):
            content_type = "application/json"
        else:
            content_type = "application/octet-stream"
        return content_type

    def __send_file(self, file_path):
        content_type = self.__get_content_type(file_path)
        self.send_header("Content-Type", content_type)
        self.end_headers()
        with open(file_path, "rb") as file:
            self.wfile.write(file.read())

    def __handle_404(self):
        self.send_response(404)
        file_path_404 = os.path.join(self.html_dir, "404.html")
        self.__send_file(file_path_404)

    def do_GET(self):
        parsed_path = urllib.parse.urlparse(self.path)
        query = urllib.parse.parse_qs(parsed_path.query)

        if parsed_path.path == "/api":
            data = query.get("data", [None])[0]
            if data:
                try:
                    data = json.loads(data)
                    if self.callback:
                        response = self.callback(data)
                    else:
                        response = {"status": "error", "message": "No callback provided"}
                except json.JSONDecodeError:
                    response = {"status": "error", "message": "Invalid JSON"}
                except Exception as e:
                    response = {"status": "error", "message": f"Unexpected error: {str(e)}"}
            else:
                response = {"status": "error", "message": "No data provided"}

            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(json.dumps(response).encode())

        else:
            file_path = os.path.join(self.html_dir, parsed_path.path[1:])

            if os.path.exists(file_path) and os.path.isfile(file_path):
                self.send_response(200)
                self.__send_file(file_path)
                return
            
            file_path = os.path.join(file_path, "index.html")
            if os.path.exists(file_path) and os.path.isfile(file_path):
                self.send_response(200)
                self.__send_file(file_path)            
                return
            
            self.__handle_404()

class SimpleHTTPApiStaticServer:
    def __init__(self, callback=None, port=8000, html_dir="./html"):
        self.callback = callback
        self.port = port
        self.html_dir = html_dir

    def start(self):
        socketserver.TCPServer.allow_reuse_address = True
        with socketserver.TCPServer(("", self.port), lambda *args, **kwargs: SimpleHTTPRequestHandler(*args, callback=self.callback,
                                                                                                         html_dir=self.html_dir ,
                                                                                                       **kwargs)) as httpd:
            try:
                print(f"Serving on port {self.port}")
                httpd.serve_forever()
            except:
                pass