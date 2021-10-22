Class Three inherits Object {
    f(): One {
        new One
    };
    init(): One {
        self.f()
    };
};

Class One inherits Three {
    g(): Object {
        f()
    };
};

Class Two inherits One {
    questionableChoice() : One {
        self
    };
};

Class Main inherits IO {

    not_thing: One;

    thing: One <- (new One).init();

    result: String <- "";

    main(): Object{
        {
            let done: Bool, new_thing: One <- thing.copy() in
            {

                while not done loop
                    let input: String <- in_string() in

                    if input = "" then
                        done <- true
                    else
                        {
                            result <- input;
                            new_thing <- new_thing.copy();
                        }
                    fi
                pool;
            };
            isvoid thing;
            not isvoid thing;

            thing.f();
        }
    };
};

Class AttemptCase {

    number: Int <- 1;
    number2: Int <- ~number;

    thing1: One;
    thing2: AttemptCase;
    thing3: One <- (new One).init();

    init(): AttemptCase {
        {
            case (new Three).init() of
                id_1: One => 7;
                id_2: Two => "asdf";
                id_3: Bool => true;
            esac;
            number / 2;
            number / 0;
            number <- number / 0;

            if thing1 < thing2 then
                self
            else
                if thing1 = thing3 then
                    self
                else
                    if thing1 <= thing2 then
                        self
                    else
                        new AttemptCase
                    fi
                fi
            fi;
        }
    };
    
};