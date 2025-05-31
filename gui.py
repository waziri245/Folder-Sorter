from tkinter import *
from tkinter import messagebox, filedialog
import platform
import ctypes

if platform.system() == "Windows":
    lib = ctypes.CDLL("./folder_sorter.dll")
else:
    lib = ctypes.CDLL("./folder_sorter.so")

# Set argument types for the move_files function
lib.move_files.argtypes = [ctypes.c_char_p, ctypes.c_char_p]



root = Tk()
root.title("Folder-Sorter")
root.geometry("700x450")
root.resizable(False, False)
root.configure(bg="#305065")

def open_folder():
    path = filedialog.askdirectory()
    if not path:
        return
    else:
        lib.move_files(path.encode("utf-8"), path.encode("utf-8"))
        


btn = Button(root, text="Select Folder", compound=CENTER, width=60, font="arial 14 bold", command=open_folder)    
btn.place(x=50, y=120)

if __name__ == "__main__":
    root.mainloop()