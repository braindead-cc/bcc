use std::collections::*;

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
            BFCommandKind::Nullify => "0".to_string(),
            BFCommandKind::ScanRight => "»".to_string(),
            BFCommandKind::ScanLeft => "«".to_string(),
            BFCommandKind::MultiplyMove(c) => format!("mmov({:?})", c),
        }
    }
}

#[derive(Clone, Debug)]
pub struct BFCommand {
    pub dead: bool,
    pub kind: BFCommandKind,
    pub count: usize,
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
        if idx >= self.cmds.len() {
            Err(format!("idx out of range (idx is {}, but len is {})",
                idx, self.cmds.len()))?
        }

        let cmd = &self.cmds[idx];

        // FIXME: should we really return Err if the cmd is dead?
        // I'm not sure if there are cases where we'd want to check
        // the length of a dead loop
        if cmd.dead {
            Err(format!("Command at position {} is dead", idx))?
        }

        if let BFCommandKind::LoopStart(endpos) = cmd.kind {
            debug_assert!(endpos < self.cmds.len());
            Ok(endpos - idx - 1)
        } else if let BFCommandKind::LoopEnd(startpos) = cmd.kind {
            debug_assert!(startpos < self.cmds.len());
            Ok(idx - startpos - 1)
        } else {
            Err(format!("Expected command LoopStart or LoopEnd, found {:?}",
                    cmd.kind))
        }
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
            Ok(self.cmds[(idx + 1)..endpos].to_vec())
        } else if let BFCommandKind::LoopEnd(startpos) = cmd.kind {
            assert!(startpos < self.cmds.len());
            Ok(self.cmds[startpos..idx].to_vec())
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

impl From<String> for Program {
    // TODO: return result;
    fn from(stuff: String) -> Self {
        let mut prog = Program::new();

        // stack of unmatched brackets. accessed
        // by its index in Program::cmds
        let mut unmatched: Vec<usize> = Vec::new();

        let mut ctr = 0;
        let chs = stuff.chars().collect::<Vec<char>>();

        while ctr < chs.len() {
            let kind = match chs[ctr] {
                '+' => BFCommandKind::CellInc,
                '-' => BFCommandKind::CellDec,
                '.' => BFCommandKind::Write,
                ',' => BFCommandKind::Read,
                '<' => BFCommandKind::MemPtrLeft,
                '>' => BFCommandKind::MemPtrRight,

                '[' => BFCommandKind::LoopStart(0),
                ']' => BFCommandKind::LoopEnd(0),

                // ignore other characters
                _ => { ctr += 1; continue; },
            };

            // match loops
            match kind {
                BFCommandKind::LoopStart(_) => {
                    let new = BFCommand {
                        dead: false, kind: kind, count: 1
                    };
                    prog.cmds.push(new);

                    // push index of new BF command
                    unmatched.push(prog.cmds.len() - 1);
                },

                BFCommandKind::LoopEnd(_) => {
                    let last = unmatched.pop().unwrap();
                    let new = BFCommand {
                        dead: false,
                        kind: BFCommandKind::LoopEnd(last),
                        count: 1
                    };
                    prog.cmds.push(new);

                    // set the matching bracket for the opening bracket
                    prog.cmds[last].kind
                        = BFCommandKind::LoopStart(prog.cmds.len() - 1);
                },

                _ => prog.cmds.push(BFCommand {
                    dead: false, kind: kind, count: 1,
                }),

            }

            ctr += 1;
        }

        prog
    }
}
