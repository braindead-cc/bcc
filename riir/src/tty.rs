// NOTE: items are commented out to avoid cluttering my terminal
// with "vArIaNt Is NeVeR cOnStRuCtEd" warnings.
//
// TODO: move to separate crate

use std::fmt;
use std::os::raw::c_int;
use std::result::Result;

#[derive(Copy, Clone, Debug)]
pub enum TermStyle {
    Bold,
    //Underline,
    //Italic,
    //Inverted,
    //Blink,
    //Strike,
    Reset,

    //Bg(TermColor),
    //BrightBg(TermColor),
    Fg(TermColor),
    //BrightFg(TermColor),
}

impl fmt::Display for TermStyle {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> Result<(), fmt::Error> {
        let r = match self {
            TermStyle::Bold => String::from("\x1b[1m"),
            //TermStyle::Underline => String::from("\x1b[4m"),
            //TermStyle::Italic => String::from("\x1b[3m"),
            //TermStyle::Inverted => String::from("\x1b[7m"),
            //TermStyle::Blink => String::from("\x1b[5m"),
            //TermStyle::Strike => String::from("\x1b[9m"),
            TermStyle::Reset => String::from("\x1b[m"),

            //TermStyle::Bg(c) => format!("\x1b[4{}m", c),
            //TermStyle::BrightBg(c) => format!("\x1b[10{}m", c),
            TermStyle::Fg(c) => format!("\x1b[3{}m", c),
            //TermStyle::BrightFg(c) => format!("\x1b[9{}m", c),
        };

        write!(f, "{}", r)
    }
}

#[derive(Copy, Clone, Debug)]
pub enum TermColor {
    //Black,
    Red,
    //Green,
    //Yellow,
    Blue,
    //Magenta,
    //Cyan,
    //White,
}

impl fmt::Display for TermColor {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> Result<(), fmt::Error> {
        let r = match self {
            //TermColor::Black        => "0",
            TermColor::Red          => "1",
            //TermColor::Green        => "2",
            //TermColor::Yellow       => "3",
            TermColor::Blue         => "4",
            //TermColor::Magenta      => "5",
            //TermColor::Cyan         => "6",
            //TermColor::White        => "7",
        };

        write!(f, "{}", r)
    }
}

pub enum OutputStream {
    // listen, rustc, I understand they're not being
    // used right now, but you really don't need to raise
    // a fuss about that now

    Stdout,
    //Stderr,

    //Stdin,
}

impl Into<c_int> for OutputStream {
    fn into(self) -> c_int {
        match self {
            OutputStream::Stdout   => 1 as c_int,
            //OutputStream::Stderr   => 2 as c_int,
            //OutputStream::Stdin    => 0 as c_int,
        }
    }
}

extern "C" {
    pub fn isatty(fd: c_int) -> c_int;
}

pub fn is_tty(fd: OutputStream) -> bool {
    let r = unsafe { isatty(fd.into()) };
    r != 0
}
