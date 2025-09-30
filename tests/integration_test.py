import subprocess
import time
import requests
import os
import sys

SERVER_PATH = "./bin/server"
DEFAULT_PORT = 8080
DEFAULT_DOC_ROOT = "./web"

class IntegrationTest:
    def __init__(self):
        self.server_process = None
        self.base_url = f"http://localhost:{DEFAULT_PORT}"

    def start_server(self, port=DEFAULT_PORT, doc_root=DEFAULT_DOC_ROOT):
        print(f"Iniciando servidor na porta {port} com diretório raiz {doc_root}...")
        command = [SERVER_PATH, "-p", str(port), "-d", doc_root]
        self.server_process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        # Wait for the server to start listening
        for i in range(30): # Try for up to 30 seconds
            try:
                # Attempt to connect to the server
                response = requests.get(f"http://localhost:{port}/", timeout=2) # Increased timeout for initial connection
                if response.status_code in [200, 404]: # Server is responding
                    print("Servidor está pronto.")
                    return
            except requests.exceptions.ConnectionError:
                pass # Server not ready yet, continue loop
            except requests.exceptions.Timeout:
                print(f"Timeout na tentativa {i+1}")
                pass # Server not ready yet, continue loop
            time.sleep(1) # Wait a bit before retrying

        # If we reach here, the server didn't start in time
        if self.server_process.poll() is not None:
            stdout, stderr = self.server_process.communicate()
            raise Exception(f"Servidor falhou ao iniciar. Stdout: {stdout.decode()}, Stderr: {stderr.decode()}")
        else:
            # Server is still running but not responding to requests
            # Don't call communicate with timeout, just terminate
            self.server_process.terminate()
            try:
                stdout, stderr = self.server_process.communicate(timeout=2)
                raise Exception(f"Servidor não ficou pronto dentro do timeout. Saída do servidor. Stdout: {stdout.decode()}, Stderr: {stderr.decode()}")
            except subprocess.TimeoutExpired:
                self.server_process.kill()
                raise Exception("Servidor não ficou pronto dentro do timeout e falhou ao encerrar graciosamente.")

    def stop_server(self):
        if self.server_process:
            print("Parando servidor...")
            self.server_process.terminate()
            time.sleep(1) # Give it more time to terminate
            try:
                self.server_process.wait(timeout=10) # Give it more time to terminate
            except subprocess.TimeoutExpired:
                print("Servidor não encerrou graciosamente dentro do timeout, finalizando forçadamente.")
                self.server_process.kill()
                self.server_process.wait() # Wait for kill to complete
            self.server_process = None
            print("Servidor parado.")

    def run_test(self, name, test_func):
        print(f"\n--- Executando Teste: {name} ---")
        try:
            test_func()
            print(f"✅ PASSOU: {name}")
        except AssertionError as e:
            print(f"❌ FALHOU: {name} - {e}")
            self.stop_server()
            sys.exit(1)
        except requests.exceptions.ConnectionError as e:
            print(f"❌ FALHOU: {name} - Não foi possível conectar ao servidor: {e}")
            self.stop_server()
            sys.exit(1)
        except Exception as e:
            print(f"❌ FALHOU: {name} - {e}")
            self.stop_server()
            sys.exit(1)

    def test_get_existing_file(self):
        response = requests.get(f"{self.base_url}/index.html")
        assert response.status_code == 200, f"Expected 200, got {response.status_code}"
        assert "text/html" in response.headers["Content-Type"], f"Expected Content-Type text/html, got {response.headers['Content-Type']}"
        assert "<h1>Salop Web Server</h1>" in response.text, "Expected '<h1>Salop Web Server</h1>' in body"

    def test_get_non_existent_file(self):
        response = requests.get(f"{self.base_url}/non_existent.html")
        assert response.status_code == 404, f"Expected 404, got {response.status_code}"
        assert "text/html" in response.headers["Content-Type"], f"Expected Content-Type text/html, got {response.headers['Content-Type']}"
        assert "404 Not Found" in response.text, "Expected '404 Not Found' in body"

    def test_head_existing_file(self):
        response = requests.head(f"{self.base_url}/index.html")
        assert response.status_code == 200, f"Expected 200, got {response.status_code}"
        assert "text/html" in response.headers["Content-Type"], f"Expected Content-Type text/html, got {response.headers['Content-Type']}"
        assert not response.content, "Expected empty body for HEAD request"

    def test_unsupported_method(self):
        response = requests.post(f"{self.base_url}/index.html", data={"key": "value"})
        assert response.status_code == 501, f"Expected 501, got {response.status_code}"
        assert "text/html" in response.headers["Content-Type"], f"Expected Content-Type text/html, got {response.headers['Content-Type']}"
        assert "501 Not Implemented" in response.text, "Expected '501 Not Implemented' in body"

    def test_custom_port(self):
        custom_port = 8081
        self.stop_server() # Stop default server
        time.sleep(2)  # Wait for port to be released
        self.start_server(port=custom_port)
        custom_base_url = f"http://localhost:{custom_port}"
        response = requests.get(f"{custom_base_url}/index.html")
        assert response.status_code == 200, f"Expected 200, got {response.status_code}"
        self.stop_server()
        time.sleep(2)  # Wait for port to be released before restarting on default port
        self.start_server(port=DEFAULT_PORT) # Restart default server for subsequent tests

    def test_custom_doc_root(self):
        # Create a temporary document root
        temp_doc_root = "./temp_web"
        os.makedirs(temp_doc_root, exist_ok=True)
        with open(os.path.join(temp_doc_root, "test.html"), "w") as f:
            f.write("<html><body><h1>Temp Test</h1></body></html>")

        self.stop_server() # Stop default server
        time.sleep(2)  # Wait for port to be released
        self.start_server(doc_root=temp_doc_root)
        response = requests.get(f"{self.base_url}/test.html")
        assert response.status_code == 200, f"Expected 200, got {response.status_code}"
        assert "Temp Test" in response.text, "Expected 'Temp Test' in body"
        self.stop_server()
        time.sleep(2)  # Wait for port to be released before restarting
        self.start_server(port=DEFAULT_PORT) # Restart default server

        # Clean up temporary document root
        os.remove(os.path.join(temp_doc_root, "test.html"))
        os.rmdir(temp_doc_root)


if __name__ == "__main__":
    tester = IntegrationTest()
    try:
        tester.start_server()

        tester.run_test("GET Arquivo Existente", tester.test_get_existing_file)
        tester.run_test("GET Arquivo Inexistente", tester.test_get_non_existent_file)
        tester.run_test("HEAD Arquivo Existente", tester.test_head_existing_file)
        tester.run_test("Método Não Suportado (POST)", tester.test_unsupported_method)
        tester.run_test("Porta Personalizada", tester.test_custom_port)
        tester.run_test("Diretório Raiz Personalizado", tester.test_custom_doc_root)

    finally:
        tester.stop_server()

    print("\n🎉 Todos os Testes de Integração Concluídos com Sucesso!")