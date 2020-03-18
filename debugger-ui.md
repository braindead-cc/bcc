# Debugger TUI

```
┌─ code ────────────────────────────────────────────────────────────────────────┐
│                                                                               │
│ +++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-- │
│                                           ^                                   │
└───────────────────────────────────────────────────────────────────────────────┘

┌─ memory ──────────────────────────────────────────────────────────────────────┐
│                                                                               │
│  255 | 255 | 255 | 255 | 255 | 255 | 255 |   0 | 255 |  24 | 255 | 255 | 255  │
│    1 |   2 |  13 |  24 |  35 |  46 |  57 | 255 | 255 | 255 | 255 | 255 | 255  │
│  255 | 255 | 255 | 255 | 255 | 255 | 255 |   0 | 255 |  24 | 255 | 255 | 255  │
│                                                                               │
└───────────────────────────────────────────────────────────────────────────────┘

┌─ input ───────────────────────────────────────────────────────────────────────┐
│ >_                                                                            │
└───────────────────────────────────────────────────────────────────────────────┘

┌─ output ──────────────────────────────────────────────────────────────────────┐
│Hello world!                                                                   │
│Type in a number:                                                              │
│                                                                               │
└───────────────────────────────────────────────────────────────────────────────┘

executing command #128 / 256                           789 command executed total
```

## NOTES:

- box one: this is the area which shows the commands being executed.
  the caret on the line below the brainfsck code show the command currently
  being executed. the height is always 5 lines.
- box two: this shows the current values of the memory cells. if the terminal
  height is greater than 24 lines, this box is expanded vertically.
- box three: input area. cleared after each input.
- box four: output area.
