-- p1c
-- 38a1108d19f54a1fab115779487fbbcea3ae1843

Class Main inherits IO {
    graph: AdjList <- new AdjNil;   -- how our tasks will be organized before being read
    queue: List <- new Nil;     -- the tasks to read and graph
    sorted: List <- new Nil;    -- the final task list
    cycle: Bool <- false;       -- allows to stop cycles when they're detected

    -- method to put our list back in normal order
    reverse_list(l : List) : List {
        let rev : List <- (new Nil), 
            i : Int <- 0 in {       -- index of the task we're reviewing
        while i < l.length() loop {
            -- create new list with each successive value added to the beginning, and the previous new list to the end
            rev <- (new Cons).init(l.get(i), rev);  
            i <- i + 1; -- increment the index counter
            }
        pool;
        rev; -- return the new, fully reversed list
        }
    };

    read_list(): Object {               -- reads in the task list data and adds it to the graph
        let done: Bool <- false in
        while not done loop         -- assesses each of the task pairs
            let a: String <- in_string(),   -- the task
                b: String <- in_string() in -- the prerequisite for the task
            
            if b = "" then
                done <- true    -- if there are no tasks, we're done
            else
                {
                    graph <- graph.add_node(a); -- adds a new node for the task if none exists
                    graph <- graph.add_node(b); -- adds a new node for the prereq task if none exists
                    graph.insert_edge(b, a);    -- inserts an "edge" to show the relation between the two nodes
                }
            fi
        pool
    };

    get_visit(): String {   -- used to find unvisited nodes
        let i: Int <- 0,
            str: String <- "" in {
            while i < graph.length() loop
                if graph.get_i(i).get_visited() = "" then { -- if the node is unvisited
                    str <- graph.get_i(i).name(); -- assign str the name of the unvisited node
                    i <- graph.length(); -- break the loop if an unvisited node is found
                } else
                    i <- i + 1 -- if the node is visited, check the next one
                fi
            pool;
            str; -- return the name of the unvisited node
        }
    };

    visit2(n: String): Object {             -- used to follow the path of tasks/dependencies
        let node: AdjList <- graph.get(n),  -- the node we're currently visiting
            adj: List <- node.get_adj(),    -- the nodes adjacent to the one we're visiting
            i: Int <- 0 in {
                -- all of the else clauses in this section do nothing
                if node.get_visited() = "temporary" then {
                    -- if we're visiting a node that's already been visited on this part of the traversal, we've detected a cycle
                    cycle <- true;
                    false;
                } else
                    false
                fi;

                if (node.get_visited() = "permanent") then
                -- if the node we're at is already final from a previous traversal, do nothing
                    node.set_visited("permanent")
                else
                    false
                fi;

                if node.get_visited() = "" then {
                    -- if the node has not been visited, set the visited value to a temporary mark
                    node.set_visited("temporary");

                    while i < adj.length() loop {
                        -- visit all the adjacent nodes to the current node
                        visit2(adj.get(i));
                        i <- i + 1;
                    } pool;

                    -- this is reached if there is no cycle, in which case set the value to permanent
                    node.set_visited("permanent");
                    -- add the newly permanent node to the list of sorted nodes
                    sorted <- sorted.append(node.name());
                } else 
                    false
                fi;
            }
    };


    dfs(): Object { -- depth-first search
        let toVisit: String <- self.get_visit() in -- get_visit() -> gets a node w/o permanent mark
        {
            while not (toVisit = "") loop -- as long as there is still an unvisited node, continue
            {
                visit2(toVisit);    -- visit the unmarked node
                toVisit <- self.get_visit(); -- find the next unmarked node
            } pool;
        }
    };


    main(): Object {
        {
            read_list();    -- read the list into a graph
            dfs();          -- perform a depth first search of the graph
            if cycle then   -- if the dfs found a cycle, print cycle
                out_string("cycle\n")
            else {          -- dfs succeeded
                sorted <- reverse_list(sorted); -- reverse our sorted list
                sorted.print_list();            -- print the resultant sorted list in order
            } fi;
        }
    };
};


-- support for List of String

(* The List type is not built in to Cool, so we'll have to define it 
 * ourselves. Cool classes can appear in any order, so we can define
 * List here _after_ our reference to it in Main. *) 
Class List inherits IO { 
        (* cons appends returns a new list with 'hd' as the first
         * element and this list (self) as the rest of the list *) 
	cons(hd : String) : Cons { 
		let new_cell : Cons <- new Cons in
		new_cell.init(hd,self)
	};

        (* You can think of List as an abstract interface that both
         * Cons and Nil (below) adhere to. Thus you're never supposed
         * to call insert() or print_list() on a List itself -- you're
         * supposed to build a Cons or Nil and do your work there. *) 
	insert(i : String) : List { self };
    append(i: String): List { self };

    hd(): String { "" };    -- returns the head of the list
    tl(): List { self };    -- returns a copy of the list excluding the head
    length(): Int { 0 };    -- returns the length of the list
    get(i: Int): String { "" }; -- returns the value at the given index of the list

	print_list() : Object { abort() }; -- prints the contents of the list, one line per index
} ;


