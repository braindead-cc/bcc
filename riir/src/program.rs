use std::collections::*;
use crate::errors::*;

// TODO: remove LoopStart(usize)/LoopEnd(usize), replace with
// Loop(Vec<BFCommand>)

#[derive(Clone, Debug, PartialEq)]
pub enum BFCommandKind {
    CellInc,
    CellDec,
    Write,
    Read,
    MemPtrLeft,
    MemPtrRight,

    LoopStart(usize),
    LoopEnd(usize),

    Debug,

    Nullify,
    ScanRight,
    ScanLeft,

    MultiplyMove(HashMap<isize, i8>),
}

impl BFCommandKind {
    pub fn collapsible(&self) -> bool {
        match self {
            BFCommandKind::CellInc
            | BFCommandKind::CellDec
            | BFCommandKind::MemPtrLeft
            | BFCommandKind::MemPtrRight => true,
            _ => false
        }
    }
}

impl Into<String> for BFCommandKind {
    fn into(self) -> String {
        match self {
            BFCommandKind::CellInc => "+".to_string(),
            BFCommandKind::CellDec => "-".to_string(),
            BFCommandKind::Write => ".".to_string(),
            BFCommandKind::Read => ",".to_string(),
            BFCommandKind::MemPtrLeft => "<".to_string(),
            BFCommandKind::MemPtrRight => ">".to_string(),
            BFCommandKind::LoopStart(_) => "[".to_string(),
            BFCommandKind::LoopEnd(_) => "]".to_string(),
            BFCommandKind::Debug => "#".to_string(),
            BFCommandKind::Nullify => "0".to_string(),
            BFCommandKind::ScanRight => "»".to_string(),
            BFCommandKind::ScanLeft => "«".to_string(),
            BFCommandKind::MultiplyMove(c) => format!("mmov({:?})", c),
        }
    }
}

#[derive(Clone, Debug, PartialEq)]
pub struct BFCommand {
    pub dead: bool,
    pub kind: BFCommandKind,
    pub count: usize,
    pub pos: (usize, usize),
}

impl ToString for BFCommand {
    fn to_string(&self) -> String {
        if self.dead {
            return String::new();
        }

        let x: String = self.kind.clone().into();

        if self.count == 1 {
            x
        } else {
            let mut buf = String::new();
            for _ in 0..self.count {
                buf += &format!("{}", x);
            }
            buf
        }
    }
}

pub struct Program {
    pub cmds: Vec<BFCommand>,
}

impl Program {
    pub fn new() -> Self {
        Self {
            cmds: Vec::new(),
        }
    }

    /// Get number of commands inside the loop at position idx.
    /// NOTE: idx must point to either a LoopStart or a LoopEnd.
    /// NOTE: returned length does not count the opening and
    /// closing brackets.
    pub fn loop_len(&self, idx: usize) -> Result<usize, String> {
        Ok(self.loop_items(idx)?.len())
    }

    /// Get a list of items in a loop at position idx.
    pub fn loop_items(&self, idx: usize)
        -> Result<Vec<BFCommand>, String>
    {
        if idx >= self.cmds.len() {
            Err(format!("idx out of range (idx is {}, but len is {})",
                idx, self.cmds.len()))?
        }

        let cmd = &self.cmds[idx];

        // FIXME: should we really return Err if the cmd is dead?
        if cmd.dead {
            Err(format!("Command at position {} is dead", idx))?
        }

        if let BFCommandKind::LoopStart(endpos) = cmd.kind {
            assert!(endpos < self.cmds.len());
            Ok(self.cmds[(idx + 1)..endpos].to_vec()
                .iter().filter(|i| !i.dead)
                .map(|i| i.clone()).collect::<Vec<_>>())
        } else if let BFCommandKind::LoopEnd(startpos) = cmd.kind {
            assert!(startpos < self.cmds.len());
            Ok(self.cmds[startpos..idx].to_vec()
                .iter().filter(|i| !i.dead)
                .map(|i| i.clone()).collect::<Vec<_>>())
        } else {
            Err(format!("Expected command LoopStart or LoopEnd, found {:?}",
                    cmd.kind))
        }
    }

    /// Replace the command at position <idx> with <new>.
    pub fn replace_cmd(&mut self, idx: usize, new: BFCommand)
        -> Result<(), String>
    {
        if idx >= self.cmds.len() {
            Err(format!("idx out of range (idx is {}, but len is {})",
                idx, self.cmds.len()))?
        }

        self.cmds[idx] = new;
        Ok(())
    }

    /// Make command at <idx> dead.
    pub fn remove_cmd(&mut self, idx: usize) -> Result<(), String> {
        if idx >= self.cmds.len() {
            Err(format!("idx out of range (idx is {}, but len is {})",
                idx, self.cmds.len()))?
        }

        // FIXME: should we really return Err if the cmd is dead?
        if self.cmds[idx].dead {
            Err(format!("Command at position {} is already dead", idx))?
        }

        if let BFCommandKind::LoopStart(end) = self.cmds[idx].kind {
            for i in idx..=end {
                self.cmds[i].dead = true;
            }
        }

        self.cmds[idx].dead = true;
        Ok(())
    }

