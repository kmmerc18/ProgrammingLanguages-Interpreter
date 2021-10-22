// 139ba267302516bcc9e9e23141d94ac04cd56a91

// DEFINE AST TYPES
type ident = (int, string)  // (lineno, identifer string)

// define these types together - recursive definitions allowed
type exp_subparts = 
    | Assign(ident, exp)  // var, rhs
    | DynamicDispatch(exp, ident, list(exp))  // e, method, args list
    | StaticDispatch(exp, ident, ident, list(exp))  // e, type, method, args list
    | SelfDispatch(ident, list(exp))  // method, args list
    | If(exp, exp, exp)  // predicate, then, else
    | While(exp, exp)  // predicate, body
    | Block(list(exp))  // body
    | New(ident)  // class
    | Isvoid(exp)  // e
    | Plus(exp, exp)  // x, y
    | Minus(exp, exp) 
    | Times(exp, exp)
    | Divide(exp, exp)
    | Lt(exp, exp)
    | Le(exp, exp)
    | Eq(exp, exp)
    | Not(exp)  // x
    | Negate(exp)  // int_constant
    | Integer(int)  // fix type here
    | String(string)  // string_constant
    | Identifer(ident)  // var
    | True
    | False     
    | Case(exp, list(case_element)) // case exp, list of case elements
    | Let(list(binding), exp)  // binding list, body
    | Internal(string)  // class.method
and exp = (int, string, exp_subparts)  // var, type, body
and case_element = (ident, ident, exp)  // var, typ, expression
and binding =
    | Bind(ident, ident)    // var, typ
    | BindInit(ident, ident, exp)  // var, typ, expression

// use a variant type for attributes in classes
and attr = 
    | Attr(string, string)  // name, type name
    | AttrInit(string, string, exp)  // name, type name, expression
and method_info = (list(string), string, exp)   // list of formals, type, expression
and methods = (string, method_info) // class, method info

type class_map_entry = (string, list(attr));  // class, attributes

type imp_map_entry = (string, list(methods));  // class, list of methods

type parent_entry = (string, string);  // parent, child
    
// define our functions for reading the Cl-Type files

// read in list
let read_list = (ic: in_channel, f) => {
    
    let l = input_line(ic);
    // DEBUG: print_endline("Should've read a number but got: " ++ l)

    // read number of elements
    let count = int_of_string(l);
    
    let rec helper = (c, l) => {
        // builds the list in reverse order
        if (c == 0) {
            l
        } else {
            // read one more item and append to beginning
            helper(c-1, [f(ic), ...l]);
        }
    } 

    // since appended in reverse - return the list reversed
    List.rev(helper(count, []));
};

// read an identifier
let read_ident = (ic: in_channel): ident => {
    let lineno = int_of_string(input_line(ic));
    let var = input_line(ic);

    // return an ident
    (lineno, var); 
};

