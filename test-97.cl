Class Main inherits IO {
    main(): Object {
        let x:Comp <- (new Comp).init(3), y:Comp2 <- (new Comp2).init() in {
            out_string((true = false).type_name());
            x.get_x();
            out_string((x = y).type_name());
            out_string((x = x).type_name());
            out_string((y = y).type_name());
            out_string((x < y).type_name());
            out_string((y < x).type_name());
            out_string((x <= y).type_name());
            out_string((y <= x).type_name());
            out_string(((y < x) = (x < y)).type_name());
            let x: Comp, y: Comp2 in {
                out_string((true = false).type_name());
                out_string((x = y).type_name());
                out_string((x = x).type_name());
                out_string((y = y).type_name());
                out_string((x < y).type_name());
                out_string((y < x).type_name());
                out_string((x <= y).type_name());
                out_string((y <= x).type_name());
                out_string(((y < x) = (x < y)).type_name());
                let c:Cat <- (new Cat), d: Dog <- (new Dog) in {
                    out_string((true = false).type_name());
                    out_string((c = d).type_name());
                    out_string((c = c).type_name());
                    out_string((d = d).type_name());
                    out_string((c < d).type_name());
                    out_string((d < c).type_name());
                    out_string((c <= d).type_name());
                    out_string((d <= c).type_name());
                    out_string(((d < c) = (c < d)).type_name());
                    c.case_break();
                }; 
            };
            out_string((true = false).type_name());
            x.get_x();
            out_string((x = y).type_name());
            out_string((x = x).type_name());
            out_string((y = y).type_name());
            out_string((x < y).type_name());
            out_string((y < x).type_name());
            out_string((x <= y).type_name());
            out_string((y <= x).type_name());
            out_string(((y < x) = (x < y)).type_name());
        }
    };
};

Class Comp inherits Main {
    x: Int <- 0;
    y: Comp2;

    init(x: Int): Comp {
        {
            out_int(x);
            x <- x;
            out_int(x);
        }
    };

    get_x(): Int {
        {
            out_int(x);
            x;
        }
    };
};

Class Comp2 inherits Main {
    x: Int;
    y: Comp;

    init(): Comp2 {
        {
            y <- new Comp;
            if self < y then 
                out_int(y.get_x())
            else
                out_int(y.get_x())
            fi;

            if self <= y then 
                out_int(y.get_x())
            else
                out_int(y.get_x())
            fi;

            if self = y then 
                out_int(y.get_x())
            else
                out_int(y.get_x())
            fi;
            self;
        }
    };
};

Class Cat {
    x: Comp;
    case_break(): Object {        
        case x of 
            x: Comp => "cry";
            y: Comp2 => "....";
        esac   
    };
};
Class Dog {};