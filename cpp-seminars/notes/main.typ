#set document(title: "Спецкурс — конспект")

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
  leading: 0.56em,
  spacing: 0.52em,
)

#set heading(numbering: "1.1")
#set list(spacing: 0.28em)
#set enum(spacing: 0.28em)
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
  it, 16pt, 1.15em, 0.42em,
)

#show heading.where(level: 2): it => heading-block(
  it, 13.5pt, 0.9em, 0.32em,
)

#show heading.where(level: 3): it => heading-block(
  it, 11.5pt, 0.72em, 0.24em,
)

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
  #text(size: 19pt, weight: "bold")[СПЕЦКУРС]
]

#v(0.55em)

#include "01.sem.typ"
#pagebreak()
#include "02.sem.typ"
#include "08.sem.typ"
