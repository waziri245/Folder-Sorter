import ctypes
import platform
import threading
import time
from tkinter import *
from tkinter import messagebox, filedialog
import os

# ------------------------ Load C library ------------------------
import os

current_dir = os.path.dirname(os.path.abspath(__file__))
if platform.system() == "Windows":
    lib_path = os.path.join(current_dir, "..", "shared", "folder_sorter.dll")
elif platform.system() == "Linux":
    lib_path = os.path.join(current_dir, "..", "shared", "folder_sorter.so")
else:
    messagebox.showerror("Unsupported OS", "This tool only supports Windows and Linux.")
    raise SystemExit

lib = ctypes.CDLL(lib_path)
lib.move_files.argtypes = [ctypes.c_char_p, ctypes.c_char_p]


# ------------------------ Initialize GUI ------------------------
root = Tk()
root.title("Folder Sorter")
root.geometry("700x450")
root.resizable(False, False)
root.configure(bg="#243447")


# ------------------------ Styles ------------------------
TITLE_FONT = ("Segoe UI", 24, "bold")
BUTTON_FONT = ("Segoe UI", 14, "bold")
FOOTER_FONT = ("Segoe UI", 10)
BUTTON_COLOR = "#1DB954"
BUTTON_HOVER = "#1AA34A"

# ------------------------ UI Elements ------------------------
title = Label(root, text="📁 Folder Sorter", bg="#243447", fg="white", font=TITLE_FONT)
title.pack(pady=(50, 10))

subtitle = Label(root, text="Sort your messy folders with one click!", bg="#243447", fg="#AAAAAA", font=("Segoe UI", 12))
subtitle.pack(pady=(0, 30))

def open_folder():
    path = filedialog.askdirectory()
    if not path:
        messagebox.showwarning("No folder selected", "Please select a folder to sort.")
        return

    def sort_thread():
        try:
            time.sleep(1.2)  # Fake delay for UX
            lib.move_files(path.encode("utf-8"), path.encode("utf-8"))
            messagebox.showinfo("Success", "Files sorted successfully!")
        except Exception as e:
            messagebox.showerror("Error", f"An error occurred:\n{e}")

    threading.Thread(target=sort_thread, daemon=True).start()

btn = Button(root, text="Select Folder", font=BUTTON_FONT, bg=BUTTON_COLOR, fg="white", activebackground=BUTTON_HOVER,
             activeforeground="white", width=25, height=2, bd=0, relief=FLAT, command=open_folder, cursor="hand2")
btn.pack(pady=10)

def on_enter(e): btn.config(bg=BUTTON_HOVER)
def on_leave(e): btn.config(bg=BUTTON_COLOR)
btn.bind("<Enter>", on_enter)
btn.bind("<Leave>", on_leave)

footer = Label(root, text="Created by Atal Abdullah Waziri\n© 2025 Folder-Sorter | Version 1.0", bg="#243447", fg="#666666", font=FOOTER_FONT)
footer.pack(side=BOTTOM, pady=20)

# ------------------------ Fade-in Animation ------------------------
def fade_in(alpha=0):
    if alpha < 1.0:
        root.attributes("-alpha", alpha)
        root.after(20, fade_in, alpha + 0.05)
    else:
        root.attributes("-alpha", 1.0)

root.attributes("-alpha", 0.0)
fade_in()

# ------------------------ Run the App ------------------------
if __name__ == "__main__":
    root.mainloop()
