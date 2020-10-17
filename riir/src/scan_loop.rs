// "[>]" or "[<]" => BFCommandKind::ScanRight/BFCommand::ScanLeft

use crate::program::*;
use crate::optimize::*;

pub struct ScanLoops;

impl Optimize for ScanLoops {
    fn optimize(p: &mut Program) {
        let mut ctr = 0;
        while ctr < p.cmds.len() {
            match p.cmds[ctr].kind {
                BFCommandKind::LoopStart(_) => {
                    if p.loop_len(ctr).unwrap() == 1 {
                        let loop_first = p.loop_items(ctr).unwrap()[0].kind;
                        if loop_first == BFCommandKind::MemPtrRight
                                    || loop_first == BFCommandKind::MemPtrLeft {
                            p.replace_cmd(ctr, BFCommand {
                                dead: false,
                                kind: if loop_first == BFCommandKind::MemPtrRight {
                                    BFCommandKind::ScanRight
                                } else {
                                    BFCommandKind::ScanLeft
                                },
                                count: 1,
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
