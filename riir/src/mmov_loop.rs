// multiply move loops

use crate::program::*;
use crate::optimize::*;
use std::collections::HashMap;

pub struct MultiplyMoveLoops;

impl MultiplyMoveLoops {
    // I had a lot of help from here:
    // https://github.com/Wilfred/bfc
    // ./src/peephole.rs
    fn is_multiply_loop(body: &[BFCommand]) -> bool {
        // first determine whether this is a multiply loop.
        // 1. body should only contain +, -, <, >
        // 2. body should have a total pointer movement of 0
        // 3. body should reduce value in cell #0

        for cmd in body {
            match cmd.kind {
                BFCommandKind::CellInc
                | BFCommandKind::CellDec
                | BFCommandKind::MemPtrRight
                | BFCommandKind::MemPtrLeft
                => (),
                _ => return false,
            }
        }

        let mut ptr_movement = 0_isize;
        for cmd in body {
            match cmd.kind {
                BFCommandKind::MemPtrRight => ptr_movement += cmd.count as isize,
                BFCommandKind::MemPtrLeft => ptr_movement -= cmd.count as isize,
                _ => (),
            }
        }

        if ptr_movement != 0 {
            return false;
        }

        let changes = MultiplyMoveLoops::cell_changes(body);

        if changes.get(&0).unwrap_or(&0) >= &0 || changes.len() < 2 {
            false
        } else {
            true
        }
    }

    fn cell_changes(cmds: &[BFCommand]) -> HashMap<isize, i8> {
        // check changes in cells
        let mut cells: HashMap<isize, i8> = HashMap::new();
        let mut ptr = 0_isize;

        for cmd in cmds {
            if cmd.dead {
                continue;
            }

            match cmd.kind {
                BFCommandKind::MemPtrRight => ptr += cmd.count as isize,
                BFCommandKind::MemPtrLeft  => ptr -= cmd.count as isize,
                BFCommandKind::CellInc =>
                { cells.insert(ptr, cells.get(&ptr).unwrap_or(&0) + cmd.count as i8); },
                BFCommandKind::CellDec =>
                { cells.insert(ptr, cells.get(&ptr).unwrap_or(&0) - cmd.count as i8); },
                _ => unreachable!(),
            }
        }

        cells
    }
}

impl Optimize for MultiplyMoveLoops {
    fn optimize(p: &mut Program) {
        let mut ctr = 0;
        while ctr < p.cmds.len() {
            if p.cmds[ctr].dead {
                ctr += 1;
                continue;
            }

            match p.cmds[ctr].kind {
                BFCommandKind::LoopStart(_) => {
                    let body = p.loop_items(ctr).unwrap();

                    if MultiplyMoveLoops::is_multiply_loop(&body) {
                        let mut changes = MultiplyMoveLoops::cell_changes(&body);

                        // we're not concerned about the current cell's value
                        // during the multply loop, as we already know it will
                        // be zero at the end
                        changes.remove(&0);

                        p.remove_cmd(ctr).unwrap();
                        p.replace_cmd(ctr, BFCommand {
                            dead: false,
                            kind: BFCommandKind::MultiplyMove(changes),
                            count: 1,
                            pos: p.cmds[ctr].pos,
                        }).unwrap();
                    }
                },
                _ => (),
            }

            ctr += 1;
        }
    }
}
