import os
import platform
import ctypes
import pytest
import tempfile

# --------------------- Load the shared C library ---------------------
def load_library():
    libname = "folder_sorter.dll" if platform.system() == "Windows" else "folder_sorter.so"
    lib_path = os.path.join(os.path.dirname(__file__), "..", "shared", libname)
    assert os.path.exists(lib_path), f"Library not found at {lib_path}"
    lib = ctypes.CDLL(lib_path)
    lib.move_files.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    return lib

# --------------------- Test: C library loads successfully ---------------------
def test_load_library():
    lib = load_library()
    assert lib is not None

# --------------------- Test: Files are sorted into folders ---------------------
def test_move_files_behavior():
    lib = load_library()

    with tempfile.TemporaryDirectory() as test_dir:
        # Create files with various extensions
        for ext in ['.pdf', '.txt', '.jpg']:
            open(os.path.join(test_dir, f"test{ext}"), "w").close()

        lib.move_files(test_dir.encode(), test_dir.encode())

        # Expected folders
        expected = ['pdf', 'txt', 'jpg', 'others']
        created = os.listdir(test_dir)
        print("Created folders:", created)

        assert any(ext in d.lower() for d in created for ext in expected), "Expected folders not created"

# --------------------- Test: Non-existent directory is handled ---------------------
def test_nonexistent_directory():
    lib = load_library()
    fake_path = "/path/that/does/not/exist"

    try:
        lib.move_files(fake_path.encode(), fake_path.encode())
    except Exception as e:
        pytest.fail(f"Function crashed with nonexistent path: {e}")

# --------------------- Test: Empty folder results in no subfolders ---------------------
def test_empty_directory_behavior():
    lib = load_library()

    with tempfile.TemporaryDirectory() as test_dir:
        lib.move_files(test_dir.encode(), test_dir.encode())
        assert os.listdir(test_dir) == [], "Empty folder should remain empty"

# --------------------- Test: Files with no extensions go to 'Others' ---------------------
def test_no_extension_goes_to_others():
    lib = load_library()

    with tempfile.TemporaryDirectory() as test_dir:
        open(os.path.join(test_dir, "README"), "w").close()
        lib.move_files(test_dir.encode(), test_dir.encode())
        subdirs = os.listdir(test_dir)
        print("Subfolders found:", subdirs)
        assert "Others" in subdirs, "'Others' folder not created for file with no extension"


# --------------------- Test: Unkown extensions create new folders ---------------------
def test_unknown_extension_creates_extension_folder():
    lib = load_library()

    with tempfile.TemporaryDirectory() as test_dir:
        open(os.path.join(test_dir, "file.abcde"), "w").close()
        lib.move_files(test_dir.encode(), test_dir.encode())
        subdirs = os.listdir(test_dir)
        print("Subfolders found:", subdirs)
        assert "abcde" in subdirs, "Folder named after unknown extension was not created"


# --------------------- Test: Nested folders are not touched ---------------------
def test_nested_directories_ignored():
    lib = load_library()

    with tempfile.TemporaryDirectory() as test_dir:
        # File in top-level
        open(os.path.join(test_dir, "doc.txt"), "w").close()
        # File inside subdirectory
        nested_dir = os.path.join(test_dir, "nested")
        os.makedirs(nested_dir)
        open(os.path.join(nested_dir, "pic.jpg"), "w").close()

        lib.move_files(test_dir.encode(), test_dir.encode())

        # File inside nested dir should remain untouched
        assert os.path.exists(os.path.join(nested_dir, "pic.jpg"))
        # File from top-level should be moved
        assert not os.path.exists(os.path.join(test_dir, "doc.txt"))
