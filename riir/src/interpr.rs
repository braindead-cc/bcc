use std::io::{ self, Read, Write };
use crate::program::*;

pub struct Interpreter {
    memory: Vec<u8>,
    pointer: usize,
}

impl Interpreter {
    pub fn new() -> Self {
        let i = Interpreter {
            memory: [0; 30_000].to_vec(),
            pointer: 0,
        };

        i
    }

    pub fn execute(&mut self, prog: &Program) {
        let mut ctr = 0;

        while ctr < prog.cmds.len() {
            let cur = prog.cmds[ctr];

            match cur.kind {
                BFCommandKind::CellInc =>
                    self.memory[self.pointer] = self.memory[self.pointer]
                        .wrapping_add(cur.count as u8),
                BFCommandKind::CellDec =>
                    self.memory[self.pointer] = self.memory[self.pointer]
                        .wrapping_sub(cur.count as u8),
                BFCommandKind::Write => {
                    print!("{}", self.memory[self.pointer] as char);
                    io::stdout().flush().unwrap();
                },
                BFCommandKind::Read => {
                    let chomp = io::stdin().bytes().next();
                    if !chomp.is_some() {
                        // set to EOF
                        self.memory[self.pointer] = 0;
                    } else {
                        self.memory[self.pointer] = chomp.unwrap().unwrap();
                    }
                },
                BFCommandKind::MemPtrLeft =>
                    self.pointer = self.pointer.saturating_sub(cur.count),
                BFCommandKind::MemPtrRight =>
                    self.pointer = self.pointer.saturating_add(cur.count),
                BFCommandKind::LoopStart(e) => {
                    if self.memory[self.pointer] == 0 {
                        // nothing to do in the loop, so jump
                        // to the end
                        ctr = e;
                    }
                },
                BFCommandKind::LoopEnd(s) => {
                    if self.memory[self.pointer] != 0 {
                        // execute the loop again, so jump
                        // back the the loop's start
                        ctr = s;
                    }
                },

                #[allow(unreachable_patterns)]
                _ => unreachable!(),
            }

            ctr += 1;
        }
    }
}
