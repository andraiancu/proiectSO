For this task I used google Gemini, I explained it the project requirements, and then the AI assisted part of the project. It then generated me the parse_condition function, a function that splits  a string in 3 strings, using strtok, so it can later compare all fields with the match_condition function with all reports that are members of the Report structure. 

The prompt I used was:
I am a student working on a file system UNIX project in the C language. I have a binary structure named Report . I need two functions generated , one named parse_condition, that splits a string with the structure "field:operator:value" into its three parts, and one named match_condition that compares members of the Report structure  with the extracted values.

I integrated the two functions in the file securityai.c , then myself I called them from file_ops.c. 

The AI first used strcpy and was risking a buffer overflow, and I replaced it with strncpy, so I can control how many characters can be copied;

