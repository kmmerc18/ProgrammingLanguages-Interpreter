class Main inherits IO {
    main(): Object{

            let this_test: Test <- (new Test).init(in_int()) in {
                out_int(this_test.test(in_int() + 2));
                out_string("\n");
                out_int(this_test.test(in_int()));
            }
    };
};

class Test inherits Main {

    num: Int <- in_int();
    count: Int <- 0;

    init(x: Int): Test {
        {
            num <- x;
            self;
        }
    };

    -- make 1000 Cool Activation Records
    test(a: Int): Int {
        {
            out_int(a);
            num <- a + num;
            if count < 400 then 
            {
                out_int(num);
                count <- count + 1;
                test_2(num);
            }
            else   
            {
                out_int(num);
                num;
            }fi;
        }
    };

    test_2(a: Int): Int {
        {
            out_int(a);
            num <- a + num;
            out_int(num);
            test(num);
        }
    };
};