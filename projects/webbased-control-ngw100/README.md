# WebBased Control of NGW100

Uploaded by tuxrupert on 2007-07-18 19:48:00 (rating 0 out of 5)

## Summary

As a Newbie to the NGW100 I am very impressed to find a nearly complete Linux based Computer packed in this small size.


To my intend for doing Projects like this let me tell about my experiences:  

In former Days I made some Hacking on simple embedded maschines which I had to program via Assembler and so, often. Doing so, writing a multi tasking system or a (simple) math library took about several month ... for EACH project fulltime ... I resume: Bit fuxxing is interesting but slow!


Due to that, I am (still) dreaming about using tools from the regular x86 Linux, writing code on Laptop to bring it to embedded maschines. 


So I was tinking about a cool basic project for me to do the first steps on my new NGW100. It should be not to complicated, force me to set up the System, get familar with the tools and AVR Studio and write - my first application for NGW.


Here it is:  

Dreaming about autarc maschines and so I find it cool to control these units remotely and by a cool, modern way.  

(Shell based solutions are good but not cool enough for me, I#m dreaming about controlling machines via Web Server on Mars! :o)  

So I have to gonna make a program, which does this job and is pretty easy to understand and to reconfigure (maybe I never get a machine on Mars at all :o( ) for personal purpose.


Technical speaking, I wanna do the following:  

On a Web Server, running on the NGW, a Set of Control Pages is hosted, containing information and formular pages (those contain the nesessary Variable-Names and Fields and/or Text fields, which egg. should be used for "simple Programming" of a NGW controlled Hardware, a little car or whatever)  

The Client (this is you typing data in your Explorer) hacks the data in the formular or text fields. By typing the  

 button, data is passed to a CGI program running on the NGW Server (for control I use C - Language here). Response is played back via generated und updated Info-Page.  

The CGI Program by the way does the analyses (of the parameter Stream) and sets up the nesessary actions on the NGW and generates the Info Page(s).


As I mentioned the CGI program should run as a compiled C-Program.  

I will do that because this is my way to get some highly functinal tools doing their job for me and the analyis: I would like to develop the analysis stuff by use of the brilliant tools Lex and Yacc (indeed I use Flex and Bison).  

Because I am somewhat like a training player and I like general solutions, I prefer to use unix tools designed for their special purpose.


I should be ready with a documented my work finally till september, because - unfortunately - during the day I have to work on other things (again .. :o( ).  

I hope to some reasonable drafts can be done in August - during my vacations. 


Good day  

feedback appreciated  

your tuxrupert  

from Germany

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
