// 139ba267302516bcc9e9e23141d94ac04cd56a91

// OCAMLRUNPARAM=b ./interpreter.exe xxx.cl-type

open Ast1;

// make a mutable variable to keep track of the number of activations for stack overflow
let activations = ref(0);

// update the number of activation records for each currently processing dispatch/ new object
let update = (increase: bool, lineno: int) => {
    // increase - flag to indicate increment or decrement of activations
    switch(increase) {
        | true => {
            activations := activations^ + 1;

            // if increasing, check for a potential stack overflow
            if(activations^ >= 1000) {
                print_endline("ERROR: " ++ string_of_int(lineno) ++ ": Exception: stack overflow");
                exit(1);
            };
        };
        | false => {
            activations := activations^ - 1;
        }
    };
};

// ensure the correct number of command line arguments
if (Array.length(Sys.argv) != 2) {
    Printf.printf("usage: ./interpreter cl-type-file\n");
};

// command line arguments verified by this point
// open cl-type file for reading
let ast_file = open_in(Sys.argv[1]);

// get the maps from cl-type file
let (class_map, imp_map, parent_map) = read_cl_type_file(ast_file);

// type for locations (just represented with ints)
type location = int;

// variant types for the possible values in COOL
type value = 
    | Void 
    | Bool(bool)
    | Int(int)
    | String(int, string)  // length, string_const
    | Dynamic(string, list((string, location)), location);  // type, list((name, location) tuples)

// helper function to get the type of an object
let getType = (obj): string => {
    switch(obj) {
    | Void => "Void"
    | Bool(_) => "Bool"
    | Int(_) => "Int"
    | String(_, _) => "String"
    | Dynamic(typ, _, _) => typ
    }
}

// create a type for location maps (store)
module OrderedLocation = {
    type t = location;
    let compare = compare;
};
module LocationMap = Map.Make(OrderedLocation); // store map

// use with environment
module StringMap = Map.Make(String); // environment map

// creating a new location that is currently unused in a given store
let new_loc = (sto): location => {
    // get max value, may be none
    let mbinding = LocationMap.max_binding_opt(sto);  

    // figure out if max binding is none or tuple
    let loc = switch(mbinding) {  
        |None => 0
        |Some((loc, _)) => loc  // only care about loc
    }

    // increment to get the next location
    loc + 1;
}

// helper for fold_left to insert items into the store 
let fill_sto_entry = (sto, loc, value) => {
    LocationMap.add(loc, value, sto);
};

// helper for fold_left to insert items into the environment 
let fill_env_entry = (env, name, loc) => {
    StringMap.add(name, loc, env);
};

