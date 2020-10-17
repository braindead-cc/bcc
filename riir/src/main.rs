mod options;
mod program;
mod interpr;

mod optimize;
mod collapse;

use std::fs;
use crate::optimize::*;

pub const VERSION: &'static str = "0.1.0";

fn main() {
    let config = options::Options::new().parse().unwrap();
    let mut interp = interpr::Interpreter::new();

    for file in config.file {
        let bf = fs::read_to_string(file).unwrap();
        let mut prog = program::Program::from(bf);

        collapse::Collapse::optimize(&mut prog);

        interp.execute(&prog);
    }
}
