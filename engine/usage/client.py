import socket
import time

def main():
    # Server address and port
    server_address = ('localhost', 54321)

    # Create a TCP/IP socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        client_socket.connect(server_address)
        print("Connected to server")

        # List of queries to send
        queries = [
            "CREATE DATABASE HarshDB",
            "CREATE TABLE Users",
            # Add more queries if needed
        ]

        for query in queries:
            client_socket.sendall(query.encode('utf-8'))
            print(f"Sent query: {query}")

            response = client_socket.recv(1024).decode('utf-8')
            print(f"Received response: {response}")
            time.sleep(1)  # Optional: delay between queries

    finally:
        # Close the connection
        client_socket.close()
        print("Connection closed")

if __name__ == "__main__":
    main()
