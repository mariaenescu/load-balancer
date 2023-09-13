**Enescu Maria**

## Homework 2 - Load Balancer

Load Balancer (Main Server) - It has access to 4 operations represented
by corresponding functions. These are read from the input file.  

### Command `add_server`

The `loader_add_server()` function in the `load_balancer.c` file is
dedicated to adding a new server to the server array or updating
key-value pairs.

- Implementation: 
    * Add the `server` to the servers server vector.
    * Information is moved in the following cases:
        1. Insert server at position 0 -> then we need to move all
        information that has a hash greater than the hash of the
        server at position 1 to the server at position 0;
        2. Insert server at the last position -> then we need to move
        information that has a hash greater than the server at
        position 0 but has a hash smaller than the hash of the server
        inserted at the last position;
        3. Insert the server at any position other than 0 and the last
        position -> then we only need to move information that has a
        hash smaller than the current server hash and smaller than the
        inserted server hash.

### Command `remove_server`

The `loader_remove_server()` function in the load_balancer.c file is
dedicated to removing a server from the server vector, removing all
objects from the server's hashtable, and adding the objects from the
removed server to a new server according to the hash of the keys.

- Implementation:
    * Using a for loop, we iterate through the three replicas of the server.
    * For each element in the server array, we check if its ID matches the
    ID of the server we want to delete.
    * If a match is found, we store its reference in the `deleted` variable.
    * We then assign the `nod` variable to the next server in the array
    (if the deleted server is not the last element) or the first server in
    the array (if the deleted server is the last element).
    * All elements in the array starting from the position of the deleted
    server are shifted one position to the left to fill the gap left by the
    deleted server. 
    * We free the memory, decrement the size of the array, and reallocate
    the memory.

### Command `retrieve`:

The `loader_retrieve()` function in the `load_balancer.c` file is dedicated
to returning the value corresponding to a given key.

- Implementation: 
    * For each element in the server array, it is checked whether the hash
    value of the respective server (`main->servers[i]->hash`) is greater than
    or equal to the hash value of the key being searched (`h_key`).
    * When the first server with a hash value greater than or equal to `h_key`
    is found, its index is stored in the `index` variable.
    * The function then returns the server ID (`server_id`) and the data associated
    with the key using the `server_retrieve(main->servers[index]->data, key)`
    function.

### Command `store`:

The `loader_store()` function in the `load_balancer.c` file is dedicated to
accessing the server and adding an object to the corresponding server.

- Implementation:
    * For each element in the array of servers, it checks if the hash value
    of the corresponding server (`main->servers[i]->hash`) is greater than or
    equal to the searched hash value (`h_key`).
    * When the first server with a hash value greater than or equal to h_key
    is found, its index is stored in the `index` variable.
    * The data associated with the key is updated for the found server using
    the `server_store(main->servers[index]->data, key, value) function`.
    * The function returns the ID of the server (`server_id`).