// read and return an expression 
let rec read_exp = (ic: in_channel): exp => {
    let lineno = int_of_string(input_line(ic));
    let name_type = input_line(ic);
    let exp_subpart = input_line(ic);
    
    // all exp subparts have the subpart name on a new line
    let subpart = switch(exp_subpart){
        | "assign" => {
            let var = read_ident(ic);
            let rhs = read_exp(ic);
            Assign(var, rhs)
        }
        | "dynamic_dispatch" => {
            let e = read_exp(ic);
            let m = read_ident(ic);
            let args = read_list(ic, read_exp);
            DynamicDispatch(e, m, args)
        }
        | "static_dispatch" => {
            let e = read_exp(ic);
            let typ = read_ident(ic);
            let m = read_ident(ic);
            let args = read_list(ic, read_exp);
            StaticDispatch(e, typ, m, args)
        }
        | "self_dispatch" => {
            let m = read_ident(ic);
            let args = read_list(ic, read_exp);
            SelfDispatch(m, args)
        }
        | "if" => {
            let predicate = read_exp(ic);
            let then_e = read_exp(ic);
            let else_e = read_exp(ic);
            If(predicate, then_e, else_e);
        }
        | "while" => {
            let predicate = read_exp(ic);
            let body = read_exp(ic);
            While(predicate, body);
        }
        | "block" => {
            let body = read_list(ic, read_exp);
            Block(body);
        }
        | "new" => {
            let class_name = read_ident(ic);
            New(class_name);
        }
        | "isvoid" => {
            let e = read_exp(ic);
            Isvoid(e);
        }
        | "plus" => {
            let x = read_exp(ic);
            let y = read_exp(ic);
            Plus(x, y);
        }
        | "minus" => {
            let x = read_exp(ic);
            let y = read_exp(ic);
            Minus(x, y);
        }
        | "times" => {
            let x = read_exp(ic);
            let y = read_exp(ic);
            Times(x, y);
        }
        | "divide" => {
            let x = read_exp(ic);
            let y = read_exp(ic);
            Divide(x, y);
        }
        | "lt" => {
            let x = read_exp(ic);
            let y = read_exp(ic);
            Lt(x, y);
        }
        | "le" => {
            let x = read_exp(ic);
            let y = read_exp(ic);
            Le(x, y);
        }
        | "eq" => {
            let x = read_exp(ic);
            let y = read_exp(ic);
            Eq(x, y);
        }
        | "not" => {
            let x = read_exp(ic);
            Not(x);
        }
        | "negate" => {
            let x = read_exp(ic);
            Negate(x);
        }
        | "integer" => {
            let int_const = int_of_string(input_line(ic));
            Integer(int_const);
        }
        | "string" => {
            let str_const = input_line(ic);
            String(str_const);
        }
        | "identifier" => {
            let var = read_ident(ic);
            Identifer(var);
        }
        | "true" => { True };
        | "false" => { False };
        | "let" => {
            
            // read in a binding for a let statement
            let read_let = (ic: in_channel): binding => {
                // make appropriate bindings
                switch(input_line(ic)) {
                    | "let_binding_no_init" => {
                        let var = read_ident(ic);
                        let typ = read_ident(ic);
                        Bind(var, typ)
                    }
                    | "let_binding_init" => {
                        let var = read_ident(ic);
                        let typ = read_ident(ic);
                        let value = read_exp(ic);
                        BindInit(var, typ, value)
                    }
                    | _ => failwith("unsupported let statement")
                 }
            }

            let bindinglist = read_list(ic, read_let);
            let body = read_exp(ic);
            Let(bindinglist, body);
        };
        | "case" => {
            
            // read in a case element
            let read_case = (ic: in_channel): case_element => {
                let var = read_ident(ic);
                let typ = read_ident(ic);
                let body = read_exp(ic);

                (var, typ, body);
            }

            let case_exp = read_exp(ic);
            let element_list = read_list(ic, read_case);
            Case(case_exp, element_list);
        };
        | "internal" => {
            // check if valid
            let method = input_line(ic);
            // IO.in_int IO.in_string IO.out_int IO.out_string Object.abort Object.copy Object.type_name String.concat String.length String.substr
            switch (method) {
                | "IO.in_int" | "IO.out_int" | "IO.in_string" | "IO.out_string" 
                | "Object.abort" | "Object.copy" | "Object.type_name" 
                | "String.concat" | "String.length" | "String.substr" => Internal(method);
                | _ => failwith("unsupported internal Class.method " ++ method);
            };
        };
        | _ => failwith("unsupported exp type " ++ exp_subpart);    
    };

    // return an exp
    (lineno, name_type, subpart)
};

// reading the class map
let read_attr = (ic: in_channel) => {
    // read a line and switch on string value
    switch(input_line(ic)) {
        |"no_initializer" => {
            // line no and name
            let name = input_line(ic);
            let typ = input_line(ic);
            Attr(name, typ);
        };
        |"initializer" => {
            // line no and name
            let name = input_line(ic);
            let typ = input_line(ic);
            let exp = read_exp(ic);
            AttrInit(name, typ, exp);
        };
        // we really messed up
        | _ => failwith("unreachable read_attr");
           
    };
}

let read_class = (ic: in_channel): class_map_entry=> {
    let name = input_line(ic);
    let attrs_list = read_list(ic, read_attr);
    
    // return an entry
    (name, attrs_list);
};

// read the class map from the type file
let read_class_map = (ic: in_channel) => {

    // first line should be class_map
    if (input_line(ic) != "class_map") {
        failwith("Malformed class map");
    };

    // correctly formatted, read as list
    read_list(ic, read_class);
};

//  reading the implementation map
let read_method = (ic: in_channel): methods => {
    let name = input_line(ic);
    let formals = read_list(ic, input_line);
    let typ = input_line(ic);
    let typ_exp = read_exp(ic);

    (name, (formals, typ, typ_exp));
};

let read_imp = (ic: in_channel): imp_map_entry => {
    let name = input_line(ic);
    let method_list = read_list(ic, read_method);
    
    // return an entry
    (name, method_list); 
};

let read_imp_map = (ic: in_channel): list(imp_map_entry) => {
    
    if ( input_line(ic) != "implementation_map")
    {
        failwith("Malformed implementation map");
    };

    read_list(ic, read_imp);
};

    // reading in the parent map
let read_parent = (ic: in_channel): parent_entry => {
    let child = input_line(ic);
    let parent = input_line(ic);
    (child, parent);
};

let read_parent_map = (ic: in_channel): list(parent_entry) => {
    
    if (input_line(ic) != "parent_map")
    {
        failwith("Malformed parent map");
    };
    read_list(ic, read_parent);
};

// read input file (CL-Type)
let read_cl_type_file = (ic: in_channel) => {
    let class_map = read_class_map(ic);
    let imp_map = read_imp_map(ic);
    let parent_map = read_parent_map(ic);
    
    
    (class_map, imp_map, parent_map);
    
};
