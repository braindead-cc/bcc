mod options;
mod program;
mod interpr;

mod optimize;
mod collapse;
mod nil_loop;
mod scan_loop;
mod mmov_loop;

use std::fs;
use std::io::{ stdin, Read };
use crate::optimize::*;

pub const VERSION: &'static str = "0.1.0";

fn main() {
    let config = match options::Options::new().parse() {
        Ok(con) => con,
        Err(()) => std::process::exit(1),
    };

    let mut interp = interpr::Interpreter::new(config.eof_char);

    if config.file.len() == 0 {
        let mut buf = String::new();
        stdin().read_to_string(&mut buf).unwrap();
        execute(&mut interp, buf);
    }

    for file in config.file {
        execute(&mut interp, fs::read_to_string(file).unwrap());
    }
}

fn execute(interp: &mut interpr::Interpreter, data: String) {
    let mut prog = program::Program::from(data);

    collapse::Collapse::optimize(&mut prog);
    nil_loop::NilLoops::optimize(&mut prog);
    scan_loop::ScanLoops::optimize(&mut prog);
    mmov_loop::MultiplyMoveLoops::optimize(&mut prog);
    //println!("prog: {}", prog.to_string());

    interp.execute(&prog);
}
