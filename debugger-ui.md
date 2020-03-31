# Debugger TUI

```
┌─ code ────────────────────────────────────────────────────────────────────────┐
│                                                                               │
│ +++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->,+[<]<-]>>.>---.+++++++..+++.>>.<-- │
│                                           ^                                   │
└───────────────────────────────────────────────────────────────────────────────┘

┌─ memory ──────────────────────────────────────────────────────────────────────┐
│                                                                               │
│  000 001 002 003 004 005 006 007 008 010 012 013 014 034 063 022 060 060 101  │
│                                      ^^^                                      │
└───────────────────────────────────────────────────────────────────────────────┘

┌─ I/O ─────────────────────────────────────────────────────────────────────────┐
│ Hello world!                                                                  │
│ Type in a number: _                                                           │
│                                                                               │
│                                                                               │
│                                                                               │
└───────────────────────────────────────────────────────────────────────────────┘

executing command #128 / 256                           789 command executed total
allocated 2048 bytes for tape                          1024 bytes used
```

## NOTES:

- box one: this is the area which shows the commands being executed.
  the caret on the line below the brainfsck code show the command currently
  being executed. the height is always 5 lines.
- box two: this shows the current values of the memory cells. if the terminal
  height is greater than 24 lines, this box is expanded vertically.
- box three: input area. cleared after each input.
- box four: output area.
