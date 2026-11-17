# 🎵 MP3 Tag Reader & Editor in C

A command-line application developed using **C programming** to read and modify ID3v2.3.0 metadata in MP3 files.

The project allows users to view existing song information and update selected metadata fields through simple command-line operations.

## 🚀 Features

- **View Metadata:** Read and display MP3 metadata.
- **Edit Metadata:** Modify existing ID3 tags.
- **Six Supported Tags:** Title, Artist, Album, Year, Genre and Comment.
- **Binary File Handling:** Read and write MP3 metadata at the byte level.
- **Dynamic Memory Allocation:** Manage metadata payloads dynamically.
- **File Preservation:** Retain the original audio data while updating metadata.

## 🏷️ Supported ID3 Tags

| Metadata | ID3 Frame |
|---|---|
| Title | TIT2 |
| Artist | TPE1 |
| Album | TALB |
| Year | TYER |
| Genre | TCON |
| Comment | COMM |

## 🛠️ Technologies & Concepts

- Programming Language: C
- File Handling (`fopen`, `fread`, `fwrite`)
- Structures and Pointers
- Dynamic Memory Allocation
- Binary Data Processing
- ID3v2.3.0 Metadata Format
- Command-Line Arguments

## 📂 Project Structure

```text
MP3-Tag-Reader-Editor/
│
├── main.c
├── view.c
├── view.h
├── edit.c
├── edit.h
├── types.h
└── README.md
```

## ⚙️ Compilation

Compile the source files using GCC:

```bash
gcc main.c view.c edit.c -o mp3_tag_editor
```

## ▶️ Usage

**1. Display Help**

```bash
./mp3_tag_editor -h
```

**2. View MP3 Metadata**

```bash
./mp3_tag_editor -v sample.mp3
```

**3. Edit MP3 Metadata**

Use the following commands:

```bash
./mp3_tag_editor -e -t "New Title" sample.mp3
./mp3_tag_editor -e -a "New Artist" sample.mp3
./mp3_tag_editor -e -A "New Album" sample.mp3
./mp3_tag_editor -e -y 2025 sample.mp3
./mp3_tag_editor -e -g "New Genre" sample.mp3
./mp3_tag_editor -e -c "New Comment" sample.mp3
```

**Note:** The editing operation updates the selected MP3 file. Keep a backup before editing.

## 🎯 Learning Outcomes

Through this project, I gained practical experience in:

- Understanding binary file structures.
- Working with ID3 metadata frames.
- Performing file operations in C.
- Managing dynamically allocated memory.
- Reading and modifying structured data.

## 👨‍💻 Author

**Hari Yadav**

---

*Exploring C programming through practical file-handling and metadata-processing applications.*
