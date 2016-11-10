Clone
=====

Use `--recursive` switch to clone with submodules:

`git clone --recursive git@github.com:frederic-klein/thesis.git`

To add the submodules after cloning without `--recursive` use:

`git submodule update --recursive --init --remote`

Repo Structure
==============

 code
------------------------------------------------------------------------------------------
 contains source code for a distributed SMPC (limited range of functions for serious games)
 library written in C and a Java binding for usage with Android

 sources
------------------------------------------------------------------------------------------
 papers, books, etc. related to SMPC, distributed systems, mesh networking, etc. to be 
 used in the thesis
 
  tex
------------------------------------------------------------------------------------------
 the current state of the thesis as tex and pdf
 
