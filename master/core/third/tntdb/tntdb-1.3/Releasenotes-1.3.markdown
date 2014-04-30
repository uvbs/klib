Releasenotes tntdb 1.3
======================

Major optimization of oracle driver
-----------------------------------
The oracle driver got a major optimization. When reading multiple rows, previously each row was requested separately from OCI. Now multiple rows are fetched in a single OCI call and passed row by row to the application.

Add support for cxxtools::String (unicode string class)
-------------------------------------------------------
Cxxtools has a string class, which supports unicode. Tntdb now has full support for it. Unicode strings can be read and written transparently.

Bugfixes and optimizations
--------------------------
Some minor bugfixes and optimizations are done in tntdb.
