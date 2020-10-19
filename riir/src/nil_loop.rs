// "[-]" or "[+]" => BFCommandKind::Nullify

use crate::program::*;
use crate::optimize::*;

pub struct NilLoops;

impl Optimize for NilLoops {
    fn optimize(p: &mut Program) {
        let mut ctr = 0;
        while ctr < p.cmds.len() {
            match p.cmds[ctr].kind {
                BFCommandKind::LoopStart(_) => {
                    if p.loop_len(ctr).unwrap() == 1 {
                        let loop_first = &p.loop_items(ctr).unwrap()[0].kind;
                        if loop_first == &BFCommandKind::CellDec
                                    || loop_first == &BFCommandKind::CellInc {
                            p.replace_cmd(ctr, BFCommand {
                                dead: false,
                                kind: BFCommandKind::Nullify,
                                count: 1,
                                pos: p.cmds[ctr].pos,
                            }).unwrap();
                            p.remove_cmd(ctr + 1).unwrap();
                            p.remove_cmd(ctr + 2).unwrap();
                        }
                    }
                },
                _ => (),
            }

            ctr += 1;
        }
    }
}
