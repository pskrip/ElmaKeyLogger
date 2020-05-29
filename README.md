# ElmaKeyLogger
Keylogger specifically for Elastomania game
This tool logs control keys pressed during play
Key mapping is set in "keys.ini" file using keyboard scan codes
File "Keyboard scan codes.png" shows scan codes for every key
Multiple keys can be maped to one control
Log is recorded to "keylog.txt" file
Each line contain sequence of events starting by Enter control and ending by Escape or next Enter
Line can be marked with a number by typing digits during sequence or even after escape before next Enter
This program does not log other key pressing events (except maped controls and all digits)
For safety reason avoid typing passwords or any private information when program is running
