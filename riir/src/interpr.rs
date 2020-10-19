use std::io::{ self, Read }; //, Write };
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
            let cur = &prog.cmds[ctr];

            // if a command is "dead", it has been optimized away
            // skip over it, but increment the counter so as to not mess up
            // the indexes stored by BFCommandKind::LoopStart/LoopEnd
            if cur.dead { ctr += 1; continue; }

            match &cur.kind {
                BFCommandKind::CellInc =>{
                    self.memory[self.pointer] = self.memory[self.pointer]
                        .wrapping_add(cur.count as u8);},
                BFCommandKind::CellDec =>
                    self.memory[self.pointer] = self.memory[self.pointer]
                        .wrapping_sub(cur.count as u8),
                BFCommandKind::Write => {
                    print!("{}", self.memory[self.pointer] as char);
                    //io::stdout().flush().unwrap();
                },
                BFCommandKind::Read => {
                    if let Some(c) = io::stdin().bytes().next() {
                        self.memory[self.pointer] = c.unwrap();
                    } else {
                        self.memory[self.pointer] = 0;
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
                        ctr = *e;
                    }
                },
                BFCommandKind::LoopEnd(s) => {
                    if self.memory[self.pointer] != 0 {
                        // execute the loop again, so jump
                        // back the the loop's start
                        ctr = *s;
                    }
                },
                BFCommandKind::Nullify => self.memory[self.pointer] = 0_u8,
                BFCommandKind::ScanRight => {
                    if let Some(z) = memchr::memrchr(0x0, &self.memory) {
                        self.pointer = z;
                    } else {
                        // if no cell with zero has been found
                        // do what the normal "[>]" combination would do:
                        // move to the right of the tape
                        self.pointer = self.memory.len() - 1;
                    }
                },
                BFCommandKind::ScanLeft => {
                    if let Some(z) = memchr::memchr(0x0, &self.memory) {
                        self.pointer = z;
                    } else {
                        // if no cell with zero has been found
                        // do what the normal "[<]" combination would do:
                        // move to the beginning of the tape
                        self.pointer = 0;
                    }
                },
                BFCommandKind::MultiplyMove(changes) => {
                    // get the current cell's value
                    let curval = self.memory[self.pointer];

                    for change in changes {
                        // multiply first, then add
                        let product = ((curval as i8).wrapping_mul(*change.1)) as u8;

                        // get the target cell, accounting for undeflow
                        let target = if ((self.pointer as isize) + change.0) < 0 {
                            0
                        } else {
                            ((self.pointer as isize) + change.0) as usize
                        };

                        self.memory[target] = self.memory[target].wrapping_add(product);
                    }

                    // set the current cell to zero, since that's what
                    // a multiply-move loop would do
                    self.memory[self.pointer] = 0;
                },

                #[allow(unreachable_patterns)]
                _ => unimplemented!(),
            }

            ctr += 1;
        }
    }
}
