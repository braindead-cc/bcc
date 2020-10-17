use crate::program::*;

pub trait Optimize {
    fn optimize(p: &mut Program);
}
