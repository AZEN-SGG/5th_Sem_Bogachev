#set document(title: "Программирование — конспект")

#set page(
  paper: "a4",
  margin: (x: 1.9cm, top: 1.7cm, bottom: 1.8cm),
  numbering: "1",
  number-align: center + bottom,
)

#set text(
  font: "New Computer Modern",
  lang: "ru",
  size: 11.5pt,
)

#set par(
  justify: true,
  leading: 0.7em,
  spacing: 0.68em,
)

#set heading(numbering: "1.1")
#set list(spacing: 0.36em)
#set enum(spacing: 0.36em)
#set raw(theme: auto)

#let heading-block(it, size, above, below) = block(
  above: above,
  below: below,
  sticky: true,
)[
  #set text(size: size, weight: "bold")
  #context counter(heading).display(it.numbering)
  #h(0.55em)
  #it.body
]

#show heading.where(level: 1): it => heading-block(
  it, 16.5pt, 1.25em, 0.55em,
)

#show heading.where(level: 2): it => heading-block(
  it, 14pt, 1em, 0.45em,
)

#show heading.where(level: 3): it => heading-block(
  it, 12pt, 0.8em, 0.35em,
)

#show math.equation.where(block: true): it => block(
  above: 0.6em,
  below: 0.6em,
  breakable: false,
)[#it]

#show link: set text(fill: rgb("#1f5eff"))

#show raw.where(block: true): it => block(
  width: 100%,
  breakable: true,
  fill: rgb("#f6f8fa"),
  stroke: (
    left: 1.2pt + rgb("#0969da"),
    top: 0.4pt + rgb("#d0d7de"),
    right: 0.4pt + rgb("#d0d7de"),
    bottom: 0.4pt + rgb("#d0d7de"),
  ),
  radius: 2pt,
  inset: (x: 10pt, y: 7pt),
)[
  #set text(font: "Consolas", size: 9.8pt)
  #it
]

#align(center)[
  #text(size: 20pt, weight: "bold")[ПРОГРАММИРОВАНИЕ]
]

#v(0.75em)

#include "01.sem.typ"
#pagebreak(weak: true)
#include "02.sem.typ"
#pagebreak(weak: true)
#include "03.sem.typ"
#pagebreak(weak: true)
#include "05.sem.typ"
#pagebreak(weak: true)
#include "06.sem.typ"
#pagebreak(weak: true)
#include "07.sem.typ"
#pagebreak(weak: true)
#include "08.sem.typ"
