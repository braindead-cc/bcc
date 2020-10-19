use crate::program::*;
use crate::optimize::*;

pub struct Collapse;

impl Optimize for Collapse {
    fn optimize(p: &mut Program) {
        let mut ctr = 0;
        while ctr < p.cmds.len() {
            if p.cmds[ctr].kind.collapsible()
                && ctr + 1 < p.cmds.len()
                && p.cmds[(ctr + 1)].kind == p.cmds[ctr].kind {

                let mut repeated = 1;

                while (ctr + 1) < p.cmds.len()
                    && p.cmds[ctr + 1].kind == p.cmds[ctr].kind {
                        repeated += 1;
                        p.cmds[ctr].dead = true;
                        ctr += 1;
                }

                p.cmds[ctr].count = repeated;
            } else {
                ctr += 1;
            }
        }
    }
}
