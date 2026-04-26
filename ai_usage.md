For this task I used google Gemini, I explained it the project requirements, and then the AI assisted part of the project. It then generated me the parse_condition function, a function that splits  a string in 3 strings, using strtok, so it can later compare all fields with the match_condition function with all reports that are members of the Report structure. 

The prompt I used was:
I am a student working on a file system UNIX project in the C language. I have a binary structure named Report . I need two functions generated , one named parse_condition, that splits a string with the structure "field:operator:value" into its three parts, and one named match_condition that compares members of the Report structure  with the extracted values.

What the AI did:
After I explained my project structure and requirements, the AI generated the code for two main functions:

parse_condition: This function takes a string like severity:>:1 and splits it into three parts using strtok so the program knows exactly what to look for (the field, the operator, and the value) and match_condition, a  function that compares the data inside my binary Report structure with the criteria the user asked for.

Integration and my own corrections:
I put these two functions into my security_ai.c file and then called them from file_ops.c right where the program reads the binary data from the disk.Even though the AI gave me a good starting point, I had to make several important manual corrections:

Security fix: The AI originally used strcpy, which was risky because it can cause a buffer overflow. I manually replaced it with strncpy so I could strictly control how many characters are copied, making the program more stable.

Missing headers: The AI forgot to include <stdlib.h>, so I added it myself to make sure the atoi function (which converts text to integers) works correctly.
