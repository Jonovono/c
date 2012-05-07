---
title: c
description: Add comments to files or folders.
author: Jonovono
contact: @Jonovono
created:  2012 May 02

---

c
=========

## Screenshot
![C screenshot](http://i.imgur.com/D3fPm.png)

## Give folders or files Comments!!
Sometimes I lose track of what certain folders or files are for or do. Instead of having to open the project I wanted a way to quickly give a certain directory a comment describing what the project does or what technology it makes use of. Couldn't find a way to do this easily, so I made this.

This is my first C program, so feel free to send some fixes or point out things I have done that could be better. Would love some feedback!

## About / Examples
When run this program creates a .comments directory in the current directory to store the comments for the files. So a given file, for example, names file1.txt will be given an entry called file1.txt.comment in the comment directory.

The main commands are:

* View the comment for a file or directory (or the current directory).
* Give a file or directory (or the current directory) a comment.
* View all the files/directories with their comment beside them (also in ascending or descending order based on modification time)

### Viewing Comments

    c <filename or directory name>

Or to view the current directory

    c .

To view all of the files/directories

    c all
	c all + (ascending order on modification time)
	c all - (descending order)
	
### Adding comments

This will replace whatever comment that file already has if it has one
    
	c <filename or directory name> "comment"
	
To append a comment to the end of whatever that file has use:

    c <filename or directory name> -p "comment"

Of course, use . in replace of <filename> for the current directory.
	
	
## Download
Get it using the following command:

    $ git clone git://github.com/Jonovono/c.git

And then should be able to install going:

	$ ./configure
	
	$ make
	
	$ sudo make install
	
And those on Mac should be able to use:

	$ brew install c
	
## Contact
Follow me or give me feedback/suggestions etc on twitter [@Jonovono](https://twitter.com/#!/jonovono)

Don't be afraid to make some changes and submit them!
	

	