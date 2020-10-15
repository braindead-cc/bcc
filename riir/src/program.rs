#[derive(Copy, Clone, Debug)]
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

#[derive(Copy, Clone, Debug)]
pub struct BFCommand {
    pub kind: BFCommandKind,
    pub count: usize,
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

impl From<String> for Program {
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
                    let new = BFCommand { kind: kind, count: 1 };
                    prog.cmds.push(new);

                    // push index of new BF command
                    unmatched.push(prog.cmds.len() - 1);
                },

                BFCommandKind::LoopEnd(_) => {
                    let last = unmatched.pop().unwrap();
                    let new = BFCommand {
                        kind: BFCommandKind::LoopEnd(last),
                        count: 1
                    };
                    prog.cmds.push(new);

                    // set the matching bracket for the opening bracket
                    prog.cmds[last].kind
                        = BFCommandKind::LoopStart(prog.cmds.len() - 1);
                },

                _ => prog.cmds.push(BFCommand {
                    kind: kind, count: 1,
                }),

            }

            ctr += 1;
        }

        prog
    }
}
