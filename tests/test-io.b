[
  This is for testing general IO.

  It should give two lines of output; the two lines should be identical,
  and should be lined up one over the other.

  The content of the lines tells how input is being processed; each line
  should be two uppercase letters.

  Anything with O in it means newline is not coming through as ten on
  input.

  LK means newline input is working fine, and EOF leaves the cell unchanged
  LB means newline input is working fine, and EOF translates as 0.
  LA means newline input is working fine, and EOF translates as -1.

  See: http://www.hevanet.com/cristofd/brainfuck/tests.b
]

>,>+++++++++,>+++++++++++[<++++++<++++++<+>>>-]<<.>.<<-.>.>.<<.
