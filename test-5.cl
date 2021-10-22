Class Main inherits IO{
    main(): Object {
        let something: String, turtle: Int in {
            let something: Something <- new Something in {
                out_string(something.f().type_name());
            };
            out_string(something.type_name());
            something.concat(something);
            out_string(something);
            turtle <- 2147483647;
            out_int(turtle);

            let x:Int <- 7, y:Int<-9, z:Int<-7 in {
                -- check some math stuff?
                if (9<7) then out_string("true")
                else out_string("false") fi;

                if (7<9) then out_string("true")
                else out_string("false") fi;

                if (7=9) then out_string("true")
                else out_string("false") fi;

                if (7=x) then out_string("true")
                else out_string("false") fi;

                if (7<=9) then out_string("true")
                else out_string("false") fi;

                if (7<=y) then out_string("true")
                else out_string("false") fi;

                if (x<=z) then out_string("true")
                else out_string("false") fi;

                if (x<=x) then out_string("true")
                else out_string("false") fi;

                if (9<=7) then out_string("true")
                else out_string("false") fi;
            };
            abort();
            out_string("This should be unreachable");
        }
    };
};

Class Something {
    f() : Something {
        self
    };
    init(): Something {
        self.f()
    };
};

Class Teeth inherits Something {
    questionableChoice() : Something {
        self
    };
};