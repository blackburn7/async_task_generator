/*
Assignment 5

Atticus Colwell
acolwel2@jh.edu

Matthew Blackburn
mblackb8@jh.edu
*/

MS1:

Atticus: did serialization, value stack, table, and set value
Matt: did message, get value, incr value

Interesting elements: we noticed alot of the code/format for get value, set value, and to a lesser extent incr value
could be copied over and reused as how each process would occur on the outskirts of their actual functionality were 
very similar.

MS2:

Matthew/Atticus: did a lot of pair programming for client connection

Matthew: much of the server code
Atticus: error handling


Synchronization report:

    The data structures that needed to be synchronized were the server_tables map in 
the Server class and each individual Table object. The server_tables map required 
synchronization because it is accessed and modified by multiple client threads 
concurrently when creating and finding tables. Similarly, each Table object needed 
synchronization because its internal data structures (theMap and tempMap) are accessed 
and modified by multiple client threads concurrently when performing operations like 
setting values, getting values, committing changes, and rolling back changes.
To handle synchronization, mutexes were employed. For the server_tables map, a mutex 
named tables_mutex was introduced to protect access to the map. The tables_mutex is 
locked using pthread_mutex_lock before accessing or modifying the server_tables map in 
the create_table and find_table functions, and it is unlocked using 
pthread_mutex_unlock after the critical section to allow other threads to access the 
map.
    For each individual Table object, a mutex named m_mutex was introduced as a member 
variable to protect access to its internal data structures. The m_mutex is initialized 
in the constructor of the Table class using pthread_mutex_init. The lock, unlock, and 
trylock functions were implemented in the Table class to provide convenient methods for
locking and unlocking the m_mutex. The set, get, has_key, commit_changes, and 
rollback_changes functions of the Table class are responsible for locking and 
unlocking the m_mutex appropriately to ensure synchronized access to the table's data.
    We are confident that the server implementation is free of race conditions and 
deadlocks. By using mutexes to protect critical sections where shared data structures 
are accessed and modified, potential race conditions are prevented. The mutexes ensure 
that only one thread can access the shared data at a time, eliminating concurrent 
access and maintaining data integrity. Furthermore, the code follows a consistent 
locking order when acquiring multiple locks, which helps prevent deadlocks. In the 
Server class, only the tables_mutex is acquired and released within the same function, 
avoiding potential deadlocks. In the Table class, only the m_mutex is acquired and 
released within each function, preventing deadlocks within a single table. The trylock 
function is used in certain scenarios to avoid blocking and potentially causing 
deadlocks when acquiring locks on multiple tables.
    Overall, the synchronization mechanisms employed in the code, including the use of 
mutexes to protect shared data structures, following a consistent locking order, and 
using trylock when necessary, provide confidence that the server implementation is  
thread-safe and free of race conditions and deadlocks. The synchronization ensures that 
concurrent access to shared data is properly managed, maintaining data integrity and 
preventing conflicts between threads.
