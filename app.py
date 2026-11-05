import tkinter as tk
from tkinter import ttk, messagebox
import socket
import struct

class MiniPostgresGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("🐘 Mini-PostgreSQL Studio")
        self.root.geometry("750x550")
        self.root.minsize(600, 400)
        self.sock = None

        top_frame = tk.Frame(root, bg="#2c3e50", pady=8, padx=10)
        top_frame.pack(fill=tk.X)

        self.status_label = tk.Label(top_frame, text="🔴 Disconnected from Server (port 5432)", fg="#e74c3c", bg="#2c3e50", font=("Segoe UI", 10, "bold"))
        self.status_label.pack(side=tk.LEFT)

        btn_connect = tk.Button(top_frame, text="🔄 Reconnect", command=self.connect_db, bg="#34495e", fg="white", relief=tk.FLAT, padx=10)
        btn_connect.pack(side=tk.RIGHT)

        input_frame = tk.LabelFrame(root, text=" SQL Query Editor ", font=("Segoe UI", 10, "bold"), padx=10, pady=5)
        input_frame.pack(fill=tk.X, padx=10, pady=5)

        self.sql_text = tk.Text(input_frame, height=4, font=("Consolas", 11), wrap=tk.WORD)
        self.sql_text.insert(tk.END, "SELECT * FROM users;")
        self.sql_text.pack(fill=tk.X, pady=5)

        btn_frame = tk.Frame(input_frame)
        btn_frame.pack(fill=tk.X)

        btn_run = tk.Button(btn_frame, text="▶ Execute Query", command=self.run_query, bg="#27ae60", fg="white", font=("Segoe UI", 10, "bold"), relief=tk.FLAT, padx=15, pady=4)
        btn_run.pack(side=tk.LEFT)

        tk.Button(btn_frame, text="Template: INSERT", command=lambda: self.set_sql("INSERT INTO users VALUES (1, 'Alice');"), relief=tk.GROOVE).pack(side=tk.RIGHT, padx=3)
        tk.Button(btn_frame, text="Template: SELECT All", command=lambda: self.set_sql("SELECT * FROM users;"), relief=tk.GROOVE).pack(side=tk.RIGHT, padx=3)
        tk.Button(btn_frame, text="Template: LIMIT 1", command=lambda: self.set_sql("SELECT * FROM users LIMIT 1;"), relief=tk.GROOVE).pack(side=tk.RIGHT, padx=3)

        result_frame = tk.LabelFrame(root, text=" Query Results ", font=("Segoe UI", 10, "bold"), padx=10, pady=5)
        result_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)

        columns = ("col1", "col2")
        self.tree = ttk.Treeview(result_frame, columns=columns, show="headings")
        self.tree.heading("col1", text="ID")
        self.tree.heading("col2", text="NAME")
        self.tree.pack(fill=tk.BOTH, expand=True)

        self.log_label = tk.Label(root, text="Ready.", bd=1, relief=tk.SUNKEN, anchor=tk.W, font=("Segoe UI", 9))
        self.log_label.pack(side=tk.BOTTOM, fill=tk.X)

        self.connect_db()

    def set_sql(self, sql):
        self.sql_text.delete("1.0", tk.END)
        self.sql_text.insert(tk.END, sql)

    def connect_db(self):
        try:
            if self.sock:
                self.sock.close()
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.connect(('127.0.0.1', 5432))

            startup_payload = b"user\x00postgres\x00\x00"
            length = 4 + 4 + len(startup_payload)
            packet = struct.pack('>II', length, 196608) + startup_payload
            self.sock.sendall(packet)

            self.sock.recv(9)
            self.sock.recv(6)

            self.status_label.config(text="🟢 Connected to Mini-Postgres (127.0.0.1:5432)", fg="#2ecc71")
            self.log_label.config(text="Connected successfully! Ready to accept queries.")
        except Exception:
            self.status_label.config(text="🔴 Cannot connect to Server!", fg="#e74c3c")
            self.log_label.config(text="Error: Please ensure .\\mini_postgres.exe is running!")

    def run_query(self):
        sql = self.sql_text.get("1.0", tk.END).strip().rstrip(';')
        if not sql:
            return

        if not self.sock:
            messagebox.showwarning("Warning", "Not connected to database server!")
            return

        for item in self.tree.get_children():
            self.tree.delete(item)

        try:
            sql_bytes = sql.encode('utf-8') + b'\x00'
            length = len(sql_bytes) + 4
            packet = b'Q' + struct.pack('>I', length) + sql_bytes
            self.sock.sendall(packet)

            row_count = 0
            while True:
                msg_type = self.sock.recv(1)
                if not msg_type:
                    break
                msg_type = msg_type.decode('latin1')
                len_bytes = self.sock.recv(4)
                msg_len = struct.unpack('>I', len_bytes)[0] - 4
                payload = self.sock.recv(msg_len) if msg_len > 0 else b''

                if msg_type == 'D':
                    cols = struct.unpack('>H', payload[:2])[0]
                    offset = 2
                    row_vals = []
                    for _ in range(cols):
                        col_len = struct.unpack('>I', payload[offset:offset+4])[0]
                        offset += 4
                        val = payload[offset:offset+col_len].decode('utf-8', errors='ignore')
                        offset += col_len
                        row_vals.append(val)
                    self.tree.insert("", tk.END, values=row_vals)
                    row_count += 1
                elif msg_type == 'C':
                    cmd_tag = payload.decode('latin1', errors='ignore').strip()
                    self.log_label.config(text=f"Success: {cmd_tag} ({row_count} row(s) returned)")
                elif msg_type == 'E':
                    err_msg = payload.decode('latin1', errors='ignore')
                    self.log_label.config(text=f"Server Error: {err_msg}")
                elif msg_type == 'Z':
                    break
        except Exception as e:
            self.status_label.config(text="🔴 Connection Lost!", fg="#e74c3c")
            self.log_label.config(text=f"Network Error: {e}")

if __name__ == "__main__":
    root = tk.Tk()
    app = MiniPostgresGUI(root)
    root.mainloop()