Class Cons inherits List { -- a Cons cell is a non-empty list 
	xcar : String;          -- xcar is the contents of the list head 
	xcdr : List;            -- xcdr is the rest of the list

	init(hd : String, tl : List) : Cons {
	    {
	    	xcar <- hd;
	    	xcdr <- tl;
	    	self;
	    }
	};

    (* insert() does insertion sort (using a reverse comparison) *) 
	insert(s : String) : List {
		if not (s < xcar) then          -- sort in reverse order
			(new Cons).init(s,self)
		else
            -- if the new string does not belong at the head of the current list, see if it belongs in the next index
			(new Cons).init(xcar,xcdr.insert(s)) 
		fi
	};

    append(i: String): List {   -- add a new value to a list
        let tmp : Cons <- (new Cons).init(xcar, xcdr.append(i)) in {
        tmp;    -- return the new list
        }
    };

    hd(): String { xcar }; -- return the head element of the list
    tl(): List { xcdr };    -- return all but the head element of the list
    length(): Int { 1 + xcdr.length() }; -- recursively count list length and return this value

    get(i: Int): String {   -- return the list value at the given index i
        if i = 0 then
            xcar
        else
            xcdr.get(i - 1)
        fi
    };

	print_list() : Object {
		{
		    out_string(xcar);   -- print the head of the list
		    out_string("\n"); 
		    xcdr.print_list(); -- send the tail elements of the list to recursive print_list call until there are no more
		}
	};
} ;


Class Nil inherits List { -- Nil is an empty list 

	insert(s : String) : List { (new Cons).init(s, self) }; 

    append(i: String): List { (new Cons).init(i, self) };

	print_list() : Object { true }; -- do nothing 

} ;


-- adjacency list classes
Class AdjList inherits IO {
    visited: String <- "";  -- assume all elements of adjacency list are unvisited

    cons(name: String, hd: List): AdjCons {
        (new AdjCons).init(name, hd, self)
    };

    add_node(name: String): AdjList { self };    -- add a task in sorted order
    insert_edge(src: String, dest: String): AdjList { self };   -- create an order between two tasks
    length(): Int { 0 };    -- return the length of a list
    get(name: String): AdjList { self };    -- return the adjacency list of a particular task
    get_i(i: Int): AdjList { self };        -- return the adjacency list of a task at a particular index
    get_adj(): List { new Nil };            -- return the adjacency list of a task

    get_visited(): String { visited };      -- returns either "", temporary, or permanent
    set_visited(p: String): String { visited <- p };    -- set to either "", temporary, or permanent

    print(): Object { true };

    name(): String { "" };  -- returns the task name
};


Class AdjCons inherits AdjList {
    xname: String;
    xcar: List; -- adjacent tasks
    xcdr: AdjList;

    init(name: String, hd: List, tl: AdjList): AdjCons {
        { -- a block
            xname <- name;
            xcar <- hd;
            xcdr <- tl;
            self; -- return self
        }
    };

    -- reverse insertion sort of a Set
    add_node(name: String): AdjList {
        if xname = name then
            self
        else
            if xname < name then -- reverse sorting
                self.cons(name, new Nil) -- add our task at beginning with empty adj list
            else
                (new AdjCons).init(xname, xcar, xcdr.add_node(name))
            fi
        fi
    };

    insert_edge(src: String, dest: String): AdjList {
        {
            if xname = src then     -- creates a connection between two nodes
                xcar <- xcar.insert(dest)
            else
                xcdr <- xcdr.insert_edge(src, dest)
            fi;
            self;
        }
    };

    length(): Int {     -- recursively finds the length of a list
        1 + xcdr.length()
    };

    get(name: String): AdjList {    -- recursively implemented to find the task node with a given name
        if xname = name then
            self
        else
            xcdr.get(name)
        fi
    };

    get_i(i: Int): AdjList {    -- recursively implemented to find the task node at a given index
        if i = 0 then
            self
        else
            xcdr.get_i(i - 1)
        fi
    };

    get_adj(): List { xcar };   -- returns the adjacency list of a specific node

    print(): Object {   -- allows us to print out a full list
        {
            out_string("----\nname: ");
            out_string(xname);  -- prints the name of the head of the list
            out_string("\n adj: \n");
            xcar.print_list();  -- sends the head of the list to be printed
            out_string("\n");
            xcdr.print();       -- sends the tail list of the list to be printed
        }
    };
    name(): String { xname };   -- returns the first task name
};


Class AdjNil inherits AdjList { -- creates an empty adjacency list
    add_node(name: String): AdjList {
        self.cons(name, new Nil) -- just add the task to the empty list and create an empty adj list
    };
};
