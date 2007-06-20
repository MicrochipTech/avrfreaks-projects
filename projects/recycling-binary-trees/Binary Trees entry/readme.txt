DesignStellaris2006 Contest Entry Demonstration Program
Entry Number: LM1770

File List:
  Support folder: OSRAM support routines
  Listings folder: listing output

  Readme.txt            - this file
  Startup.s             - startup code
  LM1770_main.c         - main program code
  LM1770_globals.h      - support file for main program
  LM1770 Abstract.doc   - abstract documentation file
  LM1770 Complete.doc   - complete documentation file
  random.c              - random number support (not original)
  LM1770_Fig#.bmp       - figures 1 - 7, bitmaps
  LM1770_Block#.bmp     - block diagrams 1 & 2
  LM1770_pic.jpg        - picture of project
  
  plus a host of odds and ends, the use of which is
  unknown to mere mortals.
  
  -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

This program demonstrates an algorithm for allocating and
recovering nodes for binary trees. Entire subtrees may be
recovered at once, rather than disassembling the subtree into
individual nodes for garbage collection. It is implemented
on the LM3S811 Evaluation Kit.

The program maintains 63 nodes which can be connected into
a 5 level (32 leaves) binary tree. In an endless loop, a leaf
is randomly selected and the tree constructed from the node
to that leaf. The tree is displayed, and there is a pause of
about 1/60th to 3 seconds, depending upon the potentiometer
setting.

Then, if the user is not pressing the user button, a node is
randomly selected and is deleted, along with any parent node(s)
which have a single child. The resulting tree is displayed,
and again there is a pause.

On each display, the number of nodes in the tree and in the
garbage collection pool is counted and displayed. These two
numbers will always add to 63.

The program is written in C. The random number routines, board
initialization code, and part of the ADC interrupt handler
were copied from the evaluation board demonstration program.

