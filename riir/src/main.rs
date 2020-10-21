mod options;
mod tty;

mod optimize;
mod collapse;
mod nil_loop;
mod scan_loop;
mod mmov_loop;

mod errors;
mod program;
mod interpr;

use std::fs;
use std::io::{ stdin, Read };

use crate::optimize::*;
use crate::options::*;
use crate::interpr::*;
use crate::collapse::*;
use crate::nil_loop::*;
use crate::scan_loop::*;
use crate::mmov_loop::*;
use crate::program::*;

pub const VERSION: &'static str = "0.1.0";

fn main() {
    let config = match Options::new().parse() {
        Ok(con) => con,
        Err(()) => std::process::exit(1),
    };

    let mut interp = Interpreter::new(config.eof_char);

    if config.file.len() == 0 {
        let mut buf = String::new();
        stdin().read_to_string(&mut buf).unwrap();
        execute(&config, &mut interp, buf);
    }

    for file in &config.file {
        execute(&config, &mut interp, fs::read_to_string(file).unwrap());
    }
}

fn execute(config: &Options, interp: &mut Interpreter, data: String) {
    let mut prog = match Program::parse(config.comment_char, &data) {
        Ok(pr) => pr,
        Err(e) => {
            for error in e {
                if tty::is_tty(tty::OutputStream::Stdout) {
                    error.pretty_print();
                } else {
                    println!("error: {}", error);
                }
            }

            std::process::exit(1);
        },
    };

    Collapse::optimize(&mut prog);
    NilLoops::optimize(&mut prog);
    ScanLoops::optimize(&mut prog);
    MultiplyMoveLoops::optimize(&mut prog);

    interp.execute(&prog);
}
