mod options;
mod program;
mod interpr;

use std::fs;

pub const VERSION: &'static str = "0.1.0";

fn main() {
    let config = options::Options::new().parse().unwrap();
    let mut interp = interpr::Interpreter::new();

    for file in config.file {
        let bf = fs::read_to_string(file).unwrap();
        let prog = program::Program::from(bf);
        interp.execute(&prog);
    }
}
