#[derive(Copy, Clone, Debug, PartialEq)]
pub enum BFCommandKind {
    CellInc,
    CellDec,
    Write,
    Read,
    MemPtrLeft,
    MemPtrRight,

    LoopStart(usize),
    LoopEnd(usize),
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
        }
    }
}

#[derive(Copy, Clone, Debug)]
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

        let x: String = self.kind.into();

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
