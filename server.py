import socket
from datetime import datetime
from Crypto.Cipher import AES
from Crypto.Util.Padding import unpad

if __name__ == '__main__':
    key = b"fdsdabuiifjvjdufjvfsrwqokvcxufew"
    HOST = "0.0.0.0"
    PORT = 27015

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((HOST, PORT))
    s.listen(1)

    print("Server listening...")

    while True:
        with open("task.txt", "rb") as f:
            task = f.read().strip()

        conn, addr = s.accept()
        print("Connected:", addr)

        data = conn.recv(1024)
        print("Received:", data)

        if data == b"get_task":
            conn.send(task)
            result = conn.recv(4096)

            iv = result[:16]
            ciphertext = result[16:]

            cipher = AES.new(key, AES.MODE_CBC, iv)
            plaintext = unpad(cipher.decrypt(ciphertext), 16)

            timenow = datetime.now()
            time = timenow.strftime("%d.%m.%Y %H:%M:%S")

            with open("receivedata.txt", "ab") as f:
                f.write(f"Time:    {time}\nPayload: {plaintext.decode()}\nTask:    {task.decode()}\n\n".encode())

        conn.close()