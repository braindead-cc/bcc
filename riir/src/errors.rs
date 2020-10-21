use std::fmt;
use crate::tty::*;
use crate::program::*;

const CONTEXT: usize = 32;

#[derive(Copy, Clone, Debug)]
pub enum ErrorKind {
    UnmatchedOpenBrace,
    UnmatchedCloseBrace,
}

impl fmt::Display for ErrorKind {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> Result<(), fmt::Error> {
        let r = match self {
            ErrorKind::UnmatchedOpenBrace => "unmatched '['".to_string(),
            ErrorKind::UnmatchedCloseBrace => "unmatched ']'".to_string(),
        };

        write!(f, "{}", r)
    }
}

#[derive(Clone, Debug)]
pub struct Error {
    kind: ErrorKind,
    culprit: BFCommand,
    bfcode: String,
}

impl Error {
    pub fn new(kind: ErrorKind, token: BFCommand, bf: &str) -> Self {
        Self { kind: kind, culprit: token, bfcode: bf.to_string() }
    }

    pub fn pretty_print(&self) {
        use std::cmp::max;

        let (msg, line, col) = (self.kind.to_string(),
            self.culprit.pos.0, self.culprit.pos.1);

        // how much context to show?
        let context = self.bfcode.split("\n").collect::<Vec<_>>()[line-1];
        let context_begin = context.len().saturating_sub(CONTEXT / 2);
        let context_end = {
            if context.len() + (CONTEXT / 2) >= self.bfcode.len() - 1 {
                context.len() - 1
            } else {
                context.len() + (CONTEXT / 2)
            }
        };
        let pre_ellipses = if context_begin == 0 { "   " } else { "..." };
        let after_ellipses = if context_end == context.len() - 1 { "   " }
                             else { "..." };

        // write the first bit
        println!("{bld}{r}error:{rs} {bld}{}{rs}", msg,
            bld = TermStyle::Bold, rs = TermStyle::Reset,
            r = TermStyle::Fg(TermColor::Red));

        // line/column number
        println!("  {bld}{b}-->{rs} line {bld}{}{rs}, col {bld}{}{rs}",
            line, col, bld = TermStyle::Bold, rs = TermStyle::Reset,
            b = TermStyle::Fg(TermColor::Blue));

        // now the offending code
        println!("{bld}{b}{p:>0$}{rs}", max(line.to_string().len() + 2, 4),
            p = "|", bld = TermStyle::Bold, rs = TermStyle::Reset,
            b = TermStyle::Fg(TermColor::Blue));
        println!("{bld}{b} {l:>0$} | {rs} {bef}{c}{af}",
            line.to_string().len(),
            c = context, bef = pre_ellipses, af = after_ellipses,
            l = line, bld = TermStyle::Bold, rs = TermStyle::Reset,
            b = TermStyle::Fg(TermColor::Blue));
        println!("{bld}{b}{p:>0$}{rs}    {r}{ptr:>1$}{rs}",
            max(line.to_string().len() + 2, 4),
            context.len() - context_begin + 1, p = "|", ptr = "^",
            bld = TermStyle::Bold, rs = TermStyle::Reset,
            r = TermStyle::Fg(TermColor::Red),
            b = TermStyle::Fg(TermColor::Blue));
    }
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> Result<(), fmt::Error> {
        let (msg, line, col) = (self.kind.to_string(),
            self.culprit.pos.0, self.culprit.pos.1);
        write!(f, "{}:{}: {}", line, col, msg)
    }
}

impl std::error::Error for Error {
}
