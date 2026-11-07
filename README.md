A file browser similar to midnight commander. But it has extremely few dependencies.

SPACE - EXECUTE PROGRAM
ENTER - GO TO DIR / OPEN FILE (WITH PROGRAM)

LEFT ARROW - Toggle ls -l quick reference to FALSE
RIGHT ARROW - Toggle ls -l quick reference to TRUE ( USING 'ls -l 'file'' )

ESC - QUIT

UP ARROW / DOWN ARROW - CHANGE SELECTOR LOCATION

Q - GET FULL METADATA FROM SELECTED FILE/FOLDER (USING 'stat 'f'')
W - SETTLE IN FOLDER (Standalone)
E - REMOVE FILE/FOLDER - REMOVE FILE (USING 'rm 'file'') / REMOVE FOLDER (USING 'rm -rf -v 'folder'') [PRESS E TWO TIMES FOR CONFIRMATION]
A - CREATE FILE (Standalone)
S - CREATE FOLDER (Standalone)
D - RENAME OBJECT (Standalone)

OBJECT = Filesystem Object, File, OR Directory.

How to build Midnight Club

clang/llvm/gcc are all available compilers. Practically anything counts here. But if you are a beginner, You just do this on the terminal:

gcc main.c -o midnight-club

./midnight-club

Do this in /src
