# Priamos BASIC interpreter

This is a reincarnation of an older project of mine, rewritten from scratch with a minimal C++ language (C++98 w/o templates),
to simplify porting to other languages.

It is a work in progress, currently I'm working on the interpreter itself. I plan however, to create a specialized implementation language for it that compiles to C++, though, since I want to shrink it down a bit.

Since I do not have much time to work on it, expect slow updates.

If you do want to run it, run "make" from the command line, then type "./testinterpreter" to run the interpreter test, which permits you to enter commands (and program lines). LIST, LET, and PRINT should already work (PRINT is only implemented for testing purposes, it doesn't support the full PRINT syntax yet). LET is still mandatory. 

License is GPLv3. Platform is unspecified as of yet, but I'm using G++ on Linux for development.

UPDATE 2022-08-03: Expect the current code base to be moved to a branch and then deleted from the master in the foreseeable future. I've been working on several concepts that are MUCH smaller. If I decide on something useful, I will bring it up here.
