A file browser similar to midnight commander. But it has extremely few dependencies.

CONTROLS:

SPACE - EXECUTE PROGRAM (WITH './')
ENTER - GO TO DIR / OPEN FILE (WITH 'nano')

ESC - QUIT

UP ARROW / DOWN ARROW - CHANGE SELECTOR LOCATION

Q - GET FULL METADATA FROM SELECTED FILE/FOLDER (USING 'stat 'f'')
W - SETTLE IN FOLDER
E - REMOVE FILE/FOLDER - REMOVE FILE (USING 'rm 'file'') / REMOVE FOLDER (USING 'rm -rf -v 'folder'') [PRESS E TWO TIMES FOR CONFIRMATION]

How to build Midnight Club

clang/llvm/gcc are all available compilers. Practically anything counts here. But if you are a beginner, You just do this on the terminal:
gcc main.c -o midnight-club
./midnight-club
Do this in /src
