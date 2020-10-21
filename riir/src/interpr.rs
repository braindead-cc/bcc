use std::io::{ self, Read }; //, Write };
use crate::program::*;
use crate::options::*;

// lookup table to display some nice strings for control characters
// when executing the debug command (see: ascii(7))
// TODO: handle DEL character (0x7F), which won't go neatly into this
// table
const CHARACTER_STRINGS: [&'static str; 33] = [
    /*   0 */ "NUL",      /*  12 */ "'\\f'",    /*  24 */ "CAN",
    /*   1 */ "SOH",      /*  13 */ "'\\r'",    /*  25 */ "EM",
    /*   2 */ "STX",      /*  14 */ "SO",       /*  26 */ "SUB",
    /*   3 */ "ETX",      /*  15 */ "SI",       /*  27 */ "ESC",
    /*   4 */ "EOT",      /*  16 */ "DLE",      /*  28 */ "FS",
    /*   5 */ "ENQ",      /*  17 */ "DC1",      /*  29 */ "GS",
    /*   6 */ "ACK",      /*  18 */ "DC2",      /*  30 */ "RS",
    /*   7 */ "BEL",      /*  19 */ "DC3",      /*  31 */ "US",
    /*   8 */ "BS",       /*  20 */ "DC4",      /*  32 */ "SPACE",
    /*   9 */ "'\\t'",    /*  21 */ "NAK",
    /*  10 */ "'\\n'",    /*  22 */ "SYN",
    /*  11 */ "'\\v'",    /*  23 */ "ETB",
];

#[inline(always)]
fn check_memory_bounds(mem: &mut Vec<u8>, ptr: usize) {
    if ptr >= mem.len() {
        mem.extend(vec![0; mem.len() * 2]);
    }
}

#[inline(always)]
fn set_eof(mem: &mut Vec<u8>, ptr: usize, eof: EofChar) {
    mem[ptr] = match eof {
        EofChar::MinusOne => -1_i8 as u8,
        EofChar::Zero => 0,
        EofChar::NoChange => mem[ptr],
    };
}

pub struct Interpreter {
    memory: Vec<u8>,
    pointer: usize,

    eof_char: EofChar,
    debug_context: usize,
}

impl Interpreter {
    pub fn new(opts: &Options) -> Self {
        Self {
            memory: [0; 32768].to_vec(),
            pointer: 0,
            eof_char: opts.eof_char,
            debug_context: opts.debug_context,
        }
    }

    pub fn handle_debug(&mut self) {
        // TODO: fancier output

        let ctx_begin = self.pointer.saturating_sub(self.debug_context / 2);
        let ctx_end   = std::cmp::min(self.pointer + (self.debug_context / 2),
            self.memory.len() - 1);

        for cell in ctx_begin..ctx_end {
            let value = self.memory[cell];

            println!("{}{}{}\t\t{}\t\t{:x}\t\t{:o}\t\t{}",
                if cell == self.pointer { "[" } else { " " },
                cell,
                if cell == self.pointer { "]" } else { " " },
                value, value, value,
                if value > 32 {
                    format!("'{}'", value as char)
                } else {
                    CHARACTER_STRINGS[value as usize].to_owned()
                }
            );
        }
    }

    pub fn execute(&mut self, prog: &Program) {
        let mut ctr = 0;

        while ctr < prog.cmds.len() {
            let cur = &prog.cmds[ctr];

            // if a command is "dead", it has been optimized away
            // skip over it, but increment the counter so as to not mess up
            // the indexes stored by BFCommandKind::LoopStart/LoopEnd
            if cur.dead { ctr += 1; continue; }

            // check if we need to reallocate more memory
            check_memory_bounds(&mut self.memory, self.pointer);

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
                        set_eof(&mut self.memory, self.pointer, self.eof_char);
                        //self.memory[self.pointer] = 0;
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
                BFCommandKind::Debug => self.handle_debug(),
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

                        check_memory_bounds(&mut self.memory, target);
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
