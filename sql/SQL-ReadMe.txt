Some general info regarding how we should handle SQL:

The "git tag" command should show you which version is currently live on the server, for example "live2015-01-07" means that the commit the tag is pointing on went live on the server at the 7th of January 2015.

All SQL that should be included in the next update should be placed in a file called "SQL-post-[date here].sql", for example SQL-post-2015-01-07.sql that should contain all SQL intended to be added after the 7th of January 2015.

Make sure the SQL you put in a file has commands that specify which database they should be run on and that they are correct. When a server update is being performed the SQL-file should run without errors with a single command.

The above guidelines were designed with ease of updating in mind and to ensure that no SQL is missing when we update.

Text last updated: 2015-01-11
