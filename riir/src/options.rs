use getopts::Options as g_Options;
use std::env;

#[derive(Copy, Clone, Debug)]
pub enum EofChar {
    MinusOne,
    Zero,
    NoChange,
}

#[derive(Clone, Debug)]
pub struct Options {
    pub file: Vec<String>,
    pub eof_char: EofChar,
}

impl Options {
    // set default values of options
    pub fn new() -> Self {
        Self {
            file: Vec::new(),
            eof_char: EofChar::Zero,
        }
    }

    pub fn parse(mut self) -> Result<Self, ()> {
        let args: Vec<String> = env::args().collect();
        let argv0 = args[0].clone();

        let mut opts = g_Options::new();

        // NOTE: empty hints/descriptions are given, because
        // we create our own help message instead of relying on
        // getopts' poorly-formatted help string.

        opts.optflag("h", "help",     "");
        opts.optflag("V", "version",  "");

        // options, similar to gcc's -f argument
        opts.optmulti("f", "option", "", "");

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

        for argument in matches.opt_strs("option") {
            // options take the form -fOPTION[=VALUE]
            let parsed = argument.split("=").collect::<Vec<_>>();

            let option = parsed[0];
            let value = if parsed.len() > 1 {
                parsed[1]
            } else {
                ""
            };

            match option {
                "eof" => {
                    self.eof_char = match value {
                        "-1" => EofChar::MinusOne,
                        "zero" | "0" => EofChar::Zero,
                        "none" => EofChar::NoChange,
                        _ => {
                            eprintln!("error: invalid value for -feof: {:?}", value);
                            eprintln!("hint: valid values for -feof are '-1', '0', or 'none'.");
                            return Err(());
                        }
                    };
                },
                _ => {
                    eprintln!("error: invalid option: {}", option);
                    return Err(());
                },
            }
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
