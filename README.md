# encrypted-communication-lab
Educational laboratory project demonstrating AES-CBC encrypted TCP client-server communication.
The project was created for studying network traffic analysis in Wireshark and practicing reverse engineering of encrypted communication in a controlled environment.

# How it works
Before running the project, create a file named task.txt in the server directory and place a single digit inside it: either 1 or 2.
    Task 1 — the client collects and transmits the hostname.
    Task 2 — the client collects and transmits the username of the active user.

When the client application starts, you can choose between:
    Localhost mode (127.0.0.1)
    Remote server mode (manual IP input)
    Remote server mode with IP loaded from config.txt
This allows testing either in a local lab environment or against a remote server without retyping the IP address each time.

After selecting the connection mode, the client establishes a TCP connection to the server. The communication flow is as follows:
    The client sends the string "get_task".
    The server responds with the task value read from task.txt.
    Based on the received task:
        The client collects the corresponding system information.
        The data is encrypted using AES-CBC.
        The packet [IV + ciphertext] is sent to the server.
    The server receives the packet, extracts the IV, decrypts the payload, and logs the result into receivedata.txt along with a timestamp.

This simplified polling-based design was intentionally implemented for educational purposes to clearly demonstrate encrypted TCP communication and its analysis in a controlled environment.