    pub fn parse(exten_debug: bool, comment_char: Option<char>, stuff: &str)
        -> Result<Self, Vec<Error>>
    {
        let mut prog = Program::new();

        // stack of unmatched brackets. accessed
        // by its index in Program::cmds
        let mut unmatched: Vec<usize> = Vec::new();

        let mut line = 1;
        let mut column = 0;

        let mut ctr = 0;
        let chs = stuff.chars().collect::<Vec<char>>();

        while ctr < chs.len() {
            // track column, line
            if chs[ctr] == '\n' {
                line += 1;
                column = 0;

                ctr += 1;
                continue;
            } else {
                column += 1;
            }

            let kind = match chs[ctr] {
                '+' => BFCommandKind::CellInc,
                '-' => BFCommandKind::CellDec,
                '.' => BFCommandKind::Write,
                ',' => BFCommandKind::Read,
                '<' => BFCommandKind::MemPtrLeft,
                '>' => BFCommandKind::MemPtrRight,

                '[' => BFCommandKind::LoopStart(0),
                ']' => BFCommandKind::LoopEnd(0),

                '#' if exten_debug => BFCommandKind::Debug,

                // ignore other characters
                _ => {
                    if let Some(c) = comment_char {
                        if chs[ctr] == c {
                            while chs[ctr] != '\n' {
                                ctr += 1;
                            }

                            continue;
                        }
                    }

                    ctr += 1;
                    continue;
                },
            };

            // match loops
            match kind {
                BFCommandKind::LoopStart(_) => {
                    let new = BFCommand {
                        dead: false, kind: kind, count: 1,
                        pos: (line, column),
                    };
                    prog.cmds.push(new);

                    // push index of new BF command
                    unmatched.push(prog.cmds.len() - 1);
                },

                BFCommandKind::LoopEnd(_) => {
                    if let Some(last) = unmatched.pop() {
                        let new = BFCommand {
                            dead: false,
                            kind: BFCommandKind::LoopEnd(last),
                            count: 1,
                            pos: (line, column),
                        };
                        prog.cmds.push(new);

                        // set the matching bracket for the opening bracket
                        prog.cmds[last].kind
                            = BFCommandKind::LoopStart(prog.cmds.len() - 1);
                    } else {
                        return Err(vec![
                            Error::new(ErrorKind::UnmatchedCloseBrace,
                                BFCommand {
                                    dead: false, kind: BFCommandKind::LoopEnd(0),
                                    count: 1, pos: (line, column),
                                }, stuff
                            )
                        ]);
                    }
                },

                _ => prog.cmds.push(BFCommand {
                    dead: false, kind: kind, count: 1,
                    pos: (line, column),
                }),
            }

            ctr += 1;
        }

        // check if there are any unmatched opening braces, and if so,
        // whine and cry even though we could easily ignore it
        if unmatched.len() > 0 {
            return Err(vec![
                Error::new(ErrorKind::UnmatchedOpenBrace,
                    prog.cmds[unmatched.pop().unwrap()].clone(),
                    stuff
                )
            ]);
        }

        Ok(prog)
    }
}

impl ToString for Program {
    fn to_string(&self) -> String {
        let mut buf = String::new();
        for c in &self.cmds {
            buf += &c.to_string()
        }
        buf
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use super::BFCommandKind::*;

    #[test]
    fn test_parse() {
        let samples = vec![
            ("++[", vec![]),
            ("]", vec![]),
            ("[]", vec![
                LoopStart(1),
                LoopEnd(0)
            ]),
            ("+-[-]+ This is a comment [-<+> Comment ]", vec![
                CellInc, CellDec, LoopStart(4), CellDec, LoopEnd(2),
                LoopStart(6), CellDec, MemPtrLeft, CellInc,
                MemPtrRight, LoopEnd(11),
            ]),
        ];

        for sample in samples {
            let p = Program::parse(None, sample.0).unwrap();
            assert_eq!(sample.1, p.cmds.iter()
                .map(|i| i.kind.clone()).collect::<Vec<_>>());
        }
    }

    #[test]
    fn test_loop_len() {
        fn assert_ok(bf: &str, shouldbe: usize, idx: usize) {
            assert!(shouldbe == Program::parse(None, bf).unwrap().loop_len(idx)
                .unwrap());
        }

        fn assert_err(bf: &str, idx: usize) {
            assert!(Program::parse(None, bf).unwrap().loop_len(idx).is_err());
        }

        assert_ok("[]    ", 0, 0); assert_ok("[-]   ", 1, 0);
        assert_ok("[+-]  ", 2, 0); assert_ok("[->+<]", 4, 0);

        assert_ok(">>+[-]", 1, 3); assert_ok("[[]]->", 0, 1);
        assert_ok("[->][]", 0, 4); assert_ok("[[]]->", 2, 0);

        assert_err(">++>>.[<]", 0);
    }

    #[test]
    fn test_loop_items() {
        let res = Program::parse(None, "[-]").unwrap().loop_items(0);
        assert!(res.unwrap() == vec![BFCommand {
            dead: false, kind: BFCommandKind::CellDec,
            count: 1, pos: (1, 2),
        }]);
    }

    #[test]
    fn test_replace_cmd() {
        let mut prog = Program::parse(None, "[-]").unwrap();
        let replacement = BFCommand {
            dead: false, kind: BFCommandKind::CellInc,
            count: 1, pos: prog.cmds[1].pos,
        };

        prog.replace_cmd(1, replacement.clone()).unwrap();
        assert!(prog.loop_items(0) == Ok(vec![replacement]));
    }

    #[test]
    fn test_remove_cmd() {
        let mut prog = Program::parse(None, "[-]").unwrap();
        prog.remove_cmd(1).unwrap();
        assert!(prog.loop_len(0) == Ok(0));
    }
}