// return the value and a new store that maps locations to values
let rec eval_expr = (so, env, sto, expr): (value, LocationMap.t(value)) => {

    // decompose expr contents
    let(line, typ_expr, exp) = expr;
    
    // INTERNAL HELPERS
    let rec dispatch_process = (args, sto, lst): (list(value), LocationMap.t(value)) => {

        // create list to store values 
        switch(args) {
            | [hd,...tl] => {
                let(v, sto2) = eval_expr(so, env, sto, hd);  // get value and location map for this head
                let lst = lst @ [v];  // add this value to list
                dispatch_process(tl, sto2, lst);  // eval tail
            }
            | [] => (lst, sto);
        };
    };

    // make a list of n locations
    let rec get_l = (n, sto, lst) => {

        // create list to store values 
        switch(n) {
            | 0 => lst;
            | _ => {

                // get a new location
                let loc = new_loc(sto);

                // put the new location in the store
                let sto2 = LocationMap.add(loc, Void, sto);
                
                // append the new location to the location list
                let lst = lst @ [loc];

                // get another new location with the new store 
                get_l(n - 1, sto2, lst);
            }
        };
    };

    // get the default values for each attr given a type
    let get_default_vals = (acc, typ) => {
        switch(typ) {
            | "String" => acc @ [String(0, "")];
            | "Int" => acc @ [Int(0)];
            | "Bool" =>  acc @ [Bool(false)];
            | _ => acc @ [Void];
        }
    };

    // pass in two Int values and the aritmatic operation to evaluate
    let compute_arithmetic = (x, y, op, divide): (value, LocationMap.t(value)) => {

        let (v_x, sto2) = eval_expr(so, env, sto, x);
        let (v_y, sto3) = eval_expr(so, env, sto2, y);

        switch (v_x, v_y) {
            | (Int(i), Int(j)) => {
                if (!divide) {
                    (Int(op(i, j)), sto3);
                } else {
                    if (j == 0) {
                            print_endline("ERROR: " ++ string_of_int(line) ++  ": Exception: division by zero");
                            exit(1);
                    } else {
                        (Int(i/j), sto3);
                    };
                }
            };
            | _ => {
                print_endline("not ints");
                exit(1);
            };
        };
    };

    // EVALUATE EXPRESSIONS
    switch(exp) {
        | Identifer(id) => {
            // get name
            let(_, name) = id;

            // get the associated value
            let v = switch(name) {
                | "self" => so; // just the self object if self
                | _ => {
                    // Search environment by name
                    let loc = switch(StringMap.find(name, env)) {
                        | l => l;
                        | exception(_) => print_endline("failed to find id"); 1;
                    };

                    // Search store by location
                    switch(LocationMap.find(loc, sto)){
                        | l => l;
                        | exception(_) => print_endline("failed to find value (id)" ++ name ++ string_of_int(line)); exit(1);
                    };
                };
            };

            // return the value and the new store
            (v, sto);
        };
        | Assign(id, expr) => {
            let(_, name) = id;
            // evaluate the expression
            let (v1, sto2) = eval_expr(so, env, sto, expr);
            
            // get the location of the identifier and store the value there
            let loc = switch(StringMap.find(name, env)){
                        | l => l;
                        | exception(_) => print_endline("failed to find id (assign)" ++ name); exit(1);
                    };
            
            let sto3 = LocationMap.add(loc, v1, sto2);

            // return the value and the new store
            (v1, sto3);
        }
        | DynamicDispatch(e0, f, args) => {
            // increment the activations counter
            update(true, line);

            // get a list of values from evaluating the args
            let(lst, sto2) = dispatch_process(args, sto, []); // v1-vn

            // evaluate e0 to get the object v0
            let(v0, sto3) = eval_expr(so, env, sto2, e0);
            
            // get the type of v0
            let typ = getType(v0);

            // get attr info from v0
            let v0_info = switch(v0) {
                // for non dynamic types we give the name of the type and an empty arg list
                    // all methods relating to these will be handled in an internal exp
                | Void => {
                    print_endline("ERROR: " ++ string_of_int(line) ++  ": Exception: dispatch on void");
                    exit(1);
                };
                | Bool(_) => ("Bool", [])
                | Int(_) => ("Int", [])
                | String(_, _) => ("String", []);
                | Dynamic(name, attr_tuple, _) => (name, attr_tuple);
            }
            
            // break up the info - type, list(name, loc)
            let(_, attr_tuple) = v0_info; 
        
            // use the type to find the list of methods of v0
            let(methods) = switch(List.assoc(typ, imp_map)) {
                        | l => l;
                        | exception(_) => print_endline("failed to find methods (dynamic)"); exit(1);
                    };

            // use the function name to get the method information
            let(_, f_name) = f;

            let((formals, _, body)) = switch(List.assoc(f_name, methods)) {
                        | l => l;
                        | exception(_) => print_endline("failed to find method information (dynamic)"); exit(1);
                    };
             
            // list of new locations for the formals
            let lx = get_l(List.length(lst), sto3, []);  

            // decompose
            let(attrs, la) = List.split(attr_tuple);  
            
            // insert the values and new locations to the store
            let sto4 = List.fold_left2(fill_sto_entry, sto3, lx, lst);

            // insert the attr names and their locations in the environment
            let env2 = List.fold_left2(fill_env_entry, env, attrs, la);

            // insert the formal names and the new locations in then environment
            let env3 = List.fold_left2(fill_env_entry, env2, formals, lx);

            // save the results of eval body - returns final (value, store): (v, S_n+3)
            let tup = eval_expr(v0, env3, sto4, body);  

            // decrement activation counter
            update(false, line);

            // return the eval tuple
            tup;
        }
        | StaticDispatch(e0, typ, m, args) => {
            // increment activations counter
            update(true, line);

            let(lst, sto2) = dispatch_process(args, sto, []);
            let(v0, sto3) = eval_expr(so, env, sto2, e0);
            let v0_info = switch(v0) {
                | Void => {
                    print_endline("ERROR: " ++ string_of_int(line) ++  ": Exception: dispatch on void");
                    exit(1);
                };
                | Bool(_) => ("Bool", [])
                | Int(_) => ("Int", [])
                | String(_, _) => ("String", []);
                | Dynamic(name, attr_tuple, _) => (name, attr_tuple);
            }
            // type, list(name, loc)
            let(_, attr_tuple) = v0_info;
            let (_, static_typ) = typ;

            // use the type to find the list of methods of v0
            let(methods) = switch(List.assoc(static_typ, imp_map)) {
                        | l => l;
                        | exception(_) => print_endline("failed to find methods (static)"); exit(1);
                    };

            // use the function name to get the method information
            let(_, m_name) = m;
            let((formals, _, body)) = switch(List.assoc(m_name, methods)) {
                        | l => l;
                        | exception(_) => print_endline("failed to find method info (static)"); exit(1);
                    };
            
            // list of new locations for the formals
            let lx = get_l(List.length(lst), sto3, []);

            // decompose
            let(attrs, la) = List.split(attr_tuple);  
            
            // same as dynamic
            let sto4 = List.fold_left2(fill_sto_entry, sto, lx, lst);
            let env2 = List.fold_left2(fill_env_entry, env, attrs, la);
            let env3 = List.fold_left2(fill_env_entry, env2, formals, lx);
            
            // save the results of eval body - returns final (value, store): (v, S_n+3)
            let tup = eval_expr(v0, env3, sto4, body);  

            // decrement activation counter
            update(false, line);

            // return the eval tuple
            tup;
        }
        | SelfDispatch(f, args) => {
            // increment activations counter
            update(true, line);

            // get the values
            let(lst, sto2) = dispatch_process(args, sto, []);
            // get the type of the object 
            let self_type = getType(so);

            // get the attributes from the object
            let so_info = switch(so) {
                // for string we should give the string value a location at some point
                | Void => ("Void", []);
                | Bool(_) => ("Bool", []);
                | Int(_) => ("Int", []);
                | String(_, _) => ("String", []);
                | Dynamic(name, attr_tuple, _) => (name, attr_tuple);
            }
            // type, list(name, loc)
            let(_, attr_tuple) = so_info;

            // use the type to find the list of methods of v0
            let(methods) = switch(List.assoc(self_type, imp_map)) {
                        | l => l;
                        | exception(_) => print_endline("failed to find method (self)"); exit(1);
                    };

            // use the function name to get the method information
            let(_, f_name) = f;
            let((formals, _, body)) = switch(List.assoc(f_name, methods)){
                        | l => l;
                        | exception(_) => print_endline("failed to find method info (self)"); exit(1);
                    };
            
            // list of new locations for the formals
            let lx = get_l(List.length(lst), sto2, []);

            // decompose
            let(attrs, la) = List.split(attr_tuple);  
            
            // same as previous dispatches
            let sto3 = List.fold_left2(fill_sto_entry, sto, lx, lst);
            let env2 = List.fold_left2(fill_env_entry, env, attrs, la);
            let env3 = List.fold_left2(fill_env_entry, env2, formals, lx);
            
            // save the results of eval body - returns final (value, store): (v, S_n+3)
            let tup = eval_expr(so, env3, sto3, body);   

            // decrement activation counter
            update(false, line);

            // return the eval tuple
            tup;
        }
        | If(pred, then_exp, else_exp) => {
            // evaluate the predicate
            let (v1, sto2) = eval_expr(so, env, sto, pred);

            // evaluate the expression depending on the result of the predicate
                // returns the value and new store of the eval'd expr
            switch(v1) {
                | Bool(true) => eval_expr(so, env, sto2, then_exp);
                | Bool(false) => eval_expr(so, env, sto2, else_exp);
                | _ => {print_endline("400"); exit(1)};
            } 
        }
        | While(pred, body) => {
            // evaluate the predicate
            let (v1, sto2) = eval_expr(so, env, sto, pred);

            // evaluate the expression depending on the result of the predicate
                // returns the value and new store of the eval'd expr
            switch(v1) {
                | Bool(true) => {
                    // evaluate the body
                    let(_, sto3) = eval_expr(so, env, sto2, body);

                    // make a new while to evaluate
                    let while_exp = (line, typ_expr, While(pred, body));

                    // evaluate the new while with the new store
                    eval_expr(so, env, sto3, while_exp);
                }
                | Bool(false) => (Void, sto2);
                | _ => {print_endline("421"); exit(1)};
            }
        }
        | Block(expr_list) => {
            // helper to get the value and new store from an expr
            let eval_seq = (acc, expr) => {
                let (_, store) = acc;
                let(v_new, sto2) = eval_expr(so, env, store, expr);
                (v_new, sto2);
            };
            // go through all the expressions and get the store and values
            // use a void to start the acc bc it doesnt impact the eval
            List.fold_left(eval_seq, (Void, sto), expr_list);
        };
        | New(id) => {
            // increment activations counter
            update(true, line);
            
            // get type from id
            let(_, typ) = id;

            // determine type
            let t0 = switch(typ){
                | "SELF_TYPE" => getType(so);
                | _ => typ;
            };

            // get the attributes from the class map
            let attrs = switch(List.assoc(t0, class_map)){
                        | l => l;
                        | exception(_) => print_endline("failed to find attrs (new)"); exit(1);
                    }; // list(name, type, expr)
            
            // create new locations to put attrs
            let li = get_l(List.length(attrs), sto, []);  

            // helpers to extra information from the attributes
            // get names of attr
            let get_name = (acc, attr) => {
                switch(attr) {
                    | Attr(name, _) => acc @ [name];
                    | AttrInit(name, _, _) => acc @ [name];
                }
            };
            // get types of attr
            let get_types = (acc, attr) => {
                switch(attr) {
                    | Attr(_, typ) => acc @ [typ];
                    | AttrInit(_, typ, _) => acc @ [typ];
                }
            };

            // get the names and types of the attributes into lists
            let names = List.fold_left(get_name, [], attrs);
            let types = List.fold_left(get_types, [], attrs);

            // create new list(name, location)
            let make_new_attrs = (acc, name, loc) => {
                acc @ [(name, loc)];
            }
            let new_attrs = List.fold_left2(make_new_attrs, [], names, li);

            // make a new object of type t0 with default types
            let v1 = switch(t0) {
                | "Bool" => Bool(false);
                | "Int" => Int(0);
                | "String" => String(0, "");
                | _ => Dynamic(t0, new_attrs, new_loc(sto));
            };
            let defaults = List.fold_left(get_default_vals, [], types)

            // add in the new location-default value pairs to the store
            let sto2 = List.fold_left2(fill_sto_entry, sto, li, defaults);

            // add in the name-new location
            let env2 = List.fold_left2(fill_env_entry, env, names, li); 

            // create assign expression for those attrs with initializers
            // and filter out the ones that do not need to be initialized
            let make_assigns = (acc, attr) => {
                switch(attr){
                    | Attr(_,_) => acc; // skip non init attr
                    | AttrInit(name, typ, expr) => 
                        // potentially wrong types here
                        // exp = (lineno, type, subpart)
                        // subpart = Assign(ident, expr)
                        acc @ [(line, typ, Assign((line, name), expr))];
                }
            };

            // create a block of assignments to evaluated
            let assigns = List.fold_left(make_assigns, [], attrs);
            let block = (line, t0, Block(assigns)); // currently returns VOID
            let(_, sto3) = eval_expr(v1, env2, sto2, block); // returns v2, sto3

            // decrement activation counter
            update(false, line);

            // return the new object and new store from the assigns
            (v1, sto3); 
        }
        | Isvoid(expr) => {
            let (v, sto2) = eval_expr(so, env, sto, expr);

            switch(v) {
                | Void => (Bool(true), sto2); // if void, true
                | _ => (Bool(false), sto2); // if anything else, false
            }
        }
        | Plus(x, y) => { // use functional programming to do the math
            compute_arithmetic(x, y, (+), false)
        }
        | Minus(x, y) => {
            compute_arithmetic(x, y, (-), false)
        }
        | Times(x, y) => {
            compute_arithmetic(x, y, (*), false)
        }
        | Divide(x, y) => {
            compute_arithmetic(x, y, (/), true);
        }
        | Lt(x, y) => {
            // evaluate the expressions
            let (v_x, sto2) = eval_expr(so, env, sto, x);
            let (v_y, sto3) = eval_expr(so, env, sto2, y);

            switch(v_x, v_y) {
                | (Void, Void) => (Bool(false), sto3); // Void is always false
                | (Bool(i), Bool(j)) => (Bool(i < j), sto3);
                | (Int(i), Int(j)) => (Bool(i < j), sto3);
                | (String(_, str_i), String(_, str_j)) => {
                    // compare the strings
                    if (String.compare(str_i, str_j) < 0) {
                        (Bool(true), sto3);
                    } else {
                        (Bool(false), sto3);
                    }
                } 
                | (Dynamic(_, _, _), Dynamic(_, _, _)) => (Bool(false), sto3); // always returns false if different objects
                | _ => {print_endline("557"); exit(1)};
            }
        } 
        | Eq(x, y) =>  {
            // evaluate the expressions
            let (v_x, sto2) = eval_expr(so, env, sto, x);
            let (v_y, sto3) = eval_expr(so, env, sto2, y);

            switch(v_x, v_y) {
                | (Void, Void) => (Bool(true), sto3); // two voids are always equal to each other 
                | (Bool(i), Bool(j)) => (Bool(i == j), sto3);
                | (Int(i), Int(j)) => (Bool(i == j), sto3);
                | (String(_, str_i), String(_, str_j)) => {
                    if (String.compare(str_i, str_j) == 0) {
                        (Bool(true), sto3);
                    } else {
                        (Bool(false), sto3);
                    }
                } 
                | (Dynamic(_, _, loc_i), Dynamic(_, _, loc_j)) => {
                    if (loc_i == loc_j) {
                        // check that all the locations are equal, if so then the objects are the same
                        (Bool(true), sto3);
                    } else {
                        (Bool(false), sto3); // always returns false if different objects
                    };
                }; 
                | (_, Void) | (Void, _) => (Bool(false), sto3) // void is only equal to void
                | _ => {print_endline("587"); exit(1)};
            }
        }
        | Le(x, y) =>  {
            // evaluate the expressions
            let (v_x, sto2) = eval_expr(so, env, sto, x);
            let (v_y, sto3) = eval_expr(so, env, sto2, y);

            switch(v_x, v_y) {
                | (Void, Void) => (Bool(true), sto3); // two voids are always equal to each other 
                | (Bool(i), Bool(j)) => (Bool(i <= j), sto3);
                | (Int(i), Int(j)) => (Bool(i <= j), sto3);
                | (String(_, str_i), String(_, str_j)) => {
                    if (String.compare(str_i, str_j) <= 0) {
                        (Bool(true), sto3);
                    }
                    else {
                        (Bool(false), sto3);
                    }
                } 
                | (Dynamic(_, _, loc_i), Dynamic(_, _, loc_j)) => {
                    if (loc_i == loc_j) {
                        // check that all the locations are equal, if so then the objects are the same
                        (Bool(true), sto3);
                    } else {
                        (Bool(false), sto3); // always returns false if different objects
                    };
                    
                    
                };
                | (_, Void) | (Void, _) => (Bool(false), sto3) // void is only equal to void
                | _ => {print_endline("617"); exit(1)};
            }
        } 
        | Not(expr) => {
            // evaluate the expressions
            let (v, sto2) = eval_expr(so, env, sto, expr);

            // flip the bool
            switch(v) {
                | Bool(true) => (Bool(false), sto2); 
                | Bool(false) => (Bool(true), sto2);
                | _ => {print_endline("626"); exit(1)};
            }
        }
        | Negate(expr) => { // again may need to check for number in bounds of 32 bits
            // evaluate the expressions
            let (v, sto2) = eval_expr(so, env, sto, expr);

            // multiply by -1 to negate
            switch(v) {
                | Int(i) => (Int(-1*i), sto2);
                | _ => {print_endline("634"); exit(1)};
            }
        }
        | Integer(num) => {
            // evaluates to value Int and the same store
            (Int(num), sto);
        }
        | String(str) => { 
            // get length of the string
            let len = String.length(str);
            // return string object (length, string) and store            
            (String(len, str), sto);  
        }
        | True => {
            // evaluates to Bool value true and the same store
            (Bool(true), sto);
        }
        | False => {
            // evaluates to Bool value false and the same store
            (Bool(false), sto);
        }
        | Let(bindings, expr) => {
            
            // internal to evaluate
            let eval_one_let = (binding, expr) => {
                // get the name of the variable, its value, and the new store55
                let (var, (v1, sto2)) = switch(binding) {
                        // evaluate the init exp
                        | BindInit(var, _, e) => {
                            (var, eval_expr(so, env, sto, e))
                        };

                        // get the default value if there is not init exp
                        | Bind(var, typ) => {
                            let (_, type_info) = typ;
                            
                            // function returns a list of defaults so get the one value
                            let v1 = switch(get_default_vals([], type_info)) {
                                | [hd, ...[]] => hd;
                                | _ => {print_endline("671"); exit(1)};
                            };

                            (var, (v1, sto));
                        };
                    };

                    // make a new location for the variable
                    let l1 = new_loc(sto2); 

                    // put it in the store
                    let sto3 = LocationMap.add(l1, v1, sto2);

                    // put it in the env
                    let(_, name) = var;
                    let env2 = StringMap.add(name, l1, env);

                    // evaluate the let exp with the new store and env
                    let (vf, stof) = eval_expr(so, env2, sto3, expr);
                    
                    (vf, stof);
            };
            
            // evaluate the bindings
            switch(bindings) {
                | [hd, ...[]] => {
                    eval_one_let(hd, expr);
                };
                | [hd, ... tl] => {
                    // evaluate the first, but make a new let to evalate with all the other bindings
                    let inner_let = (line, typ_expr, Let(tl, expr));
                    eval_one_let(hd, inner_let);
                };
                | _ => {print_endline("703"); exit(1)};
            };
        };
        | Case(expr, case_elements) => {
            
            // evaluate the case expression
            let(v0, sto2) = eval_expr(so, env, sto, expr);

            // get the type of v0
            let x_typ = switch(getType(v0)) {
                | "Void" => { // error if case exp is void
                    print_endline("ERROR: "++ string_of_int(line) ++ ": Exception: case on void");
                    exit(1);
                };
                | typ => typ;	// otherwise return the type
            }

            // check if a branch matches x, then check parents, etc.
            // filter to get any case elements that match the current x type
            let rec same_as_x = (x_typ, elements, acc) => {
                switch(elements) {
                    | [hd,...tl] => {
                        let (_, (_,typ), _) = hd; 
                        if (x_typ == typ) {
                            same_as_x(x_typ, tl, acc @ [hd]);
                        } else {
                            acc;
                        }
                    }
                    | [] => acc;
                };
            };

            let rec get_branch = (x_typ, case_elements) => {
                // See if this type matches any branches
                // same as x will return an empty list if none of the elements had x_type or will return the one case that does match 
                switch(same_as_x(x_typ, case_elements, [])) {
                    | [] => {
                        // if at object, there is no more parents to check
                        if (x_typ == "Object") {
                            print_endline("ERROR: " ++ string_of_int(line) ++ ": Exception: case without matching branch: " ++ x_typ ++ "(...)");
                            exit(1);
                        } else { // otherwise check the next parent
                            get_branch(List.assoc(x_typ, parent_map), case_elements)
                        }   
                    }
                    | [hd] => hd; // got a match
                    | _ => {
                        print_endline("something is wrong"); 
                        exit(1);
                    }
                };
            };    

            // get variable and the expression from the selected branch 
            let ((_, id_i), (_,_), expr_i) = get_branch(x_typ, case_elements);
            
            // put the results in the environment and the store
            let l0 = new_loc(sto2);
            let sto3 = LocationMap.add(l0, v0, sto);
            let env2 = StringMap.add(id_i, l0, env);

            // returns (v1, sto4)
            eval_expr(so, env2, sto3, expr_i); 
        }
        | Internal(str) => {
            switch (str) {
                | "IO.in_int" => {
                    // read entire line first, catching error if nothing before end of line
                    let inp = switch(read_line()) {
                        | line => line;
                        | exception(_) => "0";
                    };

                    // get only the part we want from the line
                    let i = switch(Scanf.bscanf(Scanf.Scanning.from_string(inp), " %d", x => x)) {
                        | exception(_) => 0;
                        | i => i;
                    };

                    // ensure it is less than 32 bits
                    if(i < -2147483648 || i > 2147483647) {
                        (Int(0), sto)
                    } else {
                        (Int(i), sto);
                    };
                };
                | "IO.out_int" => {
                    // look for IO methods
                    let(methods) = List.assoc("IO", imp_map);

                    // use the function name to get the method information
                    let((formals, _, _)) = List.assoc("out_int", methods);

                    let arg = switch(formals) {
                        | [hd, ...[]] => {
                            let loc = StringMap.find(hd, env);
                            LocationMap.find(loc, sto);
                        };
                        | _ => {print_endline("798"); exit(1)};
                    };

                    // print out the number
                    switch(arg) {
                        | Int(num) => Printf.printf("%d", num);
                        | _ => {print_endline("803"); exit(1)};
                    };
                    
                    // self_type, store
                    (so, sto);
                };
                | "IO.in_string" => {
                    // read the string, reason will break if it contains a null char
                    // marker is true if null character is found
                    let marker= ref(false);
                    let input = switch(input_line(stdin)) {
                        | str => {
                            // look at each char to identify any null character
                            for(i in 0 to String.length(str)-1) {
                                let ch : char = str.[i];
                                if (Char.code(ch) == 00) {
                                    marker := true;
                                }
                            }
                            // return an empty string if null character is found
                            if(marker^ == true) "" else str;
                        };
                        | exception(_) => "";
                    };

                    // return the string read in and the store
                    (String(String.length(input), input), sto);
                };
                | "IO.out_string" => {
                    // look for IO methods
                    let(methods) = List.assoc("IO", imp_map);

                    // use the function name to get the method information
                    let((formals, _, _)) = List.assoc("out_string", methods);

                    // get the string to output from the store
                    let arg = switch(formals) {
                        | [hd, ...[]] => {
                            let loc = StringMap.find(hd, env);
                            LocationMap.find(loc, sto);
                        };
                        | _ => {print_endline("842"); exit(1)};
                    };

                    switch(arg) {
                        | String(_, str) => {

                            // look for escape characters
                            let newline = Str.regexp("\\\\n");
                            let str = Str.global_replace(newline, "\n", str);
                            
                            let tab = Str.regexp("\\\\t");
                            let str = Str.global_replace(tab, "\t", str);
                            
                            // print it out
                            print_string(str);
                        }
                        | _ => {print_endline("860"); exit(1)};
                    };
                    
                    // self_type, store
                    (so, sto);
                };
                | "Object.abort" => {
                    // flush and print
                    print_endline("abort");  
                    exit(1);
                }; 
                | "Object.copy" => {
                    // Get the object's attributes
                    let so_info = switch(so) {
                        | Void => ("Void", [])
                        | Bool(_) => ("Bool", [])
                        | Int(_) => ("Int", [])
                        | String(_, _) => ("String", []);
                        | Dynamic(name, attr_tuple, _) => (name, attr_tuple);
                    }
                    
                    // type, list(name, loc)
                    let(name, attr_tuple) = so_info; 

                    // decompose
                    let(attrs, la) = List.split(attr_tuple);  

                    // Get copies of the attributes
                    let copy_val = (acc: list(value), loc: location) => {
                        acc @ [switch(LocationMap.find(loc, sto)) {
                        | l => l;
                        | exception(_) => print_endline("failed to find value (copy)"); exit(1);
                        }];
                    }
                    let vals = List.fold_left(copy_val, [], la)

                    // Create a new store location for each attribute
                    let locs = get_l(List.length(vals), sto, []);

                    // Assign copy of attribute value to the location
                    let sto2 = List.fold_left2(fill_sto_entry, sto, locs, vals);

                    (Dynamic(name, List.combine(attrs, locs), new_loc(sto2)), sto2);
                };
                   
                | "Object.type_name" => {
                    let typ = getType(so);
                    // get the type and return it as a string with the store
                    (String(String.length(typ), typ), sto);
                }
                | "String.concat" => { // concat(s: string)
                    
                    // get a new string object wiht the concat'd string
                    let concat = switch(so) {
                        // concat uses string from the so
                    | String(_, str) => {
                        // look up the parameter s in the env and get its value
                        let loc_s = StringMap.find("s", env);
                        let s_val = LocationMap.find(loc_s, sto);
                        
                        // combine the string from so and the s
                        switch(s_val) {
                            | (String(_, s)) => {
                                
                                let new_str = String.concat("", [str, s]);
                                String(String.length(new_str), new_str);
                            }
                            | _ => {print_endline("922"); exit(1)};
                        };
                    }
                    | _ => failwith("too many concat parameters");
                    };

                    // new concat string value and store
                    (concat, sto);
                };
                | "String.length" => {
                    // get the length from the so
                    switch(so) {
                        | String(num, _) => (Int(num), sto);
                        | _ => {print_endline("935"); exit(1)};
                    };
                };
                | "String.substr" => {  
                    // get the index and length parameter values
                    let loc_i = StringMap.find("i", env);
                    let i_val = LocationMap.find(loc_i, sto);

                    let loc_l = StringMap.find("l", env);
                    let l_val = LocationMap.find(loc_l, sto);

                    // get the substring
                    let sub = switch(i_val, l_val) {
                        // get the i and l
                        | (Int(i), Int(l)) => {
                            let (str_len, str) = switch(so) {
                                | String(str_len, str) => (str_len, str);
                                | _ => {print_endline("952"); exit(1)};
                            };

                            // out of bounds
                            if (i + l >= str_len) {
                                print_endline("ERROR: 0: Exception: String.substr out of range");
                                exit(1);
                            };
                            
                            let substr = String.sub(str, i, l);
                            String(String.length(substr), substr);
                        }
                        | _ => {print_endline("964"); exit(1)};
                    };
                    (sub, sto);
                };
                | _ => {print_endline("968"); exit(1)};
            };
        }
    }
};

// create an empty store and environment and a self object
let sto = LocationMap.empty;
let env = StringMap.empty;
let so = Void;

let init_exp = (0, "object", DynamicDispatch(
    // target object expression - new Main = New(ident)
    (0, "Main", New((0, "Main"))),
    // method name - ident
    (0, "main"),
    //list of args
    [])
);

// run the program
eval_expr(so, env, sto, init_exp);
