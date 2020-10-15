use getopts::Options as g_Options;
use std::env;

#[derive(Clone, Debug)]
pub struct Options {
    pub file: Vec<String>,
}

impl Options {
    // set default values of options
    pub fn new() -> Self {
        Self {
            file: Vec::new(),
        }
    }

    pub fn parse(mut self) -> Result<Self, ()> {
        let args: Vec<String> = env::args().collect();
        let argv0 = args[0].clone();

        let mut opts = g_Options::new();

        opts.optflag("h", "help",     "");
        opts.optflag("V", "version",  "");

        let matches = match opts.parse(&args[1..]) {
            Ok(ma) => ma,
            Err(e) => {
                println!("bcc: error: {}", e);
                return Err(());
            },
        };

        if matches.opt_present("h") {
            Options::usage(&argv0);
            return Err(());
        } else if matches.opt_present("V") {
            Options::version();
            return Err(());
        }

        self.file = if !matches.free.is_empty() {
            matches.free.clone()
        } else {
            self.file
        };

        Ok(self)
    }

    fn usage(argv0: &str) {
        println!("Usage: {} [OPTION]... [FILE]
Copyright (c) 2020 Kiëd Llaentenn

Options:
    -h, --help             Print a this help message and exit.
    -V, --version          Print pescli's version and exit.

Full documentation is available as a manpage (bcc(1)).
Source: https://github.com/braindead-cc/bcc
Reporting issues: https://github.com/braindead-cc/bcc/issues/new
", argv0);
    }

    fn version() {
        println!("bcc v{}", crate::VERSION);
    }
}
